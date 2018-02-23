////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRFunctionTest.cpp (emitters_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRFunctionTest.h"

// emitters
#include "CompilerOptions.h"
#include "CompilableIRFunction.h"
#include "EmitterException.h"
#include "EmitterTypes.h"
#include "IRBlockRegion.h"
#include "IREmitter.h"
#include "IRExecutionEngine.h"
#include "IRFunctionEmitter.h"
#include "IRModuleEmitter.h"
#include "Variable.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

using namespace ell;
using namespace ell::emitters;

//
// Helpers
//
class CompilablePlusFiveFunction : public CompilableIRFunction<double, double>
{
public:
    double Compute(double x) const override
    {
        return x + 5.0;
    }

    llvm::Value* Compile(IRFunctionEmitter& function, llvm::Value* x) const override
    {
        llvm::Value* sum = function.Operator(emitters::GetAddForValueType<double>(), x, function.Literal<double>(5.0));
        return sum;
    }

    std::string GetRuntimeTypeName() const override { return "PlusFive"; }
};

using UnaryScalarDoubleFunction = double (*)(double);
using BinaryScalarDoubleFunction = double (*)(double, double);

//
// Tests
//
void TestIRAddFunction()
{
    CompilerOptions options;
    IRModuleEmitter module("CompilableIRAddFunction", options);
    module.DeclarePrintf();

    IRAddFunction<double> func;

    std::string functionName = func.GetRuntimeTypeName();

    NamedVariableTypeList args;
    args.push_back({ "x", VariableType::Double });
    args.push_back({ "y", VariableType::Double });
    auto function = module.BeginFunction(functionName, VariableType::Double, args);

    llvm::Value* xArg = function.GetFunctionArgument("x");
    llvm::Value* yArg = function.GetFunctionArgument("y");
    auto result = func.Compile(function, xArg, yArg);
    function.Return(result);
    module.EndFunction();

    module.DebugDump();

    IRExecutionEngine executionEngine(std::move(module));
    auto compiledFunction = (BinaryScalarDoubleFunction)executionEngine.ResolveFunctionAddress(functionName);
    std::vector<double> xData({ 1.1, 2.1, 3.1, 4.1, 5.1 });
    std::vector<double> yData({ 10.0, 11.0, 12.0, 13.0, 14.0 });
    std::vector<double> computedResult;
    std::vector<double> compiledResult;
    for (size_t index = 0; index < xData.size(); ++index)
    {
        auto x = xData[index];
        auto y = yData[index];
        computedResult.push_back(func.Compute(x, y));
        compiledResult.push_back(compiledFunction(x, y));
    }
    testing::ProcessTest("Testing compilable add function", testing::IsEqual(computedResult, compiledResult));
}

void TestIRFunction()
{
    CompilerOptions options;
    IRModuleEmitter module("CompilableIRFunction", options);
    module.DeclarePrintf();

    CompilablePlusFiveFunction func;

    std::string functionName = func.GetRuntimeTypeName();

    NamedVariableTypeList args;
    args.push_back({ "x", VariableType::Double });
    auto function = module.BeginFunction(functionName, VariableType::Double, args);

    llvm::Value* arg = function.GetFunctionArgument("x");
    auto result = func.Compile(function, arg);
    function.Return(result);
    module.EndFunction();

    module.DebugDump();

    IRExecutionEngine executionEngine(std::move(module));
    UnaryScalarDoubleFunction compiledFunction = (UnaryScalarDoubleFunction)executionEngine.ResolveFunctionAddress(functionName);
    std::vector<double> data({ 1.1, 2.1, 3.1, 4.1, 5.1 });
    std::vector<double> computedResult;
    std::vector<double> compiledResult;
    for (auto x : data)
    {
        computedResult.push_back(func.Compute(x));
        compiledResult.push_back(compiledFunction(x));
    }
    testing::ProcessTest("Testing compilable function", testing::IsEqual(computedResult, compiledResult));
}

