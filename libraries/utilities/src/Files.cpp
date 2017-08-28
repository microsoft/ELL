////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     files.cpp (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Files.h"
#include "StringUtil.h"
// utiliites
#include "Exception.h"

// stl
#include <ios>
#include <locale>
#include <memory>
#include <sys/stat.h>
#include <codecvt>
#ifndef _WIN32
#include <unistd.h>
#endif

namespace ell
{
    namespace utilities
    {
        std::ifstream OpenIfstream(std::string filepath)
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

        std::ofstream OpenOfstream(std::string filepath)
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

        bool IsFileReadable(std::string filepath)
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

        bool IsFileWritable(std::string filepath)
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

        bool FileExists(std::string filepath)
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

        bool DirectoryExists(std::string path)
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


        std::string GetFileExtension(std::string filepath, bool toLowercase)
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

        std::string RemoveFileExtension(std::string filepath)
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

        std::string GetFileName(std::string filepath)
        {
            // PORTABILITY should be replaced by C++17 filesystem when available
            return filepath.substr(filepath.find_last_of("/\\") + 1);
        }

        std::string GetDirectoryPath(std::string filepath)
        {
            size_t pos = filepath.find_last_of("/\\");
            if (pos == std::string::npos)
            {
                return "";
            }
            auto path = filepath.substr(0, pos);
            return path;
        }

        std::string JoinPaths(std::string path1, std::string path2)
        {
            if (path1 == "")
            {
                return path2;
            }

            if (path2 == "")
            {
                return path1;
            }

            // PORTABILITY should be replaced by C++17 filesystem when available
#ifdef WIN32
            return path1 + "\\" + path2;
#else
            return path1 + "/" + path2;
#endif
        }

        void EnsureDirectoryExists(std::string path) {

            if (!DirectoryExists(path)) {

                // mkdir can only do one directory level at a time, so here we
                // recurrsively walk up the path making sure all intermediate paths exist.
                std::string dirName = GetFileName(path);
                std::string parentPath = GetDirectoryPath(path);
                if (dirName != "")
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
                int rc = mkdir(path.c_str(), 0x777);
#endif
                if (rc != 0)
                {
                    throw std::runtime_error(ell::utilities::stringf("mkdir failed with error code %d", errno));
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
            if (nullptr == getcwd(path, max_path)) {
                rc = errno;
            }
            utf8wd = std::string(path);
#endif
            if (rc != 0) {
                throw std::runtime_error(ell::utilities::stringf("error getting current working directory: %d", rc));
            }
            return utf8wd;
        }
    }
}
