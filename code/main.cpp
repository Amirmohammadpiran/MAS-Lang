#include "Lexer.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/raw_ostream.h"
#include <iostream>
#include "AST.h"
#include "Parser.h"

using namespace std;


// Input in llvm format
static llvm::cl::opt<std::string> Input(llvm::cl::Positional,
	llvm::cl::desc("<input expression>"),
	llvm::cl::init(""));


int main(int argc, const char** argv)
{
	// parse command line with builtin llvm function
	llvm::InitLLVM X(argc, argv);
	llvm::cl::ParseCommandLineOptions(argc, argv, "MAS-Lang Compiler\n");

	Token nextToken;

	Lexer lexer("a = 3 ^ (5 * 53 + 2) - 2");


	Parser Parser(lexer);
	AST* Tree = Parser.parse();




	return 0;
}
