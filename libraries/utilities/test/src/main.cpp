////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (utilities_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "XMLSerialization_test.h"
#include "Variant_test.h"
#include "ISerializable_test.h"

// utilities
#include "IIterator.h"
#include "StlIterator.h"
#include "TransformIterator.h"
#include "ParallelTransformIterator.h"
#include "Format.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <numeric>
#include <thread>

void testIteratorAdapter()
{
    // utilities::StlIterator test
    std::vector<int> vec { 1, 2, 3, 4, 5, 6 };
    auto it = utilities::MakeStlIterator(vec.begin(), vec.end());
    
    bool passed = true;
    int index = 0;
    while(it.IsValid())
    {
        passed = passed && (it.Get() == vec[index]);
        it.Next();
        index++;
    }
            
    testing::ProcessTest("utilities::StlIterator.Get", passed);
    testing::ProcessTest("utilities::StlIteratorlength", index == vec.size());
}

float twoPointFiveTimes(int x)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return float(2.5*x);
}
 
template <typename FuncType>
std::chrono::milliseconds::rep timeIt(FuncType fn)
{
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    fn();
    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

class MillisecondTimer
{
public:
    MillisecondTimer() : _start(std::chrono::system_clock::now()), _running(true) {}
    void Start()
    {
        _start = std::chrono::system_clock::now();
        _running = true;
    }

    void Stop()
    {
        _end = std::chrono::system_clock::now();
        _running = false;
    }

    std::chrono::milliseconds::rep Elapsed()
    {
        if (_running)
        {
            _end = std::chrono::system_clock::now();
        }

        return std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start).count();
    }


private:
    std::chrono::system_clock::time_point _start;
    std::chrono::system_clock::time_point _end;
    bool _running;
};

void testTransformIterator()
{
    std::vector<int> vec(64);
    std::iota(vec.begin(), vec.end(), 5);

    auto srcIt = utilities::MakeStlIterator(vec.begin(), vec.end());
    auto transIt = MakeTransformIterator(srcIt, twoPointFiveTimes);

    MillisecondTimer timer;
    bool passed = true;
    int index = 0;
    while(transIt.IsValid())
    {
        passed = passed && transIt.Get() == float(2.5*vec[index]);
        transIt.Next();
        index++;
    }

    testing::ProcessTest("utilities::TransformIterator.Get", passed);
    auto elapsed = timer.Elapsed();
    std::cout << "Elapsed time: " << elapsed << " ms" << std::endl;
}

void testParallelTransformIterator()
{
    std::vector<int> vec(64);
    std::iota(vec.begin(), vec.end(), 5);

    auto srcIt = utilities::MakeStlIterator(vec.begin(), vec.end());
    auto transIt = MakeParallelTransformIterator(srcIt, twoPointFiveTimes);

    bool passed = true;
    MillisecondTimer timer;
    int index = 0;
    while(transIt.IsValid())
    {
        passed = passed && transIt.Get() == float(2.5*vec[index]);
        transIt.Next();
        index++;
    }
    testing::ProcessTest("utilities::ParallelTransformIterator.Get", passed);
    auto elapsed = timer.Elapsed();
    std::cout << "Elapsed time: " << elapsed << " ms" << std::endl;
}

template <typename ... Args>
void testMatchFormat(utilities::MatchResult expectedResult, const char* content, const char* format, Args ...args)
{
    auto result = utilities::MatchFormat(content, format, args...);
    testing::ProcessTest("utilities::Format:MatchFormat", result == expectedResult);
}

void testMatchFormat()
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
    testMatchFormat(MatchResult::success, "integer 123 and float -33.3", "integer % and % %", int(), Match("float") , double());

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


/// Runs all tests
///
int main()
{
    try
    {
        // misc tests
        testIteratorAdapter();
        testTransformIterator();
        testParallelTransformIterator();
        testMatchFormat();

        // XMLSerialization tests
        XMLSerializationTest();
        TypeFactoryTest();

        // Variant tests
        TestVariant();

        // TestJsonSerializer();
        // TestJsonDeserializer();

        TestXmlSerializer();
        TestXmlDeserializer();
    }
    catch(utilities::Exception& exception)
    {
        std::cerr << "ERROR, got EMLL exception. Message: " << exception.GetMessage() << std::endl;
        throw;
    }

    if (testing::DidTestFail())
    {
        return 1;
    }
    return 0;
}
