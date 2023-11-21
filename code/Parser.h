#ifndef _PARSER_H
#define _PARSER_H

#include "AST.h"



class Parser {

private:

	// parsing functions according to the regex
	// pattern specified. each one produces its own node
	// one node can have multiple subnodes inside it

	Base* parseS();
	AssignStatement* parseDefine();
	ValExpresion* parseLineSPC();
	Expr* parseExpr();
	ValExpresion* parseTerm();
	ValExpresion* parsePower();
	Expr* parseFactor();
	AssignStatement* parseAssign();
	CondExpression* parseCondition();
	Expr* parseSubCondition();
	LoopStatement* parseLoop();

public:
	Parser();
};

#endif