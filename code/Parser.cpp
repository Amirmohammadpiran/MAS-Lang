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
        case Token::ident:
        {
            Statement* state = parseAssign();
            if (state)
            {
                statements.push_back(state);
            }
            break;
        }
        case Token::KW_int:
        {
            Statement* state = parseDefine();
            if (state)
            {
                statements.push_back(state);
            }
            break;
        }

        default:
        {
            goto _error2;
            break;
        }


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
    return NULL;
}
Expression* Parser::parseLineSPC()
{
    return NULL;
}


Expression* Parser::parseExpr()
{
    Expression* Left = parseTerm();
    while (Tok.isOneOf(Token::plus, Token::minus))
    {
        BinaryOp::Operator Op =
            Tok.is(Token::plus) ? BinaryOp::Plus : BinaryOp::Minus;
        advance();
        Expression* Right = parseTerm();
        Left = new BinaryOp(Op, Left, Right);
    }
    return Left;
}


Expression* Parser::parseTerm()
{
    Expression* Left = parseFactor();
    while (Tok.isOneOf(Token::star, Token::slash))
    {
        BinaryOp::Operator Op =
            Tok.is(Token::star) ? BinaryOp::Mul : BinaryOp::Div;
        advance();
        Expression* Right = parseFactor();
        Left = new BinaryOp(Op, Left, Right);
    }
    return Left;
}

Expression* Parser::parsePower()
{
    return NULL;
}

Expression* Parser::parseFactor()
{
    Expression* Res = nullptr;
    switch (Tok.getKind())
    {
    case Token::number:
    {
        int number;
        Tok.getText().getAsInteger(10, number);
        Res = new Expression(number);
        advance();
        break;
    }
    case Token::ident:
    {
        Res = new Expression(Tok.getText());
        advance();
        break;
    }
    case Token::l_paren:
    {
        advance();
        Res = parseExpr();
        if (!consume(Token::r_paren))
            break;
    }
    default: // error handling
    {
        if (!Res)
            error();
        while (!Tok.isOneOf(Token::r_paren, Token::star, Token::plus, Token::minus, Token::slash, Token::eoi))
            advance();
        break;
    }

    }
    return Res;
}


AssignStatement* Parser::parseAssign()
{
    Expression* lhand;
    Expression* rhand;

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


Expression* Parser::parseVar()
{
    if (!Tok.is(Token::ident))
    {
        error();
        return nullptr;
    }

    Expression* variable = new Expression(Tok.getText());
    advance();
    return variable;
}

Expression* Parser::parseCondition()
{
    return NULL;
}


Expression* Parser::parseSubCondition()
{
    return NULL;
}

LoopStatement* Parser::parseLoop()
{
    return NULL;
}




Base* Parser::parse()
{
    Base* Res = parseS();
    return (Base*) Res;
}