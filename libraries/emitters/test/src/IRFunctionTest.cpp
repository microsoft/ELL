////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRFunctionTest.cpp (emitters_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRFunctionTest.h"

#include <emitters/include/CompilableFunction.h>
#include <emitters/include/CompilerOptions.h>
#include <emitters/include/EmitterException.h>
#include <emitters/include/EmitterTypes.h>
#include <emitters/include/IRBlockRegion.h>
#include <emitters/include/IREmitter.h>
#include <emitters/include/IRExecutionEngine.h>
#include <emitters/include/IRFunctionEmitter.h>
#include <emitters/include/IRLocalScalar.h>
#include <emitters/include/IRModuleEmitter.h>
#include <emitters/include/Variable.h>

#include <testing/include/testing.h>

#include <iostream>
#include <memory>
#include <ostream>
#include <string>

using namespace ell;
using namespace ell::emitters;

//
// Helpers
//
class CompilablePlusFiveFunction : public CompilableFunction<double(double)>
{
public:
    double Compute(double x) const override
    {
        return x + 5.0;
    }

    IRLocalScalar Compile(IRFunctionEmitter& function, IRLocalScalar x) const override
    {
        return x + 5.0;
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

    IRAddFunction<double> func;

    std::string functionName = func.GetRuntimeTypeName();

    NamedVariableTypeList args;
    args.push_back({ "x", VariableType::Double });
    args.push_back({ "y", VariableType::Double });
    auto function = module.BeginFunction(functionName, VariableType::Double, args);

    IRLocalScalar xArg = function.LocalScalar(function.GetFunctionArgument("x"));
    IRLocalScalar yArg = function.LocalScalar(function.GetFunctionArgument("y"));
    auto result = func.Compile(function, xArg, yArg);
    function.Return(result);
    module.EndFunction();

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

void TestCompilableFunction()
{
    CompilerOptions options;
    IRModuleEmitter module("CompilableFunction", options);

    CompilablePlusFiveFunction func;

    std::string functionName = func.GetRuntimeTypeName();

    NamedVariableTypeList args;
    args.push_back({ "x", VariableType::Double });
    auto function = module.BeginFunction(functionName, VariableType::Double, args);

    IRLocalScalar arg = function.LocalScalar(function.GetFunctionArgument("x"));
    auto result = func.Compile(function, arg);
    function.Return(result);
    module.EndFunction();

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

void TestStringCompareFunction()
{
    CompilerOptions options;
    IRModuleEmitter module("StringCompareModule", options);

    auto strcompare = module.GetRuntime().GetStringCompareFunction();
    std::string name = strcompare->getName();

    IRExecutionEngine executionEngine(std::move(module));
    auto compiledFunction = executionEngine.GetFunction<int(const char*, const char*)>(name);

    int u = compiledFunction("eleph", "elephant");
    int v = compiledFunction("elephant", "eleph");
    int x = compiledFunction("elephant", "");
    int y = compiledFunction("elephant", "orange");
    int z = compiledFunction("elephant", "elephant");

    testing::ProcessTest("Testing string comparison function",
                         testing::IsEqual(u, 0) && testing::IsEqual(v, 0) && testing::IsEqual(x, 0) && testing::IsEqual(y, 0) &&
                             testing::IsEqual(z, 1));
}

void TestAllocaPlacement()
{
    CompilerOptions options;
    IRModuleEmitter module("TestAllocasInEntryBlock", options);

    NamedVariableTypeList args;
    args.push_back({ "x", VariableType::Double });
    args.push_back({ "y", VariableType::Double });
    auto function = module.BeginFunction("test", VariableType::Double, args);

    // Get args
    IRLocalScalar x = function.LocalScalar(function.GetFunctionArgument("x"));
    IRLocalScalar y = function.LocalScalar(function.GetFunctionArgument("y"));

    // Compute a value
    auto result = x + y;

    // Create a local variable (alloca)
    auto resultVar = function.Variable(VariableType::Double);
    function.Store(resultVar, result);

    function.For(10, [&](IRFunctionEmitter& function, IRLocalScalar i) {
        // Create a new local variable
        auto temp = function.Variable(VariableType::Int32);
        function.Store(temp, 2 * i);
    });

    auto temp2 = function.Variable(VariableType::Double);
    function.Store(temp2, function.Literal<double>(0.0));

    function.Return(result);
    module.EndFunction();

    bool ok = true;
    bool firstBlock = true;
    for (auto& bb : *function.GetFunction())
    {
        if (firstBlock)
        {
            bool foundNonAlloca = false;
            // check that the only allocas in this block are at the very beginning
            for (auto& inst : bb)
            {
                if (llvm::isa<llvm::AllocaInst>(inst))
                {
                    if (foundNonAlloca)
                    {
                        ok = false;
                    }
                }
                else
                {
                    foundNonAlloca = true;
                }
            }
            firstBlock = false;
        }
        else
        {
            // check that there are no allocas in this block at all
            for (auto& inst : bb)
            {
                if (llvm::isa<llvm::AllocaInst>(inst))
                {
                    ok = false;
                }
            }
        }
    }
    testing::ProcessTest("Testing alloca placement", ok);
}