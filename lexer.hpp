#ifndef LEXER_HPP
#define LEXER_HPP

#include <istream>
#include <iostream>
#include <algorithm>
#include "token.hpp"

using namespace std;

/* Tokenize input text string into a list of tokens.
On error, the last token is of type ERROR and the value is the
error message.
*/
TokenList tokenize(std::istream & ins);

// This is small function that checks if the string tempString is empty or not. If it is not then it adds the string and clears the variable tempString
void checkTempString(string& tempString, TokenList& tokensStorage, size_t lineNumber, unsigned int stringDelimCounter, int parenthesisCounter);

#endif