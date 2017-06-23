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

using namespace ell;

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

void PrintHeader(emitters::IRModuleEmitter& module);
void PrintHeader(model::IRCompiledMap& compiledMap);

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

template <typename InputType, typename OutputType>
void VerifyMapOutput(const model::DynamicMap& map, std::vector<std::vector<InputType>>& signal, std::vector<std::vector<OutputType>>& expectedOutput, const std::string& name);

void PrintDiagnostics(emitters::IRDiagnosticHandler& handler);

template <typename InputType>
struct InputCallbackTester
{
    void Initialize(const std::vector<std::vector<InputType>>& inputSeries);
    bool InputCallback(std::vector<InputType>& input);
    bool InputCallback(InputType* input);

private:
    typename std::vector<std::vector<InputType>>::const_iterator begin;
    typename std::vector<std::vector<InputType>>::const_iterator cur;
    typename std::vector<std::vector<InputType>>::const_iterator end;
};

#include "../tcc/ModelTestUtilities.tcc"
