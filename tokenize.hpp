#ifndef TOKENIZE_HPP_
#define TOKENIZE_HPP_

#include <istream>
#include <deque>
#include <string>

using std::string;

typedef std::deque<std::string> TokenSequenceType;

const char OPEN = '(';
const char CLOSE = ')';
const char COMMENT = ';';

// split string into a list of tokens where a token is one of
// OPEN or CLOSE or a space-delimited string
// ignores any whitespace and from any ";" to end-of-line
TokenSequenceType tokenize(std::istream & seq);

//methods to either check for invalid characters/escape keys or brackets (OPEN, CLOSE)
bool checkInvalid(std::istream &seq);
bool checkBrackets(std::istream &seq);

#endif
