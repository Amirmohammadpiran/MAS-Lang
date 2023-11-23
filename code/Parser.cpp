#ifndef PARSER_H
#define PARSER_H
#include "Parser.h"
#include "Error.h"
#endif




Base* Parser::parseS() 
{
    llvm::SmallVector<Statement*> statements;
    while (!Tok.is(Token::eof))
    {
        switch (Tok.getKind())
        {
        case Token::ident:
        {
            llvm::SmallVector<AssignStatement*> states = parseAssign();

            if (states.size() == 0)
            {
                return nullptr;
            }

            while (states.size() > 0)
            {
                statements.push_back(states.back());
                states.pop_back();
            }
            break;
        }
        case Token::KW_int:
        {
            llvm::SmallVector<DecStatement*> states = parseDefine();
            if (states.size() == 0)
            {
                return nullptr;
            }

            while (states.size() > 0)
            {
                statements.push_back(states.back());
                states.pop_back();
            }
            break;
        }

        default:
        {
            goto _error2;
            break;
        }


        }
    }
    return new Base(statements);
_error2:
    while (Tok.getKind() != Token::eoi)
        advance();
    return nullptr;

}

llvm::SmallVector<DecStatement*> Parser::parseDefine()
{
    llvm::SmallVector<DecStatement*> assignments;

    llvm::SmallVector<Expression*> variables;
    llvm::SmallVector<Expression*> values;

    advance();
    bool SeenTokenVariable = true;
    while (SeenTokenVariable)
    {
        Expression* lhand = parseVar();
        variables.push_back(lhand);

        if (!Tok.is(Token::comma))
            SeenTokenVariable = false;
        else
            advance();
    }



    if (Tok.is(Token::eoi))
    {
        bool SeenTokenValue = true;
        for (int i=0;i<variables.size();i++)
        {
            Expression* rhand = new Expression(0);
            values.push_back(rhand);
        }
    }
    else if (!Tok.is(Token::equal))
    {
        Error::AssignmentEqualNotFound();
    }
    else
    {
        advance(); // pass equal

        bool SeenTokenValue = true;
        while (SeenTokenValue)
        {
            Expression* rhand = parseExpr();
            values.push_back(rhand);

            if (!Tok.is(Token::comma))
                SeenTokenValue = false;
            else
                advance();
        }
    }




    if (variables.size() != values.size())
    {
        Error::AssignmentSidesNotEqual();
    }
    else
    {
        while (variables.size() != 0)
        {
            assignments.push_back(new DecStatement(variables.back(), values.back()));
            variables.pop_back();
            values.pop_back();
        }

        if (Tok.is(Token::eoi))
        {
            advance();
            return assignments;
        }
        else
        {
            Error::SemiColonNotFound();
        }
    }


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


llvm::SmallVector<AssignStatement*> Parser::parseAssign()
{
    llvm::SmallVector<AssignStatement*> assignments;

    llvm::SmallVector<Expression*> variables;
    llvm::SmallVector<Expression*> values;


    bool SeenTokenVariable = true;
    while (SeenTokenVariable)
    {
        Expression* lhand = parseVar();
        variables.push_back(lhand);

        if (!Tok.is(Token::comma))
            SeenTokenVariable = false;
        else
            advance();
    }


    
    if (!Tok.is(Token::equal))
    {
        Error::AssignmentEqualNotFound();
    }
    

    advance(); // pass equal

    bool SeenTokenValue = true;
    while (SeenTokenValue)
    {
        Expression* rhand = parseExpr();
        values.push_back(rhand);

        if (!Tok.is(Token::comma))
            SeenTokenValue = false;
        else
            advance();
    }

    if (variables.size() != values.size())
    {
        Error::AssignmentSidesNotEqual();
    }
    else
    {
        while (variables.size() != 0)
        {
            assignments.push_back(new AssignStatement(variables.back(), values.back()));
            variables.pop_back();
            values.pop_back();
        }

        if (Tok.is(Token::eoi))
        {
            advance();
            return assignments;
        }
        else
        {
            Error::SemiColonNotFound();
        }
    }

}


Expression* Parser::parseVar()
{
    if (!Tok.is(Token::ident))
    {
        Error::VariableNameNotFound();
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