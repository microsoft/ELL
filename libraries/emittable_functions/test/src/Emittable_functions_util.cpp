////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Emittable_functions_util.cpp (emittable_functions)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Emittable_functions_util.h"

#include <emitters/include/IRModuleEmitter.h>

#include <value/include/ComputeContext.h>
#include <value/include/LLVMContext.h>
#include <value/include/Value.h>

#include <memory>
#include <vector>

#define PRINT_IR 0

using namespace ell::emitters;
using namespace ell::utilities;
using namespace ell::value;

namespace ell
{

void PrintIR(TestLLVMContext& context)
{
#if PRINT_IR
    context.DebugDump();
#endif // PRINT_IR
}

std::vector<std::unique_ptr<EmitterContext>> GetContexts()
{
    std::vector<std::unique_ptr<EmitterContext>> contexts;
    contexts.push_back(std::make_unique<ComputeContext>("Emittable_functions_test"));
    contexts.push_back(std::make_unique<TestLLVMContext>(
        std::make_unique<IRModuleEmitter>("Emittable_functions_test", CompilerOptions{})));
    return contexts;
}

} // namespace ell
