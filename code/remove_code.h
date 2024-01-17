#ifndef REMOVECODE_H
#define REMOVECODE_H

#include "precheck.h"

class Remove{

    Check& checker;
    std::vector<std::string> variables;
    std::vector<char*> variable_end_ptr;
    std::vector<std::vector<char*>> line_start;
    std::vector<std::string> live_variables;
    std::vector<char*> live_lines;
    char* last_result_line;
    std::vector<char*> final_program;


    void find_variables(){

        checker.find_vars(variables, variable_end_ptr);
        checker.find_lines(variables, line_start);
        checker.set_live(variables, line_start, live_variables, live_lines, last_result_line);
        live_variables.push_back("result");
    }

    void remove_lines(){

        std::vector<char*> raw_lines = checker.sent_tokenize();
        for (auto const line : raw_lines){

            if(checker.is_def_line(line, live_variables)){
                final_program.push_back(line);
            }
            else{
                for (auto const live_line : live_lines){

                    if(line == live_line){
                        final_program.push_back(line);
                        break;
                    }
                }
            }
        }
    }

public:
    std::string pointer_to_string(){

        std::string code = "";
        std::vector<std::string> lines = checker.finalize_lines(final_program);
        for (std::string line : lines){
            code += line;
        }
        return code;
    }

public:
	Remove(Check& checker) : checker(checker)
	{
		find_variables();
        remove_lines();
	}
};
#endif