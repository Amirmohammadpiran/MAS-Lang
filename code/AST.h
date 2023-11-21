#ifndef AST_H
#define AST_H

class AST {
public:
    virtual void accept(ASTVisitor& V)
    {
        V.visit(*this);
    }
};


class ASTVisitor
{
public:
    // Virtual visit functions for each AST node type
    virtual void visit(AST&) {}               
    virtual void visit(Expr&) {}              
    virtual void visit(Base&) = 0;             
    virtual void visit(Statement&) = 0;          
    virtual void visit(BinaryOp&) = 0;        
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


};

class TopLevelEntity : AST {
public:
    TopLevelEntity() {}
};

// expression that evaluates to a value -> variable_name, boolean, number
class Expr : public TopLevelEntity {

    virtual void accept(ASTVisitor& V) override
    {
        V.visit(*this);
    }
};

// condition expression that evaluates to boolean
class CondExpression : public Expr {
private:
    bool ConditionBool;

public:
    CondExpression(bool ConditionBool): ConditionBool(ConditionBool) {}

    bool getBooleanValue()
    {
        return ConditionBool;
    }

    virtual void accept(ASTVisitor& V) override
    {
        V.visit(*this);
    }
};

// value expression that evaluates to number
class ValExpression : public Expr {
private:
    int val;

public:
    ValExpression(): val(0) {}
    ValExpression(int val) : val(val) {}


    int getVal()
    {
        return val;
    }

    virtual void accept(ASTVisitor& V) override
    {
        V.visit(*this);
    }
};

// variable expression that evaluates to variable name
class VarExpression : public Expr {
private:
    llvm::StringRef varName;

public:
    VarExpression(llvm::StringRef varName): varName(varName) {}

    llvm::StringRef getName()
    {
        return varName;
    }

    virtual void accept(ASTVisitor& V) override
    {
        V.visit(*this);
    }
};

// stores information of a statement
class Statement : public TopLevelEntity {
public:
    Statement() {}
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

// assignment statement. this can have declaration or
// assignment inside it
class AssignStatement : public Statement {
public:
    enum AssignType {
        Declaration,
        Assignment
    };
private:

    VarExpression* lvalue;
    Expr* rvalue;
    AssignType type;

public:
    AssignStatement(VarExpression* lvalue, Expr* rvalue, AssignType type) : lvalue(lvalue), rvalue(rvalue), type(type) {}

    VarExpression* getLValue() {
        return lvalue;
    }

    Expr* getRValue() {
        return rvalue;
    }

    virtual void accept(ASTVisitor& V) override
    {
        V.visit(*this);
    }
};


class BinaryOp : public ValExpression
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
    Expr* Left;                               // Left-hand side expression
    Expr* Right;                              // Right-hand side expression
    Operator Op;                              // Operator of the binary operation

public:
    BinaryOp(Operator Op, Expr* L, Expr* R) : Op(Op), Left(L), Right(R) {}

    Expr* getLeft() { return Left; }

    Expr* getRight() { return Right; }

    Operator getOperator() { return Op; }

    virtual void accept(ASTVisitor& V) override
    {
        V.visit(*this);
    }
};

#endif