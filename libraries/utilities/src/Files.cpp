////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Files.cpp (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Files.h"
#include "Exception.h"
#include "StringUtil.h"

// stl
#include <algorithm>
#include <codecvt>
#include <cstdlib>
#include <ios>
#include <locale>
#include <memory>
#include <sys/stat.h>
#ifndef _WIN32
#include <unistd.h>
#endif

namespace ell
{
namespace utilities
{
    std::ifstream OpenIfstream(const std::string& filepath)
    {
#ifdef WIN32
        // open file
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        std::wstring wide_path = converter.from_bytes(filepath);
        auto fs = std::ifstream(wide_path);
#else
        // open file
        auto fs = std::ifstream(filepath);

#endif
        // check that it opened
        if (!fs.is_open())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "error opening file " + filepath);
        }

        return fs;
    }

    std::ofstream OpenOfstream(const std::string& filepath)
    {
#ifdef WIN32
        // open file
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        std::wstring wide_path = converter.from_bytes(filepath);
        auto fs = std::ofstream(wide_path);
#else
        // open file
        auto fs = std::ofstream(filepath);
#endif
        // check that it opened
        if (!fs.is_open())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "error opening file " + filepath);
        }

        return fs;
    }

    bool IsFileReadable(const std::string& filepath)
    {
#ifdef WIN32
        // open file
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        std::wstring wide_path = converter.from_bytes(filepath);
        std::ifstream fs(wide_path);
#else
        // open file
        std::ifstream fs(filepath);
#endif
        // check that it opened
        if (fs.is_open())
        {
            return true;
        }

        return false;
    }

    bool IsFileWritable(const std::string& filepath)
    {
#ifdef WIN32
        // open file
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        std::wstring wide_path = converter.from_bytes(filepath);
        std::ofstream fs(wide_path);
#else
        // open file
        std::ofstream fs(filepath);
#endif
        // check that it opened
        if (fs.is_open())
        {
            return true;
        }

        return false;
    }

    bool FileExists(const std::string& filepath)
    {
#ifdef WIN32
        struct _stat64i32 buf;
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        std::wstring wide_path = converter.from_bytes(filepath);
        int rc = _wstat(wide_path.c_str(), &buf);
#else
        struct stat buf;
        int rc = stat(filepath.c_str(), &buf);
#endif
        if (rc != -1)
        {
            return (buf.st_mode & S_IFDIR) == 0;
        }
        return false;
    }

    bool DirectoryExists(const std::string& path)
    {
#ifdef WIN32
        struct _stat64i32 buf;
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        std::wstring wide_path = converter.from_bytes(path);
        int rc = _wstat(wide_path.c_str(), &buf);
#else
        struct stat buf;
        int rc = stat(path.c_str(), &buf);
#endif
        if (rc != -1)
        {
            return (buf.st_mode & S_IFDIR) == S_IFDIR;
        }
        return false;
    }

    std::string GetFileExtension(const std::string& filepath, bool toLowercase)
    {
        auto dotPos = filepath.find_last_of('.');
        if (dotPos == std::string::npos)
        {
            return "";
        }
        else
        {
            auto ext = filepath.substr(dotPos + 1);
            if (toLowercase)
            {
                return ext;
            }
            else
            {
                return ext;
            }
        }
    }

    std::string RemoveFileExtension(const std::string& filepath)
    {
        auto filename = filepath.substr(filepath.find_last_of("/\\") + 1);
        auto dotPos = filename.find_last_of('.');
        if (dotPos == std::string::npos)
        {
            return filename;
        }
        else
        {
            auto ext = filename.substr(0, dotPos);
            return ext;
        }
    }

    // PORTABILITY should be replaced by C++17 filesystem when available
#ifdef WIN32
    std::string path_separator = "\\";
#else
    std::string path_separator = "/";
#endif

    std::string GetFileName(const std::string& filepath)
    {
        // PORTABILITY should be replaced by C++17 filesystem when available
        return filepath.substr(filepath.find_last_of(path_separator) + 1);
    }

    std::string GetDirectoryPath(const std::string& filepath)
    {
        size_t pos = filepath.find_last_of(path_separator);
        if (pos == std::string::npos)
        {
            return "";
        }
        auto path = filepath.substr(0, pos);
        return path;
    }


    std::string JoinPaths(const std::string& path1, const std::string& path2)
    {
        return JoinPaths(path1, { path2 });
    }

    std::string JoinPaths(const std::string& path, std::initializer_list<std::string> toAdd)
    {
        std::vector<std::string> list(toAdd.begin(), toAdd.end());
        return JoinPaths(path, list);
    }

    inline bool ends_with(std::string const & value, std::string const & ending)
    {
        if (ending.size() > value.size()) return false;
        return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
    }

    std::string JoinPaths(const std::string& path, std::vector<std::string> toAdd)
    {
        std::string result = path;
        for (const auto& current : toAdd)
        {
            if (current.length() == 0 && result.length() == 0)
            {
                // special case for paths that start with a slash.
                result = path_separator;
            }
            else if (current.length() > 0)
            {
                if (result.length() > 0 && !ends_with(result, path_separator))
                {
                    result += path_separator;
                }
                result += current;
            }
        }
        return result;
    }

    std::vector<std::string> SplitPath(const std::string& path)
    {
        std::vector<std::string> result;
        size_t start = 0;
        size_t pos = path.find_first_of(path_separator, start);
        while (pos != std::string::npos)
        {
            auto part = path.substr(start, pos - start);
            result.push_back(part);
            start = pos + path_separator.size();
            pos = path.find_first_of(path_separator, start);
        }
        if (start < path.length()) 
        {
            auto part = path.substr(start, path.length() - start);
            result.push_back(part);
        }
        return result;
    }

    void EnsureDirectoryExists(const std::string& path)
    {

        if (!DirectoryExists(path))
        {

            // mkdir can only do one directory level at a time, so here we
            // recurrsively walk up the path making sure all intermediate paths exist.
            std::string dirName = GetFileName(path);
            std::string parentPath = GetDirectoryPath(path);
            if (parentPath != "")
            {
                EnsureDirectoryExists(parentPath);
            }

#ifdef WIN32
            // Windows requires UTF-16 for unicode path support.
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
            std::wstring wide_path = converter.from_bytes(path);
            int rc = _wmkdir(wide_path.c_str());
#else
            // Linux can do unicode file names in utf-8.
            int rc = mkdir(path.c_str(), 0777); // Note: Keep the prepended zero -- 0777 is 777 in octal.
#endif
            if (rc != 0)
            {
                ell::utilities::Exception(ell::utilities::FormatString("mkdir failed with error code %d", errno));
            }
        }
    }

    std::string GetWorkingDirectory()
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
        const int max_path = 8192;
        int rc = 0;
        std::string utf8wd;
#ifdef WIN32
        wchar_t path[max_path];
        if (NULL == _wgetcwd(path, max_path))
        {
            rc = errno;
        }
        else
        {
            // convert to utf-8
            std::wstring cwd(path);
            utf8wd = converter.to_bytes(cwd);
        }
#else
        char path[max_path];
        if (getcwd(path, max_path) == nullptr)
        {
            rc = errno;
        }
        utf8wd = std::string(path);
#endif
        if (rc != 0)
        {
            ell::utilities::Exception(ell::utilities::FormatString("error getting current working directory: %d", rc));
        }
        return utf8wd;
    }


    std::string FindExecutable(const std::string& name)
    {
#ifdef WIN32
#pragma warning(disable : 4996)
        std::string path = getenv("PATH");
        char separator = ';';
#else
        std::string path = getenv("PATH");
        char separator = ':';
#endif
        std::vector<std::string> paths = Split(path, separator);
        for (auto ptr = paths.begin(), end = paths.end(); ptr != end; ptr++)
        {
            std::string fullPath = JoinPaths(*ptr, name);
            if (FileExists(fullPath))
            {
                return *ptr;
            }
        }
        throw ell::utilities::Exception("Could not find '" + name + "' in your PATH environment");
    }

}
}
