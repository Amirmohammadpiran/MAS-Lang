#include "CodeGen.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

// Define a visitor class for generating LLVM IR from the AST.
namespace
{
    class ToIRVisitor : public ASTVisitor
    {
        Module* M;
        IRBuilder<> Builder;
        Type* VoidTy;
        Type* Int32Ty;
        Type* Int8PtrTy;
        Type* Int8PtrPtrTy;
        Constant* Int32Zero;

        Value* V;
        StringMap<AllocaInst*> nameMap;

        llvm::FunctionType* MainFty;
        llvm::Function* MainFn;

    public:
        // Constructor for the visitor class.
        ToIRVisitor(Module* M) : M(M), Builder(M->getContext())
        {
            // Initialize LLVM types and constants.
            VoidTy = Type::getVoidTy(M->getContext());
            Int32Ty = Type::getInt32Ty(M->getContext());
            Int8PtrTy = Type::getInt8PtrTy(M->getContext());
            Int8PtrPtrTy = Int8PtrTy->getPointerTo();
            Int32Zero = ConstantInt::get(Int32Ty, 0, true);
        }

        // Entry point for generating LLVM IR from the AST.
        void run(AST* Tree)
        {
            // Create the main function with the appropriate function type.
            MainFty = FunctionType::get(Int32Ty, { Int32Ty, Int8PtrPtrTy }, false);
            MainFn = Function::Create(MainFty, GlobalValue::ExternalLinkage, "main", M);

            // Create a basic block for the entry point of the main function.
            BasicBlock* BB = BasicBlock::Create(M->getContext(), "entry", MainFn);
            Builder.SetInsertPoint(BB);

            // Visit the root node of the AST to generate IR.
            Tree->accept(*this);

            // Create a return instruction at the end of the main function.
            Builder.CreateRet(Int32Zero);
        }


        virtual void visit(Base& Node) override
        {
            for (auto I = Node.begin(), E = Node.end(); I != E; ++I)
            {
                (*I)->accept(*this);
            }
        }

        virtual void visit(Statement& Node) override
        {
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
            
        }

        virtual void visit(Expression& Node) override
        {
            if (Node.getKind() == Expression::ExpressionType::Identifier)
            {
                V = Builder.CreateLoad(Int32Ty, nameMap[Node.getValue()]);
            }
            else if (Node.getKind() == Expression::ExpressionType::Number)
            {
               
                int intval = Node.getNumber();
                V = ConstantInt::get(Int32Ty, intval, true);
            }
        }


        virtual void visit(BooleanOp& Node) override
        {
            // Visit the left-hand side of the binary operation and get its value.
            Node.getLeft()->accept(*this);
            Value* Left = V;

            // Visit the right-hand side of the binary operation and get its value.
            Node.getRight()->accept(*this);
            Value* Right = V;

            // Perform the boolean operation based on the operator type and create the corresponding instruction.
            switch (Node.getOperator())
            {
            case BooleanOp::Equal:
                V = Builder.CreateICmpEQ(Left, Right);
                break;
            case BooleanOp::Less:
                V = Builder.CreateICmpSLT(Left, Right);
                break;
            case BooleanOp::LessEqual:
                V = Builder.CreateICmpSLE(Left, Right);
                break;
            case BooleanOp::Greater:
                V = Builder.CreateICmpSGT(Left, Right);
                break;
            case BooleanOp::GreaterEqual:
                V = Builder.CreateICmpSGE(Left, Right);
            case BooleanOp::And:
                V = Builder.CreateAnd(Left, Right);
            case BooleanOp::Or:
                V = Builder.CreateOr(Left, Right);
            }
        }


        virtual void visit(BinaryOp& Node) override
        {
            // Visit the left-hand side of the binary operation and get its value.
            Node.getLeft()->accept(*this);
            Value* Left = V;

            // Visit the right-hand side of the binary operation and get its value.
            Node.getRight()->accept(*this);
            Value* Right = V;

            // Perform the binary operation based on the operator type and create the corresponding instruction.
            switch (Node.getOperator())
            {
            case BinaryOp::Plus:
                V = Builder.CreateNSWAdd(Left, Right);
                break;
            case BinaryOp::Minus:
                V = Builder.CreateNSWSub(Left, Right);
                break;
            case BinaryOp::Mul:
                V = Builder.CreateNSWMul(Left, Right);
                break;
            case BinaryOp::Div:
                V = Builder.CreateSDiv(Left, Right);
                break;
            case BinaryOp::Pow:
                // V = Builder.CreateNSWMul(Left, Right);
                break;
            case BinaryOp::Mod:
                Value* division = Builder.CreateSDiv(Left, Right);
                Value* multiplication = Builder.CreateNSWMul(division, Right);
                V = Builder.CreateNSWSub(Left, multiplication);
            }
        }

        virtual void visit(DecStatement& Node) override
        {
            Value* val = nullptr;

            if (Node.getRValue()->getKind() == Expression::ExpressionType::BinaryOpType || Node.getRValue()->isNumber())
            {
                // If there is an expression provided, visit it and get its value.
                Node.getRValue()->accept(*this);
                val = V;
            }

            // Iterate over the variables declared in the declaration statement.

            auto I = Node.getLValue()->getValue();
            StringRef Var = I;

            // Create an alloca instruction to allocate memory for the variable.
            nameMap[Var] = Builder.CreateAlloca(Int32Ty);

            // Store the initial value (if any) in the variable's memory location.
            if (val != nullptr)
            {
                Builder.CreateStore(val, nameMap[Var]);
            }
            else
            {
                Value* Zero = ConstantInt::get(Type::getInt32Ty(M->getContext()), 0);
                Builder.CreateStore(Zero, nameMap[Var]);
            }
            
        }

        virtual void visit(AssignStatement& Node) override
        {
            // Visit the right-hand side of the assignment and get its value.
            Node.getRValue()->accept(*this);
            Value* val = V;

            // Get the name of the variable being assigned.
            auto varName = Node.getLValue()->getValue();

            // Create a store instruction to assign the value to the variable.
            Builder.CreateStore(val, nameMap[varName]);

        }

        virtual void visit(IfStatement& Node) override
        {




        }

        virtual void visit(ElifStatement& Node) override
        {




        }

        virtual void visit(ElseStatement& Node) override
        {




        }

        virtual void visit(LoopStatement& Node) override
        {

            llvm::BasicBlock* WhileCondBB = llvm::BasicBlock::Create(M->getContext(), "loop.cond", MainFn);
            // The basic block for the while body.
            llvm::BasicBlock* WhileBodyBB = llvm::BasicBlock::Create(M->getContext(), "loop.body", MainFn);
            // The basic block after the while statement.
            llvm::BasicBlock* AfterWhileBB = llvm::BasicBlock::Create(M->getContext(), "after.loop", MainFn);

            // Branch to the condition block.
            Builder.CreateBr(WhileCondBB);

            // Set the insertion point to the condition block.
            Builder.SetInsertPoint(WhileCondBB);

            // Visit the condition expression and create the conditional branch.
            Node.getCondition()->accept(*this);
            Value* Cond = V;
            Builder.CreateCondBr(Cond, WhileBodyBB, AfterWhileBB);

            // Set the insertion point to the body block.
            Builder.SetInsertPoint(WhileBodyBB);

            llvm::SmallVector<Statement* > stmts = Node.getStatements();
            for (auto I = stmts.begin(), E = stmts.end(); I != E; ++I)
            {
                (*I)->accept(*this);
            }

            // Branch back to the condition block.
            Builder.CreateBr(WhileCondBB);

            // Set the insertion point to the block after the while loop.
            Builder.SetInsertPoint(AfterWhileBB);


        }



    };
}; // namespace

void CodeGen::compile(AST* Tree)
{
    // Create an LLVM context and a module.
    LLVMContext Ctx;
    Module* M = new Module("mas.expr", Ctx);

    // Create an instance of the ToIRVisitor and run it on the AST to generate LLVM IR.
    ToIRVisitor ToIRn(M);

    ToIRn.run(Tree);

    // Print the generated module to the standard output.
    M->print(outs(), nullptr);
}
