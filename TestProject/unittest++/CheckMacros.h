#ifndef UNITTEST_CHECKMACROS_H
#define UNITTEST_CHECKMACROS_H

#include "Checks.h"
#include "AssertException.h"
#include "MemoryOutStream.h"
#include "TestDetails.h"
#include "CurrentTest.h"

#include <stdexcept>

#ifdef CHECK
    #error UnitTest++ redefines CHECK
#endif

#ifdef CHECK_EQUAL
	#error UnitTest++ redefines CHECK_EQUAL
#endif

#ifdef CHECK_CLOSE
	#error UnitTest++ redefines CHECK_CLOSE
#endif

#ifdef CHECK_ARRAY_EQUAL
	#error UnitTest++ redefines CHECK_ARRAY_EQUAL
#endif

#ifdef CHECK_ARRAY_CLOSE
	#error UnitTest++ redefines CHECK_ARRAY_CLOSE
#endif

#ifdef CHECK_ARRAY2D_CLOSE
	#error UnitTest++ redefines CHECK_ARRAY2D_CLOSE
#endif

#define CHECK(value) \
    do \
    { \
        try { \
			UnitTest::Check(value, #value, *UnitTest::CurrentTest::Results(), UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__));\
        } \
        catch (std::exception &e) { \
			std::string failure = "Unhandled exception in CHECK(" #value ")"; \
			failure += e.what(); \
            UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__), failure.c_str()); \
        } \
    } while (0)

#define CHECK_EQUAL(expected, actual) \
    do \
    { \
        try { \
            UnitTest::CheckEqual(*UnitTest::CurrentTest::Results(), expected, actual, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__)); \
        } \
        catch (std::exception &e) { \
			std::string failure = "Unhandled exception in CHECK_EQUAL(" #expected ", " #actual ")"; \
			failure += e.what(); \
            UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__), failure.c_str()); \
        } \
    } while (0)

#define CHECK_NOT_EQUAL(notExpected, actual) \
    do \
    { \
        try { \
            UnitTest::CheckNotEqual(*UnitTest::CurrentTest::Results(), notExpected, actual, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__)); \
        } \
        catch (std::exception &e) { \
			std::string failure = "Unhandled exception in CHECK_NOT_EQUAL(" #notExpected ", " #actual ")"; \
			failure += e.what(); \
            UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__), failure.c_str()); \
        } \
    } while (0)

#define CHECK_NULL(actual) \
    do \
    { \
        try { \
            UnitTest::CheckEqual(*UnitTest::CurrentTest::Results(), (void*)nullptr, actual, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__)); \
        } \
        catch (std::exception &e) { \
            std::string failure = "Unhandled exception in CHECK_NULL(" #actual ")"; \
			failure += e.what(); \
            UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__), failure.c_str()); \
        } \
    } while(0)

#define CHECK_NOT_NULL(actual) \
    do \
    { \
        try { \
            UnitTest::CheckNotEqual(*UnitTest::CurrentTest::Results(), (void*)nullptr, actual, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__)); \
        } \
        catch (std::exception &e) { \
			std::string failure = "Unhandled exception in CHECK_NOT_NULL(" #actual ")"; \
			failure += e.what(); \
            UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__), failure.c_str()); \
        } \
        if(actual == nullptr) return; \
    } while (0)


#define CHECK_CLOSE(expected, actual, tolerance) \
    do \
    { \
        try { \
            UnitTest::CheckClose(*UnitTest::CurrentTest::Results(), expected, actual, tolerance, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__)); \
        } \
        catch (std::exception &e) { \
			std::string failure = "Unhandled exception in CHECK_CLOSE(" #expected ", " #actual ")"; \
			failure += e.what(); \
            UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__), failure.c_str()); \
        } \
    } while (0)

#define CHECK_CLOSE_EXP(expected, actual, tolerance) \
    do \
    { \
        try { \
            UnitTest::CheckCloseExp(*UnitTest::CurrentTest::Results(), expected, actual, tolerance, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__)); \
        } \
        catch (std::exception &e) { \
			std::string failure = "Unhandled exception in CHECK_CLOSE_EXP(" #expected ", " #actual ")"; \
			failure += e.what(); \
            UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__), failure.c_str()); \
        } \
    } while (0)

#define CHECK_CONTAINER_EQUAL(expected, actual) \
    do \
    { \
        try { \
            UnitTest::CheckContainerEqual(*UnitTest::CurrentTest::Results(), expected, actual, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__)); \
        } \
        catch (std::exception &e) { \
			std::string failure = "Unhandled exception in CHECK_CONTAINER_EQUAL(" #expected ", " #actual ")"; \
			failure += e.what(); \
            UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__), failure.c_str()); \
        } \
    } while (0)

#define CHECK_ARRAY_EQUAL(expected, actual, count) \
    do \
    { \
        try { \
            UnitTest::CheckArrayEqual(*UnitTest::CurrentTest::Results(), expected, actual, count, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__)); \
        } \
        catch (std::exception &e) { \
			std::string failure = "Unhandled exception in CHECK_ARRAY_EQUAL(" #expected ", " #actual ")"; \
			failure += e.what(); \
            UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__), failure.c_str()); \
        } \
    } while (0)

#define CHECK_ARRAY_CLOSE(expected, actual, count, tolerance) \
    do \
    { \
        try { \
            UnitTest::CheckArrayClose(*UnitTest::CurrentTest::Results(), expected, actual, count, tolerance, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__)); \
        } \
        catch (std::exception &e) { \
			std::string failure = "Unhandled exception in CHECK_ARRAY_CLOSE(" #expected ", " #actual ")"; \
			failure += e.what(); \
            UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__), failure.c_str()); \
        } \
    } while (0)

#define CHECK_ARRAY2D_CLOSE(expected, actual, rows, columns, tolerance) \
    do \
    { \
        try { \
            UnitTest::CheckArray2DClose(*UnitTest::CurrentTest::Results(), expected, actual, rows, columns, tolerance, UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__)); \
        } \
        catch (std::exception &e) { \
			std::string failure = "Unhandled exception in CHECK_ARRAY_CLOSE(" #expected ", " #actual ")"; \
			failure += e.what(); \
            UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__), failure.c_str()); \
        } \
    } while (0)


#define CHECK_THROW(expression, ExpectedExceptionType) \
    do \
    { \
        bool caught_ = false; \
        try { expression; } \
        catch (ExpectedExceptionType const&) { caught_ = true; } \
        catch (...) {} \
        if (!caught_) \
            UnitTest::CurrentTest::Results()->OnTestFailure(UnitTest::TestDetails(*UnitTest::CurrentTest::Details(), __FILE__, __LINE__), "Expected exception: \"" #ExpectedExceptionType "\" not thrown"); \
    } while(0)

#define CHECK_ASSERT(expression) \
    CHECK_THROW(expression, UnitTest::AssertException);

#endif
