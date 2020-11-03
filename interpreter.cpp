#include "interpreter.hpp"

// system includes
#include <stack>
#include <stdexcept>
#include <iostream>



bool Interpreter::parse(std::istream & expression) noexcept {
	// TODO: implement this function
	// return true if input is valid. otherwise, return false.
	  // call the tokenize method in tokenize.hpp in order to obtain the istream parameter to obtain parsed tokens.
	TokenSequenceType tokenHolder = tokenize(expression);
	//-------------Checking if the expression is balanced-----------------------------------------------------
	if (!checkBalance(tokenHolder)) //call checkBalance function
	{
		return false;
	}
	//------Parsing the Expression into the internal AST--------------------------------------------------
	if (tokenHolder.empty()) {//case empty
		return false; //cannot parse an empty token list
	}
	if (tokenHolder.size() == 1) {//CASE: if tokens contains one token.
		Atom atomHolder;
		bool isOk = token_to_atom(tokenHolder.at(0), atomHolder);
		if (isOk && (atomHolder.type == NumberType || atomHolder.type == BooleanType)) {
			ast.head = atomHolder;
			return true;
		}
		return false;
	}
	return makeAST(tokenHolder, ast);
};

Expression Interpreter::eval()
{
	// TODO: implement this function

	  //if try/catch not implemented then ast will not be reset when error is thrown, which would cause weird mess in REPL mode.
	try
	{
		Expression result = ast.evaluate(envir, graphics);
		ast = Expression(); //NOTE: reset the ast after running program or else it will keep the entire abstract syntax tree. This will cause weird errors in REPL mode.
		return result;
	}
	catch (InterpreterSemanticError &w)
	{
		ast = Expression(); //reset ast if error is caught and throw error.
		throw InterpreterSemanticError(w.what());
	}
}

bool checkBalance(TokenSequenceType tokenHolder)
{
	bool result = true;
	int balanceCount = 0; // this is the loop counter to check that if the deque has a balance set of parenthesis 
	for (unsigned int i = 0; i < tokenHolder.size(); i++)
	{
		if (tokenHolder[i] == "(")
		{
			balanceCount++;
		}
		else if (tokenHolder[i] == ")")
		{
			balanceCount--;
		}

		if (balanceCount < 0)
		{
			result = false;
		}
	}

	//post balance loop check
	if (balanceCount != 0)
	{
		//if balanceCount is not 0 then the expression is not balanced
		result = false;
	}

	return result;
}
//this method is called after checking the balance of the "(" and ")" in the tokenized deque
//thus, this method assumes that balance is already conserved
bool makeAST(const TokenSequenceType &tokens, Expression &ast) {
	if (tokens.size() > 1) { //case if size is greater than 1
		std::stack<Expression*> currentExp;
		bool foundHead = false, convertAtom = false, firstRun = true, result = true; //parsing flags
		for (unsigned int i = 0; i < tokens.size(); i++) {
			Expression exp;
			Atom atomHolder;
			if (foundHead && tokens[i] != "(" && tokens[i] != ")") { //since allows for checking the next token after the "(" has been found
				if (token_to_atom(tokens[i], atomHolder)) {  //push the item into the stack, signifying that that expression's tail is what we have to access now
					if (firstRun) {//if this is the first head found, set it to the ast's head
						ast.head = atomHolder; //then push AST to the stack. Then set firstHead to false
						currentExp.push(&ast);
						firstRun = false;
					}
					else {//so if a new layer was going to be added in the AST that isn't the first layer. set the new expression's head to the atom that was formed from token_to_atom.
						exp.head = atomHolder; //then we want to connect that to the current layer's tail, so access the tail of the current element
						currentExp.top()->tail.emplace_back(exp); //afterwards, since we have to be on the next layer, push the new expression in the stack to access its tail until ")" is found
						currentExp.push(currentExp.top()->tailPtr());
					}
				}
				else {//conversion of token to atom failed
					result = false;
				}
				foundHead = false; //***RESET the foundHead boolean to false or else it is going to do this with every element***
			}// end foundHead statement
			else if (tokens[i] == "(") {//make new expression and access its tail
				foundHead = true; //make boolean true
			}
			else if (tokens[i] == ")"){ //go the tail's head to access the previous tail
				if (currentExp.empty()) {//do this by popping off the last pushed expression from the stack as the entire expression is closed off to being accessed
					result = false; //empty stack cannot pop
				}
				else {
					currentExp.pop();
					if (currentExp.empty()) {//check if the balance is internally conserved
						if (i + 1 != tokens.size()){
							result = false; //end of token list
						}
					}
				}
			} //end else if ")"
			else {
				if (currentExp.empty()) {//if there is no head to push onto. Then cannot parse the symbols/numbers/booleans into a stack
					result = false; //closed expression but tokens still remain
				}
				if (token_to_atom(tokens[i], atomHolder)) {//if the conversion between a token to an Atom is valid and if this is not right after an "(" token then create a new expression with its head as the atomHolder and then push it back to the top expression's tail in the currentExp stack
					exp.head = atomHolder;
					currentExp.top()->tail.emplace_back(exp);
				}
				else {//if the conversion between a token to an atom is invalid then return false
					result = false; //closed expression but tokens still remain
				}
			}
			if (!result) {
				return result;
			}
		} //end forloop
		return result;
	}// end if
	return false; //if doesn't get to any of the conditions then return false
}

int Interpreter::getGVectorSize()
{
	return graphics.size();
}

void Interpreter::outputG()
{
	for (auto a : graphics)
	{
		std::cout << a << std::endl;
	}
}

std::vector<Atom> Interpreter::getGraphics() const
{
	return graphics;
}