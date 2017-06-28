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
            EnsureClockGetTime();
        }
    }

    void IRExecutionEngine::EnsureClockGetTime()
    {
        auto machine = _pEngine->getTargetMachine();
        if (machine->getTargetTriple().isOSWindows())
        {
            llvm::Function* fun = _pEngine->FindFunctionNamed("clock_gettime");
            if (fun == nullptr)
            {

                // We normally assume there is a system function "clock_gettime" that provides high resolution times for profiling the emitted model.
                // But this function doesn't exist on windows.  So on Windows we implement this function to call the Win32 QueryPerformanceCounter API.
                // Like this:
                //
                //    int clock_gettime(int32_t clk_id, struct timespec *tp) {
                //        LARGE_INTEGER lp;
                //        LARGE_INTEGER freq;
                //        // this timer is in 100 nanosecond intervals.
                //        QueryPerformanceCounter(&lp);
                //        QueryPerformanceFrequency(&freq);
                //        double seconds = (double)lp.QuadPart / (double)freq.QuadPart;
                //        int32_t sec = (int32_t)seconds;
                //        tp->tv_nsec = (int32_t)((seconds - sec) * 10000000);
                //        tp->tv_sec = sec;
                //        return 0;
                //    }

                IRModuleEmitter module("clock_gettime_module");
                auto& context = module.GetLLVMContext();
                auto int32Type = llvm::Type::getInt32Ty(context);
                auto int64Type = llvm::Type::getInt64Ty(context);
                auto doubleType = llvm::Type::getDoubleTy(context);
                llvm::Type* tmFieldType = int32Type;
                VariableType tmFieldVarType = VariableType::Int32;
                llvm::Value* zero = llvm::ConstantInt::get(int32Type, 0);
                llvm::Value* hundredNanoSeconds = llvm::ConstantFP::get(doubleType, 10000000.0);

                llvm::StructType* timespecType = nullptr;
                if (machine->getTargetTriple().isArch64Bit())
                {
                    // These are really time_t and long
                    tmFieldType = int64Type;
                    tmFieldVarType = VariableType::Int64;
                }

                timespecType = llvm::StructType::create(context, { tmFieldType, tmFieldType }, "timespec");

                llvm::FunctionType* qpcProto = llvm::FunctionType::get(int32Type, { int64Type->getPointerTo() }, false);
                module.DeclareFunction("QueryPerformanceCounter", qpcProto);
                auto qpcFunction = module.GetFunction("QueryPerformanceCounter");
                module.DeclareFunction("QueryPerformanceFrequency", qpcProto);
                auto qpfFunction = module.GetFunction("QueryPerformanceFrequency");

                std::vector<llvm::Type*> args;
                args.push_back(int32Type);
                auto timeSpecPointerType = timespecType->getPointerTo();
                args.push_back(timeSpecPointerType);
                IRFunctionEmitter& emitter = module.BeginFunction("clock_gettime", int32Type, args);
                llvm::Function* func = emitter.GetFunction();

                // Get pointer to the timespec argument.
                auto iterator = func->arg_begin();
                iterator++; // get second argument
                llvm::Argument* timeSpecArg = &*iterator; // Get the timespec.

                // get access to the tv_sec and nano fields of the time struct argument.
                auto irBuilder = emitter.GetEmitter().GetIRBuilder();
                auto secondsPtr = irBuilder.CreateInBoundsGEP(timespecType, timeSpecArg, { emitter.Literal(0), emitter.Literal(0) });
                auto nanoPtr = irBuilder.CreateInBoundsGEP(timespecType, timeSpecArg, { emitter.Literal(0), emitter.Literal(1) });

                llvm::AllocaInst* timeVar = emitter.Variable(int64Type, "time");
                llvm::AllocaInst* freqVar = emitter.Variable(int64Type, "freq");

                // QueryPerformanceCounter(&lp);
                emitter.Call(qpcFunction, { timeVar });
                // QueryPerformanceFrequency(&freq);
                emitter.Call(qpfFunction, { freqVar });

                auto timeDouble = emitter.CastIntToFloat(emitter.Load(timeVar), VariableType::Double, true);
                auto freqDouble = emitter.CastIntToFloat(emitter.Load(freqVar), VariableType::Double, true);
                // double seconds = (double)time.QuadPart / (double)freq.QuadPart;
                llvm::Value* seconds = emitter.Operator(TypedOperator::divideFloat, timeDouble, freqDouble);

                // int32_t sec = (int64_t)seconds;
                llvm::Value* intSeconds = emitter.CastFloatToInt(seconds, VariableType::Int64);
                llvm::Value* floatSeconds = emitter.CastIntToFloat(intSeconds, VariableType::Double, true);

                //  tp->tv_nsec = (int32_t)((seconds - sec) * 10000000);
                llvm::Value* remainder = emitter.Operator(TypedOperator::subtractFloat, seconds, floatSeconds);
                llvm::Value* nanoseconds = emitter.Operator(TypedOperator::multiplyFloat, remainder, hundredNanoSeconds);
                llvm::Value* tv_nsec = emitter.CastFloatToInt(nanoseconds, tmFieldVarType);
                emitter.Store(nanoPtr, tv_nsec);

                // tp->tv_sec = sec;
                llvm::Value* tv_sec = emitter.CastFloatToInt(floatSeconds, tmFieldVarType);
                emitter.Store(secondsPtr, tv_sec);

                emitter.Return(zero);

                // now move the module to the engine.
                _pEngine->addModule(module.TransferOwnership());
            }
        }
    }
}
}
