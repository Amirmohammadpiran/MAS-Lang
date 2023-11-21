#ifndef AST_H
#define AST_H


// base Node that contains all the syntax nodes
class Base {

};

// expression that evaluates to a value -> variable_name, boolean, number
class Expr : Base{

};

// condition expression that evaluates to boolean
class CondExpression : Expr {

};

// value expression that evaluates to number
class ValExpresion : Expr {

};

// variable expression that evaluates to variable name
class VarExpression : Expr {

};

// stores information of a statement
class Statement : Base {

};

// if statement
class IfStatement : Statement {

};

// loop statement
class LoopStatement : Statement {

};

// assignment statement. this can have declaration or
// assignment inside it
class AssignStatement : Statement {

};

#endif