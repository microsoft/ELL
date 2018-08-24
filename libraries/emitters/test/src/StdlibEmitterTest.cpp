////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StdlibEmitterTest.cpp (emitters_test)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "StdlibEmitterTest.h"

// emitters
#include "EmitterTypes.h"
#include "IRBlockRegion.h"
#include "IREmitter.h"
#include "IRExecutionEngine.h"
#include "IRFunctionEmitter.h"
#include "IRModuleEmitter.h"

// testing
#include "testing.h"

// utilities
#include "Unused.h"

// stl
#include <string>

using namespace ell;
using namespace ell::emitters;

using Int64CharPointerFunction = char*(*)(int64_t);
using VoidCharPointerFunction = void(*)(char*);
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
        function.Return(result);
        module.EndFunction();
    }
    {
        NamedVariableTypeList args;
        args.push_back({ "block", VariableType::Char8Pointer });
        auto function = module.BeginFunction(functionTestFree, VariableType::Int64, args);
        LLVMValue arg = function.GetFunctionArgument("block");
        function.Free(arg);
        module.EndFunction();
    }

    module.DebugDump();

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
