#include "Sema.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/Support/raw_ostream.h"

namespace {
    class DeclCheck : public ASTVisitor {
        llvm::StringSet<> Scope;
        bool HasError;

        enum ErrorType { Twice, Not };

        void error(ErrorType ET, llvm::StringRef V) {
            llvm::errs() << "Variable " << V << " is " << (ET == Twice ? "already" : "not") << " declared!\n";
            HasError = true;
        }

    public:
        DeclCheck() : HasError(false) {}

        bool hasError() { return HasError; }

        virtual void visit(Expression& Node) override {
            if (Node.getKind() == Expression::ExpressionType::Identifier) {
                if (Scope.find(Node.getValue()) == Scope.end())
                    error(Not, Node.getValue());
            }
        };

        virtual void visit(BinaryOp& Node) override {
            if (Node.getLeft())
                Node.getLeft()->accept(*this);
            else
                HasError = true;
            if (Node.getRight())
                Node.getRight()->accept(*this);
            else
                HasError = true;
        };

        virtual void visit(DecStatement& Node) override {

            auto I = (Node.getLValue());

            if (!Scope.insert(Node.getLValue()->getValue()).second)
                error(Twice, Node.getLValue()->getValue());


            
        };

        // TODO
        virtual void visit(IfStatement& Node) override {


        };

        // TODO
        virtual void visit(ElifStatement& Node) override {


        };

        // TODO
        virtual void visit(ElseStatement& Node) override {


        };

        virtual void visit(LoopStatement& Node) override {

            Node.getCondition()->accept(*this);

            llvm::SmallVector<Statement* > stmts = Node.getStatements();
            for (auto I = stmts.begin(), E = stmts.end(); I != E; ++I)
            {
                (*I)->accept(*this);
            }

        };

        virtual void visit(AssignStatement& Node) override {
            auto I = (Node.getLValue());

           
            if (!Scope.count(Node.getLValue()->getValue()))
                error(Not, Node.getLValue()->getValue());


        };

        virtual void visit(Statement& Node) override {
            if (Node.getKind() == Statement::StateMentType::Declaration)
            {
                DecStatement* declaration = (DecStatement*)&Node;
                declaration->accept(*this);
            }
            else if (Node.getKind() == Statement::StateMentType::Assignment)
            {
                AssignStatement* declaration = (AssignStatement*)&Node;
                declaration->accept(*this);
            }
            else if (Node.getKind() == Statement::StateMentType::If)
            {
                IfStatement* declaration = (IfStatement*)&Node;
                declaration->accept(*this);
            }
            else if (Node.getKind() == Statement::StateMentType::Elif)
            {
                ElifStatement* declaration = (ElifStatement*)&Node;
                declaration->accept(*this);
            }
            else if (Node.getKind() == Statement::StateMentType::Else)
            {
                ElseStatement* declaration = (ElseStatement*)&Node;
                declaration->accept(*this);
            }
            else if (Node.getKind() == Statement::StateMentType::Loop)
            {
                LoopStatement* declaration = (LoopStatement*)&Node;
                declaration->accept(*this);
            }

        };

        virtual void visit(BooleanOp& Node) override {
            if (Node.getLeft())
                Node.getLeft()->accept(*this);
            else
                HasError = true;
            if (Node.getRight())
                Node.getRight()->accept(*this);
            else
                HasError = true;

        };

        virtual void visit(Base& Node) override {
            for (auto I = Node.begin(), E = Node.end(); I != E; ++I)
            {
                (*I)->accept(*this);
            }

        };


    };
}

bool Sema::semantic(AST* Tree) {
    if (!Tree)
        return false;
    DeclCheck Check;
    Tree->accept(Check);
    return Check.hasError();
}
