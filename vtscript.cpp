#include <cstdlib>
#include <iostream>
#include <istream>
#include <fstream>
#include <sstream>

#include "interpreter.hpp"
//method to set prompt
void prompt() {
	std::cout << "\nvtscript> ";
}

//method to readline
std::string readline() {
	std::string line;
	std::getline(std::cin, line);

	return line;
}

//method for REPL mode
void repl()
{
	Interpreter interp;
	while (!std::cin.fail())
	{
		prompt();
		std::string strHolder = readline();
		Expression exp;

		std::istringstream expression(strHolder);

		if (!interp.parse(expression))
		{
			std::cout << (" Error : Invalid Expression. Could not parse.");
		}
		else {
			try {
				Expression exp = interp.eval();
				std::cout << exp << std::endl;
			}
			catch (const InterpreterSemanticError & ex) {
				std::cerr << ex.what() << std::endl;
			}
		}
		expression.str(""); //reset the istringstream
		expression.clear();
	} //end while loop
}

//method to obtain a string and interpret/evaluate it for the [-e] command
int evaluateString(const std::string & strHolder)
{
	std::istringstream iss(strHolder);

	Interpreter interp;

	bool ok = interp.parse(iss);
	if (!ok)
	{
		std::cout << "Error : Failed to parse: " << strHolder << "--- check formatting. Use () to enclose an expression." << std::endl;
		return EXIT_FAILURE;
	}

	try
	{
		Expression result;
		result = interp.eval();
		std::cout << result << std::endl;
		return EXIT_SUCCESS;
	}
	catch (const InterpreterSemanticError & ex)
	{
		std::cout << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

}



//this is to evaluate the a given stream from a file
bool evaluateStream(std::istream & stream)
{
	Interpreter interp;

	if (!interp.parse(stream))
	{
		std::cout << "Error : Unable to parse program" << std::endl;
		return false;
	}

	try
	{
		Expression exp = interp.eval();
		std::cout << exp << std::endl;
		return true;
	}
	catch (const InterpreterSemanticError & ex)
	{
		std::cerr << ex.what() << std::endl;
		return false;
	}

}

//opening a file given a string argument and evaluating it from iftsream
bool openFile(std::string fileName)
{
	bool success = true;
	std::ifstream in(fileName);

	if (!in)
	{
		std::cout << "Error : Unable to open or locate file." << std::endl;
		success = false;
	}

	if (!evaluateStream(in))
	{
		success = false;
	}
	return success;
}

int main(int argc, char **argv)
{	
		if (argc == 1)
		{
			//REPL Mode
			repl();
		}
		else if (argc == 2)
		{
			if (!openFile(std::string(argv[1])))
			{
				return EXIT_FAILURE;
			}

		}
		else if (argc == 3)
		{
			//three cases: 
			//if second argument is -e, then parse the line and evaluate it.
			//if second argument is cat, open file
			//else invalid argument
			if (std::string(argv[1]) == "-e")
			{
				//concatenate argument into string
				std::string strHolder = std::string(argv[2]);

				return evaluateString(strHolder);

			}
			//else
			std::cout << "Error : Invalid Arguments. Options: [-e] to evaluate line, [fileName] to evaluate file, \n or run executable without args to enter REPL mode.\n";
			return EXIT_FAILURE;
		}
		else
		{
			std::cout << "Error : Invalid Number of arguments." << std::endl;
			return EXIT_FAILURE;
		}
		return EXIT_SUCCESS;

}
