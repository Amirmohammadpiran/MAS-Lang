#include "Lexer.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
#include "AST.h"

using namespace std;


// Input in llvm format
static llvm::cl::opt<std::string> Input(llvm::cl::Positional,
    llvm::cl::desc("<input expression>"),
    llvm::cl::init(""));


int main(int argc, const char **argv)
{
    // parse command line with builtin llvm function
    llvm::InitLLVM X(argc, argv);
    llvm::cl::ParseCommandLineOptions(argc, argv, "MAS-Lang Compiler\n");

    Token nextToken;

    Lexer lexer(Input);

    lexer.next(nextToken);
    while (nextToken.getKind() != 0) {
        cout << nextToken.getKind();
        cout << "\n";
        lexer.next(nextToken);
    }
    

    return 0;
}
