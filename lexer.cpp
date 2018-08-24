#include "lexer.hpp"
#include "token.hpp"

// implement the lexer function here

TokenList tokenize(std::istream & ins) // In the directions TokenSequence == TokenList, and lexer == tokenize 
{
	string tempString; // This variable will be used to store the string value before I insert it in my tokensStorage
	TokenList tokensStorage; // This is where I will store all my tokens
	char inputCharacter; // This is a character from the input
	size_t lineNumber = 1; // This will keep track of the line number and increment everytime the lexer moves onto the next line. Starts at 1

	bool emptyLine = true; // Initially say that I am on an empty line until I hit some text. This is so I do not emmit a EOL Token for an empty line
	bool isError = false; // Initially there is no error, but when the first lexing error appears, store that Token at the end of the tokenList to indicate so.

	int parenthesisCounter = 0; //counter that starts at 0. When you hit a ( increase it by 1, when you hit a ) decrease by 1. If it ever goes negative or is not 0 at the end of the line there is an error
	unsigned int stringDelimCounter = 0; // counter that starts at 0. when you hit a " increases by 1. If the counter is ever odd by the end of the line then there means there was an error.

	while (ins.get(inputCharacter)) // This stays in the while loop until I can not get anymore characters and the input is good(no bad bits and hasnt reached end of the file).
	{
		//If the inputCharacter is # that means all the characters on that line will be a comment and should be ignored.
		if (inputCharacter == '#' && ((stringDelimCounter%2) == 0) )
		{
			if (!tempString.empty())
			{
				tokensStorage.emplace_back(STRING, lineNumber, tempString);
				tempString.clear();
				tokensStorage.emplace_back(EOL, lineNumber);
			}
			while (ins.good() && ins.peek() != '\n') // This stripes the comments until a new line
			{
				ins.get(inputCharacter);
			}
			
		}
		else
		{
			switch (inputCharacter) {
			case '\n': // the end of a source line has been reached (no value)
				checkTempString(tempString, tokensStorage, lineNumber, stringDelimCounter, parenthesisCounter);
				if (!emptyLine)
				{
					tokensStorage.emplace_back(EOL, lineNumber);
					emptyLine = true; // Because now we are on a new line so we must restart the process of checking for empty lines
				}

				if ((stringDelimCounter % 2 != 0) && !isError)
				{
					isError = true;
					// Stores the error token in the front of the Token List. At the end of the lexing will move the Error token to the back.
					string errorMsg = "Error: " + to_string(lineNumber) + ": The string delimiter does not have a pair on the same line.";
					tokensStorage.emplace_front(ERROR, lineNumber, errorMsg);
				}
				else if ((parenthesisCounter != 0) && !isError)
				{
					isError = true;
					string errorMsg = "Error: " + to_string(lineNumber) + ": There are unmatched parenthesis on the same line.";
					tokensStorage.emplace_front(ERROR, lineNumber, errorMsg);
				}
				lineNumber++;
				break;

			case ',': // is the separator for lists (no value)
				if (emptyLine) { emptyLine = false; } // This so I know I have hit some text on a line so It is not an empty line.
				checkTempString(tempString, tokensStorage, lineNumber, stringDelimCounter, parenthesisCounter);
				tokensStorage.emplace_back(SEP, lineNumber);
				break;

			case '(': // indicates the start of an indirect address (no value)
				if (emptyLine) { emptyLine = false; }
				if ((parenthesisCounter < 0) && !isError) // Error Handling: case for ex. This ) is some text (\n , is an error
				{
					isError = true;
					string errorMsg = "Error: " + to_string(lineNumber) + ": More closed parenthesis than open. Or could have a form: )text( at the end of the line.";
					tokensStorage.emplace_front(ERROR, lineNumber, errorMsg);
				}
				parenthesisCounter++;
				checkTempString(tempString, tokensStorage, lineNumber, stringDelimCounter, parenthesisCounter);
				tokensStorage.emplace_back(OPEN_PAREN, lineNumber);
				break;

			case ')': // indicates the end of an indirect address (no value)
				if (emptyLine) { emptyLine = false; }
				parenthesisCounter--;
				checkTempString(tempString, tokensStorage, lineNumber, stringDelimCounter, parenthesisCounter);
				tokensStorage.emplace_back(CLOSE_PAREN, lineNumber);

				if ((parenthesisCounter == 0) && (stringDelimCounter % 2 != 0) && !isError) // Error Handling: Case for ex. ("Blah) , is an error 
				{
					isError = true;
					string errorMsg = "Error: " + to_string(lineNumber) + ": A pair of string delimiters mixed with ( and ) corresponding parenthesis.";
					tokensStorage.emplace_front(ERROR, lineNumber, errorMsg);
				}
				break;

			case '"': // is used to wrap literal strings (no value)
				if (emptyLine) { emptyLine = false; }
				stringDelimCounter++;
				checkTempString(tempString, tokensStorage, lineNumber, stringDelimCounter, parenthesisCounter);
				tokensStorage.emplace_back(STRING_DELIM, lineNumber);

				if ((ins.peek() == '"') && (stringDelimCounter % 2 == 1)) // If the next character is a string delimiter " ,that means I need to manually add an empty string
				{
					tokensStorage.emplace_back(STRING, lineNumber, "");
				}

				if ((stringDelimCounter % 2 == 0) && (parenthesisCounter != 0) && !isError) // Error Handling: Case for ex. "(Blah" is an error 
				{
					isError = true;
					string errorMsg = "Error: " + to_string(lineNumber) + ": A pair of ( and ) corresponding parenthesis mixed with string delimiters.";
					tokensStorage.emplace_front(ERROR, lineNumber, errorMsg);
				}
				break;

			case '=': // is used in a constant assignment (no value)
				if (emptyLine) { emptyLine = false; }
				checkTempString(tempString, tokensStorage, lineNumber, stringDelimCounter, parenthesisCounter);
				tokensStorage.emplace_back(EQUAL, lineNumber);
				break;

			default: // This is for the value of the string(value is the string)
				if (isspace(inputCharacter) && (stringDelimCounter % 2 != 1)) // If the input character is a space then we want to tokenize the current string while skipping(ignore) the spaces. HOWEVER, we keep the spaces if we are inside a string Delimiter.
				{
					checkTempString(tempString, tokensStorage, lineNumber, stringDelimCounter, parenthesisCounter);
					while (isspace(inputCharacter) && ins.good() && (ins.peek() != '\n') && isspace(ins.peek()))
					{
						ins.get(inputCharacter);
					}
				}
				else
				{
					if (emptyLine) { emptyLine = false; }

					tempString.push_back(inputCharacter); // This pushes the current character into the temporary string
				}
				break;
			}// End of Switch statement

		}// End of else statement that decided if the current input was comments

	}// End of While loop

	checkTempString(tempString, tokensStorage, lineNumber, stringDelimCounter, parenthesisCounter); // This is to make sure there was not a string at the end of the input

	if (!(tokensStorage.empty()) && ((tokensStorage.back().type()) != EOL)) //If the back is not an EOL then force an EOL to indicate the line is ended
	{
		tokensStorage.emplace_back(EOL, lineNumber);
	}

	if ( ((parenthesisCounter != 0) || (stringDelimCounter % 2 != 0)) && (isError == false) ) { // means by the end of scanning the file if there are uneven amounts of parenthesis or string delims then it is an error
		isError = true;
		string errorMsg = "Error:" + to_string(lineNumber) + ": Uneven amount of string delimiters or parenthesis.";
		tokensStorage.emplace_front(ERROR, lineNumber, errorMsg);
	}


	if (isError) // If there is an error add the ERROR token to the end of the Token List
	{
		Token errorToken = tokensStorage.front(); // Takes the error token which is stored in the front of the token list into a temp variable
		tokensStorage.emplace_back(errorToken); // places the error token at the back of the list
		tokensStorage.pop_front(); // Then deletes the error token from the front of the list
	}
	return tokensStorage;
}



void checkTempString(string& tempString, TokenList& tokensList, size_t lineNumber, unsigned int stringDelimCounter, int parenthesisCounter)
{
	bool case1 = (parenthesisCounter == 0) && (stringDelimCounter % 2 != 0); // for ex. ("bar) , and we want bar to be inserted in the list before the closing parenthesis
	bool case2 = (stringDelimCounter % 2 == 0) && (parenthesisCounter != 0);// for ex. "(bar" , and we want bar to be inserted in the list before the second string delimiter
	if (!tempString.empty() && ((stringDelimCounter % 2 == 0) || case1 || case2))
	{
		tokensList.emplace_back(STRING, lineNumber, tempString);
		tempString.clear();
	}
}