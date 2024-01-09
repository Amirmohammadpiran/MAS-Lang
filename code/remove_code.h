#ifndef REMOVECODE_H
#define REMOVECODE_H

#include "precheck.h"

class Remove{

    Check& checker;
    std::vector<std::string> variables;

    void remove(){

        variables = checker.find_vars();

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
		remove();
	}
};
#endif