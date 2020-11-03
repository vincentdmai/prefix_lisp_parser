#include "environment.hpp"

#include <cassert>
#include <cmath>



Expression default_proc(const std::vector<Expression> & args)
{
	args.size(); // make compiler happy we used this parameter
	return Expression();
}
Environment::Environment()
{
	// TODO: implement your code ...
	reset();
}

void Environment::reset()
{
	//clean the map and re-add all the symbol definition
	envmap.clear();
	// Built-In value of pi
	envmap.emplace("pi", EnvResult(ExpressionType, Expression(PI)));
	// Procedure: add;
	envmap.emplace("+", EnvResult(ProcedureType, add));
	// Procedure: subneg;
	envmap.emplace("-", EnvResult(ProcedureType, subneg));
	// Procedure: mul;
	envmap.emplace("*", EnvResult(ProcedureType, mul));
	// Procedure: divide;
	envmap.emplace("/", EnvResult(ProcedureType, divide));
	//Procedure: Not
	envmap.emplace("not", EnvResult(ProcedureType, not_operator));
	//Procedure: AND
	envmap.emplace("and", EnvResult(ProcedureType, and_operator));
	//Procedure: OR
	envmap.emplace("or", EnvResult(ProcedureType, or_operator));
	//Procedure: <
	envmap.emplace("<", EnvResult(ProcedureType, less_operator));
	//Procedure: <=
	envmap.emplace("<=", EnvResult(ProcedureType, lessEqual_operator));
	//Procedure: >
	envmap.emplace(">", EnvResult(ProcedureType, greater_operator));
	//Procedure: >=
	envmap.emplace(">=", EnvResult(ProcedureType, greaterEqual_operator));
	//Procedure: ==
	envmap.emplace("=", EnvResult(ProcedureType, equal_operator));
	//Procedure: log10
	envmap.emplace("log10", EnvResult(ProcedureType, log10_operator));
	//Procedure: power
	envmap.emplace("pow", EnvResult(ProcedureType, pow_operator));
	//Special Case: if
	envmap.emplace("if", EnvResult(ProcedureType, handle_if));
	//trigonometric functions
	envmap.emplace("sin", EnvResult(ProcedureType, sin_operator));
	envmap.emplace("cos", EnvResult(ProcedureType, cos_operator));
	envmap.emplace("arctan", EnvResult(ProcedureType, atan2_operator));
	//GRAPHICAL functions
	envmap.emplace("arc", EnvResult(ProcedureType, arc_operator));
	envmap.emplace("line", EnvResult(ProcedureType, line_operator));
	envmap.emplace("point", EnvResult(ProcedureType, point_operator));
}

Expression add(const std::vector<Expression> & args) {

	// check all arguments are numbers, while adding
	double result = 0;
	for (auto & a : args) {
		if (a.head.type == NumberType)
		{
			result += a.head.value.num_value;
		}
		else
		{
			throw InterpreterSemanticError("Error in call to add, argument not a number");
		}
	}

	return Expression(result);
}

Expression mul(const std::vector<Expression> & args) {

	// check all aruments are numbers, while multiplying
	double result = 1;
	for (auto & a : args) {
		if (a.head.type == NumberType) {
			result *= a.head.value.num_value;
		}
		else {
			throw InterpreterSemanticError("Error in call to mul, argument not a number");
		}
	}

	return Expression(result);
}

Expression subneg(const std::vector<Expression> & args)
{
	//check if there is one or two arguments
	//if there is one argument, we want to apply the negative of that
	//if there are two then subtract them from each other
	//hence, the function name, subneg
	double result = 0;

	if ((args.size()) == 1) //negative operation
	{
		if (args[0].head.type == NumberType)
		{
			result = -args[0].head.value.num_value;
		}
		else
		{
			throw InterpreterSemanticError("Error in call to subneg, one argument vector contains an atom not of numeric type");
		}
	}
	else if ((args.size()) == 2) //subtraction operation
	{
		if (args[0].head.type == NumberType && args[1].head.type == NumberType)
		{
			result = args[0].head.value.num_value - args[1].head.value.num_value;
		}
		else
		{
			throw InterpreterSemanticError("Error in call to subneg, two argument vector contains an atom not of numeric type");
		}
	}
	else
	{
		throw InterpreterSemanticError("Error in call to subneg, number of arguments invalid. Subneg is either a unary or binary operator");
	}

	return Expression(result);
}

Expression divide(const std::vector<Expression> & args)
{
	/*For this division operator, it is considered as a binary operator and thus can only perform division on two arguments
	/if the number of arguments are strictly NOT 2 OR if any of the atoms in the argument vector are not numbers then throw error
	/else calculate the divsion between the first argument and second argument*/

	double result = 0;

	if ((args.size()) == 2) {
		if ((args[0].head.type == NumberType) && (args[1].head.type == NumberType)) {
			result = args[0].head.value.num_value / args[1].head.value.num_value;
		}
		else {
			throw InterpreterSemanticError("Error in call to division: invalid argument.");
		}
	}
	else {
		throw InterpreterSemanticError("Error in call to division: invalid number of arguments.");
	}
	return Expression(result);
}


//Check if expression type
bool Environment::is_exp(const Atom & sym) const {
	if (!(sym.type == SymbolType))
	{
		return false;
	}
	auto result = envmap.find(sym.value.sym_value);
	return ((result != envmap.end()) && (result->second.type == ExpressionType));
}

//check if procedure type
bool Environment::is_proc(const Atom & sym) const {
	if (!(sym.type == SymbolType))
	{
		return false;
	}
	auto result = envmap.find(sym.value.sym_value);
	return ((result != envmap.end()) && (result->second.type == ProcedureType));
}

void Environment::add_exp(const Atom & sym, const Expression & exp) {

	//must be symbol type to be a key
	if (!(sym.type == SymbolType)) {
		throw InterpreterSemanticError("Error : Attempt to add non-symbol to environment");
	}

	// error if overwriting symbol map. If symbol already exists, then cannot overwrite what is already existing
	if (envmap.find(sym.value.sym_value) != envmap.end()) {
		throw InterpreterSemanticError("Error : Attempt to overwrite symbol in environemnt");
	}

	envmap.emplace(sym.value.sym_value, EnvResult(ExpressionType, exp));
}

Expression Environment::get_exp(const Atom & sym) const
{
	Expression exp;

	//check if key is a symbol type and that there is an expression associated with that key. Else, return an empty expression
	if (sym.type == SymbolType) {
		auto result = envmap.find(sym.value.sym_value);
		if ((result != envmap.end()) && (result->second.type == ExpressionType)) {
			exp = result->second.exp;
		}
	}

	return exp;
}

//returns procedures associated with the key (sym value)
Procedure Environment::get_proc(const Atom & sym) const
{
	if (sym.type == SymbolType)
	{
		auto results = envmap.find(sym.value.sym_value);
		if (results != envmap.end() && results->second.type == ProcedureType)
		{
			return results->second.proc;
		}
	}

	return default_proc;
}


Expression not_operator(const std::vector<Expression> & args)
{
	if (args.size() == 1) //unary operator of only boolean type
	{
		if (args[0].head.type == BooleanType)
		{
			return Expression(!(args[0].head.value.bool_value));
		}

		throw InterpreterSemanticError("Error : Invalid argument type.");
	}
	//else args size is not 1
	throw InterpreterSemanticError("Error : Invalid argument count for unary operator");
}

Expression and_operator(const std::vector<Expression> & args)
{
	if (!args.empty())
	{
		bool result = true;
		for (auto & a : args) {
			if (a.head.type == BooleanType) //has to be boolean type
			{
				result = result && a.head.value.bool_value; //logically AND operator upon it. If there is a false, then result would result false.
			}
			else
			{
				throw InterpreterSemanticError("Error in call to and, argument not booleans");
			}
		}

		return Expression(result);
	}
	//else if args list is empty
	throw InterpreterSemanticError("Error : No Arguments");
	return Expression();
}

Expression or_operator(const std::vector<Expression> & args)
{
	if (!args.empty())
	{
		bool result = false; //initialize variable result to be false so that if OR'd with true then it would remain true.
		for (auto & a : args) {
			if (a.head.type == BooleanType) //has to be boolean type
			{
				result = result || a.head.value.bool_value; //logically OR operator upon it. If there is at least one true, then result would be true.
			}
			else
			{
				throw InterpreterSemanticError("Error :  in call to or, argument not booleans");
			}
		}

		return Expression(result);
	}

	//args list empty
	throw InterpreterSemanticError("Error : No Arguments");
	return Expression();
}


Expression less_operator(const std::vector<Expression> & args)
{
	if (args.size() == 2)
	{
		if (args[0].head.type == NumberType && args[1].head.type == NumberType)
		{
			return (args[0].head.value.num_value < args[1].head.value.num_value ? Expression(true) : Expression(false));
		}
		//else invalid arg type

		throw InterpreterSemanticError("Error : Invalid Argument Types");
	}
	//else invalid number of args
	throw InterpreterSemanticError("Error : Invalid number of arguments");
	return Expression();
}

Expression lessEqual_operator(const std::vector<Expression> & args)
{
	if (args.size() == 2)
	{
		if (args[0].head.type == NumberType && args[1].head.type == NumberType)
		{
			return (args[0].head.value.num_value <= args[1].head.value.num_value ? Expression(true) : Expression(false));
		}
		//else invalid arg type
		throw InterpreterSemanticError("Error : Invalid Argument Types");
	}

	throw InterpreterSemanticError("Error : Invalid number of arguments");
	return Expression();
}

Expression greater_operator(const std::vector<Expression> & args)
{
	if (args.size() == 2)
	{
		if (args[0].head.type == NumberType && args[1].head.type == NumberType)
		{
			return (args[0].head.value.num_value > args[1].head.value.num_value ? Expression(true) : Expression(false));
		}
		//else invalid arg type
		throw InterpreterSemanticError("Error : Invalid Argument Types");
	}
	//invalid arg list size
	throw InterpreterSemanticError("Error : Invalid number of arguments");
	return Expression();
}

Expression greaterEqual_operator(const std::vector<Expression> & args)
{
	if (args.size() == 2)
	{
		if (args[0].head.type == NumberType && args[1].head.type == NumberType)
		{
			return (args[0].head.value.num_value >= args[1].head.value.num_value ? Expression(true) : Expression(false));
		}
		//else invalid arg type
		throw InterpreterSemanticError("Error : Invalid Argument Types");
	}
	//invalid arg list size
	throw InterpreterSemanticError("Error : Invalid number of arguments");
	return Expression();
}

Expression equal_operator(const std::vector<Expression> & args)
{

	if (args.size() == 2)
	{
		if (args[0].head.type == NumberType && args[1].head.type == NumberType)
		{
			return (args[0].head.value.num_value == args[1].head.value.num_value ? Expression(true) : Expression(false));
		}
		//else invalid arg type
		throw InterpreterSemanticError("Error : Invalid Argument Types");
	}
	//invalid arg list size
	throw InterpreterSemanticError("Error : Invalid number of arguments");
	return Expression();
}

Expression log10_operator(const std::vector<Expression> & args)
{
	if (args.size() == 1)
	{
		if (args[0].head.type == NumberType)
		{
			return Expression(std::log10(args[0].head.value.num_value));
		}
		//else invalid arg type
		throw InterpreterSemanticError("Error : Invalid Argument Types");
	}
	//invalid arg list size
	throw InterpreterSemanticError("Error : Invalid number of arguments for unary operator");
	return Expression();
}

Expression pow_operator(const std::vector<Expression> & args)
{
	if (args.size() == 2)
	{
		if (args[0].head.type == NumberType && args[1].head.type == NumberType)
		{
			return Expression(std::pow(args[0].head.value.num_value, args[1].head.value.num_value));
		}
		//else invalid arg type
		throw InterpreterSemanticError("Error : Invalid Argument Types");
	}
	//invalid arg list size
	throw InterpreterSemanticError("Error : Invalid number of arguments for binary operator");
	return Expression();
}

Expression sin_operator(const std::vector<Expression> & args)
{
	if (args.size() == 1)
	{
		if (args[0].head.type == NumberType)
		{
			return Expression(std::sin(args[0].head.value.num_value));
		}
		//else invalid arg type
		throw InterpreterSemanticError("Error : Invalid Argument Types");
	}
	//invalid arg list size
	throw InterpreterSemanticError("Error : Invalid number of arguments for unary operator");
	return Expression();
}

Expression cos_operator(const std::vector<Expression> & args)
{
	if (args.size() == 1)
	{
		if (args[0].head.type == NumberType)
		{
			return Expression(std::cos(args[0].head.value.num_value));
		}
		//else invalid arg type
		throw InterpreterSemanticError("Error : Invalid Argument Types");
	}
	//invalid arg list size
	throw InterpreterSemanticError("Error : Invalid number of arguments for unary operator");
	return Expression();
}

Expression atan2_operator(const std::vector<Expression> &args)
{
	if (args.size() == 2)
	{
		if(args[0].head.type == NumberType && args[1].head.type == NumberType)
		{
			//return arctan of the arguments such that args[0] is the y and args[1] is x
			return Expression(std::atan2(args[0].head.value.num_value, args[1].head.value.num_value));
		}
		//else invalid arg type
		throw InterpreterSemanticError("Error : Invalid Argument Types");
	}
	//invalid arg list size
	throw InterpreterSemanticError("Error : Invalid number of arguments for binary operator");
	return Expression();
}

Expression handle_if(const std::vector<Expression> & args)
{
	/*for this "if" special case operator. There are several error cases.
	/1) if the first argument of the tail is NOT a boolean
	/2) if there are not two other arguments in the vector to evaluate the "if" case.
	/3) inconsitent srgument type list*/

	if (args.size() == 3)
	{
		if (args[0].head.type == BooleanType)
		{
			if ((args[1].head.type == NumberType || args[1].head.type == BooleanType) && (args[2].head.type == NumberType || args[2].head.type == BooleanType))
			{
				Expression first_exp;
				Expression second_exp;
				if (args[1].head.type == BooleanType)
				{
					first_exp = Expression(args[1].head.value.bool_value);
				}
				else
				{
					first_exp = Expression(args[1].head.value.num_value);
				}

				if (args[2].head.type == BooleanType)
				{
					second_exp = Expression(args[2].head.value.bool_value);
				}
				else
				{
					second_exp = Expression(args[2].head.value.num_value);
				}
				return (args[0].head.value.bool_value) ? first_exp : second_exp;
			}
			//invalid arg type list
			throw InterpreterSemanticError("Error during calculation of (if): inconsistent argument types.");
		}
		//invalid arg type for head
		throw InterpreterSemanticError("Error in call to (if): invalid argument type. First argument must be of a BooleanType.");
	}
	//invalid arg count
	throw InterpreterSemanticError("Error in call to (if): invalid number of arguments to begin with.");

	return Expression(); //return empty expression if never enters conditional statements or never throws an error.
}


//GRAPHICS OPERATORS:

Expression point_operator(const std::vector<Expression> & args)
{
	if (args.size() == 2)
	{
		if (args[0].head.type == NumberType && args[1].head.type == NumberType)
		{
			return Expression(std::make_tuple(args[0].head.value.num_value, args[1].head.value.num_value));
		}
		//else invalid arg type
		throw InterpreterSemanticError("Error in Graphics Env: Invalid Argument Types");

	}

	//else invalid arg type
	throw InterpreterSemanticError("Error in Graphics Env : Invalid Argument Count");
	return Expression();
}

Expression line_operator(const std::vector<Expression> & args)
{
	if (args.size() == 2)
	{
		if (args[0].head.type == PointType && args[1].head.type == PointType)
		{
			//reverse point types into tuple form
			std::tuple<double, double> p1 = std::make_tuple(args[0].head.value.point_value.x, args[0].head.value.point_value.y);
			std::tuple<double, double> p2 = std::make_tuple(args[1].head.value.point_value.x, args[1].head.value.point_value.y);

			//return expression with a starting tuple point and an ending tuple point
			return Expression(p1, p2);
		}
		//else invalid arg type
		throw InterpreterSemanticError("Error in Graphics Env : Invalid Argument Types");
	}
	//else invalid arg type
	throw InterpreterSemanticError("Error in Graphics Env : Invalid Argument Count");
	return Expression();
}

//form: center point , start point , span
Expression arc_operator(const std::vector<Expression> & args)
{
	if (args.size() == 3)
	{
		if (args[0].head.type == PointType && args[1].head.type == PointType && args[2].head.type == NumberType)
		{
			std::tuple<double, double> c1 = std::make_tuple(args[0].head.value.point_value.x, args[0].head.value.point_value.y);
			std::tuple<double, double> s1 = std::make_tuple(args[1].head.value.point_value.x, args[1].head.value.point_value.y);
			double span = args[2].head.value.num_value;
			
			return Expression(c1, s1, span);
		}
		//else invalid arg type
		throw InterpreterSemanticError("Error in Graphics Env : Invalid Argument Types");
	}
	//else invalid arg type
	throw InterpreterSemanticError("Error in Graphics Env : Invalid Argument Count");
	return Expression();
}