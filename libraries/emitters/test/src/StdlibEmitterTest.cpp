////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StdlibEmitterTest.cpp (emitters_test)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdlibEmitterTest.h"

#include <emitters/include/EmitterTypes.h>
#include <emitters/include/IRBlockRegion.h>
#include <emitters/include/IREmitter.h>
#include <emitters/include/IRExecutionEngine.h>
#include <emitters/include/IRFunctionEmitter.h>
#include <emitters/include/IRModuleEmitter.h>

#include <testing/include/testing.h>

#include <utilities/include/Unused.h>

#include <string>

using namespace ell;
using namespace ell::emitters;

using Int64CharPointerFunction = char* (*)(int64_t);
using VoidCharPointerFunction = void (*)(char*);
//
// Malloc
//
void TestIRMallocFunction()
{
    CompilerOptions options;
    IRModuleEmitter module("CompilableIRMallocFunction", options);

    std::string functionTestMalloc = "TestMalloc";
    std::string functionTestFree = "TestFree";
    char* block = nullptr;
    {
        NamedVariableTypeList args;
        args.push_back({ "size", VariableType::Int64 });
        auto function = module.BeginFunction(functionTestMalloc, VariableType::Int64, args);
        LLVMValue arg = function.GetFunctionArgument("size");
        auto result = function.Malloc(module.GetIREmitter().Type(VariableType::Char8Pointer), arg);
        function.Return(function.CastPointerToInt(result, VariableType::Int64));
        module.EndFunction();
    }
    {
        NamedVariableTypeList args;
        args.push_back({ "block", VariableType::Char8Pointer });
        auto function = module.BeginFunction(functionTestFree, VariableType::Void, args);
        LLVMValue arg = function.GetFunctionArgument("block");
        function.Free(arg);
        module.EndFunction();
    }

    IRExecutionEngine executionEngine(std::move(module));
    Int64CharPointerFunction compiledMallocFunction = (Int64CharPointerFunction)executionEngine.ResolveFunctionAddress(functionTestMalloc);
    block = compiledMallocFunction(800);

    // make sure block is addressable, test will crash if this fails...
    ::memset(block, 0, 800);

    // make sure we can free the block
    VoidCharPointerFunction compiledFreeFunction = (VoidCharPointerFunction)executionEngine.ResolveFunctionAddress(functionTestFree);
    compiledFreeFunction(block);

    // TBD: Note if I call 'free(block)' it crashes... not sure why
}
