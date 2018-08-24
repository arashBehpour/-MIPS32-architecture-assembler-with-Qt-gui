
#include "catch.hpp"

#include "lexer.hpp"

#include <string>
#include <sstream>
#include <map>

TEST_CASE("test empty stream", "[lexer]")
{
	
	{
		std::string input = "";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		REQUIRE(tl.size() == 0);
	}

	{
		std::string input = "    \t    \r      ";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		REQUIRE(tl.size() == 0);
	}
}

TEST_CASE("test simple strings, one per line", "[lexer]") {

	{ // ends with newline
		std::string input = "this\nis\na\nsequence\nof\nlines\n";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		REQUIRE(tl.size() == 12);
		REQUIRE(tl.back() != Token(ERROR, 0));

		TokenList::iterator it = tl.begin();
		//An iterator is any object that, pointing to some element in a range of elements (such as an array or a container), has 
		//the ability to iterate through the elements of that range using a set of
		//operators (with at least the increment (++) and dereference (*) operators).

		cout << *it << endl;

		REQUIRE(*it == Token(STRING, 1, "this"));
		REQUIRE(*(++it) == Token(EOL, 1));
		REQUIRE(*(++it) == Token(STRING, 2, "is"));
		REQUIRE(*(++it) == Token(EOL, 2));
		REQUIRE(*(++it) == Token(STRING, 3, "a"));
		REQUIRE(*(++it) == Token(EOL, 3));
		REQUIRE(*(++it) == Token(STRING, 4, "sequence"));
		REQUIRE(*(++it) == Token(EOL, 4));
		REQUIRE(*(++it) == Token(STRING, 5, "of"));
		REQUIRE(*(++it) == Token(EOL, 5));
		REQUIRE(*(++it) == Token(STRING, 6, "lines"));
		REQUIRE(*(++it) == Token(EOL, 6));
	}

	{ // does not end with newline
		std::string input = "this\nis\na\nsequence\nof\nlines";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		REQUIRE(tl.size() == 12);
		REQUIRE(tl.back() != Token(ERROR, 0));

		TokenList::iterator it = tl.begin();
		REQUIRE(*it == Token(STRING, 1, "this"));
		REQUIRE(*(++it) == Token(EOL, 1));
		REQUIRE(*(++it) == Token(STRING, 2, "is"));
		REQUIRE(*(++it) == Token(EOL, 2));
		REQUIRE(*(++it) == Token(STRING, 3, "a"));
		REQUIRE(*(++it) == Token(EOL, 3));
		REQUIRE(*(++it) == Token(STRING, 4, "sequence"));
		REQUIRE(*(++it) == Token(EOL, 4));
		REQUIRE(*(++it) == Token(STRING, 5, "of"));
		REQUIRE(*(++it) == Token(EOL, 5));
		REQUIRE(*(++it) == Token(STRING, 6, "lines"));
		REQUIRE(*(++it) == Token(EOL, 6));
	}
}

TEST_CASE("test lists", "[lexer]") {
	{
		std::string input = "this,is, a \t , list , of, strings";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		REQUIRE(tl.size() == 12);
		REQUIRE(tl.back() != Token(ERROR, 0));

		TokenList::iterator it = tl.begin();
		REQUIRE(*it == Token(STRING, 1, "this"));
		REQUIRE(*(++it) == Token(SEP, 1));
		REQUIRE(*(++it) == Token(STRING, 1, "is"));
		REQUIRE(*(++it) == Token(SEP, 1));
		REQUIRE(*(++it) == Token(STRING, 1, "a"));
		REQUIRE(*(++it) == Token(SEP, 1));
		REQUIRE(*(++it) == Token(STRING, 1, "list"));
		REQUIRE(*(++it) == Token(SEP, 1));
		REQUIRE(*(++it) == Token(STRING, 1, "of"));
		REQUIRE(*(++it) == Token(SEP, 1));
		REQUIRE(*(++it) == Token(STRING, 1, "strings"));
		REQUIRE(*(++it) == Token(EOL, 1));
	}

	{
		std::string input = "this, is another, list";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		REQUIRE(tl.size() == 7);
		REQUIRE(tl.back() != Token(ERROR, 0));

		TokenList::iterator it = tl.begin();
		REQUIRE(*it == Token(STRING, 1, "this"));
		REQUIRE(*(++it) == Token(SEP, 1));
		REQUIRE(*(++it) == Token(STRING, 1, "is"));
		REQUIRE(*(++it) == Token(STRING, 1, "another"));
		REQUIRE(*(++it) == Token(SEP, 1));
		REQUIRE(*(++it) == Token(STRING, 1, "list"));
		REQUIRE(*(++it) == Token(EOL, 1));
	}
}


TEST_CASE("test parens", "[lexer]") {

	std::string input = "outside(inside inside )(inside)outside";
	std::istringstream iss(input);

	TokenList tl = tokenize(iss);

	REQUIRE(tl.size() == 10);
	REQUIRE(tl.back() != Token(ERROR, 0));

	TokenList::iterator it = tl.begin();
	REQUIRE(*it == Token(STRING, 1, "outside"));
	REQUIRE(*(++it) == Token(OPEN_PAREN, 1));
	REQUIRE(*(++it) == Token(STRING, 1, "inside"));
	REQUIRE(*(++it) == Token(STRING, 1, "inside"));
	REQUIRE(*(++it) == Token(CLOSE_PAREN, 1));
	REQUIRE(*(++it) == Token(OPEN_PAREN, 1));
	REQUIRE(*(++it) == Token(STRING, 1, "inside"));
	REQUIRE(*(++it) == Token(CLOSE_PAREN, 1));
	REQUIRE(*(++it) == Token(STRING, 1, "outside"));
	REQUIRE(*(++it) == Token(EOL, 1));
}


TEST_CASE("test delimited strings", "[lexer]") {
	{
		std::string input = " a \"b c d\" e \"\" f";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		REQUIRE(tl.size() == 10);
		REQUIRE(tl.back() != Token(ERROR, 0));

		TokenList::iterator it = tl.begin();
		REQUIRE(*it == Token(STRING, 1, "a"));
		REQUIRE(*(++it) == Token(STRING_DELIM, 1));
		REQUIRE(*(++it) == Token(STRING, 1, "b c d"));
		REQUIRE(*(++it) == Token(STRING_DELIM, 1));
		REQUIRE(*(++it) == Token(STRING, 1, "e"));
		REQUIRE(*(++it) == Token(STRING_DELIM, 1));
		REQUIRE(*(++it) == Token(STRING, 1, ""));
		REQUIRE(*(++it) == Token(STRING_DELIM, 1));
		REQUIRE(*(++it) == Token(STRING, 1, "f"));
		REQUIRE(*(++it) == Token(EOL, 1));
	}

	{
		std::string input = R"(
			.data
		.asciiz  "#boi^" )";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		REQUIRE(tl.back() != Token(ERROR, 0));
	}
}


TEST_CASE("test equal token", "[lexer]") {

	std::string input = R"(
  .data
  NAME1 = 1
  NAME2 = 2
  NAME3 = -3)";  // This is a raw string

	std::istringstream iss(input);
	TokenList tl = tokenize(iss);

	REQUIRE(tl.size() == 14);
	REQUIRE(tl.back() != Token(ERROR, 0));

	TokenList::iterator it = tl.begin();
	REQUIRE(*it == Token(STRING, 2, ".data"));
	REQUIRE(*(++it) == Token(EOL, 2));
	REQUIRE(*(++it) == Token(STRING, 3, "NAME1"));
	REQUIRE(*(++it) == Token(EQUAL, 3));
	REQUIRE(*(++it) == Token(STRING, 3, "1"));
	REQUIRE(*(++it) == Token(EOL, 3));
	REQUIRE(*(++it) == Token(STRING, 4, "NAME2"));
	REQUIRE(*(++it) == Token(EQUAL, 4));
	REQUIRE(*(++it) == Token(STRING, 4, "2"));
	REQUIRE(*(++it) == Token(EOL, 4));
	REQUIRE(*(++it) == Token(STRING, 5, "NAME3"));
	REQUIRE(*(++it) == Token(EQUAL, 5));
	REQUIRE(*(++it) == Token(STRING, 5, "-3"));
	REQUIRE(*(++it) == Token(EOL, 5));
}


TEST_CASE("test error handling and reporting", "[lexer]") {  

	{
		std::string input = "\n\n\nfoo \"bar\n";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		REQUIRE(tl.size() > 0);
		REQUIRE(tl.back().type() == ERROR); 
		REQUIRE(tl.back().value().substr(0, 7) == "Error: ");
	}

	{
		std::string input = "foo (bar\n";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		REQUIRE(tl.size() > 0);
		REQUIRE(tl.back().type() == ERROR);
		REQUIRE(tl.back().value().substr(0, 7) == "Error: ");
	}

	{
		std::string input = "foo )bar\n";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		REQUIRE(tl.size() > 0);
		REQUIRE(tl.back().type() == ERROR);
		REQUIRE(tl.back().value().substr(0, 7) == "Error: ");
	}

	{
		std::string input = "foo ((bar)\n";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		REQUIRE(tl.size() > 0);
		REQUIRE(tl.back().type() == ERROR);
		REQUIRE(tl.back().value().substr(0, 7) == "Error: ");
	}

	{
		std::string input = "foo (bar) baz)\n";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		REQUIRE(tl.size() > 0);
		REQUIRE(tl.back().type() == ERROR);
		REQUIRE(tl.back().value().substr(0, 7) == "Error: ");
	}

	{
		std::string input = "\nfoo (\"bar)\" baz\n";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		REQUIRE(tl.size() > 0);
		REQUIRE(tl.back().type() == ERROR);
		REQUIRE(tl.back().value().substr(0, 7) == "Error: ");
	}
}


//*******************************************************************************************************************
//****************************************Arash's Additional Tests***************************************************

TEST_CASE("test error handling and reporting PART 2", "[lexer]") {

	{
		std::string input = " \"(Blah\" ";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		REQUIRE(tl.size() > 0);
		REQUIRE(tl.back().type() == ERROR);
		REQUIRE(tl.back().value().substr(0, 7) == "Error: ");
	}

	{
		std::string input = "This ) is some text (";
		std::istringstream iss(input);

		TokenList tl = tokenize(iss);

		REQUIRE(tl.size() > 0);
		REQUIRE(tl.back().type() == ERROR);
		REQUIRE(tl.back().value().substr(0, 7) == "Error: ");
	}

	{
		std::string input = "This ( is some text& $ )\n(this(is)\nsome)more";
		std::istringstream iss(input);
		
		TokenList tl = tokenize(iss);

		REQUIRE(tl.size() > 0);
		REQUIRE(tl.back().type() == ERROR);
		REQUIRE(tl.back().value().substr(0, 7) == "Error: ");
	}

}


TEST_CASE("test comments", "[lexer]")
{

{
std::string input = "foo= 6\n#comments boi";

std::istringstream iss(input);
TokenList tl = tokenize(iss);

REQUIRE(tl.size() == 4);
REQUIRE(tl.back() != Token(ERROR, 0));

TokenList::iterator it = tl.begin();
REQUIRE(*it == Token(STRING, 1, "foo"));
REQUIRE(*(++it) == Token(EQUAL, 1));
REQUIRE(*(++it) == Token(STRING, 1, "6"));
REQUIRE(*(++it) == Token(EOL, 1));
}

{
std::string input = "foo,tree #comments boi";

std::istringstream iss(input);
TokenList tl = tokenize(iss);

REQUIRE(tl.size() == 4);
REQUIRE(tl.back() != Token(ERROR, 0));

TokenList::iterator it = tl.begin();
REQUIRE(*it == Token(STRING, 1, "foo"));
REQUIRE(*(++it) == Token(SEP, 1));
REQUIRE(*(++it) == Token(STRING, 1, "tree"));
REQUIRE(*(++it) == Token(EOL, 1));
}

{
std::string input = "foo= 6\n#comments boi\nbleed= 3";

std::istringstream iss(input);
TokenList tl = tokenize(iss);

REQUIRE(tl.size() == 8);
REQUIRE(tl.back() != Token(ERROR, 0));

TokenList::iterator it = tl.begin();
REQUIRE(*it == Token(STRING, 1, "foo"));
REQUIRE(*(++it) == Token(EQUAL, 1));
REQUIRE(*(++it) == Token(STRING, 1, "6"));
REQUIRE(*(++it) == Token(EOL, 1));
REQUIRE(*(++it) == Token(STRING, 3, "bleed"));
REQUIRE(*(++it) == Token(EQUAL, 3));
REQUIRE(*(++it) == Token(STRING, 3, "3"));
REQUIRE(*(++it) == Token(EOL, 3));
}



// FOR FUTURE MILESTONES DO NOT KNOW IF THIS: ("Blah") , WOULD BE A LEXING ERROR OR NOT

}
