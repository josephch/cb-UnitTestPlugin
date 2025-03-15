
#include "ThrowingTestReporter.h"


#include "TestReporterStdout.h"
#include <cstdio>
#include <sstream>
#include <stdexcept>

#include "TestDetails.h"

namespace UnitTest {

void ThrowingTestReporter::ReportTestStart(TestDetails const& test)
{

}

void ThrowingTestReporter::ReportFailure(TestDetails const& details, char const* failure)
{
    char const* const errorFormat = "%s:%d:0: error: Failure in %s: %s\n";

	using namespace std;

	stringstream ss;
	ss << details.filename << ":" << details.lineNumber << ": error: Failure in " << details.testName << ": " << failure;
	throw runtime_error(ss.str());
}

void ThrowingTestReporter::ReportTestFinish(TestDetails const& test, float secondsElapsed)
{

}

void ThrowingTestReporter::ReportSummary(int totalTestCount, int failedTestCount, int failureCount, float secondsElapsed)
{
	using namespace std;

    if (failureCount > 0)
        printf("FAILURE: %d out of %d tests failed (%d failures).\n", failedTestCount, totalTestCount, failureCount);
    else
        printf("Success: %d tests passed.\n", totalTestCount);

    printf("Test time: %.2f seconds.\n", secondsElapsed);

}

}
