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

void Error::NumberVariableExpected()
{
	cout << "Expected a number or a variable, but found none...";
	exit(3);
}

void Error::BeginExpectedAfterColon()
{
	cout << "Expected 'begin' after condition, but found none...";
	exit(3);
}

void Error::EndNotSeenForIf()
{
	cout << "Expected 'end' for if statement, but found none...";
	exit(3);
}

void Error::ColonExpectedAfterCondition()
{
	cout << "Colon expected after condition, but found none...";
	exit(3);
}