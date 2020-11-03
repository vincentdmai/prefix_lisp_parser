#include "expression.hpp"

// system includes
#include <sstream>
#include <cmath>
#include <limits>
#include <cctype>
#include <tuple>

#include "environment.hpp"


//Point constructor
Expression::Expression(std::tuple<double, double> value) {
	// TODO: implement this function
	head.type = PointType;
	head.value.point_value.x = std::get<0>(value);
	head.value.point_value.y = std::get<1>(value);
}

Expression::Expression(std::tuple<double, double> start,
	std::tuple<double, double> end) {
	// TODO: implement this function
	Expression p1 = Expression(start);
	Expression p2 = Expression(end);

	head.type = LineType;
	head.value.line_value.first = p1.head.value.point_value;
	head.value.line_value.second = p2.head.value.point_value;
}


Expression::Expression(std::tuple<double, double> center,
	std::tuple<double, double> start,
	double angle) {
	// TODO: implement this function
	Expression c1 = Expression(center);
	Expression p1 = Expression(start);

	head.type = ArcType; //set type
	head.value.arc_value.center = c1.head.value.point_value; //set center
	head.value.arc_value.start = p1.head.value.point_value; //set start
	head.value.arc_value.span = angle; //set span
}


Expression::Expression(bool tf) {
	// TODO: implement this function
	head.type = BooleanType;
	head.value.bool_value = tf;
}

Expression::Expression(double num) {
	// TODO: implement this function
	head.type = NumberType;
	head.value.num_value = num;
}

Expression::Expression(const std::string & sym) {
	// TODO: implement this function
	head.type = SymbolType;
	head.value.sym_value = sym;
}


bool Expression:: operator==(const Expression & exp) const noexcept {
	// TODO: implement this function
	bool result = (head == exp.head);

	result = result && (tail.size() == exp.tail.size());

	if (result)
	{
		for (auto lefte = tail.begin(), righte = exp.tail.begin();
			(lefte != tail.end()) && (righte != exp.tail.end());
			++lefte, ++righte)
		{
			result = result && (*lefte == *righte);
		}
	}

	return result;
}


//format atom output for expression output
std::ostream & operator <<(std::ostream & out, const Atom & atom)
{
	if (atom.type == NumberType)
	{
		out << atom.value.num_value;
	}
	else if (atom.type == BooleanType)
	{
		if (atom.value.bool_value)
		{
			out << std::string("True");
		}
		else
		{
			out << std::string("False");
		}
	}
	else if (atom.type == SymbolType)
	{
		out << std::string(atom.value.sym_value);
	}
	else if (atom.type == PointType)
	{
		out << atom.value.point_value.x << "," << atom.value.point_value.y;
	}
	else if (atom.type == LineType)
	{
		out << "(" << atom.value.line_value.first.x << "," << atom.value.line_value.first.y << "),(" << 
			atom.value.line_value.second.x << "," << atom.value.line_value.second.y << ")";
	}
	else if (atom.type == ArcType)
	{
		out << "(" << atom.value.arc_value.center.x << "," << atom.value.arc_value.center.y << "),(" <<
			atom.value.arc_value.start.x << "," << atom.value.arc_value.start.y << ") " << atom.value.arc_value.span;
	}
	else
	{
		out << "";
	}

	return out;
}


//format expression output
std::ostream & operator<<(std::ostream & out, const Expression & exp) {
	// TODO: implement this function

	if (exp.head.type != NoneType) //check if the head of the expression is not NoneType
	{
		out << "(";

		//implemented operator << for atom
		out << exp.head;

		for (auto e = exp.tail.cbegin(); e != exp.tail.cend(); ++e)
		{
			out << *e;
		}
		out << ")";
	}

	return out;
}

//reference: obtained from https://stackoverflow.com/questions/29169153/how-do-i-verify-a-string-is-valid-double-even-if-it-has-a-point-in-it
bool is_numeric(std::string token)
{
	double d;
	return((std::istringstream(token) >> d >> std::ws).eof());
}

//conversion of tokenized object (a string) to an atom type
bool token_to_atom(const std::string &token, Atom & atom) {
	// TODO: implement this function
	// return true if it a token is valid. otherwise, return false.
	  //a double can be 1.4 or 1 or 1e4
	  //boolean return flag
	bool result = true;
	if (token == "(" || token == ")")
	{
		result = false;
	}
	else if (is_numeric(token) && !(token.size() == 1 && (token[0] == '+' || token[0] == '-')))
	{
		//error if token is only + or only - when parsing string to double, so check if the character of the token is 1 and if they are '+' or '-'

		//if token is numeric then set it to a numeric atom
		// return true;
		atom.value.num_value = std::stod(token);
		atom.type = NumberType;
	}
	else if (token.size() > 1 && ((token[0] == '-') || (std::isdigit(token[0]) != 0 || token[0] == '+'))) //check if the first chars are -# or #
	{
		//token contains a numeric value as a first value 1abc or -1abc
		result = false;
	}
	else if (token == "True" || token == "False" || token == "true" || token == "false")
	{
		atom.type = BooleanType;
		if (token == "True" || token == "true")
		{
			atom.value.bool_value = true;
		}
		else
		{
			atom.value.bool_value = false;
		}
	}
	else
	{
		//convert to symbol
		atom.type = SymbolType;
		atom.value.sym_value = std::string(token);
	}

	return result;
}


//method to compare whether two atoms with the == operator are the same or not
bool operator== (const Atom &a, const Atom &b)
{
	if (a.type == NoneType)
	{
		return (a.type == b.type);
	}

	if (a.type == NumberType) //NOTE: test comparison with double/floating point numbers for std::sin and std::cos and std::atan2
	{
		return (a.type == b.type && (std::fabs(a.value.num_value - b.value.num_value) < std::numeric_limits<double>::epsilon()));
	}

	if (a.type == SymbolType)
	{
		return (a.type == b.type && a.value.sym_value == b.value.sym_value);
	}

	if (a.type == BooleanType)
	{
		return (a.type == b.type && a.value.bool_value == b.value.bool_value);
	}

	if (a.type == PointType)
	{
		return (a.type == b.type && a.value.point_value == b.value.point_value);
	}

	if (a.type == LineType)
	{
		return (a.type == b.type && a.value.line_value.first == b.value.line_value.first && a.value.line_value.second == b.value.line_value.second);
	}

	if (a.type == ArcType)
	{
		return (a.type == b.type && a.value.arc_value.center == b.value.arc_value.center && a.value.arc_value.start == b.value.arc_value.start
			&& a.value.arc_value.span == b.value.arc_value.span);
	}
	return false;
}

//method to compare if expressions are different
bool operator!=(const Expression & left, const Expression & right) noexcept {

	return !(left == right);
}

//method to return pointer of the tail
Expression *Expression::tailPtr()
{
	Expression * ptr = nullptr;

	if (!(this->tail.empty()))
	{
		ptr = &this->tail.back();
	}
	return ptr;
}


Expression Expression::handle_lookup(const Atom & headAtom, const Environment & env) {
	if (headAtom.type == SymbolType) { // if symbol is in env return value
		if (env.is_exp(headAtom))
		{
			return env.get_exp(headAtom);
		}
		throw InterpreterSemanticError("Error during evaluation: unknown symbol");
	}

	if (headAtom.type == NumberType) {
		return Expression(headAtom.value.num_value);
	}
	if (headAtom.type == BooleanType)
	{
		return Expression(headAtom.value.bool_value);
	}
	if (headAtom.type == PointType)
	{
		return Expression(std::make_tuple(headAtom.value.point_value.x, headAtom.value.point_value.y));
	}
	if (headAtom.type == LineType)
	{
		std::tuple<double, double> p1 = std::make_tuple(headAtom.value.line_value.first.x, headAtom.value.line_value.first.y);
		std::tuple<double, double> p2 = std::make_tuple(headAtom.value.line_value.second.x, headAtom.value.line_value.second.y);
		return Expression(p1, p2);
	}
	if (headAtom.type == ArcType)
	{
		std::tuple<double, double> c1 = std::make_tuple(headAtom.value.arc_value.center.x, headAtom.value.arc_value.center.y);
		std::tuple<double, double> s1 = std::make_tuple(headAtom.value.arc_value.start.x, headAtom.value.arc_value.start.y);
		return Expression(c1, s1, headAtom.value.arc_value.span);
	}
	//if headtype is none of the above throw error
	throw InterpreterSemanticError("Error during evaluation: Invalid type in terminal expression");
}

Expression Expression::handle_begin(Environment & env, std::vector<Atom> &graphical) {

	if (tail.empty()) {
		throw InterpreterSemanticError("Error during evaluation: zero arguments to begin");
	}

	// evaluate each arg from tail, return the last
	Expression result;
	for (Expression::IteratorType it = tail.begin(); it != tail.end(); ++it) {
		result = it->evaluate(env, graphical);
	}

	return result;
}


Expression Expression::handle_define(Environment & env, std::vector<Atom> &graphical) {

	// tail must have size 3 or error
	if (tail.size() != 2) {
		throw InterpreterSemanticError("Error during evaluation: invalid number of arguments to define");
	}

	// tail[0] must be symbol
	if (!(tail[0].head.type == SymbolType)) {
		throw InterpreterSemanticError("Error during evaluation: first argument to define not symbol");
	}

	// but tail[0] must not be a special-form or procedure
	std::string s = tail[0].head.value.sym_value;
	if ((s == "define") || (s == "begin") || (s == "draw")) {
		throw InterpreterSemanticError("Error during evaluation: attempt to redefine a special-form");
	}

	if (env.is_proc(head)) {
		throw InterpreterSemanticError("Error during evaluation: attempt to redefine a built-in procedure");
	}

	// eval tail[1]
	Expression result = tail[1].evaluate(env, graphical);

	if (env.is_exp(head)) {
		throw InterpreterSemanticError("Error during evaluation: attempt to redefine a previously defined symbol");
	}

	//and add to env
	env.add_exp(tail[0].head, result);

	return result;
}


Expression Expression::handle_draw(Environment & env, std::vector<Atom> &graphical)
{
	Expression exp;
	//iterate through the draw's expression tail. 
	for (auto a : tail)
	{
		if ((a.head.value.sym_value == "point" || a.head.value.sym_value == "arc" || a.head.value.sym_value == "line" )&& a.head.type == SymbolType)
		{
			exp = a.evaluate(env, graphical);
			//TODO: convert exp to atom and push it into graphics vector
			graphical.push_back(exp.head);
			
		}
		else if (a.head.type == SymbolType && env.is_exp(a.head)) //if a defined symbol in the environment
		{
			Expression result = env.get_exp(a.head);
			if (result.head.type == PointType || result.head.type == LineType || result.head.type == ArcType)
			{
				exp = result;
				graphical.push_back(result.head);
			}
			else
			{
				throw InterpreterSemanticError("Error during evaluation: parsing for special case: draw, symbol not defined as graphic type");
			}
		}
		else
		{
			throw InterpreterSemanticError("Error during evaluation: parsing for special case: draw, but invalid graphical types");
		}
		
	}
	return exp;
}


// this is a simple recursive version. the iterative version is more
// difficult with the ast data structure used (no parent pointer).
// this limits the practical depth of our AST
Expression Expression::evaluate(Environment & env, std::vector<Atom> &graphical)
{
	if (tail.empty())
	{
		return handle_lookup(head, env);
	}
	if (head.type == SymbolType && head.value.sym_value == "begin") // handle begin special-form
	{
		return handle_begin(env, graphical);
	}
	if (head.type == SymbolType && head.value.sym_value == "define") // handle define special-form
	{
		return handle_define(env, graphical);
	}
	if (head.type == SymbolType && head.value.sym_value == "draw")
	{
		return handle_draw(env, graphical);
	}

	// else attempt to treat as procedure
	std::vector<Expression> results;
	for (Expression::IteratorType it = tail.begin(); it != tail.end(); ++it)
	{
		results.push_back(it->evaluate(env, graphical));
	}
	return apply(head, results, env);
}

Expression apply(const Atom & op, const std::vector<Expression> & args, const Environment & env) {

	// head must be a symbol
	if (!(op.type == SymbolType)) {
		throw InterpreterSemanticError("Error during evaluation: procedure name not symbol");
	}

	// must map to a proc
	if (!env.is_proc(op)) {
		throw InterpreterSemanticError("Error during evaluation: symbol does not name a procedure");
	}

	// map from symbol to proc
	Procedure proc = env.get_proc(op);

	// call proc with args
	return proc(args);
}

