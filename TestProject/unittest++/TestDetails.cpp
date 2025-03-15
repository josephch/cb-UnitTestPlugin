#include "TestDetails.h"

namespace UnitTest {

TestDetails::TestDetails(char const* testName_, char const* suiteName_, char const* filename_, int lineNumber_)
    : suiteName(suiteName_)
    , testName(testName_)
    , filename(filename_)
    , lineNumber(lineNumber_)
{
}

TestDetails::TestDetails(const TestDetails& details, char const* filename_, int lineNumber_)
    : suiteName(details.suiteName)
    , testName(details.testName)
    , filename(filename_)
    , lineNumber(lineNumber_)
{
}


}
