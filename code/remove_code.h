#ifndef REMOVECODE_H
#define REMOVECODE_H

#include "precheck.h"

class Remove{

    Check& checker;
    std::vector<std::string> variables;
    std::vector<char*> variable_end_ptr;
    std::vector<std::vector<char*>> line_start;
    std::vector<std::string> live_variables;


    void find_variables(){

        checker.find_vars(variables, variable_end_ptr);
        checker.find_lines(variables, line_start);
        checker.set_live(variables, line_start, live_variables);


        std::cout << "live variables:";
        for (const auto& element : live_variables) {
            std::cout << " " << element;
        }
        std::cout << std::endl;

    }

public:
	// initializes all members and retrieves the first token
	Remove(Check& checker) : checker(checker)
	{
		find_variables();
	}
};
#endif