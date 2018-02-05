////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InvokePython.h (pythonPlugins)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>
#include <vector>

///<summary>Execute the given python script, passing the given arguments as sys.argv and getting back an array of floating point numbers</summary>
std::vector<double> ExecutePythonScript(const std::string& filePath, const std::vector<std::string>& args);

