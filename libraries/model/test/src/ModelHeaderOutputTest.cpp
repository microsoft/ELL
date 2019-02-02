////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelHeaderOutputTest.cpp (nodes_test)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelHeaderOutputTest.h"

#include <model/include/IRCompiledMap.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/InputNode.h>
#include <model/include/Map.h>
#include <model/include/Model.h>
#include <model/optimizer/include/OptimizationPass.h>
#include <model/include/OutputNode.h>

#include <emitters/include/IRHeaderWriter.h>
#include <emitters/include/IRModuleEmitter.h>
#include <emitters/include/IRSwigInterfaceWriter.h>

#include <nodes/include/ClockNode.h>
#include <nodes/include/ConstantNode.h>
#include <nodes/include/SinkNode.h>
#include <nodes/include/SourceNode.h>
#include <nodes/include/SumNode.h>

#include <testing/include/testing.h>

#include <iostream>
#include <sstream>
#include <string>

using namespace ell;
using namespace ell::emitters;

namespace
{

model::IRMapCompiler CreateMapCompiler(const std::string& moduleName, const std::string& mapFunctionName)
{
    model::MapCompilerOptions settings;
    settings.moduleName = moduleName;
    settings.mapFunctionName = mapFunctionName;
    settings.compilerSettings.optimize = true;

    return model::IRMapCompiler(settings);
}

template <typename ElementType>
model::IRCompiledMap GetCompiledMapWithCallbacks(model::IRMapCompiler& compiler)
{
    // Create the map
    constexpr nodes::TimeTickType lagThreshold = 200;
    constexpr nodes::TimeTickType interval = 40;
    constexpr size_t inputSize = 1000;

    model::Model model;

    auto inputNode = model.AddNode<model::InputNode<nodes::TimeTickType>>(1);
    inputNode->GetMetadata().SetEntry("name", std::string("currentTime")); // this input is a clock tick!
    auto clockNode = model.AddNode<nodes::ClockNode>(inputNode->output, interval, lagThreshold, "MyLagNotificationCallback");
    auto sourceNode = model.AddNode<nodes::SourceNode<ElementType>>(clockNode->output, inputSize, "MyDataCallback");
    auto conditionNode = model.AddNode<nodes::ConstantNode<bool>>(true);
    auto sumNode = model.AddNode<nodes::SumNode<ElementType>>(sourceNode->output);
    auto sinkNode = model.AddNode<nodes::SinkNode<ElementType>>(sumNode->output, conditionNode->output, "MyResultsCallback");
    sinkNode->GetMetadata().SetEntry("name", std::string("sum")); // to test that header emit can pick this up.
    auto outputNode = model.AddNode<model::OutputNode<ElementType>>(sinkNode->output);
    auto map = model::Map(model, { { "time", inputNode } }, { { "output", outputNode->output } });

    return compiler.Compile(map);
}

template <typename ElementType>
model::IRCompiledMap GetCompiledMapNoCallbacks(model::IRMapCompiler& compiler)
{
    // Create the map
    constexpr size_t inputSize = 1000;

    model::Model model;

    auto inputNode = model.AddNode<model::InputNode<ElementType>>(inputSize);
    auto sumNode = model.AddNode<nodes::SumNode<ElementType>>(inputNode->output);
    auto outputNode = model.AddNode<model::OutputNode<ElementType>>(sumNode->output);
    auto map = model::Map(model, { { "input", inputNode } }, { { "output", outputNode->output } });

    return compiler.Compile(map);
}

// Empty class used for type information only
template <typename ElementType>
struct CallbackBase
{
};

// Not using typeid because returns compiler-specific names, but we need well-known names here
template <typename ElementType>
const char* ToTypeString();
template <>
const char* ToTypeString<double>()
{
    return "double";
}
template <>
const char* ToTypeString<float>()
{
    return "float";
}
template <>
const char* ToTypeString<std::vector<double>>()
{
    return "DoubleVector";
}
template <>
const char* ToTypeString<std::vector<float>>()
{
    return "FloatVector";
}
template <>
const char* ToTypeString<CallbackBase<double>>()
{
    return "DoubleCallbackBase";
}
template <>
const char* ToTypeString<CallbackBase<float>>()
{
    return "FloatCallbackBase";
}

template <typename ElementType>
void TestCppHeader()
{
    auto mapCompiler = CreateMapCompiler("TestModule", "TestModule_Predict");
    auto compiledMap = GetCompiledMapWithCallbacks<ElementType>(mapCompiler);
    auto& module = compiledMap.GetModule();

    std::stringstream ss;
    WriteModuleHeader(ss, module);
    WriteModuleCppWrapper(ss, module);
    auto result = ss.str();

    std::string typeString = ToTypeString<ElementType>();
    std::string timeTypeString = ToTypeString<nodes::TimeTickType>();

    testing::ProcessTest("Testing C predict function", testing::IsTrue(std::string::npos != result.find(std::string("void TestModule_Predict(void* context, ") + timeTypeString + "* currentTime, " + typeString + "* output);")));
    testing::ProcessTest("Testing C++ wrapper 1", testing::IsTrue(std::string::npos != result.find("class TestModuleWrapper")));
    testing::ProcessTest("Testing C++ wrapper 2", testing::IsTrue(std::string::npos != result.find(std::string("void TestModule_MyDataCallback(void* context, ") + typeString + "* input)")));
    testing::ProcessTest("Testing C++ wrapper 3", testing::IsTrue(std::string::npos != result.find(std::string("void TestModule_MyResultsCallback(void* context, ") + typeString + "* sum)")));
    testing::ProcessTest("Testing C++ wrapper 4", testing::IsTrue(std::string::npos != result.find("TestModule_Predict(this, &time, nullptr);")));

    testing::ProcessTest("Checking that all delimiters are processed", testing::IsTrue(std::string::npos == result.find("@@")));

    if (testing::DidTestFail())
    {
        std::cout << result << std::endl;
    }
}

void TestCppHeader()
{
    TestCppHeader<double>();
    TestCppHeader<float>();
}

template <typename ElementType>
void TestSwigCallbackInterfaces()
{
    auto mapCompiler = CreateMapCompiler("TestModuleWithCallbacks", "step");
    auto compiledMap = GetCompiledMapWithCallbacks<ElementType>(mapCompiler);
    auto& module = compiledMap.GetModule();

    std::stringstream ss;
    WriteModuleSwigInterface(ss, module, "TestModuleWithCallbacks.h");
    auto result = ss.str();
    std::string typeString = ToTypeString<ElementType>();
    std::string vectorTypeString = ToTypeString<std::vector<ElementType>>();
    std::string callbackTypeString = ToTypeString<CallbackBase<ElementType>>();

    // Sanity tests
    testing::ProcessTest("Testing generated python code 1", testing::IsTrue(std::string::npos != result.find("%pythoncode %{")));
    testing::ProcessTest("Testing generated python code 3", testing::IsTrue(std::string::npos != result.find("def predict(inputData")));

    testing::ProcessTest("Testing shape wrappers 1", testing::IsTrue(std::string::npos != result.find("ell::api::math::TensorShape get_default_input_shape() {")));
    testing::ProcessTest("Testing shape wrappers 2", testing::IsTrue(std::string::npos != result.find("TestModuleWithCallbacks_GetInputShape(0, &s);")));
    testing::ProcessTest("Testing shape wrappers 3", testing::IsTrue(std::string::npos != result.find("ell::api::math::TensorShape get_default_output_shape() {")));
    testing::ProcessTest("Testing shape wrappers 4", testing::IsTrue(std::string::npos != result.find("TestModuleWithCallbacks_GetOutputShape(0, &s);")));

    testing::ProcessTest("Checking that all delimiters are processed", testing::IsTrue(std::string::npos == result.find("@@")));

    if (testing::DidTestFail())
    {
       std::cout << result << std::endl;
    }
}

void TestSwigCallbackInterfaces()
{
    TestSwigCallbackInterfaces<double>();
    TestSwigCallbackInterfaces<float>();
}

template <typename ElementType>
void TestSwigNoCallbackInterfaces()
{
    auto mapCompiler = CreateMapCompiler("TestModule", "TestModule_predict");
    auto compiledMap = GetCompiledMapNoCallbacks<ElementType>(mapCompiler);
    auto& module = compiledMap.GetModule();

    std::stringstream ss;
    WriteModuleSwigInterface(ss, module, "TestModule.h");
    auto result = ss.str();
    std::string vectorTypeString = ToTypeString<std::vector<ElementType>>();

    // Sanity tests
    testing::ProcessTest("Testing generated python code 1", testing::IsTrue(std::string::npos != result.find("def predict(inputData: 'numpy.ndarray') -> \"numpy.ndarray\":")));
    testing::ProcessTest("Checking that all delimiters are processed", testing::IsTrue(std::string::npos == result.find("@@")));

    if (testing::DidTestFail())
    {
       std::cout << result << std::endl;
    }
}

void TestSwigNoCallbackInterfaces()
{
    TestSwigNoCallbackInterfaces<double>();
    TestSwigNoCallbackInterfaces<float>();
}

} // namespace

//
// Invoke all the tests
//

void TestModelHeaderOutput()
{
    TestCppHeader();
    TestSwigCallbackInterfaces();
    TestSwigNoCallbackInterfaces();
}
