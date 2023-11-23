#ifndef ERROR_H
#define ERROR_H

#include <iostream>
using namespace std;

static class Error {

public:
	static void SemiColonNotFound();
	static void AssignmentEqualNotFound();
	static void AssignmentSidesNotEqual();
	static void VariableNameNotFound();
	static void BooleanValueExpected();
	static void RightParanthesisExpected();
};

#endif