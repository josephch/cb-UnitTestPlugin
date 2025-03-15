
#include "unittest++/UnitTest++.h"

#include <iostream>

using namespace std;


SUITE(TheOtherTestSuite)
{

string expectedString = "Foo";

TEST(AnotherTest)
{
	stringstream ss;
	ss << "Foo";

	CHECK_EQUAL(expectedString, ss.str());
}

TEST(AnotherFailingTest)
{
	int b = 2, c = 4;
	int a = b + c;

	string str = "TestString";

	CHECK_EQUAL(expectedString, str);
	CHECK_EQUAL(5, a);

	CHECK_EQUAL(4, a);
}

}

