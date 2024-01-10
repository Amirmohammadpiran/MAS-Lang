#ifndef PRECHECK_H
#define PRECHECK_H

#include <string>
#include <iostream>

namespace charinfo {

	LLVM_READNONE inline bool isWhitespace(char c) {
		return c == ' ' || c == '\t' || c == '\f' ||
			c == '\v' || c == '\r' || c == '\n';
	}

	LLVM_READNONE inline bool isDigit(char c) {
		return c >= '0' && c <= '9';
	}

	LLVM_READNONE inline bool isLetter(char c) {
		return (c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z');
	}

    LLVM_READNONE inline bool isSemiColon(char c) {
		return c == ';';
	}

    LLVM_READNONE inline bool isEqual(char c) {
		return c == '=';
	}

}

class Check{

    std::string code = "";
    const char* BufferPtr;

public: Check(const llvm::StringRef& Buffer) {    // constructor scans the whole context

		BufferPtr = Buffer.begin();
        const char* end = BufferPtr + 1;
        while (*end) {              // since end of context is 0 -> !0 = true -> end of context
		    end++;
	    }
        llvm::StringRef Context(BufferPtr, end - BufferPtr);
        code = (std::string) Context;
	}

public:
    void find_vars(std::vector<std::string>& variables,
                   std::vector<char*>& variable_end_ptr){

    const char* pointer = BufferPtr;

    while (*pointer) {                      // since end of context is 0 -> !0 = true -> end of context
        
        while (*pointer && charinfo::isWhitespace(*pointer)) {      // Skips whitespace like " "
            ++pointer;
        }

        if (charinfo::isLetter(*pointer)) {   // looking for keywords or identifiers like "int", a123 , ...

            const char* end = pointer + 1;

            while (charinfo::isLetter(*end) || charinfo::isDigit(*end))
                ++end;                          // until reaches the end of lexeme
            // example: ".int " -> "i.nt " -> "in.t " -> "int. "

            llvm::StringRef Context(pointer, end - pointer);  // start of lexeme, length of lexeme

            if (Context == "int") {}

            else if (Context == "result") {

                bool duplicate = false;

                for (const auto& element1 : variables) {

                    if(element1 == Context){
                        duplicate = true;
                    }
                }
                if(!duplicate){
                    variables.push_back("result");
                    variable_end_ptr.push_back((char*) end);
                }
                
            }
            else {
                bool duplicate = false;

                for (const auto& element1 : variables) {

                    if(element1 == Context){
                        duplicate = true;
                    }
                }
                if(!duplicate){
                    variables.push_back((std::string) Context);
                    variable_end_ptr.push_back((char*) end);
                }
            }

			pointer = end;

        }

        ++pointer;
    }

    return;

	};

    void find_lines(std::vector<std::string> variables, std::vector<std::vector<char*>>& lines){

        for (const auto& variable : variables) {

            const char* pointer = BufferPtr;
            const char* line_start = BufferPtr;
            std::vector<char*> single_lines;

            while (*pointer) {                      // since end of context is 0 -> !0 = true -> end of context
        
                while (*pointer && charinfo::isWhitespace(*pointer)) {      // Skips whitespace like " "
                    ++pointer;
                }

                if (charinfo::isLetter(*pointer)) {   // looking for keywords or identifiers like "int", a123 , ...

                    const char* end = pointer + 1;

                    while (charinfo::isLetter(*end) || charinfo::isDigit(*end))
                        ++end;                          // until reaches the end of lexeme

                    llvm::StringRef Context(pointer, end - pointer);  // start of lexeme, length of lexeme

                    if ((std::string) Context == variable) {

                        bool duplicate = false;

                        for (const auto& element1 : single_lines) {

                            if(element1 == line_start){
                                duplicate = true;
                            }
                        }
                        if(!duplicate){
                            single_lines.push_back((char*) line_start);
                        }
                    }

                    pointer = end;

                }
                if(charinfo::isSemiColon(*pointer)){
                    line_start = pointer+1;
                }

                ++pointer;
            }
            
            lines.push_back(single_lines);
        }
    }

    void set_live(std::vector<std::string> variables, std::vector<std::vector<char*>> lines,
                  std::vector<std::string>& live_variables){

        std::vector<char*> live_lines = lines[0];       // first variable (result) is always live

        std::reverse(live_lines.begin(), live_lines.end());

        for (const auto& line : live_lines) {

            if(!is_left_side(line))
                continue;

            const char* Buffer = line;

            while (!charinfo::isSemiColon(*Buffer)) {

                if(charinfo::isEqual(*Buffer)){

                    while (!charinfo::isLetter(*Buffer) && !charinfo::isSemiColon(*Buffer)) {      // Skips whitespace like " "
                        ++Buffer;
                    }

                    find_right_side_vars(Buffer, live_variables);
                }

                ++Buffer;
            }

            if(live_variables.size() > 0)
                break;
        }

        ///// Extra 
    }

    bool is_left_side(const char* line){
            
        const char* Buffer = line;

        while (!charinfo::isEqual(*Buffer)) {

            if(charinfo::isSemiColon(*Buffer)){
                return false;
            }


            while (!charinfo::isLetter(*Buffer) && !charinfo::isSemiColon(*Buffer)) {      // Skips whitespace like " "
                ++Buffer;
            }

            if (charinfo::isLetter(*Buffer)) {   // looking for keywords or identifiers like "int", a123 , ...

                const char* end = Buffer + 1;

                while (charinfo::isLetter(*end) || charinfo::isDigit(*end))
                    ++end;                          // until reaches the end of lexeme

                llvm::StringRef Context(Buffer, end - Buffer);  // start of lexeme, length of lexeme

                if (Context == "result") {
                    return true;
                }

                Buffer = end;
            }

            ++Buffer;
        }

        return false;
    }


    void find_right_side_vars(const char* new_buffer, std::vector<std::string>& live_variables){

        const char* Buffer = new_buffer;

        while(!charinfo::isSemiColon(*Buffer)){

            if (charinfo::isLetter(*Buffer)) {   // looking for keywords or identifiers like "int", a123 , ...

                const char* end = Buffer + 1;

                while (charinfo::isLetter(*end) || charinfo::isDigit(*end))
                    ++end;                          // until reaches the end of lexeme

                llvm::StringRef Context(Buffer, end - Buffer);  // start of lexeme, length of lexeme

                if (Context != "int" && Context != "result") {

                    bool duplicate = false;

                    for (const auto& element1 : live_variables) {

                        if(element1 == Context){
                            duplicate = true;
                        }
                    }
                    if(!duplicate){
                        live_variables.push_back((std::string) Context);
                    }
                }

                Buffer = end;
            }
            else{
                ++Buffer;
            }
        }
    }

};
#endif
