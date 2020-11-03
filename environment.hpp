#ifndef ENVIRONMENT_HPP_
#define ENVIRONMENT_HPP_

// system includes
#include <map>
#include <cmath>

// module includes
#include "expression.hpp"
#include "interpreter_semantic_error.hpp"


class Environment {
public:
	//The constructor
	Environment();

	//call reset in constructor to clear map and re-add all the symbol definition
	void reset();

	//check if expression type
	bool is_exp(const Atom & sym) const;

	//uses symbol to obtain the expression and return it
	Expression get_exp(const Atom & sym) const;

	//bassically adds another symbol, with an expression to the map, if it doesn't already exist
	void add_exp(const Atom & sym, const Expression & exp);

	//check if procedure type
	bool is_proc(const Atom & sym) const;

	//obtaining the procedure if found, and returns it
	Procedure get_proc(const Atom & sym) const;




private:

	// Environment is a mapping from symbols to expressions or procedures
	enum EnvResultType { ExpressionType, ProcedureType };
	struct EnvResult {

		EnvResultType type;
		Expression exp;
		Procedure proc;

		// constructors for use in container emplace
		EnvResult() {};
		EnvResult(EnvResultType t, Expression e) : type(t), exp(e) {};
		EnvResult(EnvResultType t, Procedure p) : type(t), proc(p) {};
	};



	std::map<Symbol, EnvResult> envmap;
};

Expression default_proc(const std::vector<Expression> & args);

//Expression calculation for add, the "+" symbol. M-nary argument operator
Expression add(const std::vector<Expression> & args);

//Expression calculation for multiply, the "*" symbol. M-nary argument operator
Expression mul(const std::vector<Expression> & args);

//Expression calculation for subtraction or negation, the "-" symbol. Unary or Binary argument operator
Expression subneg(const std::vector<Expression> & args);

//Expression calculation for division, the "/" symbol. Binary argument operator
Expression divide(const std::vector<Expression> & args);

//Expression for defintion of PI, "pi" symbol
const double PI = std::atan2(0, -1);

//Expression calculation for NOT. Argument is unary boolean. Negate the bool value.
Expression not_operator(const std::vector<Expression> & args);

//Expression calculation for AND. Arguments are m-nary booleans.
Expression and_operator(const std::vector<Expression> & args);

//Expression calculation for OR. Arguments are m-nary booleans.
Expression or_operator(const std::vector<Expression> & args);

//Expression calculation for <. Arguments are binary numbers.
Expression less_operator(const std::vector<Expression> & args);

//Expression calculation for <=. Arguments are binary numbers.
Expression lessEqual_operator(const std::vector<Expression> & args);

//Expression calculation for >. Arguments are binary numbers.
Expression greater_operator(const std::vector<Expression> & args);

//Expression calculation for >=. Arguments are binary numbers.
Expression greaterEqual_operator(const std::vector<Expression> & args);

//Expression calculation for =. Arguments are binary numbers.
Expression equal_operator(const std::vector<Expression> & args);

//Expression calculation for log10 unary expression of Numbers, computes the common (base-10) logarithm of the argument.
Expression log10_operator(const std::vector<Expression> & args);

//Expression calculation for pow, binary expression of Numbers, base and exponent, computes the result of raising base to the power exponent.
Expression pow_operator(const std::vector<Expression> & args);

//if handler
Expression handle_if(const std::vector<Expression> & args);

//sin/cos operator -> Unary. While atan operator is binary atan(y/x). Units are in radians
Expression sin_operator(const std::vector<Expression> & args);

Expression cos_operator(const std::vector<Expression> & args);

Expression atan2_operator(const std::vector<Expression> & args);


//Create the graphics operator to create the respective expressions: point, line, arc

//Binary operator of numbers
Expression point_operator(const std::vector<Expression> & args);

//binary operator of point type
Expression line_operator(const std::vector<Expression> & args);

//binary operator of point type with another single argument of number
Expression arc_operator(const std::vector<Expression> & args);




#endif
