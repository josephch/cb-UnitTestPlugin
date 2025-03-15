#ifndef UNITTEST_TESTRUNNER_H
#define UNITTEST_TESTRUNNER_H

#include <string>
#include <list>
#include <string.h>

#include "Test.h"
#include "TestList.h"
#include "CurrentTest.h"

namespace UnitTest
{

class TestReporter;
class TestResults;
class Timer;

int RunAllTests();
int RunAllTestsThrowing();
int RunTestList(const std::list<std::string> &testList);
int RunTestListThrowing(const std::list<std::string> &testList);
int RunTestSuite(const std::string &suiteName);
int RunTestSuiteThrowing(const std::string &suiteName);
int RunTestSuites(const std::list<std::string> &suiteName);
int RunTestSuitesThrowing(const std::list<std::string> &suiteName);

struct True
{
	bool operator()(const Test *const) const
	{
		return true;
	}
};

class TestRunner
{
	public:
		explicit TestRunner(TestReporter &reporter);
		~TestRunner();

		template <class Predicate>
		int RunTestSuites(TestList const &list, const std::list<std::string> &suites, const Predicate &predicate, int maxTestTimeInMs) const
		{
			Test *curTest = list.GetHead();

			while (curTest != 0)
			{
				bool testInASuite = false;
				if (suites.empty() == false)
				{
					for (const std::string &suiteName : suites)
					{
						if (IsTestInSuite(curTest, suiteName.c_str()) == true)
						{
							testInASuite = true;
							break;
						}
					}
				}
				else
				{
					testInASuite = true;
				}

				if (testInASuite == true && predicate(curTest))
				{
					RunTest(m_result, curTest, maxTestTimeInMs);
				}

				curTest = curTest->next;
			}

			return Finish();
		}

		template <class Predicate>
		int RunTestList(const std::list<std::string> &testList, const Predicate &predicate, int maxTestTimeInMs) const
		{
			for (const std::string testName : testList)
			{
				Test *curTest = Test::GetTestList().GetHead();

				while (curTest != 0)
				{
					if (!strcmp(curTest->m_details.testName, testName.c_str()) && predicate(curTest))
					{
						RunTest(m_result, curTest, maxTestTimeInMs);
						break;
					}

					curTest = curTest->next;
				}
			}

			return Finish();
		}

	private:
		TestReporter *m_reporter;
		TestResults *m_result;
		Timer *m_timer;

		int Finish() const;
		bool IsTestInSuite(const Test *const curTest, char const *suiteName) const;
		void RunTest(TestResults *const result, Test *const curTest, int const maxTestTimeInMs) const;
};

}

#endif
