////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelHeaderOutputTest.cpp (nodes_test)
//  Authors:  Lisa Ong
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelHeaderOutputTest.h"

// model
#include "IRCompiledMap.h"
#include "InputNode.h"
#include "Map.h"
#include "Model.h"
#include "OutputNode.h"

// emitters
#include "IRHeaderWriter.h"
#include "IRMapCompiler.h"
#include "IRModuleEmitter.h"
#include "IRSwigInterfaceWriter.h"

// nodes
#include "ClockNode.h"
#include "ConstantNode.h"
#include "SinkNode.h"
#include "SourceNode.h"
#include "SumNode.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <sstream>
#include <string>

using namespace ell;
using namespace ell::emitters;

template <typename ElementType>
model::IRCompiledMap GetCompiledMapWithCallbacks(
    const std::string& moduleName,
    const std::string& mapFunctionName)
{
    // Create the map
    constexpr nodes::TimeTickType lagThreshold = 200;
    constexpr nodes::TimeTickType interval = 40;
    constexpr size_t inputSize = 1000;

    model::Model model;

    auto inputNode = model.AddNode<model::InputNode<nodes::TimeTickType>>(1 /*currentTime*/);
    auto clockNode = model.AddNode<nodes::ClockNode>(inputNode->output, interval, lagThreshold, "MyLagNotificationCallback");
    auto sourceNode = model.AddNode<nodes::SourceNode<ElementType>>(clockNode->output, inputSize, "MyDataCallback");
    auto conditionNode = model.AddNode<nodes::ConstantNode<bool>>(true);
    auto sumNode = model.AddNode<nodes::SumNode<ElementType>>(sourceNode->output);
    auto sinkNode = model.AddNode<nodes::SinkNode<ElementType>>(sumNode->output, conditionNode->output, "MyResultsCallback");

    auto outputNode = model.AddNode<model::OutputNode<ElementType>>(sinkNode->output);
    auto map = model::Map(model, { { "time", inputNode } }, { { "output", outputNode->output } });

    model::MapCompilerOptions settings;
    settings.moduleName = moduleName;
    settings.mapFunctionName = mapFunctionName;
    settings.compilerSettings.optimize = true;

    model::IRMapCompiler compiler(settings);
    return compiler.Compile(map);
}

template <typename ElementType>
model::IRCompiledMap GetCompiledMapNoCallbacks(
    const std::string& moduleName,
    const std::string& mapFunctionName)
{
    // Create the map
    constexpr size_t inputSize = 1000;

    model::Model model;

    auto inputNode = model.AddNode<model::InputNode<ElementType>>(inputSize);
    auto sumNode = model.AddNode<nodes::SumNode<ElementType>>(inputNode->output);
    auto outputNode = model.AddNode<model::OutputNode<ElementType>>(sumNode->output);
    auto map = model::Map(model, { { "input", inputNode } }, { { "output", outputNode->output } });

    model::MapCompilerOptions settings;
    settings.moduleName = moduleName;
    settings.mapFunctionName = mapFunctionName;
    settings.compilerSettings.optimize = true;

    model::IRMapCompiler compiler(settings);
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
    auto compiledMap = GetCompiledMapWithCallbacks<ElementType>("TestModule", "TestModule_Predict");
    auto& module = compiledMap.GetModule();

    std::stringstream ss;
    WriteModuleHeader(ss, module);
    WriteModuleCppWrapper(ss, module);
    auto result = ss.str();

    std::string typeString = ToTypeString<ElementType>();
    std::string timeTypeString = ToTypeString<nodes::TimeTickType>();

    testing::ProcessTest("Testing C predict function", testing::IsTrue(std::string::npos != result.find(std::string("void TestModule_Predict(") + timeTypeString + " input0, " + typeString + "* output0);")));
    testing::ProcessTest("Testing C++ wrapper 1", testing::IsTrue(std::string::npos != result.find("class TestModule_PredictWrapper")));
    testing::ProcessTest("Testing C++ wrapper 2", testing::IsTrue(std::string::npos != result.find(std::string("int8_t TestModule_MyDataCallback(") + typeString + "* buffer")));
    testing::ProcessTest("Testing C++ wrapper 3", testing::IsTrue(std::string::npos != result.find(std::string("void TestModule_MyResultsCallback(") + typeString + "* buffer")));
    testing::ProcessTest("Testing C++ wrapper 4", testing::IsTrue(std::string::npos != result.find(std::string("void TestModule_Predict(const std::vector<") + typeString + ">& input, std::vector<" + typeString + ">& output)")));
    testing::ProcessTest("Testing C++ wrapper 5", testing::IsTrue(std::string::npos != result.find("TestModule_Predict(0.0, nullptr);")));

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
    auto compiledMap = GetCompiledMapWithCallbacks<ElementType>("TestModuleWithCallbacks", "step");
    auto& module = compiledMap.GetModule();

    std::stringstream ss;
    WriteModuleSwigInterface(ss, module, "TestModuleWithCallbacks.h");
    auto result = ss.str();
    std::string typeString = ToTypeString<ElementType>();
    std::string vectorTypeString = ToTypeString<std::vector<ElementType>>();
    std::string callbackTypeString = ToTypeString<CallbackBase<ElementType>>();

    // Sanity tests
    testing::ProcessTest("Testing generated python code 1", testing::IsTrue(std::string::npos != result.find("%pythoncode %{\nclass Model:")));
    testing::ProcessTest("Testing generated python code 2", testing::IsTrue(std::string::npos != result.find("self.predictor = TestModuleWithCallbacks_Predictor.GetInstance(self._my_data_callback,")));
    testing::ProcessTest("Testing generated python code 3", testing::IsTrue(std::string::npos != result.find("def _my_data_callback(self, data: '" + vectorTypeString + "') -> \"bool\":")));
    testing::ProcessTest("Testing generated python code 3", testing::IsTrue(std::string::npos != result.find("def my_data_callback(self) -> \"numpy.ndarray\":")));
    testing::ProcessTest("Testing generated python code 4", testing::IsTrue(std::string::npos != result.find(std::string("def my_results_callback(self, output: '" + vectorTypeString + "'):"))));
    testing::ProcessTest("Testing generated python code 5", testing::IsTrue(std::string::npos != result.find("def my_lag_notification_callback(self, lag):")));
    testing::ProcessTest("Testing generated python code 6", testing::IsTrue(std::string::npos != result.find("def step(self, current_time=0):")));

    testing::ProcessTest("Testing shape wrappers 1", testing::IsTrue(std::string::npos != result.find("ell::api::math::TensorShape get_default_input_shape() {")));
    testing::ProcessTest("Testing shape wrappers 2", testing::IsTrue(std::string::npos != result.find("TestModuleWithCallbacks_GetInputShape(0, &s);")));
    testing::ProcessTest("Testing shape wrappers 3", testing::IsTrue(std::string::npos != result.find("ell::api::math::TensorShape get_default_output_shape() {")));
    testing::ProcessTest("Testing shape wrappers 4", testing::IsTrue(std::string::npos != result.find("TestModuleWithCallbacks_GetOutputShape(0, &s);")));

    testing::ProcessTest("Testing wrapper macro", testing::IsTrue(std::string::npos != result.find(std::string("TestModuleWithCallbacks_Predictor, " + callbackTypeString + ", " + typeString + ", " + callbackTypeString + ", " + typeString + ", " + ToTypeString<CallbackBase<nodes::TimeTickType>>()))));

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
void TestSwigCallbackHeader()
{
    auto compiledMap = GetCompiledMapWithCallbacks<ElementType>("TestModuleWithCallbacks", "step");
    auto& module = compiledMap.GetModule();

    std::stringstream ss;
    WriteModuleSwigHeader(ss, module);
    auto result = ss.str();

    std::string typeString = ToTypeString<ElementType>();
    std::string timeTypeString = ToTypeString<nodes::TimeTickType>();

    // Sanity tests
    testing::ProcessTest("Testing generated C++ class 1", testing::IsTrue(std::string::npos != result.find(std::string("class TestModuleWithCallbacks_Predictor : public ell::api::CallbackForwarder<" + typeString + ", " + typeString + ">"))));
    testing::ProcessTest("Testing generated C++ class 2", testing::IsTrue(std::string::npos != result.find(std::string("ell::api::CallbackBase<" + typeString + ">& inputCallback"))));
    testing::ProcessTest("Testing generated C++ class 3", testing::IsTrue(std::string::npos != result.find(std::string("ell::api::CallbackBase<" + typeString + ">& outputCallback"))));
    testing::ProcessTest("Testing generated C++ class 4", testing::IsTrue(std::string::npos != result.find(std::string("ell::api::CallbackBase<" + timeTypeString + ">& lagCallback"))));

    testing::ProcessTest("Testing generated callback definitions 1", testing::IsTrue(std::string::npos != result.find(std::string("int8_t TestModuleWithCallbacks_MyDataCallback(" + typeString + "* input"))));
    testing::ProcessTest("Testing generated callback definitions 2", testing::IsTrue(std::string::npos != result.find(std::string("void TestModuleWithCallbacks_MyResultsCallback(" + typeString + "* output"))));
    testing::ProcessTest("Testing generated callback definitions 3", testing::IsTrue(std::string::npos != result.find(std::string("void TestModuleWithCallbacks_MyLagNotificationCallback(" + timeTypeString + " lag"))));

    testing::ProcessTest("Testing step function wrapper 1", testing::IsTrue(std::string::npos != result.find(std::string("void step(" + timeTypeString + " input, std::vector<" + typeString + ">& output)"))));
    testing::ProcessTest("Testing step function wrapper 2", testing::IsTrue(std::string::npos != result.find(std::string("step(input, &output[0]);"))));

    testing::ProcessTest("Checking that all delimiters are processed", testing::IsTrue(std::string::npos == result.find("@@")));

    if (testing::DidTestFail())
    {
        std::cout << result << std::endl;
    }
}

void TestSwigCallbackHeader()
{
    TestSwigCallbackHeader<double>();
    TestSwigCallbackHeader<float>();
}

template <typename ElementType>
void TestSwigNoCallbackInterfaces()
{
    auto compiledMap = GetCompiledMapNoCallbacks<ElementType>("TestModule", "TestModule_predict");
    auto& module = compiledMap.GetModule();

    std::stringstream ss;
    WriteModuleSwigInterface(ss, module, "TestModule.h");
    auto result = ss.str();
    std::string vectorTypeString = ToTypeString<std::vector<ElementType>>();

    // Sanity tests
    testing::ProcessTest("Testing generated python code 1", testing::IsTrue(std::string::npos != result.find("def predict(inputData: 'numpy.ndarray') -> \"numpy.ndarray\":")));
    testing::ProcessTest("Testing generated python code 2", testing::IsTrue(std::string::npos != result.find(std::string("results = " + vectorTypeString + "(get_default_output_shape().Size())"))));
    testing::ProcessTest("Testing generated python code 3", testing::IsTrue(std::string::npos != result.find("TestModule_predict(inputData, results)")));

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

template <typename ElementType>
void TestSwigNoCallbackHeader()
{
    auto compiledMap = GetCompiledMapNoCallbacks<ElementType>("TestModule", "TestModule_predict");
    auto& module = compiledMap.GetModule();

    std::stringstream ss;
    WriteModuleSwigHeader(ss, module);
    auto result = ss.str();
    std::string typeString = ToTypeString<ElementType>();

    // Sanity tests
    testing::ProcessTest("Testing predict function 1", testing::IsTrue(std::string::npos != result.find(std::string("void TestModule_predict(" + typeString + "* input0, " + typeString + "* output0);"))));
    testing::ProcessTest("Testing predict function 2", testing::IsTrue(std::string::npos != result.find(std::string("void TestModule_predict(const std::vector<" + typeString + ">& input, std::vector<" + typeString + ">& output)"))));
    testing::ProcessTest("Testing predict function 3", testing::IsTrue(std::string::npos != result.find(std::string("TestModule_predict(const_cast<" + typeString + "*>(&input[0]), &output[0]);"))));

    testing::ProcessTest("Testing shape function 1", testing::IsTrue(std::string::npos != result.find("void TestModule_GetInputShape(int32_t index, TensorShape* shape)")));
    testing::ProcessTest("Testing shape function 2", testing::IsTrue(std::string::npos != result.find("void TestModule_GetOutputShape(int32_t index, TensorShape* shape)")));

    testing::ProcessTest("Checking that all delimiters are processed", testing::IsTrue(std::string::npos == result.find("@@")));

    if (testing::DidTestFail())
    {
        std::cout << result << std::endl;
    }
}

void TestSwigNoCallbackHeader()
{
    TestSwigNoCallbackHeader<double>();
    TestSwigNoCallbackHeader<float>();
}

//
// Invoke all the tests
//

void TestModelHeaderOutput()
{
    TestCppHeader();
    TestSwigCallbackInterfaces();
    TestSwigCallbackHeader();
    TestSwigNoCallbackInterfaces();
    TestSwigNoCallbackHeader();
}
