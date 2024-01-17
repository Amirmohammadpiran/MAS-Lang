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
                  std::vector<std::string>& live_variables, std::vector<char*>& final_lines,
                  char*& last_result_line){

        std::vector<char*> live_lines = lines[0];       // first variable (result) is always live

        std::reverse(live_lines.begin(), live_lines.end());

        for (size_t i=0; i<live_lines.size(); i++) {

            if(!is_left_side(live_lines[i]))
                continue;

            const char* Buffer = live_lines[i];

            while (!charinfo::isSemiColon(*Buffer)) {

                if(charinfo::isEqual(*Buffer)){

                    while (!charinfo::isLetter(*Buffer) && !charinfo::isSemiColon(*Buffer)) {      // Skips whitespace like " "
                        ++Buffer;
                    }

                    std::string var = "result";
                    find_right_side_vars(Buffer, live_variables, var);
                    final_lines.push_back(live_lines[i]);

                    if(live_variables.size() == 0)
                        return;

                    last_result_line = live_lines[i];
                    break;
                }

                ++Buffer;
            }

            if(live_variables.size() > 0)
                break;
        }

        find_extra_live_variables(variables, lines, live_variables, final_lines, last_result_line);
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


    void find_right_side_vars(const char* new_buffer, std::vector<std::string>& live_variables,
                              std::string var){

        const char* Buffer = new_buffer;

        while(!charinfo::isSemiColon(*Buffer)){

            if (charinfo::isLetter(*Buffer)) {   // looking for keywords or identifiers like "int", a123 , ...

                const char* end = Buffer + 1;

                while (charinfo::isLetter(*end) || charinfo::isDigit(*end))
                    ++end;                          // until reaches the end of lexeme

                llvm::StringRef Context(Buffer, end - Buffer);  // start of lexeme, length of lexeme

                if (Context != "int" && Context != var) {

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

    bool found_result_after(const char* buffer, char* last_result_line){

        while(*buffer){

            if(buffer == last_result_line){
                return true;
            }
            buffer++;
        }
        return false;
    }


    bool is_def_line(char* line, std::vector<std::string> live_variables){

        char* Buffer = line;
        char* end_sent = Buffer + 1;
        while(!charinfo::isSemiColon(*end_sent))
            end_sent++;

        while (Buffer != end_sent) {

            while (charinfo::isWhitespace(*Buffer)) {      // Skips whitespace like " "
                ++Buffer;
            }

            if (charinfo::isLetter(*Buffer)) {   // looking for keywords or identifiers like "int", a123 , ...

                char* end = Buffer + 1;

                while (charinfo::isLetter(*end) || charinfo::isDigit(*end))
                    ++end;                          // until reaches the end of lexeme

                llvm::StringRef Context(Buffer, end - Buffer);  // start of lexeme, length of lexeme

                Buffer = end;
                if ((std::string) Context == "int") {
                    
                    while (charinfo::isWhitespace(*Buffer)) {
                        ++Buffer;
                    }
                    char* end = Buffer + 1;
                    while (charinfo::isLetter(*end) || charinfo::isDigit(*end))
                        ++end;

                    llvm::StringRef Context(Buffer, end - Buffer);  // start of lexeme, length of lexeme

                    for(auto const& var : live_variables){
                        if((std::string) Context == var){
                            return true;
                        }
                    }
                    return false;
                }else{
                    return false;
                }

                Buffer = end;
            }


            ++Buffer;
        }

        return false;

    }

    void find_extra_live_variables(std::vector<std::string> variables,
                                   std::vector<std::vector<char*>> lines,
                                   std::vector<std::string>& live_variables,
                                   std::vector<char*>& final_lines,
                                   char* last_result_line){


        for (size_t i=0; i<live_variables.size(); i++) {

            std::vector<char*> live_lines;

            for (size_t j=0; j<variables.size(); j++){

                if(variables[j] == live_variables[i]){
                    live_lines = lines[j];

                }
            }

            std::reverse(live_lines.begin(), live_lines.end());
            std::vector<std::string> temp_live_variables = live_variables;

            for (const auto& line : live_lines) {

                if(!is_left_side(line))
                    continue;

                const char* Buffer = line;
                bool found_extra = false;

                while (!charinfo::isSemiColon(*Buffer)) {

                    if(charinfo::isEqual(*Buffer)){

                        while (!charinfo::isLetter(*Buffer) && !charinfo::isSemiColon(*Buffer)) {      // Skips whitespace like " "
                            ++Buffer;
                        }

                        if(found_result_after(Buffer, last_result_line)){
                        
                            size_t length = live_variables.size();
                            find_right_side_vars(Buffer, live_variables, live_variables[i]);
                            if(live_variables.size() > length){
                                final_lines.push_back(line);
                                found_extra = true;
                            }
                        }
                        break;
                    }

                    ++Buffer;
                }

                if(live_variables.size() > temp_live_variables.size() || found_extra){
                    temp_live_variables = live_variables;
                    break;
                }
            }
        }
    
    }

    std::vector<std::string> finalize_lines(std::vector<char*> final_program){

        std::vector<std::string> lines;

        for(auto const& line : final_program){

            const char* pointer = line;
            while (!charinfo::isSemiColon(*pointer)) { 
                ++pointer;
            }
            llvm::StringRef Context(line, pointer+1 - line);
            lines.push_back((std::string) Context);
        }

        return lines;
    }


    std::vector<char*> sent_tokenize(){

        const char* pointer = BufferPtr;
        const char* line_start = BufferPtr;
        std::vector<char*> start_buffers;


        while (*pointer) {                      // since end of context is 0 -> !0 = true -> end of context
    
            while (*pointer && charinfo::isWhitespace(*pointer)) {      // Skips whitespace like " "
                ++pointer;
            }

            if(charinfo::isSemiColon(*pointer)){

                start_buffers.push_back((char*) line_start);
                line_start = pointer+1;
            }

            ++pointer;
        }

        return start_buffers;
    }

};
#endif
