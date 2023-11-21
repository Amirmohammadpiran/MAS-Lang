#ifndef PARSER_H
#define PARSER_H
#include "Parser.h"
#endif




Base* Parser::parseS() 
{
    llvm::SmallVector<Statement*> statements;
    while (!Tok.is(Token::eoi))
    {
        switch (Tok.getKind())
        {
        case Token::KW_int:
            Statement* state = parseAssign();
            if (state)
            {
                statements.push_back(state);
            }
        default:
            goto _error2;
            break;
        }
        advance(); 
    }
    return new Base(statements);
_error2:
    while (Tok.getKind() != Token::eoi)
        advance();
    return nullptr;

}

AssignStatement* Parser::parseDefine()
{

}
ValExpression* Parser::parseLineSPC()
{

}


Expr* Parser::parseExpr()
{
    Expr* Left = parseTerm();
    while (Tok.isOneOf(Token::plus, Token::minus))
    {
        BinaryOp::Operator Op =
            Tok.is(Token::plus) ? BinaryOp::Plus : BinaryOp::Minus;
        advance();
        Expr* Right = parseTerm();
        Left = new BinaryOp(Op, Left, Right);
    }
    return Left;
}


ValExpression* Parser::parseTerm()
{
    Expr* Left = parseFactor();
    ValExpression* top;
    while (Tok.isOneOf(Token::star, Token::slash))
    {
        BinaryOp::Operator Op =
            Tok.is(Token::star) ? BinaryOp::Mul : BinaryOp::Div;
        advance();
        Expr* Right = parseFactor();
        top = new BinaryOp(Op, Left, Right);
    }
    return top;
}

ValExpression* Parser::parsePower()
{

}

Expr* Parser::parseFactor()
{

}


AssignStatement* Parser::parseAssign()
{
    VarExpression* lhand;
    Expr* rhand;

    lhand = parseVar();

    if (!Tok.is(Token::equal))
    {
        error();
        return nullptr;
    }

    advance();
    rhand = parseExpr();
    return new AssignStatement(lhand, rhand, AssignStatement::AssignType::Assignment);
}


VarExpression* Parser::parseVar()
{
    if (!Tok.is(Token::ident))
    {
        error();
        return nullptr;
    }

    VarExpression* variable = new VarExpression(Tok.getText());
    advance();

}

CondExpression* Parser::parseCondition()
{

}


Expr* Parser::parseSubCondition()
{

}

LoopStatement* Parser::parseLoop()
{

}









