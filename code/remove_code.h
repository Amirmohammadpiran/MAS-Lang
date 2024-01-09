#ifndef REMOVECODE_H
#define REMOVECODE_H

#include "precheck.h"

class Remove{

    Check& checker;
    std::vector<std::string> variables;
    std::vector<char*> pointers_start;
    std::vector<char*> pointers_end;

    void find_variables(){

        checker.find_vars(variables, pointers_start, pointers_end);

        std::cout << "Vector elements:";
        for (const auto& element : variables) {
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