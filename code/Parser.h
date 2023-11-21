#ifndef _PARSER_H
#define _PARSER_H

#include "AST.h"
#include "Lexer.h"
#include "llvm/Support/raw_ostream.h"



class Parser {
    Lexer& Lex;  
    Token Tok;    
    bool HasError; 

    void error()
    {
        llvm::errs() << "Unexpected: " << Tok.getText() << "\n";
        HasError = true;
    }


    void advance() { Lex.next(Tok); }

    bool expect(Token::TokenKind Kind)
    {
        if (Tok.getKind() != Kind)
        {
            error();
            return true;
        }
        return false;
    }

    bool consume(Token::TokenKind Kind)
    {
        if (expect(Kind))
            return true;
        advance();
        return false;
    }

	// parsing functions according to the regex
	// pattern specified. each one produces its own node
	// one node can have multiple subnodes inside it

	Base* parseS();
	AssignStatement* parseDefine();
	ValExpression* parseLineSPC();
	Expr* parseExpr();
	ValExpression* parseTerm();
	ValExpression* parsePower();
	Expr* parseFactor();
	AssignStatement* parseAssign();
	CondExpression* parseCondition();
	Expr* parseSubCondition();
	LoopStatement* parseLoop();
    VarExpression* parseVar();

public:
    // initializes all members and retrieves the first token
    Parser(Lexer& Lex) : Lex(Lex), HasError(false)
    {
        advance();
    }

    // get the value of error flag
    bool hasError() { return HasError; }

    AST* parse();
};

#endif