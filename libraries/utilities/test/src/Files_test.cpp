
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Files_test.cpp (utilities)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Files_test.h"

#include <utilities/include/Files.h>
#include <utilities/include/StringUtil.h>

#include <testing/include/testing.h>

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>

#ifdef WIN32
#include <filesystem>
namespace fs = std::filesystem;
#endif

namespace ell
{
//
// Tests
//
void TestStringf()
{
    testing::ProcessTest("Stringf with args", utilities::FormatString("test %d is %s", 10, "fun") == "test 10 is fun");
}

void TestJoinPaths(const std::string& basePath)
{
    std::vector<std::string> parts = utilities::SplitPath(basePath);
    std::string result = utilities::JoinPaths("", parts);

    // normalize path for platform differences (just for testing)
    std::string norm = basePath;
    std::replace(norm.begin(), norm.end(), '\\', '/');
    std::replace(result.begin(), result.end(), '\\', '/');

    std::cout << "TestJoinPaths: basePath=" << norm << std::endl;
    std::cout << "TestJoinPaths: result=" << result << std::endl;
    testing::ProcessTest("JoinPaths", norm == result);
}

std::string GetUnicodeTestPath(const std::string& basePath, const std::string& utf8test)
{
    std::string testing = utilities::JoinPaths(basePath, "Testing");
    std::string unicode = utilities::JoinPaths(testing, "Unicode");

    std::string testdir = utilities::JoinPaths(unicode, utf8test);

    return testdir;
}

void TestUnicodePaths(const std::string& basePath)
{
#ifdef WIN32
    // chinese for 'test'
    std::wstring test(L"\u6D4B\u8bd5");
    auto path = fs::path(test);
    auto utf8test = path.u8string();
#else
    std::string utf8test{ "测试" };
#endif
    auto testdir = GetUnicodeTestPath(basePath, utf8test);
    std::cout << "writing test output to " << testdir << std::endl;

    // bugbug: the rolling build for Linux is giving us EACCESSDENIED on this testdir for some reason...
    utilities::EnsureDirectoryExists(testdir);
    testing::ProcessTest("Unicode paths", utilities::DirectoryExists(testdir));

    std::string testContent = "this is a test";
    int testContentLength = static_cast<int>(testContent.size());

    // chinese for 'banana'
#ifdef WIN32
    std::wstring banana(L"\u9999\u8549");
    std::string utf8banana = fs::path(banana).u8string();
#else
    std::string utf8banana{ "香蕉" };
#endif
    utf8banana += ".txt";

    std::string testfile = utilities::JoinPaths(testdir, utf8banana);
    {
        auto outputStream = utilities::OpenOfstream(testfile);
        outputStream.write(testContent.c_str(), testContentLength);
    }
    {
        auto inputStream = utilities::OpenIfstream(testfile);
        char buffer[100];
        inputStream.read(buffer, testContentLength);
        buffer[testContentLength] = '\0';
        std::string actual(buffer);

        testing::ProcessTest("Unicode file names", actual == testContent);
    }
}

} // namespace ell
