#include "tokenize.hpp"
#include <cctype>

#include <iostream>

//function to check invalid/escape characters. If istream contains them then return true
bool checkInvalid(std::istream &seq)
{
	bool result = false;
	if (seq.peek() == '"' || seq.peek() == ' ' || seq.peek() == '\n' || seq.peek() == '\t' || seq.peek() == '\r')
	{
		return true;
	}

	return result;
}

//function to check OPEN and CLOSE brackets, if contains them then return true
bool checkBrackets(std::istream &seq)
{
	bool result = false;
	if (seq.peek() == OPEN || seq.peek() == CLOSE)
	{
		return true;
	}

	return result;
}


//method to tokenize a given istream, returns a deque of tokens
TokenSequenceType tokenize(std::istream & seq) {
	TokenSequenceType tokens;
	//intialization: the token_count and the string holder that will holder the character/string values and will be placed in the deque
	string strHolder;
	//start loop: while the reading of characters/strings will not fail or while the reading does not reach the end of the file
	//read seq into the string holder and push it to the front of the deque. The older characters will remain in the back of the deque.
	while (!seq.fail() && !seq.eof() && seq.good())
	{
		/*character analysis
		if quotes then ignore
		if blank then ignore
		if ( ) then get only that character
		if ; then ignore the whole line
		else (any other characters/string) then get each character and concatenate it into a single string until next character is a character stated above*/
		if (checkInvalid(seq)) //use .peek() to access the next character
		{
			seq.ignore(); //use .ignore() to ignore the next character
		}
		else if (checkBrackets(seq))
		{
			strHolder = seq.get();
			tokens.push_back(strHolder);
		}
		else if (seq.peek() == COMMENT)
		{
			getline(seq, strHolder, '\n'); //ignore line
			strHolder = "";
		}
		else
		{
			string strBuilder; //string used to concatenate and parse characters into a single string to push in
			bool fail = false; //used as a flag when next character is blank, OPEN, CLOSE, or COMMENT
			bool hasString = false; //used as a flag when eof executes and to make sure that before pushing strBuilding it has content
			while (!seq.fail() && !fail)
			{
				if (checkInvalid(seq) || checkBrackets(seq) || seq.peek() == COMMENT || seq.eof())
				{
					// the next character is a special character as stated above or if it is at the end of file for a single atom. This is will flag fail.
					fail = true;
				}
				else
				{
					strHolder = seq.get();
					strBuilder = strBuilder + strHolder;

					if (!strBuilder.empty())
					{
						hasString = true;
					}
				}
			} //end while-loop
			if (hasString)
			{
				tokens.push_back(strBuilder);
			}
		}
	} //end while-loop
	return tokens;
}