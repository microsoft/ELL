////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRRuntime.cpp (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRRuntime.h"
#include "IRMetadata.h"
#include "IRModuleEmitter.h"

#include <iostream>
#include <time.h>

namespace ell
{
namespace emitters
{
    static const std::string& countName = "count";
    static const std::string& lVectorName = "pLVector";
    static const std::string& rVectorName = "pRVector";
    static const std::string& resultName = "pResult";

    static const std::string& dotProductFloatName = "DotProductF";
    static const std::string& dotProductIntName = "DotProduct";
    static const std::string& getTimeFunctionName = "GetTime";

    IRRuntime::IRRuntime(IRModuleEmitter& module)
        : _module(module)
    {
    }

    std::string IRRuntime::GetNamespacePrefix() const
    {
        return _module.GetModuleName();
    }

    llvm::Function* IRRuntime::EmitDotProductFunctionF()
    {
        auto functionName = GetNamespacePrefix() + "_" + dotProductFloatName;
        NamedVariableTypeList argList = { { countName, VariableType::Int32 },
                                          { lVectorName, VariableType::DoublePointer },
                                          { rVectorName, VariableType::DoublePointer },
                                          { resultName, VariableType::DoublePointer } };
        auto function = _module.BeginFunction(functionName, VariableType::Void, argList);
        function.IncludeInHeader();

        auto arguments = function.Arguments().begin();
        llvm::Argument& count = *arguments++;
        llvm::Argument& leftValue = *arguments++;
        llvm::Argument& rightValue = *arguments++;
        llvm::Argument& result = *arguments++;
        function.DotProductFloat(&count, &leftValue, &rightValue, &result);
        function.Return();
        _module.EndFunction();

        return function.GetFunction();
    }

    llvm::Function* IRRuntime::EmitDotProductFunction()
    {
        auto functionName = GetNamespacePrefix() + "_" + dotProductIntName;
        NamedVariableTypeList argList = { { countName, VariableType::Int32 },
                                          { lVectorName, VariableType::Int32Pointer },
                                          { rVectorName, VariableType::Int32Pointer },
                                          { resultName, VariableType::Int32Pointer } };
        auto function = _module.BeginFunction(functionName, VariableType::Void, argList);
        function.IncludeInHeader();

        auto arguments = function.Arguments().begin();
        llvm::Argument& count = *arguments++;
        llvm::Argument& leftValue = *arguments++;
        llvm::Argument& rightValue = *arguments++;
        llvm::Argument& result = *arguments++;
        function.DotProduct(&count, &leftValue, &rightValue, &result);
        function.Return();
        _module.EndFunction();
        return function.GetFunction();
    }

    llvm::Function* IRRuntime::ResolveCurrentTimeFunction(llvm::StructType* timespecType)
    {
        llvm::Function* function = nullptr;

        if (_module.GetCompilerParameters().targetDevice.IsWindows())
        {
            // We normally assume there is a system function "clock_gettime" that provides high resolution times for profiling the emitted model.
            // But this function doesn't exist on Windows.  So on Windows we implement this function to call the Win32 QueryPerformanceCounter API.
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

            auto& context = _module.GetLLVMContext();
            auto int32Type = llvm::Type::getInt32Ty(context);
            auto int64Type = llvm::Type::getInt64Ty(context);
            auto doubleType = llvm::Type::getDoubleTy(context);

            auto tmFieldType = timespecType->getElementType(0);
            const VariableType tmFieldVarType = tmFieldType == int64Type ? VariableType::Int64 : VariableType::Int32;

            auto zero = llvm::ConstantInt::get(int32Type, 0);
            auto hundredNanoSeconds = llvm::ConstantFP::get(doubleType, 10000000.0);

            llvm::FunctionType* qpcProto = llvm::FunctionType::get(int32Type, { int64Type->getPointerTo() }, false);
            _module.DeclareFunction("QueryPerformanceCounter", qpcProto);
            auto qpcFunction = _module.GetFunction("QueryPerformanceCounter");
            _module.DeclareFunction("QueryPerformanceFrequency", qpcProto);
            auto qpfFunction = _module.GetFunction("QueryPerformanceFrequency");

            std::vector<llvm::Type*> args;
            args.push_back(int32Type);
            auto timeSpecPointerType = timespecType->getPointerTo();
            args.push_back(timeSpecPointerType);
            IRFunctionEmitter& emitter = _module.BeginFunction("clock_gettime", int32Type, args);
            function = emitter.GetFunction();

            // Get pointer to the timespec argument.
            auto iterator = function->arg_begin();
            iterator++; // get second argument
            auto timeSpecArg = &*iterator; // Get the timespec.

            // get access to the tv_sec and nano fields of the time struct argument.
            auto irBuilder = emitter.GetEmitter().GetIRBuilder();
            auto secondsPtr = irBuilder.CreateInBoundsGEP(timespecType, timeSpecArg, { emitter.Literal(0), emitter.Literal(0) });
            auto nanoPtr = irBuilder.CreateInBoundsGEP(timespecType, timeSpecArg, { emitter.Literal(0), emitter.Literal(1) });

            auto timeVar = emitter.Variable(int64Type, "time");
            auto freqVar = emitter.Variable(int64Type, "freq");

            // QueryPerformanceCounter(&lp);
            emitter.Call(qpcFunction, { timeVar });
            // QueryPerformanceFrequency(&freq);
            emitter.Call(qpfFunction, { freqVar });

            auto timeDouble = emitter.CastIntToFloat(emitter.Load(timeVar), VariableType::Double, true);
            auto freqDouble = emitter.CastIntToFloat(emitter.Load(freqVar), VariableType::Double, true);
            // double seconds = (double)time.QuadPart / (double)freq.QuadPart;
            auto seconds = emitter.Operator(TypedOperator::divideFloat, timeDouble, freqDouble);

            // int32_t sec = (int64_t)seconds;
            auto intSeconds = emitter.CastFloatToInt(seconds, VariableType::Int64);
            auto floatSeconds = emitter.CastIntToFloat(intSeconds, VariableType::Double, true);

            //  tp->tv_nsec = (int32_t)((seconds - sec) * 10000000);
            auto remainder = emitter.Operator(TypedOperator::subtractFloat, seconds, floatSeconds);
            auto nanoseconds = emitter.Operator(TypedOperator::multiplyFloat, remainder, hundredNanoSeconds);

            // STYLE matching casing style of timespec struct members
            auto tv_nsec = emitter.CastFloatToInt(nanoseconds, tmFieldVarType);
            emitter.Store(nanoPtr, tv_nsec);

            // tp->tv_sec = sec;
            // STYLE matching casing style of timespec struct members
            auto tv_sec = emitter.CastFloatToInt(floatSeconds, tmFieldVarType);
            emitter.Store(secondsPtr, tv_sec);

            emitter.Return(zero);
            _module.EndFunction();
        }
        else
        {
            function = _module.GetFunction("clock_gettime");
        }

        return function;
    }

    llvm::Function* IRRuntime::GetCurrentTimeFunction()
    {
        if (_pGetCurrentTimeFunction == nullptr)
        {
            auto& emitter = _module.GetIREmitter();
            auto& context = _module.GetLLVMContext();
            auto& irBuilder = emitter.GetIRBuilder();
            auto int32Type = llvm::Type::getInt32Ty(context);
            auto int64Type = llvm::Type::getInt64Ty(context);

            llvm::StructType* timespecType = nullptr;
            if (_module.GetCompilerParameters().targetDevice.numBits == 32)
            {
                // These are really time_t and long
                timespecType = llvm::StructType::create(context, { int32Type, int32Type }, "timespec");
            }
            else
            {
                timespecType = llvm::StructType::create(context, { int64Type, int64Type }, "timespec");
            }
            llvm::FunctionType* gettimeType = llvm::FunctionType::get(int32Type, { int32Type, timespecType->getPointerTo() }, false);
            _module.DeclareFunction("clock_gettime", gettimeType);

            // make struct
            auto getTimeFunction = ResolveCurrentTimeFunction(timespecType);
            if (getTimeFunction != nullptr)
            {
                auto functionName = GetNamespacePrefix() + "_" + getTimeFunctionName;
                auto function = _module.BeginFunction(functionName, VariableType::Double);

                llvm::AllocaInst* timeStruct = function.Variable(timespecType, "tp");

#ifdef _MSC_VER
                int CLOCK_REALTIME = 0;
#endif

                auto callResult = function.Call(getTimeFunction, { function.Literal(CLOCK_REALTIME), timeStruct });

                // llvm::Value* timeStructBase = irBuilder.CreateInBoundsGEP(timespecType, timeStruct, function.Literal(0));
                auto secondsPtr = irBuilder.CreateInBoundsGEP(timespecType, timeStruct, { function.Literal(0), function.Literal(0) });
                auto nanosecondsPtr = irBuilder.CreateInBoundsGEP(timespecType, timeStruct, { function.Literal(0), function.Literal(1) });
                auto secondsIntVal = function.Load(secondsPtr);
                auto nanosecondsIntVal = function.Load(nanosecondsPtr);
                auto secondsDoubleVal = emitter.CastIntToFloat(secondsIntVal, VariableType::Double, true);
                auto nanosecondsDoubleVal = emitter.CastIntToFloat(nanosecondsIntVal, VariableType::Double, false);
                auto divisor = function.Literal(1000000000.0);
                auto totalSecondsDoubleVal = function.Operator(TypedOperator::addFloat, secondsDoubleVal, function.Operator(TypedOperator::divideFloat, nanosecondsDoubleVal, divisor));
                function.Return(function.Operator(TypedOperator::multiplyFloat, totalSecondsDoubleVal, function.Literal(1000.0)));
                _module.EndFunction();
                _pGetCurrentTimeFunction = function.GetFunction();
            }
            else
            {
                throw EmitterException(EmitterError::functionNotFound);
            }
        }
        return _pGetCurrentTimeFunction;
    }

    llvm::Function* IRRuntime::GetSqrtFunction(VariableType argType)
    {
        return _module.GetIntrinsic(llvm::Intrinsic::sqrt, { argType });
    }

    llvm::Function* IRRuntime::GetAbsFunction(VariableType argType)
    {
        return _module.GetIntrinsic(llvm::Intrinsic::fabs, { argType });
    }

    llvm::Function* IRRuntime::GetExpFunction(VariableType argType)
    {
        return _module.GetIntrinsic(llvm::Intrinsic::exp, { argType });
    }

    llvm::Function* IRRuntime::GetLogFunction(VariableType argType)
    {
        return _module.GetIntrinsic(llvm::Intrinsic::log, { argType });
    }

    //
    // BLAS
    //
    llvm::Function* IRRuntime::GetSGEMVFunction()
    {
        ValueTypeList argTypes = {
            VariableType::Int32, // order
            VariableType::Int32, // transpose
            VariableType::Int32, // m
            VariableType::Int32, // n
            VariableType::Float, // alpha
            VariableType::FloatPointer, // M
            VariableType::Int32, // lda
            VariableType::FloatPointer, // x
            VariableType::Int32, // incx
            VariableType::Float, // beta
            VariableType::FloatPointer, // y
            VariableType::Int32 // incy
        };

        auto pModule = _module.GetLLVMModule();
        auto types = _module.GetIREmitter().GetLLVMTypes(argTypes);
        auto functionType = llvm::FunctionType::get(_module.GetIREmitter().Type(emitters::VariableType::Int32), types, false);
        return static_cast<llvm::Function*>(pModule->getOrInsertFunction("cblas_sgemv", functionType));
    }

    llvm::Function* IRRuntime::GetDGEMVFunction()
    {
        ValueTypeList argTypes = {
            VariableType::Int32, // order
            VariableType::Int32, // transpose
            VariableType::Int32, // m
            VariableType::Int32, // n
            VariableType::Double, // alpha
            VariableType::DoublePointer, // M
            VariableType::Int32, // lda
            VariableType::DoublePointer, // x
            VariableType::Int32, // incx
            VariableType::Double, // beta
            VariableType::DoublePointer, // y
            VariableType::Int32 // incy
        };

        auto pModule = _module.GetLLVMModule();
        auto types = _module.GetIREmitter().GetLLVMTypes(argTypes);
        auto functionType = llvm::FunctionType::get(_module.GetIREmitter().Type(emitters::VariableType::Int32), types, false);
        return static_cast<llvm::Function*>(pModule->getOrInsertFunction("cblas_dgemv", functionType));
    }

    llvm::Function* IRRuntime::GetSGEMMFunction()
    {
        ValueTypeList argTypes = {
            VariableType::Int32, // order
            VariableType::Int32, // transposeA
            VariableType::Int32, // transposeB
            VariableType::Int32, // m
            VariableType::Int32, // n
            VariableType::Int32, // k
            VariableType::Float, // alpha
            VariableType::FloatPointer, // A
            VariableType::Int32, // lda
            VariableType::FloatPointer, // B
            VariableType::Int32, // ldb
            VariableType::Float, // beta
            VariableType::FloatPointer, // C
            VariableType::Int32 // ldc
        };

        auto pModule = _module.GetLLVMModule();
        auto types = _module.GetIREmitter().GetLLVMTypes(argTypes);
        auto functionType = llvm::FunctionType::get(_module.GetIREmitter().Type(emitters::VariableType::Int32), types, false);
        return static_cast<llvm::Function*>(pModule->getOrInsertFunction("cblas_sgemm", functionType));
    }

    llvm::Function* IRRuntime::GetDGEMMFunction()
    {
        ValueTypeList argTypes = {
            VariableType::Int32, // order
            VariableType::Int32, // transposeA
            VariableType::Int32, // transposeB
            VariableType::Int32, // m
            VariableType::Int32, // n
            VariableType::Int32, // k
            VariableType::Double, // alpha
            VariableType::DoublePointer, // A
            VariableType::Int32, // lda
            VariableType::DoublePointer, // B
            VariableType::Int32, // ldb
            VariableType::Double, // beta
            VariableType::DoublePointer, // C
            VariableType::Int32 // ldc
        };

        auto pModule = _module.GetLLVMModule();
        auto types = _module.GetIREmitter().GetLLVMTypes(argTypes);
        auto functionType = llvm::FunctionType::get(_module.GetIREmitter().Type(emitters::VariableType::Int32), types, false);
        return static_cast<llvm::Function*>(pModule->getOrInsertFunction("cblas_dgemm", functionType));
    }

    template <>
    llvm::Function* IRRuntime::GetGEMVFunction<float>()
    {
        return GetSGEMVFunction();
    }

    template <>
    llvm::Function* IRRuntime::GetGEMVFunction<double>()
    {
        return GetDGEMVFunction();
    }

    template <>
    llvm::Function* IRRuntime::GetGEMMFunction<float>()
    {
        return GetSGEMMFunction();
    }

    template <>
    llvm::Function* IRRuntime::GetGEMMFunction<double>()
    {
        return GetDGEMMFunction();
    }
}
}
