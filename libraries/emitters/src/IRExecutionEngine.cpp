////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRExecutionEngine.cpp (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRExecutionEngine.h"
#include "IRModuleEmitter.h"

// llvm
#include "llvm/Support/TargetSelect.h"

// stl
#include <sstream>

namespace ell
{
namespace emitters
{

    void FatalErrorHandler(void* userData, const std::string& reason, bool genCrashDiag)
    {
        std::string msg = "llvm fatal error: " + reason;
        throw emitters::EmitterException(emitters::EmitterError::unexpected, msg);
    }

    IRExecutionEngine::IRExecutionEngine(IRModuleEmitter&& module)
        : IRExecutionEngine(module.TransferOwnership())
    {
    }

    IRExecutionEngine::IRExecutionEngine(std::unique_ptr<llvm::Module> pModule)
    {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();

        _pBuilder = std::make_unique<llvm::EngineBuilder>(std::move(pModule));
        _pBuilder->setEngineKind(llvm::EngineKind::JIT).setUseOrcMCJITReplacement(false);

        static bool installed = false;
        if (!installed)
        {
            installed = true;
            llvm::remove_fatal_error_handler();
            llvm::install_fatal_error_handler(&FatalErrorHandler, nullptr);
        }
    }

    void IRExecutionEngine::SelectTarget(const llvm::Triple& targetTriple, const std::string& cpuArchitecture, const std::string& cpuName, const std::vector<std::string>& attributes)
    {
        llvm::SmallVector<std::string, 4> attrs;
        for (auto attribute : attributes)
        {
            attrs.push_back(attribute);
        }
        _pBuilder->selectTarget(targetTriple, cpuArchitecture, cpuName, attrs);
    }

    void IRExecutionEngine::AddModule(std::unique_ptr<llvm::Module> pModule)
    {
        assert(pModule != nullptr);
        EnsureEngine();
        _pEngine->addModule(std::move(pModule));
    }

    uint64_t IRExecutionEngine::GetFunctionAddress(const std::string& name)
    {
        EnsureEngine();
        return _pEngine->getFunctionAddress(name);
    }

    uint64_t IRExecutionEngine::ResolveFunctionAddress(const std::string& name)
    {
        auto functionAddress = GetFunctionAddress(name);
        if (functionAddress == 0)
        {
            throw EmitterException(EmitterError::functionNotFound);
        }
        return functionAddress;
    }

    DynamicFunction IRExecutionEngine::GetMain()
    {
        return reinterpret_cast<DynamicFunction>(GetFunctionAddress("main"));
    }

    DynamicFunction IRExecutionEngine::ResolveMain()
    {
        auto mainFunction = GetMain();
        if (mainFunction == nullptr)
        {
            throw EmitterException(EmitterError::functionNotFound);
        }
        return mainFunction;
    }

    void IRExecutionEngine::RunMain()
    {
        auto mainFunction = ResolveMain();
        // Call the function
        mainFunction();
    }

    void IRExecutionEngine::EnsureEngine()
    {
        if (!_pEngine)
        {
            auto pEngine = _pBuilder->create();
            _pEngine.reset(pEngine);
        }
    }
}
}
