#if defined(PYTHON_FOUND)

#if defined(_DEBUG)
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "InvokePython.h"
#include "Files.h"

using namespace ell::utilities;

std::vector<std::string> split(const std::string& s, char separator)
{
    std::vector<std::string> output;

    std::string::size_type prev_pos = 0, pos = 0;

    while ((pos = s.find(separator, pos)) != std::string::npos)
    {
        std::string substring(s.substr(prev_pos, pos - prev_pos));

        output.push_back(substring);

        prev_pos = ++pos;
    }
    if (prev_pos < pos)
    {
        output.push_back(s.substr(prev_pos, pos - prev_pos));
    }
    return output;

}

std::string FindExecutable(const std::string& name)
{
    std::string path = getenv("PATH");
    char separator = ':';
#ifdef WIN32
    separator = ';';
#endif
    std::vector<std::string> paths = split(path, separator);
    for (auto ptr = paths.begin(), end = paths.end(); ptr != end; ptr++)
    {
        std::string fullPath = JoinPaths(*ptr, name);
        if (FileExists(fullPath))
        {
            return *ptr;
        }
    }
    throw std::runtime_error( "Could not find '" + name + "' in your PATH environment");
}

void ExecutePythonScript(const std::string& script, const std::vector<std::string>& args)
{
    size_t argc = args.size();
    auto argv = std::make_unique<wchar_t*[]>(argc);

    std::string pythonExe = "python";
#ifdef WIN32
    pythonExe = "python.exe";
#endif
    std::string path = FindExecutable(pythonExe);
    std::string tail = GetFileName(path);
    if (tail == "bin")
    {
        // on linux python is in a 'bin' dir and we want the parent of that.
        path = GetDirectoryPath(path);
    }
    //std::cout << "Found '" << pythonExe << "' home at " << path << std::endl;

    std::wstring wide(path.begin(), path.end());

    // set the location of libs so python can initialize correctly
    Py_SetPythonHome((wchar_t*)wide.c_str());
    Py_Initialize();

    std::transform(args.begin(), args.end(), argv.get(), [](const auto& s) { return Py_DecodeLocale(s.c_str(), nullptr); });

    PySys_SetArgvEx((int)argc, argv.get(), 0);
    PyRun_SimpleString(script.c_str());

    for (size_t i = 0; i < argc; i++)
    {
        PyMem_RawFree(argv[i]);
    }
    Py_Finalize();
}

#else // defined(PYTHON_FOUND)

#include <string>
#include <vector>
#include <stdexcept>

void ExecutePythonScript(const std::string&, const std::vector<std::string>&)
{
    throw std::runtime_error("When imageConverter project was built cmake could not find Python.h");
}
#endif // defined(PYTHON_FOUND)
