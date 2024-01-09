// #include "precheck.h"
// #include <vector>

// namespace charinfo {

// 	LLVM_READNONE inline bool isWhitespace(char c) {
// 		return c == ' ' || c == '\t' || c == '\f' ||
// 			c == '\v' || c == '\r' || c == '\n';
// 	}

// 	LLVM_READNONE inline bool isDigit(char c) {
// 		return c >= '0' && c <= '9';
// 	}

// 	LLVM_READNONE inline bool isLetter(char c) {
// 		return (c >= 'a' && c <= 'z') ||
// 			(c >= 'A' && c <= 'Z');
// 	}

// }
// std::vector<std::string> Check::find_vars(){

//     std::vector<std::string> variables;

//     while (*BufferPtr) {                      // since end of context is 0 -> !0 = true -> end of context
        
//         while (*BufferPtr && charinfo::isWhitespace(*BufferPtr)) {      // Skips whitespace like " "
//             ++BufferPtr;
//         }

//         if (charinfo::isLetter(*BufferPtr)) {   // looking for keywords or identifiers like "int", a123 , ...

//             const char* end = BufferPtr + 1;

//             while (charinfo::isLetter(*end) || charinfo::isDigit(*end))
//                 ++end;                          // until reaches the end of lexeme
//             // example: ".int " -> "i.nt " -> "in.t " -> "int. "

//             llvm::StringRef Context(BufferPtr, end - BufferPtr);  // start of lexeme, length of lexeme

//             if (Context == "int") {}

//             else if (Context == "result") {
//                 variables.push_back("result");
//             }
//             else {
//                 variables.push_back((std::string) Context);
//             }

//         }

//         BufferPtr++;
//     }

//     return variables;
// }