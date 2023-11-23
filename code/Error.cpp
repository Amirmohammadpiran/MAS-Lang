#include "Error.h"

void Error::SemiColonNotFound()
{
	cout << "Semicolon not found...";
	exit(3);
}

void Error::AssignmentEqualNotFound()
{
	cout << "Assignment does not have a '=' character...";
	exit(3);
}

void Error::AssignmentSidesNotEqual()
{
	cout << "Assignment sides are not equal in size...";
	exit(3);
}

void Error::VariableNameNotFound()
{
	cout << "Variable name not found...";
	exit(3);
}

void Error::BooleanValueExpected()
{
	cout << "Boolean value expected...";
	exit(3);
}

void Error::RightParanthesisExpected()
{
	cout << "Right paranthesis expected but not found...";
	exit(3);
}