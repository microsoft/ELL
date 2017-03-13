////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelTestUtilities.h (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// model
#include "DynamicMap.h"
#include "IRCompiledMap.h"
#include "IRDiagnosticHandler.h"
#include "Port.h"

// testing
#include "testing.h"

// stl
#include <ostream>
#include <string>
#include <vector>

namespace ell
{
// RAII class for setting and restoring verbosity
class VerboseRegion
{
public:
    VerboseRegion(bool verbose = true);
    ~VerboseRegion();

private:
    bool _oldVerbose;
};

void SetVerbose(bool verbose);
bool IsVerbose();

template <typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v);

void PrintModel(const model::Model& model);
void PrintModel(const model::Model& model, const model::Node* output);

void PrintIR(emitters::IRModuleEmitter& module);
void PrintIR(model::IRCompiledMap& compiledMap);

model::Model GetSimpleModel();
model::Model GetComplexModel();

template <typename InputType, typename OutputType>
void PrintCompiledOutput(const model::DynamicMap& map, const model::IRCompiledMap& compiledMap, std::vector<std::vector<InputType>>& signal, const std::string& name);

template <typename InputType>
void PrintCompiledOutput(const model::DynamicMap& map, const model::IRCompiledMap& compiledMap, std::vector<std::vector<InputType>>& signal, const std::string& name);

template <typename InputType, typename OutputType>
void VerifyCompiledOutput(const model::DynamicMap& map, const model::IRCompiledMap& compiledMap, std::vector<std::vector<InputType>>& signal, const std::string& name);

template <typename InputType>
void VerifyCompiledOutput(const model::DynamicMap& map, const model::IRCompiledMap& compiledMap, std::vector<std::vector<InputType>>& signal, const std::string& name);

void PrintDiagnostics(emitters::IRDiagnosticHandler& handler);
}

#include "../tcc/ModelTestUtilities.tcc"
