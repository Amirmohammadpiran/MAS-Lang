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

        std::cout << "Vector elements:";
        for (const auto& element : variables) {
            std::cout << " " << element;
        }
        std::cout << std::endl;

        std::cout << "Vector lines:\n";
        for (const auto& row : line_start) {
            for (const auto& element : row) {
                std::cout << element << "\n";
            }
            std::cout << "------------------------------\n";
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