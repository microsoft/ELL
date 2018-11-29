////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Emittable_functions_util.h (emittable_functions)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <emitters/include/IRModuleEmitter.h>

#include <value/include/EmitterContext.h>
#include <value/include/LLVMContext.h>

#include <memory>
#include <vector>

namespace ell
{

struct TestLLVMContext : public ell::value::LLVMContext
{
    TestLLVMContext(std::unique_ptr<ell::emitters::IRModuleEmitter> emitter) :
        LLVMContext(*emitter),
        _emitter(std::move(emitter))
    {}

    void DebugDump() { _emitter->DebugDump(); }

private:
    std::unique_ptr<ell::emitters::IRModuleEmitter> _emitter;
};

void PrintIR(TestLLVMContext& context);

std::vector<std::unique_ptr<ell::value::EmitterContext>> GetContexts();

} // namespace ell
