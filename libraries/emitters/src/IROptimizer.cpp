////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IROptimizer.cpp (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IROptimizer.h"
#include <memory>
namespace ell
{
namespace emitters
{
    using namespace llvm;

    IRFunctionOptimizer::IRFunctionOptimizer(llvm::Module* pModule)
    {
        assert(pModule != nullptr);
        _pPasses = std::make_unique<legacy::FunctionPassManager>(pModule);
    }

    void IRFunctionOptimizer::AddStandardPasses()
    {
        AddInstructionCombiner();
        AddMemoryToRegisterPass();
        AddReassociation();
        AddCommonSubexpressionEliminator();
        AddControlFlowSimplification();
        AddDeadCodeEliminator();
        AddInstructionCombiner();
    }

    void IRFunctionOptimizer::AddInstructionCombiner()
    {
        _pPasses->add(llvm::createInstructionCombiningPass());
    }

    void IRFunctionOptimizer::AddDeadCodeEliminator()
    {
        _pPasses->add(llvm::createAggressiveDCEPass());
    }

    void IRFunctionOptimizer::AddReassociation()
    {
        _pPasses->add(llvm::createReassociatePass());
    }

    void IRFunctionOptimizer::AddMemoryToRegisterPass()
    {
        _pPasses->add(llvm::createPromoteMemoryToRegisterPass());
    }

    void IRFunctionOptimizer::AddCommonSubexpressionEliminator()
    {
        _pPasses->add(llvm::createGVNPass());
    }

    void IRFunctionOptimizer::AddControlFlowSimplification()
    {
        _pPasses->add(llvm::createCFGSimplificationPass());
    }

    void IRFunctionOptimizer::Run(llvm::Function* pFunction)
    {
        assert(pFunction != nullptr);
        if (!_initialized)
        {
            _pPasses->doFinalization();
            _initialized = true;
        }
        _pPasses->run(*pFunction);
    }

    //
    // IRModuleOptimizer
    //

    void IRModuleOptimizer::AddStandardPasses()
    {
    }

    void IRModuleOptimizer::Run(llvm::Module* pModule)
    {
        assert(pModule != nullptr);
        _passes.run(*pModule, _analysis);
    }
}
}
