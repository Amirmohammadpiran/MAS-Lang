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
            FunctionType* MainFty = FunctionType::get(Int32Ty, { Int32Ty, Int8PtrPtrTy }, false);
            Function* MainFn = Function::Create(MainFty, GlobalValue::ExternalLinkage, "main", M);

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
            }
        }

        virtual void visit(DecStatement& Node) override
        {
            Value* val = nullptr;

            if (Node.getRValue()->getNumber() != 0)
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
            
        }

        virtual void visit(AssignStatement& Node) override
        {

        }

        virtual void visit(BooleanOp& Node) override
        {

        }





    };
}; // namespace

void CodeGen::compile(AST* Tree)
{
    // Create an LLVM context and a module.
    LLVMContext Ctx;
    Module* M = new Module("calc.expr", Ctx);

    // Create an instance of the ToIRVisitor and run it on the AST to generate LLVM IR.
    ToIRVisitor ToIRn(M);
    ToIRn.run(Tree);

    // Print the generated module to the standard output.
    M->print(outs(), nullptr);
}
