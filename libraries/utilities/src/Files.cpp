////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     files.cpp (utilities)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Files.h"

// utiliites
#include "Exception.h"

// stl
#include <ios>
#include <locale>
#include <memory>
#include <sys/stat.h>

namespace ell
{
namespace utilities
{
    std::ifstream OpenIfstream(std::string filepath)
    {
        // open file
        auto fs = std::ifstream(filepath);

        // check that it opened
        if (!fs.is_open())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "error opening file " + filepath);
        }

        return fs;
    }

    std::ofstream OpenOfstream(std::string filepath)
    {
        // open file
        auto fs = std::ofstream(filepath);

        // check that it opened
        if (!fs.is_open())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "error opening file " + filepath);
        }

        return fs;
    }

    bool IsFileReadable(std::string filepath)
    {
        // open file
        std::ifstream fs(filepath);

        // check that it opened
        if (fs.is_open())
        {
            return true;
        }

        return false;
    }

    bool IsFileWritable(std::string filepath)
    {
        // open file
        std::ofstream fs(filepath);

        // check that it opened
        if (fs.is_open())
        {
            return true;
        }

        return false;
    }

    bool FileExists(std::string filepath)
    {
        struct stat buf;
        if (stat(filepath.c_str(), &buf) != -1) 
        {
            return (buf.st_mode & S_IFDIR) == 0;
        }
        return false;
    }

    bool DirectoryExists(std::string path)
    {
        struct stat buf;
        if (stat(path.c_str(), &buf) != -1) 
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
}
}
