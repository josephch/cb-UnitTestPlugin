#ifndef UNITTEST_CHECKS_H
#define UNITTEST_CHECKS_H

#include <math.h>
#include <stdexcept>

#include "Config.h"
#include "TestResults.h"
#include "MemoryOutStream.h"

namespace UnitTest {


template< typename Value >
void Check(Value const value, const char *value_str, TestResults& results, TestDetails const& details)
{
	if(!(value))
	{
		UnitTest::MemoryOutStream stream;
        stream <<  "'" << value_str << "' did not evaluate to true";
        results.OnTestFailure(details, stream.GetText());
	}
}


template< typename Expected, typename Actual >
void CheckNotEqual(TestResults& results, Expected const& expected, Actual const& actual, TestDetails const& details)
{
    if (!(expected != actual))
    {
        UnitTest::MemoryOutStream stream;
        stream << "Expected anything but '" << expected << "', but was '" << actual << "'";
        results.OnTestFailure(details, stream.GetText());
    }
}

void CheckNotEqual(TestResults& results, char const* expected, char const* actual, TestDetails const& details);

void CheckNotEqual(TestResults& results, char* expected, char* actual, TestDetails const& details);

void CheckNotEqual(TestResults& results, char* expected, char const* actual, TestDetails const& details);

void CheckNotEqual(TestResults& results, char const* expected, char* actual, TestDetails const& details);


template< typename Expected, typename Actual >
void CheckEqual(TestResults& results, Expected const& expected, Actual const& actual, TestDetails const& details)
{
    if (!(expected == actual))
    {
        UnitTest::MemoryOutStream stream;
        stream << "Expected '" << expected << "' but was '" << actual << "'";

        results.OnTestFailure(details, stream.GetText());
    }
}

void CheckEqual(TestResults& results, char const* expected, char const* actual, TestDetails const& details);

void CheckEqual(TestResults& results, char* expected, char* actual, TestDetails const& details);

void CheckEqual(TestResults& results, char* expected, char const* actual, TestDetails const& details);

void CheckEqual(TestResults& results, char const* expected, char* actual, TestDetails const& details);

template< typename Expected, typename Actual, typename Tolerance >
bool AreClose(Expected const& expected, Actual const& actual, Tolerance const& tolerance)
{
    return abs(expected - actual) < tolerance;
}

template< typename Expected, typename Actual, typename Tolerance >
bool AreCloseExp(Expected const& expected, Actual const& actual, Tolerance const& tolerance)
{
	bool result = false;
	if(expected == 0.0) result = result || (fabs(actual) < tolerance);
	if(actual == 0.0) result = result || (fabs(expected) < tolerance);

	result = result || (fabs((expected + actual) / expected - 2.0) < tolerance);

    return result;
}

template< typename Expected, typename Actual, typename Tolerance >
void CheckClose(TestResults& results, Expected const& expected, Actual const& actual, Tolerance const& tolerance,
                TestDetails const& details)
{
    if (!AreClose(expected, actual, tolerance))
    {
        UnitTest::MemoryOutStream stream;
        stream << "Expected " << expected << " +/- " << tolerance << "% but was " << actual;

        results.OnTestFailure(details, stream.GetText());
    }
}

template< typename Expected, typename Actual, typename Tolerance >
void CheckCloseExp(TestResults& results, Expected const& expected, Actual const& actual, Tolerance const& tolerance,
                TestDetails const& details)
{
    if (!AreCloseExp(expected, actual, tolerance))
    {
        UnitTest::MemoryOutStream stream;
        stream << "Expected " << expected << " +/- " << tolerance << "% but was " << actual;

        results.OnTestFailure(details, stream.GetText());
    }
}


template< typename Expected, typename Actual >
void CheckContainerEqual(TestResults& results, Expected const& expected, Actual const& actual,
                TestDetails const& details)
{

    bool equal = true;

    if(expected.size() == actual.size())
	{
		auto ite = expected.begin();
		auto ita = actual.begin();

		while(ite != expected.end())
		{
			equal &= (*ite == *ita);

			ite++;
			ita++;
		}
	}
	else
	{
		equal = false;
	}

    if (!equal)
    {
        UnitTest::MemoryOutStream stream;

		stream << "Expected [ ";

		for(auto &value : expected)
            stream << value << " ";

		stream << "] but was [ ";

		for(auto &value : actual)
            stream << value << " ";

		stream << "]";

        results.OnTestFailure(details, stream.GetText());
    }
}


template< typename Expected, typename Actual >
void CheckArrayEqual(TestResults& results, Expected const& expected, Actual const& actual,
                int const count, TestDetails const& details)
{
    bool equal = true;
    for (int i = 0; i < count; ++i)
        equal &= (expected[i] == actual[i]);

    if (!equal)
    {
        UnitTest::MemoryOutStream stream;

		stream << "Expected [ ";

		for (int expectedIndex = 0; expectedIndex < count; ++expectedIndex)
            stream << expected[expectedIndex] << " ";

		stream << "] but was [ ";

		for (int actualIndex = 0; actualIndex < count; ++actualIndex)
            stream << actual[actualIndex] << " ";

		stream << "]";

        results.OnTestFailure(details, stream.GetText());
    }
}

template< typename Expected, typename Actual, typename Tolerance >
bool ArrayAreClose(Expected const& expected, Actual const& actual, int const count, Tolerance const& tolerance)
{
    bool equal = true;
    for (int i = 0; i < count; ++i)
        equal &= AreClose(expected[i], actual[i], tolerance);
    return equal;
}

template< typename Expected, typename Actual, typename Tolerance >
void CheckArrayClose(TestResults& results, Expected const& expected, Actual const& actual,
                   int const count, Tolerance const& tolerance, TestDetails const& details)
{
    bool equal = ArrayAreClose(expected, actual, count, tolerance);

    if (!equal)
    {
        UnitTest::MemoryOutStream stream;

        stream << "Expected [ ";
        for (int expectedIndex = 0; expectedIndex < count; ++expectedIndex)
            stream << expected[expectedIndex] << " ";
        stream << "] +/- " << tolerance << " but was [ ";

		for (int actualIndex = 0; actualIndex < count; ++actualIndex)
            stream << actual[actualIndex] << " ";
        stream << "]";

        results.OnTestFailure(details, stream.GetText());
    }
}

template< typename Expected, typename Actual, typename Tolerance >
void CheckArray2DClose(TestResults& results, Expected const& expected, Actual const& actual,
                   int const rows, int const columns, Tolerance const& tolerance, TestDetails const& details)
{
    bool equal = true;
    for (int i = 0; i < rows; ++i)
        equal &= ArrayAreClose(expected[i], actual[i], columns, tolerance);

    if (!equal)
    {
        UnitTest::MemoryOutStream stream;

        stream << "Expected [ ";

		for (int expectedRow = 0; expectedRow < rows; ++expectedRow)
        {
            stream << "[ ";
            for (int expectedColumn = 0; expectedColumn < columns; ++expectedColumn)
                stream << expected[expectedRow][expectedColumn] << " ";
            stream << "] ";
        }

		stream << "] +/- " << tolerance << " but was [ ";

		for (int actualRow = 0; actualRow < rows; ++actualRow)
        {
            stream << "[ ";
            for (int actualColumn = 0; actualColumn < columns; ++actualColumn)
                stream << actual[actualRow][actualColumn] << " ";
            stream << "] ";
        }

		stream << "]";

        results.OnTestFailure(details, stream.GetText());
    }
}

}

#endif
