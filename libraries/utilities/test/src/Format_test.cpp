
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Format_test.cpp (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Format_test.h"

// utilities
#include "Format.h"

// testing
#include "testing.h"

namespace ell
{
template <typename... Args>
void testMatchFormat(utilities::MatchResult expectedResult, const char* content, const char* format, Args... args)
{
    auto result = utilities::MatchFormat(content, format, args...);
    testing::ProcessTest("utilities::Format:MatchFormat", result == expectedResult);
}

void TestMatchFormat()
{
    using utilities::MatchResult;

    // standard match
    testMatchFormat(MatchResult::success, "integer 123 and float -33.3", "integer % and float %", int(), double());

    // tolerate extra spaces in content, in places where format has a single space
    testMatchFormat(MatchResult::success, "integer    123   and float    -33.3     ", "integer % and float %", int(), double());

    // tolerate extra spaces in content, with the whitespace symbol ^
    testMatchFormat(MatchResult::success, "       integer    123   and float    -33.3     ", "^integer % and float %", int(), double());

    // tolerate extra whitespace in content, tabs
    testMatchFormat(MatchResult::success, "integer \t   123 \t  and float    -33.3   \t  ", "integer % and float %", int(), double());

    // tolerate extra spaces in format
    testMatchFormat(MatchResult::success, " integer 123 and float -33.3 ", "     integer  %  and     float  %    ", int(), double());

    using utilities::Match;

    // match a string
    testMatchFormat(MatchResult::success, "integer 123 and float -33.3", "integer % and % %", int(), Match("float"), double());

    // match two strings in a row
    testMatchFormat(MatchResult::success, "integer hello float", "integer %% float", Match("he"), Match("llo"));

    // match two strings in a row with optional whitespace
    testMatchFormat(MatchResult::success, "integer hello float", "integer %^^% float", Match("he"), Match("llo"));

    // early end of content
    testMatchFormat(MatchResult::earlyEndOfContent, "integer 123 and ", "integer % and float %", int(), double());

    // early end of content
    testMatchFormat(MatchResult::earlyEndOfContent, "integer 123 and float -33.3", "integer % and float %X", int(), double());

    // mismatch
    testMatchFormat(MatchResult::mismatch, "integer 123 and X float -33.3", "integer % and float %", int(), double());

    // mismatch
    testMatchFormat(MatchResult::mismatch, "Xinteger 123 and float -33.3", "integer % and float %", int(), double());

    // mismatch
    testMatchFormat(MatchResult::mismatch, "integer 123 and float -33.3", "integer % and X float %", int(), double());

    // mismatch
    testMatchFormat(MatchResult::mismatch, "integer 123 and float -33.3", "Xinteger % and float %", int(), double());

    // parser error
    testMatchFormat(MatchResult::parserError, "integer X and float -33.3", "integer % and float %", int(), double());
}
}
