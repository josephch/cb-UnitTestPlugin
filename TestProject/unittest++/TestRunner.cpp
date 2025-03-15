#include "TestRunner.h"
#include "TestResults.h"
#include "TestReporter.h"
#include "TestReporterStdout.h"
#include "ThrowingTestReporter.h"
#include "TimeHelpers.h"
#include "MemoryOutStream.h"

#include <string>
#include <cstring>
#include <iostream>

using namespace std;

namespace UnitTest {

int RunAllTests()
{
	TestReporterStdout reporter;
	TestRunner runner(reporter);
	return runner.RunTestSuites(Test::GetTestList(), list<string>(), True(), 0);
}

int RunAllTestsThrowing()
{
	ThrowingTestReporter reporter;
	TestRunner runner(reporter);
	return runner.RunTestSuites(Test::GetTestList(), list<string>(), True(), 0);
}

int RunTestList(const std::list<std::string> &testList)
{
	TestReporterStdout reporter;
	TestRunner runner(reporter);

	return runner.RunTestList(testList, True(), 0);
}

int RunTestListThrowing(const std::list<std::string> &testList)
{
	ThrowingTestReporter reporter;
	TestRunner runner(reporter);
	return runner.RunTestList(testList, True(), 0);
}

int RunTestSuite(const std::string &suiteName)
{
	TestReporterStdout reporter;
	TestRunner runner(reporter);
	return runner.RunTestSuites(Test::GetTestList(), {suiteName}, True(), 0);
}

int RunTestSuiteThrowing(const std::string &suiteName)
{
	ThrowingTestReporter reporter;
	TestRunner runner(reporter);
	return runner.RunTestSuites(Test::GetTestList(), {suiteName}, True(), 0);
}

int RunTestSuites(const std::list<std::string> &suites)
{
	TestReporterStdout reporter;
	TestRunner runner(reporter);

	return runner.RunTestSuites(Test::GetTestList(), suites, True(), 0);
}

int RunTestSuitesThrowing(const std::list<std::string> &suites)
{
	ThrowingTestReporter reporter;
	TestRunner runner(reporter);
	return runner.RunTestSuites(Test::GetTestList(), suites, True(), 0);
}


TestRunner::TestRunner(TestReporter& reporter)
	: m_reporter(&reporter)
	, m_result(new TestResults(&reporter))
	, m_timer(new Timer)
{
	m_timer->Start();
}

TestRunner::~TestRunner()
{
	delete m_result;
	delete m_timer;
}

int TestRunner::Finish() const
{
    float const secondsElapsed = m_timer->GetTimeInMs() / 1000.0f;
    m_reporter->ReportSummary(m_result->GetTotalTestCount(),
							  m_result->GetFailedTestCount(),
							  m_result->GetFailureCount(),
							  secondsElapsed);

	return m_result->GetFailureCount();
}

bool TestRunner::IsTestInSuite(const Test* const curTest, char const* suiteName) const
{
	using namespace std;
	return (suiteName == nullptr) || !strcmp(curTest->m_details.suiteName, suiteName);
}

void TestRunner::RunTest(TestResults* const result, Test* const curTest, int const maxTestTimeInMs) const
{
	CurrentTest::Results() = result;

	Timer testTimer;
	testTimer.Start();

	result->OnTestStart(curTest->m_details);

	std::cout << "running Test " << curTest->m_details.testName << " " << endl;

	curTest->Run();

	int const testTimeInMs = testTimer.GetTimeInMs();
	if (maxTestTimeInMs > 0 && testTimeInMs > maxTestTimeInMs && !curTest->m_timeConstraintExempt)
	{
	    MemoryOutStream stream;
	    stream << "Global time constraint failed. Expected under " << maxTestTimeInMs <<
	            "ms but took " << testTimeInMs << "ms.";

	    result->OnTestFailure(curTest->m_details, stream.GetText());
	}

	result->OnTestFinish(curTest->m_details, testTimeInMs/1000.0f);
}

}
