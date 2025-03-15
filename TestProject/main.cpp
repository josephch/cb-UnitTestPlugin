
#include "unittest++/UnitTest++.h"

#include <list>
#include <string>

using namespace std;

int main(int numArgs, char **args)
{
	list<string> tests;
	list<string> suites;

	if(numArgs >= 3)
	{
		if(string(args[1]) == string("-s"))
		{
			for(int i = 2; i < numArgs; ++i)
			{
				suites.push_back(args[i]);
			}
		}
		else if (string(args[1]) == string("-t"))
		{
			for(int i = 2; i < numArgs; ++i)
			{
				tests.push_back(args[i]);
			}
		}
	}

	if(tests.empty() == false)
	{
		UnitTest::RunTestList(tests);
	}
	else if(suites.empty() == false)
	{
		UnitTest::RunTestSuites(suites);
	}
	else
	{
		UnitTest::RunAllTests();
	}

    return 0;
}
