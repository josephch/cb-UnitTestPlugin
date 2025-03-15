
#include "unittest++/UnitTest++.h"

#include <iostream>

using namespace std;

SUITE(MyFirstTestSuite)
{

TEST(SomeTest)
{

}

TEST(TestDuplicate)
{
	cout << "First Test Duplicate" << endl;
}

}


SUITE(MySecondTestSuite)
{

TEST(TestDuplicate)
{
	cout << "Second Test Duplicate" << endl;

	int b = 2, c = 3;
	int a = b + c;

	CHECK_EQUAL(5, a);
}

TEST(AFailingTest)
{
	int b = 11, c = 4;
	int a = b + c;

	CHECK_EQUAL(17, a);
}

}




