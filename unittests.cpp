#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "tokenize.hpp"
#include "expression.hpp"
#include "interpreter.hpp"
#include "environment.hpp"
#include "interpreter_semantic_error.hpp"

#include <iostream>
#include <istream>
#include <string>
#include <tuple>
#include <cmath>
#include <vector>

// TODO: add your unit tests here

TEST_CASE("Test Tokenize")
{
	std::string program = "(begin	(define a 1) \n (define b 2) \n (if (< a b) b a))";
	std::istringstream iss(program);
	TokenSequenceType tokens = tokenize(iss);

	REQUIRE(tokens.size() == 23);
	REQUIRE(tokens[0] == "(");
	REQUIRE(tokens[1] == "begin");
	REQUIRE(tokens[2] == "(");
	REQUIRE(tokens[3] == "define");
	REQUIRE(tokens[4] == "a");
	REQUIRE(tokens[5] == "1");
	REQUIRE(tokens[6] == ")");
	REQUIRE(tokens[7] == "(");
	REQUIRE(tokens[8] == "define");
	REQUIRE(tokens[9] == "b");
	REQUIRE(tokens[10] == "2");
	REQUIRE(tokens[11] == ")");
	REQUIRE(tokens[12] == "(");
	REQUIRE(tokens[13] == "if");
	REQUIRE(tokens[14] == "(");
	REQUIRE(tokens[15] == "<");
	REQUIRE(tokens[16] == "a");
	REQUIRE(tokens[17] == "b");
	REQUIRE(tokens[18] == ")");
	REQUIRE(tokens[19] == "b");
	REQUIRE(tokens[20] == "a");
	REQUIRE(tokens[21] == ")");
	REQUIRE(tokens[22] == ")");

	//clear tokens deque to test the comment operator in vtscripts ";"
	tokens.clear();
	REQUIRE(tokens.empty());

	INFO("Testing ; comment operator of vtscripts");
	program = ";hi this is a comment \n cat";
	std::istringstream iss2(program);
	tokens = tokenize(iss2);
	REQUIRE(tokens.size() == 1);
	REQUIRE(tokens[0] == "cat");

}

TEST_CASE("Test Expressions Constructors")
{
	//Testing the == operator
	INFO("Testing == operator");
	Expression e;
	Expression q;
	Expression a;

	e.head.type = NumberType;
	q.head.type = e.head.type;

	e.head.value.num_value = 100;
	q.head.value.num_value = 100;

	e.tail.push_back(a);
	q.tail.push_back(a);


	REQUIRE(e == q);

	//push back on a second layer expression with another new expression w
	Expression w(true);
	e.tail.push_back(w);

	REQUIRE((e == q) == false);

	Expression r(true);
	q.tail.push_back(r);
	REQUIRE(e == q);

	//clear q's tail so it doesn't match
	q.tail.clear();
	REQUIRE((e == q) == false);


	INFO("Testing Constructors");

	Expression g(true);
	REQUIRE(g.head.type == BooleanType);
	REQUIRE(g.head.value.bool_value == true);

	double d = 100;
	Expression f(d);
	REQUIRE(f.head.type == NumberType);
	REQUIRE(f.head.value.num_value == 100);

	string strHolder = "hello";
	Expression s(strHolder);
	REQUIRE(s.head.type == SymbolType);
	REQUIRE(s.head.value.sym_value == "hello");

	Expression n;
	REQUIRE(n.head.type == NoneType);
}

TEST_CASE("Testing token to atom") {

	INFO("Testing token_to_atom functions");
	Atom a;

	std::string token = "True";
	REQUIRE(token_to_atom(token, a));
	REQUIRE(a.type == BooleanType);
	REQUIRE(a.value.bool_value == true);

	token = "False";
	REQUIRE(token_to_atom(token, a));
	REQUIRE(a.type == BooleanType);
	REQUIRE(a.value.bool_value == false);

	token = "10934.092";
	REQUIRE(token_to_atom(token, a));
	REQUIRE(a.type == NumberType);
	REQUIRE(a.value.num_value == 10934.092);

	token = "1e4";
	REQUIRE(token_to_atom(token, a));
	REQUIRE(a.type == NumberType);
	REQUIRE(a.value.num_value == 1e4);

	token = "-12dlf";
	REQUIRE(!token_to_atom(token, a));

	token = "1abc";
	REQUIRE(!token_to_atom(token, a));

	token = "var1";
	REQUIRE(token_to_atom(token, a));
	REQUIRE(a.type == SymbolType);
	REQUIRE(a.value.sym_value == token);
}

TEST_CASE("Testing Atom Equality")
{
	//Using operator= to set atoms to each other
	INFO("Testing Operator= on two Atoms");

	Atom atom1;
	Atom atom2;

	//testing symbols
	atom1.type = SymbolType;
	atom1.value.sym_value = std::string("hello");

	REQUIRE(atom1.type != atom2.type);

	atom2 = atom1;
	REQUIRE(atom2.type == atom1.type);
	REQUIRE(atom2.value.sym_value == atom1.value.sym_value);
	REQUIRE(atom2 == atom1);

	//testing numbers
	atom1.type = NumberType;
	atom1.value.num_value = 10;
	atom2 = atom1;
	REQUIRE(atom2.type == atom1.type);
	REQUIRE(atom2.value.num_value == atom1.value.num_value);
	REQUIRE(atom2 == atom1);

	//testing booleans
	atom1.type = BooleanType;
	atom1.value.bool_value = true;
	atom2 = atom1;
	REQUIRE(atom2.type == atom1.type);
	REQUIRE(atom2.value.bool_value == atom1.value.bool_value);
	REQUIRE(atom2 == atom1);
}

TEST_CASE("Testing Parse Method, [interpreter]")
{
	std::string program = "(begin (define r 10) (* pi (* r r)))";

	std::istringstream iss(program);

	Interpreter interp;

	bool ok = interp.parse(iss);

	REQUIRE(ok == true);


	INFO("Testing makeAST method and check balance");

	std::string program2 = "(+ 1 2)";
	std::istringstream iss2(program2);

	Expression e;
	TokenSequenceType tokens = tokenize(iss2);
	REQUIRE(checkBalance(tokens) == 1);
	REQUIRE(makeAST(tokens, e) == 1);
	REQUIRE(e.head.value.sym_value == "+");
	REQUIRE(e.tail.empty() == 0);
	REQUIRE(e.tail[0].head.value.num_value == 1);


	//check invalidity
	std::string program3 = ")((begin (- 3 2))";
	std::istringstream iss3(program3);
	std::istringstream iss3_copy(program3);

	TokenSequenceType bad_tokens = tokenize(iss3);

	REQUIRE(checkBalance(bad_tokens) == 0);
	REQUIRE(interp.parse(iss3_copy) == 0);
}

TEST_CASE("Test Interpreter parser with numerical literals of different types", "[interpreter]")
{
	//testing double, positive declarative, floats, and negatives
	std::vector<std::string> programs = { "(1.34983)", "(+1)", "(+1e+4)", "(1e-0)" , "(-1)" };

	for (auto program : programs) {
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);

		REQUIRE(ok == true);
	}

	{
		std::string programName = "1";
		std::istringstream iss2(programName);
		Interpreter interp2;
		bool ok = interp2.parse(iss2);
		REQUIRE(ok == true);
	}
}

TEST_CASE("Testing Basic Environment")
{
	INFO("Testing Simple Operators");

	//testing Add
	std::vector<Expression> expList;

	expList.push_back(Expression(2.0));
	expList.push_back(Expression(5.0));

	REQUIRE(add(expList) == (7.0));

	//testing multiply
	REQUIRE(mul(expList) == (10.0));

	//testing sub
	REQUIRE(subneg(expList) == (-3.0));

	//testing divide
	REQUIRE(divide(expList) == (.4));

	//testing neg
	expList.pop_back();
	REQUIRE(subneg(expList) == -2.0);

	//testing invalids
	expList.push_back(Expression(std::string("hello")));

	REQUIRE_THROWS_AS(add(expList), InterpreterSemanticError);

	expList.pop_back();
	expList.push_back(Expression(10.0));
	expList.push_back(Expression(1.0));

	REQUIRE_THROWS_AS(subneg(expList), InterpreterSemanticError);
	REQUIRE_THROWS_AS(divide(expList), InterpreterSemanticError);

}

//METHOD FOR TESTING EVALUATION*********************************
Expression runProgram(const std::string & program)
{
	std::istringstream iss(program);

	Interpreter interp;

	bool ok = interp.parse(iss);
	if (!ok) {
		std::cerr << "Failed to parse: " << program << std::endl;
	}
	REQUIRE(ok == true);

	Expression result;
	REQUIRE_NOTHROW(result = interp.eval());

	return result;
}
//****************************************************************
TEST_CASE("Testing Evaluate Special Cases")
{
	{
		//testing constant expression PI
		std::string program = "(pi)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(atan2(0, -1)));
	}

	INFO("Testing Special Form: if - with only numeral argument"); //-----------------------------------------------------------
	{
		std::string program = "(if True (4) (-4))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(4.));
	}

	{
		std::string program = "(if False (1) (-4e4))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(-4e4));
	}

	//using if with defined symbols with True
	{
		std::string program = "(begin (define (a) (42)) (if True (a) (1)))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(42.));
	}
	//using if with defined symbols with False
	{
		std::string program = "(begin (define (a) (42)) (if False (1) (a)))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(42.));
	}
	//using if with defined symbols for both arguments
	{
		std::string program = "(begin (define (a) (42)) (define b 9)(if True (b) (a)))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(9.));
	}
	//using if with boolean arguments
	{
		std::string program = "(if False (True) (False))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(false));
	}
}
TEST_CASE("Testing Invalids of Begin Cases")
{
	//using if with invalid symbols and arguments
	{
		std::string program = "(begin (define (a) (42)) (if True (b) (a)))";
		std::istringstream iss(program);
		Interpreter interp;
		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);
		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//invalid size arugment list
	{
		std::string program = "(begin (define (a) (42)) (if True (b) (a) (3)))";
		std::istringstream iss(program);
		Interpreter interp;
		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);
		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//invalid boolean
	{
		std::string program = "(begin (define (a) (42)) (if T (b) (a)))";
		std::istringstream iss(program);
		Interpreter interp;
		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);
		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//invalid begin
	{
		std::string program = "(begin)";
		std::istringstream iss(program);
		Interpreter interp;
		bool ok = interp.parse(iss);

		REQUIRE(ok == true);
		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
}
TEST_CASE("Testing Invalid Define") {
	//---------------------------------------------------------------------------------
	{
		std::string program = "(define answer 42)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(42.));
	}
	INFO("Testing Begin and Define"); //------------------------------------------------------
	{
		std::string program = "(begin (define answer (* 1 2))\n(answer))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(2.));
	}
	{
		std::string program = "(begin (define answer (+ 9 11)) (answer))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(20.));
	}
	{
		std::string program = "(begin (define a 3) (define b 1.1) (+ a b))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(4.1));
	}
	//invalid define usages
	//invalid define by having no argument
	{
		std::string program = "(define)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//invalid define by attempting to define a non-symbol
	{
		std::string program = "(define 3 3)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
}
TEST_CASE("Testing Invalid Define 2"){
	//invalid define with non-binary arguments
	{
		std::string program = "(define a 3 3)";
		std::istringstream iss(program);
		Interpreter interp;
		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);
		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//invalid define with redefinitions
	{
		std::string program = "(define a 3 (define a 5))";
		std::istringstream iss(program);
		Interpreter interp;
		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);
		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	{
		std::string program = "(define define a)";
		std::istringstream iss(program);
		Interpreter interp;
		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);
		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//invalid definition without begin
	{
		std::string program = "(define a 3 (+ a 3))";
		std::istringstream iss(program);
		Interpreter interp;
		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);
		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
}

TEST_CASE("Testing Evaluate with Addition")
{
	INFO("Testing Addition");
	{
		//testing add binary
		std::string program = "(+ 1 5)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(6.));
	}

	{
		//testing add m-nary
		std::string program = "(+ 1 5 9 3 -4)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(14.));
	}

	{
		//testing with defined symbols
		std::string program = "(begin (define a 3) (+ 1 5 9 a -4))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(14.));
	}

	{
		//testing invalid add with non defined argument
		std::string program = "(+ 1 5 a 3 -4)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
}
TEST_CASE("Testin Subtraction and Negation") {
	//-----------------------------------------------------------------------------------------
	INFO("Testing Subtraction");
	{
		//testing negation
		std::string program = "(- 1)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(-1.));
	}
	{
		//testing subtraction
		std::string program = "(- 1 5)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(-4.));
	}
	{
		//testing invalid args
		std::string program = "(- 1 5 3)";
		std::istringstream iss(program);
		Interpreter interp;
		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	{
		//testing invalid types on negation
		std::string program = "(- a)";
		std::istringstream iss(program);
		Interpreter interp;
		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);
		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	{
		//testing invalid types on subtraction
		std::string program = "(- 1 a)";
		std::istringstream iss(program);
		Interpreter interp;
		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);
		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
}
TEST_CASE("Testing Mul")
{
	//-----------------------------------------------------------------
	INFO("Testing Multiplication");
	{
		//testing m-nary multiplication
		std::string program = "(* 1 5 6)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(30.));
	}

	{
		//testing symbol definition
		std::string program = "(begin (define a 13) (* 1 2 a))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(26.));
	}

	{
		//testing invalid multiplication
		std::string program = "(* 1 a)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
}
TEST_CASE("Testing Div") {
	//-----------------------------------------------------------------
	INFO("Testing Division");
	{
		//testing binary division
		std::string program = "(/ 6 3)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(2.));
	}

	{
		//testing invalid args size
		std::string program = "(/ 3 1 0)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	{
		//testing invalid types
		std::string program = "(/ 1 a)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
}
TEST_CASE("Testing Not")
{
	//------------------------------------------------------------------
	INFO("Testing Not Operator");
	{
		std::string program = "(not True)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(false));
	}

	//testing invalid argument count
	{
		std::string program = "(not True False)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}

	//testing invalid argument type
	{
		std::string program = "(not 2)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}

	//testing more complex evaluation
	{
		std::string program = "(begin (define a True) (not a))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(false));
	}
	//-----------------------------------------------------------------------------------
}
TEST_CASE("Testing And") {
	INFO("Testing AND operator");
	//testing unary argument list
	{
		std::string program = "(and True)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(true));
	}
	//testing m-nary argument list
	{
		std::string program = "(and True False True)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(false));
	}
	//testing no arg list
	{
		std::string program = "(and)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//testing invalid types
	{
		std::string program = "(and a)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
}
TEST_CASE("Testing Or")
{
	//-----------------------------------------------------------------------------
	INFO("Testing OR operator");
	//testing unary
	{
		std::string program = "(or True)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(true));
	}
	//testing mnary
	{
		std::string program = "(or False False True)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(true));
	}
	//testing no arg list
	{
		std::string program = "(or)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//testing invalid types
	{
		std::string program = "(or True 3)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
}
TEST_CASE("Testing <") {
	//-----------------------------------------------------------------------------
	INFO("Testing < operator");
	{
		std::string program = "(< 1 2)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(true));
	}

	//testing invalid argument count and types
	{
		std::string program = "(< 1 2 3)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}

	{
		std::string program = "(< 1 True)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//testing complex expression
	{
		std::string program = "(begin (define a 3) (< a 4))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(true));
	}
}
TEST_CASE("Testing <=")
{
	//-------------------------------------------------------------------------------------------------
	INFO("Testing <= operator");
	{
		std::string program = "(<= 1 1)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(true));
	}

	//testing invalid argument count and types
	{
		std::string program = "(<= 1 2 3)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}

	{
		std::string program = "(<= 1 True)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//testing complex expression
	{
		std::string program = "(begin (define a 9) (<= a 4))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(false));
	}
}
TEST_CASE("Testing >=") {
	//------------------------------------------------------------------------------
	INFO("Testing >= operator");
	{
		std::string program = "(>= 1 1)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(true));
	}

	//testing invalid argument count and types
	{
		std::string program = "(>= 1 2 3)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}

	{
		std::string program = "(>= 1 d)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//testing complex expression
	{
		std::string program = "(begin (define a 9) (>= a 4))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(true));
	}
}
TEST_CASE("Testing >")
{
	//--------------------------------------------------------------------------------
	INFO("Testing > operator");
	{
		std::string program = "(> 1 1)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(false));
	}
	//testing invalid argument count and types
	{
		std::string program = "(> 1 2 3)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}

	{
		std::string program = "(> 1 hi)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//testing complex expression
	{
		std::string program = "(begin (define a 3) (> a 4))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(false));
	}
}
TEST_CASE("Testing =") {
	//-------------------------------------------------------------------------------------------
	INFO("Testing = operator");
	{
		std::string program = "(= (1.03) (1.02))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(false));
	}

	//testing invalid argument count and types
	{
		std::string program = "(= 1 2 3)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}

	{
		std::string program = "(= 1 a)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//testing complex expression
	{
		std::string program = "(begin (define a 3) (= a 3))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(true));
	}
}
TEST_CASE("Testing log10")
{
	//-------------------------------------------------------------------------------------
	INFO("Testing log10 operator");
	{
		std::string program = "(log10 1)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(0.));
	}

	{
		std::string program = "(log10 10)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(1.));
	}

	//testing invalid argument count and types
	{
		std::string program = "(log10 1 2)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}

	{
		std::string program = "(log10 a)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//testing complex expression
	{
		std::string program = "(begin (define a 10) (log10 a))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(1.));
	}
}
TEST_CASE("Testing Pow") {
	//------------------------------------------------------------------------------
	INFO("Testing power operator");
	{
		std::string program = "(pow 10 2)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(100.));
	}

	{
		std::string program = "(pow 3 3)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(27.));
	}

	//testing invalid argument count and types
	{
		std::string program = "(pow 3 2 2)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}

	{
		std::string program = "(pow a 3)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
}
TEST_CASE("Complex Exp.") {
	//testing complex expression
	{
		std::string program = "(begin (define a 10) (pow a 2))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(100.));
	}
	//---------------------------------------------------------------------------------
	INFO("Testing Complex Programs");
	{
		std::string program = "(begin(define a 1)(define b 2)(if (< a b) b a))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(2.));
	}
}

TEST_CASE("Other Preliminaries")
{
	//token to atom with ( )
	Atom a;
	std::string token = "(";
	REQUIRE(!token_to_atom(token, a));
	token = ")";
	REQUIRE(!token_to_atom(token, a));
	Expression fail;
	TokenSequenceType t;
	REQUIRE(!makeAST(t, fail));
	t.push_back(token);
	REQUIRE(!makeAST(t, fail));
	//testing invalid equality test for atoms
	a.type = NumberType;
	Atom b;
	b.type = SymbolType;
	REQUIRE(!(a == b));
	//testing invalid equality test for expressions
	Expression exp1(10.);
	Expression exp2(1.);
	REQUIRE(exp1 != exp2);
	//no argument list
	Environment e;
	std::vector<Atom> graphics;
	REQUIRE_THROWS_AS(exp1.handle_begin(e, graphics), InterpreterSemanticError);
	std::vector<Expression> expList;
	expList.push_back(Expression(1.));
	REQUIRE_THROWS_AS(apply(a, expList, e), InterpreterSemanticError);
	a.type = SymbolType;
	a.value.sym_value = "a";
	REQUIRE_THROWS_AS(apply(a, expList, e), InterpreterSemanticError);
	//testing default proc
	REQUIRE(default_proc(expList) == Expression());
	//testing is_proc
	a.type = NumberType;
	REQUIRE(!e.is_proc(a));
	REQUIRE(!e.is_exp(a));
	REQUIRE_THROWS_AS(e.add_exp(a, exp1), InterpreterSemanticError);
	//additional invalid testing for operators
	REQUIRE_THROWS_AS(and_operator(expList), InterpreterSemanticError);
	expList.clear();
	REQUIRE_THROWS_AS(and_operator(expList), InterpreterSemanticError);
	REQUIRE_THROWS_AS(or_operator(expList), InterpreterSemanticError);
	expList.push_back(Expression(std::string("a")));
	expList.push_back(Expression(1.));
	REQUIRE_THROWS_AS(lessEqual_operator(expList), InterpreterSemanticError);
	REQUIRE_THROWS_AS(greaterEqual_operator(expList), InterpreterSemanticError);
	REQUIRE_THROWS_AS(less_operator(expList), InterpreterSemanticError);
	REQUIRE_THROWS_AS(greater_operator(expList), InterpreterSemanticError);
	expList.clear();
	expList.push_back(Expression(true));
	REQUIRE_THROWS_AS(log10_operator(expList), InterpreterSemanticError);
	expList.push_back(Expression(1.));
	REQUIRE_THROWS_AS(pow_operator(expList), InterpreterSemanticError);
}

TEST_CASE("Testing Point Type")
{
	std::tuple<double, double> p1;
	p1 = std::make_tuple(10.0, 20.0);

	std::tuple<double, double> p2;
	p2 = std::make_tuple(10.0, 20.0);

	Expression point1 = Expression(p1);
	Expression point2 = Expression(p2);

	REQUIRE(point1.head.type == PointType);
	REQUIRE(point2.head.type == PointType);

	REQUIRE(point1.head.value.point_value.x == 10.0);
	REQUIRE(point1.head.value.point_value.y == 20.0);

	REQUIRE(point1 == point2);

	//redefine p1
	std::get<0>(p1) = 20.0;

	point1 = Expression(p1);

	REQUIRE(point1 != point2);
}

TEST_CASE("Testing Line Type")
{
	std::tuple<double, double> p1;
	p1 = std::make_tuple(0.0, 15.0);

	std::tuple<double, double> p2;
	p2 = std::make_tuple(10.0, 20.0);

	Expression point1 = Expression(p1);
	Expression point2 = Expression(p2);

	Expression line1 = Expression(p1, p2); //make line

	REQUIRE(line1.head.type == LineType);
	REQUIRE(line1.head.value.line_value.first == point1.head.value.point_value);
	REQUIRE(line1.head.value.line_value.second == point2.head.value.point_value);

	std::tuple<double, double> p3;
	p3 = std::make_tuple(0.0, 14.0);

	std::tuple<double, double> p4;
	p4 = std::make_tuple(10.0, 20.0);

	Expression line2 = Expression(p3, p4);

	REQUIRE(line1 != line2);

	std::get<1>(p3) = 15.0;
	line2 = Expression(p3, p4);
	REQUIRE(line1 == line2);
}

TEST_CASE("Testing Arc Type")
{
	//testing arc type definitions
	std::tuple<double, double> c1;
	c1 = std::make_tuple(0.0, 15.0);

	std::tuple<double, double> s1;
	s1 = std::make_tuple(10.0, 20.0);

	Expression center1 = Expression(c1);
	Expression start1 = Expression(s1);

	Expression arc1 = Expression(c1, s1, 10.0);

	REQUIRE(arc1.head.type == ArcType);
	REQUIRE(arc1.head.value.arc_value.center == center1.head.value.point_value);
	REQUIRE(arc1.head.value.arc_value.start == start1.head.value.point_value);
	REQUIRE(arc1.head.value.arc_value.span == 10.0);

	//testing arc type comparison
	std::tuple<double, double> c2;
	c2 = std::make_tuple(0.0, 15.0);

	std::tuple<double, double> s2;
	s2 = std::make_tuple(10.0, 20.0);

	Expression center2 = Expression(c2);
	Expression start2 = Expression(s2);

	Expression arc2 = Expression(c2, s2, 10.0);

	REQUIRE(arc1 == arc2);

	arc2.head.value.arc_value.span = 5.0;
	REQUIRE(arc1 != arc2);
}

TEST_CASE("Testing Sin")
{
	INFO("Testing sin operator");
	{
		std::string program = "(sin pi)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(0.));
	}

	//testing invalid argument count
	{
		std::string program = "(sin pi 0)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}

	//testing invalid argument type
	{
		std::string program = "(sin True)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
}
TEST_CASE("Testing Cos")
{
	//---------------------------------------------------------------------------
	INFO("Testing Cos Operator")
	{
		std::string program = "(cos pi)";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(-1.));
	}

	{
		std::string program = "(cos (/ pi 2))";
		Expression result = runProgram(program);
		REQUIRE(result == Expression(0.));
	}

	//testing invalid argument count
	{
		std::string program = "(cos pi a)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}

	//testing invalid argument type
	{
		std::string program = "(cos a)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//---------------------------------------------------------------------------------
}
TEST_CASE("Testing ATan2"){
	INFO("Testing atan2")
	{
		std::string program = "(arctan 1 1)";
		Expression result = runProgram(program);
		double compareMe = std::atan2(1.0, 1.0);
		REQUIRE(result == Expression(compareMe));
	}

	//testing invalid argument count
	{
		std::string program = "(arctan pi a)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}

	//testing invalid argument type
	{
		std::string program = "(arctan False)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//-------------------------------------------------------------------------------------------
}

TEST_CASE("Testing Point")
{
	INFO("Testing Point")
	{
		std::string program = "(point 3 1)";
		std::istringstream iss(program);
		Interpreter interp;

		REQUIRE(interp.parse(iss));
		REQUIRE(interp.eval() == Expression(std::make_tuple(3., 1.)));
	}

	//invalid arg type
	{
		std::string program = "(point a 3)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//invalid arg count
	{
		std::string program = "(point 3 4 3)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
}
TEST_CASE("Testing Line") {
	INFO("Testing Line")
	{
		std::string program = "(line (point 0 3) (point 10 0))";
		std::istringstream iss(program);
		Interpreter interp;

		REQUIRE(interp.parse(iss));
		REQUIRE(interp.eval() ==
			Expression(std::make_tuple(0., 3.), std::make_tuple(10., 0.)));
	}
	//invalid type
	{
		std::string program = "(line 0 (point 10 0))";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//invalid arg count
	{
		std::string program = "(line (point 10 0))";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
}
TEST_CASE("Testing Arc") {
	INFO("Testing Arc")
	{
		std::string program = "(arc (point 0 0) (point 10 0) pi)";
		std::istringstream iss(program);
		Interpreter interp;

		REQUIRE(interp.parse(iss));
		REQUIRE(interp.eval() == Expression(std::make_tuple(0., 0.),
			std::make_tuple(10., 0.),
			atan2(0, -1)));
	}

	//invalid type
	{
		std::string program = "(arc (point 3 0) a a)";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}

	//invalid argument count
	{
		std::string program = "(arc (point 3 3))";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);
		
		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
}

TEST_CASE("Testing Graphical Vector Parsing")
{
	std::string program = "(begin (draw (point 3 3)) (cos pi))";
	std::istringstream iss(program);

	Interpreter interp;

	bool ok = interp.parse(iss);
	
	REQUIRE(ok == true);

	Expression result;
	REQUIRE_NOTHROW(result = interp.eval());
	REQUIRE(result == Expression(-1.));
	REQUIRE(interp.getGVectorSize() == 1);
}

TEST_CASE("Testing Invalid Draw")
{
	//invalid type
	{
		std::string program = "(begin (draw (point 3 3) 10))";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);

		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
	//undefined symbol
	{
		std::string program = "(begin (draw (point 3 3) a))";
		std::istringstream iss(program);

		Interpreter interp;

		bool ok = interp.parse(iss);

		REQUIRE(ok == true);

		Expression result;
		REQUIRE_THROWS_AS(result = interp.eval(), InterpreterSemanticError);
	}
}

TEST_CASE("Define with Lookup for Draw")
{
	{
		std::string program = "(begin (define a (point 3 1)) (a))";
		std::istringstream iss(program);
		Interpreter interp;

		REQUIRE(interp.parse(iss));
		REQUIRE(interp.eval() == Expression(std::make_tuple(3., 1.)));
	}
	{
		std::string program = "(begin (define a (line (point 0 3) (point 10 0))) (a))";
		std::istringstream iss(program);
		Interpreter interp;

		REQUIRE(interp.parse(iss));
		REQUIRE(interp.eval() ==
		Expression(std::make_tuple(0., 3.), std::make_tuple(10., 0.)));
	}
	{
		std::string program = "(begin (define a (arc (point 0 0) (point 10 0) pi)) (a))";
		std::istringstream iss(program);
		Interpreter interp;

		REQUIRE(interp.parse(iss));
		REQUIRE(interp.eval() == Expression(std::make_tuple(0., 0.),
			std::make_tuple(10., 0.),
			atan2(0, -1)));
	}
}

TEST_CASE("Testing Draw with Define")
{
	std::string program = "(begin (define a (point 3 1)) (draw (a)))";
	std::istringstream iss(program);
	Interpreter interp;

	REQUIRE(interp.parse(iss));
	REQUIRE(interp.eval() == Expression(std::make_tuple(3., 1.)));
}

TEST_CASE("Define with Draw Types")
{
	{
		std::string program = "(define a (point 3 1))";
		std::istringstream iss(program);
		Interpreter interp;

		REQUIRE(interp.parse(iss));
		REQUIRE(interp.eval() == Expression(std::make_tuple(3., 1.)));
	}

	{
		std::string program = "(define a (line (point 0 3) (point 10 0)))";
		std::istringstream iss(program);
		Interpreter interp;

		REQUIRE(interp.parse(iss));
		REQUIRE(interp.eval() ==
		Expression(std::make_tuple(0., 3.), std::make_tuple(10., 0.)));
	}

	{
		std::string program = "(define b (arc (point 0 0) (point 10 0) pi))";
		std::istringstream iss(program);
		Interpreter interp;

		REQUIRE(interp.parse(iss));
		REQUIRE(interp.eval() == Expression(std::make_tuple(0., 0.),
		std::make_tuple(10., 0.),
		atan2(0, -1)));
	}
}