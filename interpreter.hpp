#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

// system includes
#include <string>
#include <istream>
#include <vector>
#include <cctype>


// module includes
#include "expression.hpp"
#include "environment.hpp"
#include "tokenize.hpp"

// Interpreter has
// Environment, which starts at a default
// parse method, builds an internal AST
// eval method, updates Environment, returns last result
class Interpreter{
public:
  bool parse(std::istream & expression) noexcept;
  Expression eval();

  //testing functions to see if parsed vector correctly
  int getGVectorSize();
  void outputG();
  std::vector<Atom> getGraphics() const;

protected:
  Environment envir;
  Expression ast;
  std::vector<Atom> graphics;


};

// convenience typedef
typedef std::vector<Expression>::iterator IteratorType;

bool checkBalance(TokenSequenceType tokenHolder); //check balance of the ( )
bool makeAST(const TokenSequenceType & tokens, Expression &ast); //process that takes in the Token deque and adds it into an AST by using a stack





#endif
