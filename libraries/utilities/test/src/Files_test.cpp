
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Files_test.cpp (utilities)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Files_test.h"

// utilities
#include "StringUtil.h"
#include "Files.h"

// testing
#include "testing.h"

// stl
#include <string>
#include <cstring>
#include <codecvt>
#include <locale>
#ifndef _WIN32
#include <unistd.h>
#endif

namespace ell
{
    //
    // Tests
    //
    void TestStringf()
    {
        testing::ProcessTest("Stringf with no args", ell::utilities::stringf("test") == "test");
        testing::ProcessTest("Stringf with args", ell::utilities::stringf("test %d is %s", 10, "fun") == "test 10 is fun");
    }

    std::string GetUnicodeTestPath() 
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        const int max_path = 8192;
        int rc = 0;
        std::string utf8wd;
#ifdef WIN32
        wchar_t path[max_path];
        if (NULL == _wgetcwd(path, max_path)) {
            rc = errno;
        }
        else {
            // convert to utf-8
            std::wstring cwd(path);
            utf8wd = converter.to_bytes(cwd);
        }
#else
        char path[max_path];
        if (nullptr == getcwd(path, max_path)) {
            rc = errno;
        }
        utf8wd = std::string(path);
#endif
        if (rc != 0) {
            throw std::runtime_error(ell::utilities::stringf("error getting current working directory: %d", rc));
        }
        std::string testing = ell::utilities::JoinPaths(utf8wd, "Testing");
        std::string unicode = ell::utilities::JoinPaths(testing, "Unicode");

        // chinese for 'test'
        std::wstring test(L"\u6D4B\u8bd5");
        std::string utf8test = converter.to_bytes(test);

        std::string testdir = ell::utilities::JoinPaths(unicode, utf8test);
        return testdir;
    }

    void TestDirectories()
    {
        auto testdir = GetUnicodeTestPath();
        ell::utilities::EnsureDirectoryExists(testdir);
        testing::ProcessTest("Unicode paths", ell::utilities::DirectoryExists(testdir));
    }

    void TestUnicodePaths()
    {
        TestDirectories();

        auto testdir = GetUnicodeTestPath();
        std::string testContent = "this is a test";

        // chinese for 'banana.txt'
        std::wstring banana(L"\u9999\u8549.txt");
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        std::string utf8banana = converter.to_bytes(banana);
        std::string testfile = ell::utilities::JoinPaths(testdir, utf8banana);
        {
            auto outputStream = ell::utilities::OpenOfstream(testfile);
            outputStream.write(testContent.c_str(), testContent.size());
        } 
        {
            auto inputStream = ell::utilities::OpenIfstream(testfile);
            char buffer[100];
            inputStream.read(buffer, testContent.size());
            std::string actual(buffer);

            testing::ProcessTest("Unicode file names", actual == testContent);
        }

    }


} // end namespace
