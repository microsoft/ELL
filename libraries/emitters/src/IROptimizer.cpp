//////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IROptimizer.cpp (emitters)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IROptimizer.h"
#include "IRModuleEmitter.h"
#include "LLVMInclude.h"

// llvm
#include <llvm/IR/Module.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Transforms/IPO/PassManagerBuilder.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>

// stl
#include <memory>

namespace ell
{
namespace emitters
{
    using namespace llvm;

    IROptimizer::IROptimizer(IRModuleEmitter& module)
        : _module(module), _functionPasses(module.GetLLVMModule())
    {
    }

    void IROptimizer::AddStandardPasses()
    {
        _functionPasses.add(llvm::createVerifierPass());

        auto targetMachine = _module.GetTargetMachine();
        llvm::PassManagerBuilder builder;
        builder.OptLevel = 3;
        builder.SizeLevel = 0;
        builder.Inliner = llvm::createFunctionInliningPass(builder.OptLevel, builder.SizeLevel);
        builder.LoopVectorize = true;
        builder.SLPVectorize = true;
        builder.populateFunctionPassManager(_functionPasses);
        builder.populateModulePassManager(_modulePasses);

        if (targetMachine)
        {
            builder.addExtension(llvm::PassManagerBuilder::EP_EarlyAsPossible, [&](const llvm::PassManagerBuilder&, llvm::legacy::PassManagerBase& passManager) {
                targetMachine->addEarlyAsPossiblePasses(passManager);
            });
        }
    }

    void IROptimizer::OptimizeFunction(llvm::Function* pFunction)
    {
        assert(pFunction != nullptr);
        _functionPasses.run(*pFunction);
    }

    void IROptimizer::OptimizeModule(llvm::Module* pModule)
    {
        _modulePasses.run(*pModule);
    }
}
}
