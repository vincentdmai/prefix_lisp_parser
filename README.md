# prefix_lisp_parser
Prefix Lisp Parser with GUI Graph Functionality for Special Defined Operations

## Built-With
C++ / QT GUI Framework / CATCH Test Framework
Compiled with CMAKE on Windows

## Valid Operations
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
