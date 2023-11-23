#ifndef AST_H
#define AST_H

#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"

class AST; // Abstract Syntax Tree
class Expression; // top level expression that is evaluated to boolean, int or variable name at last
class Base; // top level program
class Statement; // top level statement
class BinaryOp;

class ASTVisitor
{
public:
    // Virtual visit functions for each AST node type
    virtual void visit(AST&) {}
    virtual void visit(Expression&) {}
    virtual void visit(Base&) = 0;
    virtual void visit(Statement&) = 0;
    virtual void visit(BinaryOp&) = 0;
};




class AST {
public:
    virtual ~AST() {}
    virtual void accept(ASTVisitor& V) = 0;
};



// base Node that contains all the syntax nodes
class Base : public AST{
private:
    llvm::SmallVector<Statement*> statements;                          // Stores the list of expressions

public:
    Base(llvm::SmallVector<Statement*> Statements) : statements(Statements) {}
    llvm::SmallVector<Statement*> getStatements() { return statements; }

    llvm::SmallVector<Statement*>::const_iterator begin() { return statements.begin(); }

    llvm::SmallVector<Statement*>::const_iterator end() { return statements.end(); }
    virtual void accept(ASTVisitor& V) override
    {
        V.visit(*this);
    }

};


class TopLevelEntity : AST {
public:
    TopLevelEntity() {}
};



// Value Expression class that holds information about
// numbers or variable names. forexample a or 56 are 
// ValExpression and become part of the syntax tree
class Expression: public TopLevelEntity {
public:
    enum ExpressionType {
        Number,
        Identifier,
        Boolean
    };
private:
    ExpressionType Type; // can be number of variable name

    // if it holds a number NumberVal is used else Value is
    // used to store variable name
    llvm::StringRef Value;
    int NumberVal;
    bool BoolVal;

public:
    Expression() {}
    Expression(llvm::StringRef value): Type(ExpressionType::Identifier), Value(value) {} // store string
    Expression(int value) : Type(ExpressionType::Number), NumberVal(value) {} // store number
    Expression(bool value) : Type(ExpressionType::Boolean), BoolVal(value) {} // store boolean
    
    bool isNumber() {
        if (Type == ExpressionType::Number)
            return true;
        return false;
    }

    bool isBoolean() {
        if (Type == ExpressionType::Boolean)
            return true;
        return false;
    }

    bool isVariable() {
        if (Type == ExpressionType::Identifier)
            return true;
        return false;
    }

    llvm::StringRef getValue() {
        return Value;
    }

    int getNumber() {
        return NumberVal;
    }

    bool getBoolean() {
        return BoolVal;
    }

    virtual void accept(ASTVisitor& V) override
    {
        V.visit(*this);
    }
};

// stores information of a statement. forexample x=56; is a statement
class Statement : public TopLevelEntity {
public:
    enum StateMentType {
        Declaration,
        Assignment,
        If,
        Loop
    };

private:
    StateMentType Type;
public:
    Statement(StateMentType type): Type(type) {}
    virtual void accept(ASTVisitor& V) override
    {
        V.visit(*this);
    }
};

// if statement
class IfStatement : public Statement {
    virtual void accept(ASTVisitor& V) override
    {
        V.visit(*this);
    }
};

// loop statement
class LoopStatement : public Statement {
    virtual void accept(ASTVisitor& V) override
    {
        V.visit(*this);
    }
};

class DecStatement : public Statement {
private:

    Expression* lvalue;
    Expression* rvalue;
    Statement::StateMentType type;

public:
    DecStatement(Expression* lvalue, Expression* rvalue) : lvalue(lvalue), rvalue(rvalue), type(Statement::StateMentType::Declaration), Statement(type) { }
    DecStatement(Expression* lvalue) : lvalue(lvalue), rvalue(rvalue), type(Statement::StateMentType::Declaration), Statement(type) { rvalue = new Expression(0); }

    Expression* getLValue() {
        return lvalue;
    }

    Expression* getRValue() {
        return rvalue;
    }

    virtual void accept(ASTVisitor& V) override
    {
        V.visit(*this);
    }
};

// assignment statement. this can have declaration or
// assignment inside it. forexample x=56; or int x=23+25;
class AssignStatement : public Statement {
private:

    Expression* lvalue;
    Expression* rvalue;
    Statement::StateMentType type;

public:
    AssignStatement(Expression* lvalue, Expression* rvalue) : lvalue(lvalue), rvalue(rvalue), type(Statement::StateMentType::Assignment), Statement(type) { }
    Expression* getLValue() {
        return lvalue;
    }

    Expression* getRValue() {
        return rvalue;
    }

    virtual void accept(ASTVisitor& V) override
    {
        V.visit(*this);
    }
};

// Binary Operation for computation of numbers
// used in the syntax tree stage
class BinaryOp : public Expression
{
public:
    enum Operator
    {
        Plus,
        Minus,
        Mul,
        Div
    };

private:
    Expression* Left;                               // Left-hand side expression
    Expression* Right;                              // Right-hand side expression
    Operator Op;                              // Operator of the binary operation

public:
    BinaryOp(Operator Op, Expression* L, Expression* R) : Op(Op), Left(L), Right(R) {}

    Expression* getLeft() { return Left; }

    Expression* getRight() { return Right; }

    Operator getOperator() { return Op; }

    virtual void accept(ASTVisitor& V) override
    {
        V.visit(*this);
    }
};

#endif