////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InvokePython.cpp (pythonPlugins)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "InvokePython.h"
#include "Files.h"
#include "Exception.h"
#include <string>
#include <vector>
#include <stdexcept>

#if defined(PYTHON_FOUND)

#if defined(_DEBUG)
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>


using namespace ell::utilities;

class PyHandle 
{
    PyObject* _obj;
public:
    PyHandle(PyObject* o)
    {
        _obj = o;
    }
    ~PyHandle() 
    {
        // exception safe cleanup of python objects
        if (_obj != nullptr) 
        {
            Py_DECREF(_obj);
        }
    }
    PyObject* operator*() 
    {
        return _obj;
    }
    PyObject* steal() 
    {
        PyObject* result = _obj;
        _obj = nullptr;
        return result;
    }
};

class PythonRuntime 
{
public:
    PythonRuntime() 
    {
        std::string pythonExe = "python";
#ifdef WIN32
        pythonExe = "python.exe";
#endif
        path = FindExecutable(pythonExe);
        std::string tail = GetFileName(path);
        if (tail == "bin")
        {
            // on linux python is in a 'bin' dir and we want the parent of that.
            path = GetDirectoryPath(path);
        }

        std::wstring wide(path.begin(), path.end());

        // set the location of libs so python can initialize correctly
        Py_SetPythonHome((wchar_t*)wide.c_str());
        Py_Initialize();
    }
    ~PythonRuntime()
    {
        // exception safe cleanup python runtime.
        Py_Finalize();
    }

    std::string path;
};

std::vector<double> ExecutePythonScript(const std::string& filePath, const std::vector<std::string>& args)
{
    std::vector<PyObject*> methodArgs;
    std::vector<double> result;
    
    PythonRuntime runtime;

    PyHandle argv = PyList_New(0);

    for (std::string arg : args)
    {
        PyObject* pyarg = PyUnicode_FromString(arg.c_str());
        PyList_Append(*argv, pyarg);
        Py_DECREF(pyarg); // list should have done it's own Py_INCREF
    }

    std::string name = ell::utilities::GetFileName(filePath);
    name = ell::utilities::RemoveFileExtension(name);
    std::string modulePath = ell::utilities::GetDirectoryPath(filePath);

    // append modulePath to sys.path so the import will work.
    PyObject* sysPath = PySys_GetObject("path"); // borrowed reference
    {
        PyHandle pyarg = PyUnicode_FromString(modulePath.c_str());
        PyList_Append(sysPath, *pyarg);
    }

    // Now import the python module from disk
    PyHandle moduleName = PyUnicode_FromString(name.c_str());
    PyHandle module = PyImport_Import(*moduleName);
    if (*module == nullptr)
    {
        ell::utilities::Exception("Error importing Python module '" + name + "' using '" + runtime.path + "'");
    }

    // args must be in a tuple
    PyHandle py_args_tuple = PyTuple_New(1);
    PyTuple_SetItem(*py_args_tuple, 0, argv.steal()); //stolen

    // find the 'main' function
    PyHandle mainFunction = PyObject_GetAttrString(*module, (char*)"main");
    if (*mainFunction == nullptr)
    {
        ell::utilities::Exception("Error missing 'main' function in Python module '" + name + "'");
    }

    // call main with the args
    PyHandle myResult = PyObject_CallObject(*mainFunction, *py_args_tuple);
    if (*myResult == nullptr)
    {
        ell::utilities::Exception("Return value from 'main' function in Python module '" + name + "' is null, it should be a list of floating point numbers");
    }

    // make sure returned object is a list.
    if (!PyList_Check(*myResult))
    {
        ell::utilities::Exception("Return value from 'main' function in Python module '" + name + "' is not a list");
    }

    // should get back a list of floats.    
    for (size_t i = 0, n = PyList_Size(*myResult); i < n; i++)
    {
        PyObject* item = PyList_GetItem(*myResult, i); // borrowed reference
        if (item != nullptr)
        {
            double x = PyFloat_AsDouble(item);
            result.push_back(x);
        }
    }

    return result;
}

#else // defined(PYTHON_FOUND)

#include <string>
#include <vector>
#include <stdexcept>

std::vector<double> ExecutePythonScript(const std::string& filePath, const std::vector<std::string>&)
{
    std::string name = ell::utilities::GetFileName(filePath);
    throw ell::utilities::Exception("Cannot run python plugin '" + name + "' because ELL was not built with python support");
}

#endif // defined(PYTHON_FOUND)
