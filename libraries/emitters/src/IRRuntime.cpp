////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRRuntime.cpp (emitters)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRRuntime.h"
#include "IRFunctionEmitter.h"
#include "IRMetadata.h"
#include "IRModuleEmitter.h"

// utilities
#include "Unused.h"

namespace ell
{
namespace emitters
{
    namespace
    {
        //
        // Native implementations of matrix operation functions (as opposed to calling out to BLAS)
        //
        template <typename ValueType>
        llvm::Function* EmitGEMVFunction(IRModuleEmitter& module, const std::string& functionName, const VariableTypeList& argTypes)
        {
            const auto plus = emitters::TypedOperator::add;
            const auto times = emitters::TypedOperator::multiply;
            const auto plusFloat = emitters::TypedOperator::addFloat;
            const auto timesFloat = emitters::TypedOperator::multiplyFloat;

            auto function = module.BeginFunction(functionName, VariableType::Int32, argTypes);
            auto arguments = function.Arguments().begin();
            auto order = &(*arguments++);
            auto transpose = &(*arguments++);
            auto m = &(*arguments++);
            auto n = &(*arguments++);
            auto alpha = &(*arguments++);
            auto A = &(*arguments++);
            auto lda = &(*arguments++);
            auto x = &(*arguments++);
            auto incx = &(*arguments++);
            auto beta = &(*arguments++);
            auto y = &(*arguments++);
            auto incy = &(*arguments++);
            UNUSED(order, transpose, alpha, beta);

            llvm::Value* accum = function.Variable(emitters::GetVariableType<ValueType>(), "accum");

            auto iLoop = function.ForLoop();
            iLoop.Begin(m);
            {
                auto i = iLoop.LoadIterationVariable();
                function.StoreZero(accum);
                auto jLoop = function.ForLoop();
                jLoop.Begin(n);
                {
                    auto j = jLoop.LoadIterationVariable();
                    auto aIndex = function.Operator(plus, function.Operator(times, i, lda), j);
                    auto xIndex = function.Operator(times, j, incx);
                    auto aVal = function.ValueAt(A, aIndex);
                    auto xVal = function.ValueAt(x, xIndex);
                    auto aTimesX = function.Operator(timesFloat, aVal, xVal);
                    function.OperationAndUpdate(accum, plusFloat, aTimesX);
                }
                jLoop.End();

                auto yIndex = function.Operator(times, i, incy);
                function.SetValueAt(y, yIndex, function.Load(accum));
            }
            iLoop.End();
            function.Return(function.Literal<int>(0));
            module.EndFunction();
            return function.GetFunction();
        }

        template <typename ValueType>
        llvm::Function* EmitGEMMFunction(IRModuleEmitter& module, const std::string& functionName, const VariableTypeList& argTypes)
        {
            const auto plus = emitters::TypedOperator::add;
            const auto times = emitters::TypedOperator::multiply;
            const auto plusFloat = emitters::TypedOperator::addFloat;
            const auto timesFloat = emitters::TypedOperator::multiplyFloat;

            auto function = module.BeginFunction(functionName, VariableType::Int32, argTypes);
            auto arguments = function.Arguments().begin();
            auto order = &(*arguments++);
            auto transposeA = &(*arguments++);
            auto transposeB = &(*arguments++);
            auto m = &(*arguments++);
            auto n = &(*arguments++);
            auto k = &(*arguments++);
            auto alpha = &(*arguments++);
            auto A = &(*arguments++);
            auto lda = &(*arguments++);
            auto B = &(*arguments++);
            auto ldb = &(*arguments++);
            auto beta = &(*arguments++);
            auto C = &(*arguments++);
            auto ldc = &(*arguments++);
            UNUSED(order, transposeA, transposeB, alpha, beta);

            // C = A x B, A: mxk, B: kxn, C: mxn
            // A': kxm, B': nxk

            // Loop orders:
            // A*B:   i, k, j
            // A'*B:  k, i, j
            // A*B':  i, j, k
            // A'*B': k, j, i (?)

            // Clear output
            auto count = function.Operator(times, ldc, m);
            function.MemorySet<ValueType>(C, function.Literal<int>(0), function.Literal<uint8_t>(0), count);

            // TODO: deal with transposes

            // Accumulate partial values into output
            auto iLoop = function.ForLoop();
            iLoop.Begin(m);
            {
                auto iIndex = iLoop.LoadIterationVariable();

                auto kLoop = function.ForLoop();
                kLoop.Begin(k);
                {
                    auto kIndex = kLoop.LoadIterationVariable();

                    auto jLoop = function.ForLoop();
                    jLoop.Begin(n);
                    {
                        auto jIndex = jLoop.LoadIterationVariable();

                        llvm::Value* aIndex = nullptr;
                        llvm::Value* bIndex = nullptr;
                        aIndex = function.Operator(plus, function.Operator(times, iIndex, lda), kIndex);
                        bIndex = function.Operator(plus, function.Operator(times, kIndex, ldb), jIndex);
                        // if (transposeA)
                        //     aIndex = function.Operator(plus, function.Operator(times, kIndex, function.Literal(lda)), iIndex);
                        // else
                        //     aIndex = function.Operator(plus, function.Operator(times, iIndex, function.Literal(lda)), kIndex);

                        // if (transposeB)
                        //     bIndex = function.Operator(plus, function.Operator(times, jIndex, function.Literal(ldb)), kIndex);
                        // else
                        //     bIndex = function.Operator(plus, function.Operator(times, kIndex, function.Literal(ldb)), jIndex);

                        auto aValue = function.ValueAt(A, aIndex);
                        auto bValue = function.ValueAt(B, bIndex);
                        auto value = function.Operator(timesFloat, aValue, bValue);
                        // store output in C[m,n]
                        auto cIndex = function.Operator(plus, function.Operator(times, iIndex, ldc), jIndex);
                        auto oldVal = function.ValueAt(C, cIndex);
                        auto sum = function.Operator(plusFloat, oldVal, value);
                        function.SetValueAt(C, cIndex, sum);
                    }
                    jLoop.End();
                }
                kLoop.End();
            }
            iLoop.End();
            function.Return(function.Literal<int>(0));

            module.EndFunction();
            return function.GetFunction();
        }

    } // end anonymous namespace

    static const std::string& countName = "count";
    static const std::string& lVectorName = "pLVector";
    static const std::string& rVectorName = "pRVector";
    static const std::string& resultName = "pResult";

    static const std::string& dotProductFloatName = "DotProductFloat";
    static const std::string& dotProductIntName = "DotProductInt";
    static const std::string& getTimeFunctionName = "GetTime";

    IRRuntime::IRRuntime(IRModuleEmitter& module)
        : _module(module), _posixRuntime(module)
    {
    }

    llvm::Type* IRRuntime::GetIntType()
    {
        auto& context = _module.GetLLVMContext();
        const auto numBits = _module.GetCompilerOptions().targetDevice.numBits;
        if (numBits != 0)
        {
            return llvm::Type::getIntNTy(context, numBits);
        }
        else
        {
            return llvm::Type::getInt32Ty(context);
        }
    }

    std::string IRRuntime::GetNamespacePrefix() const
    {
        return _module.GetModuleName();
    }

    llvm::Function* IRRuntime::GetDotProductFloatFunction()
    {
        auto functionName = GetNamespacePrefix() + "_" + dotProductFloatName;
        NamedVariableTypeList argTypes = { { countName, VariableType::Int32 },
                                           { lVectorName, VariableType::DoublePointer },
                                           { rVectorName, VariableType::DoublePointer },
                                           { resultName, VariableType::DoublePointer } };
        auto function = _module.BeginFunction(functionName, VariableType::Void, argTypes);
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

    llvm::Function* IRRuntime::GetDotProductIntFunction()
    {
        auto functionName = GetNamespacePrefix() + "_" + dotProductIntName;
        NamedVariableTypeList argTypes = { { countName, VariableType::Int32 },
                                           { lVectorName, VariableType::Int32Pointer },
                                           { rVectorName, VariableType::Int32Pointer },
                                           { resultName, VariableType::Int32Pointer } };
        auto function = _module.BeginFunction(functionName, VariableType::Void, argTypes);
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

        auto& context = _module.GetLLVMContext();
        auto int32Type = llvm::Type::getInt32Ty(context);
        auto int64Type = llvm::Type::getInt64Ty(context);
        auto doubleType = llvm::Type::getDoubleTy(context);

        auto tmFieldType = timespecType->getElementType(0); // The type of the first field of the timespec struct -- it's the correct bitsize for 'int'
        auto intType = tmFieldType;

        if (_module.GetCompilerOptions().targetDevice.IsWindows())
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

            const VariableType tmFieldVarType = tmFieldType == int64Type ? VariableType::Int64 : VariableType::Int32;

            auto zero = llvm::ConstantInt::get(int32Type, 0);
            auto hundredNanoSeconds = llvm::ConstantFP::get(doubleType, 10000000.0);

            llvm::FunctionType* qpcProto = llvm::FunctionType::get(int32Type, { int64Type->getPointerTo() }, false);
            _module.DeclareFunction("QueryPerformanceCounter", qpcProto);
            _module.DeclareFunction("QueryPerformanceFrequency", qpcProto);
            auto qpcFunction = _module.GetFunction("QueryPerformanceCounter");
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
#ifndef WIN32
            // On non-Windows, we need to make sure the linker links in
            // the clock_gettime function.
            volatile void* temp = (void*)(&clock_gettime);
            UNUSED(temp);
#endif
            llvm::FunctionType* gettimeType = llvm::FunctionType::get(intType, { int32Type, timespecType->getPointerTo() }, false);
            _module.DeclareFunction("clock_gettime", gettimeType);
            function = _module.GetFunction("clock_gettime");
        }

        return function;
    }

    llvm::Value* IRRuntime::GetCurrentTime(IRFunctionEmitter& function)
    {
        auto getTimeFunc = GetCurrentTimeFunction();
        auto time = function.Call(getTimeFunc, {});
        return time;
    }

    llvm::Function* IRRuntime::GetCurrentTimeFunction()
    {
        if (_pGetCurrentTimeFunction == nullptr)
        {
            auto& emitter = _module.GetIREmitter();
            auto& context = _module.GetLLVMContext();
            auto& irBuilder = emitter.GetIRBuilder();
            auto int32Type = llvm::Type::getInt32Ty(context);

            llvm::StructType* timespecType = _posixRuntime.GetTimespecType();
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
                UNUSED(callResult);

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

    llvm::Function* IRRuntime::GetSinFunction(VariableType argType)
    {
        return _module.GetIntrinsic(llvm::Intrinsic::sin, { argType });
    }

    llvm::Function* IRRuntime::GetCosFunction(VariableType argType)
    {
        return _module.GetIntrinsic(llvm::Intrinsic::cos, { argType });
    }

    llvm::Function* IRRuntime::GetTanhFunction(VariableType argType)
    {
        // This assumes a standard C runtime library is linked
        switch (argType)
        {
        case VariableType::Float:
        case VariableType::Double:
            break;
        default:
            throw EmitterException(EmitterError::functionNotFound);
        }

        auto& emitter = _module.GetIREmitter();
        const char* funcName = argType == VariableType::Double ? "tanh" : "tanhf";
        auto valueType = emitter.Type(argType);
        auto tanhProto = llvm::FunctionType::get(valueType, { valueType }, false);
        _module.DeclareFunction(funcName, tanhProto);
        return _module.GetFunction(funcName);
    }

    llvm::Function* IRRuntime::GetSqrtFunction(llvm::Type* argType)
    {
        return _module.GetIntrinsic(llvm::Intrinsic::sqrt, { argType });
    }

    llvm::Function* IRRuntime::GetAbsFunction(llvm::Type* argType)
    {
        return _module.GetIntrinsic(llvm::Intrinsic::fabs, { argType });
    }

    llvm::Function* IRRuntime::GetExpFunction(llvm::Type* argType)
    {
        return _module.GetIntrinsic(llvm::Intrinsic::exp, { argType });
    }

    llvm::Function* IRRuntime::GetLogFunction(llvm::Type* argType)
    {
        return _module.GetIntrinsic(llvm::Intrinsic::log, { argType });
    }

    llvm::Function* IRRuntime::GetSinFunction(llvm::Type* argType)
    {
        return _module.GetIntrinsic(llvm::Intrinsic::sin, { argType });
    }

    llvm::Function* IRRuntime::GetCosFunction(llvm::Type* argType)
    {
        return _module.GetIntrinsic(llvm::Intrinsic::cos, { argType });
    }

    //
    // BLAS
    //
    llvm::Function* IRRuntime::GetSGEMVFunction(bool useBlas)
    {
        VariableTypeList argTypes = {
            VariableType::Int32, // order
            VariableType::Int32, // transpose
            VariableType::Int32, // m
            VariableType::Int32, // n
            VariableType::Float, // alpha
            VariableType::FloatPointer, // A
            VariableType::Int32, // lda
            VariableType::FloatPointer, // x
            VariableType::Int32, // incx
            VariableType::Float, // beta
            VariableType::FloatPointer, // y
            VariableType::Int32 // incy
        };

        auto pModule = _module.GetLLVMModule();
        if (useBlas)
        {
            auto types = _module.GetIREmitter().GetLLVMTypes(argTypes);
            auto functionType = llvm::FunctionType::get(_module.GetIREmitter().Type(emitters::VariableType::Int32), types, false);
            return static_cast<llvm::Function*>(pModule->getOrInsertFunction("cblas_sgemv", functionType));
        }
        else
        {
            auto pFunction = pModule->getFunction("noblas_sgemv");
            if (pFunction != nullptr)
            {
                return pFunction;
            }
            return EmitGEMVFunction<float>(_module, "noblas_sgemv", argTypes);
        }
    }

    llvm::Function* IRRuntime::GetDGEMVFunction(bool useBlas)
    {
        VariableTypeList argTypes = {
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
        if (useBlas)
        {
            auto types = _module.GetIREmitter().GetLLVMTypes(argTypes);
            auto functionType = llvm::FunctionType::get(_module.GetIREmitter().Type(emitters::VariableType::Int32), types, false);
            return static_cast<llvm::Function*>(pModule->getOrInsertFunction("cblas_dgemv", functionType));
        }
        else
        {
            auto pFunction = pModule->getFunction("noblas_dgemv");
            if (pFunction != nullptr)
            {
                return pFunction;
            }
            return EmitGEMVFunction<double>(_module, "noblas_dgemv", argTypes);
        }
    }

    llvm::Function* IRRuntime::GetSGEMMFunction(bool useBlas)
    {
        VariableTypeList argTypes = {
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
        if (useBlas)
        {
            auto types = _module.GetIREmitter().GetLLVMTypes(argTypes);
            auto functionType = llvm::FunctionType::get(_module.GetIREmitter().Type(emitters::VariableType::Int32), types, false);
            return static_cast<llvm::Function*>(pModule->getOrInsertFunction("cblas_sgemm", functionType));
        }
        else
        {
            auto pFunction = pModule->getFunction("noblas_sgemm");
            if (pFunction != nullptr)
            {
                return pFunction;
            }
            return EmitGEMMFunction<float>(_module, "noblas_sgemm", argTypes);
        }
    }

    llvm::Function* IRRuntime::GetDGEMMFunction(bool useBlas)
    {
        VariableTypeList argTypes = {
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
        if (useBlas)
        {
            auto types = _module.GetIREmitter().GetLLVMTypes(argTypes);
            auto functionType = llvm::FunctionType::get(_module.GetIREmitter().Type(emitters::VariableType::Int32), types, false);
            return static_cast<llvm::Function*>(pModule->getOrInsertFunction("cblas_dgemm", functionType));
        }
        else
        {
            auto pFunction = pModule->getFunction("noblas_dgemm");
            if (pFunction != nullptr)
            {
                return pFunction;
            }
            return EmitGEMMFunction<double>(_module, "noblas_dgemm", argTypes);
        }
    }

    template <>
    llvm::Function* IRRuntime::GetGEMVFunction<float>(bool useBlas)
    {
        return GetSGEMVFunction(useBlas);
    }

    template <>
    llvm::Function* IRRuntime::GetGEMVFunction<double>(bool useBlas)
    {
        return GetDGEMVFunction(useBlas);
    }

    template <>
    llvm::Function* IRRuntime::GetGEMMFunction<float>(bool useBlas)
    {
        return GetSGEMMFunction(useBlas);
    }

    template <>
    llvm::Function* IRRuntime::GetGEMMFunction<double>(bool useBlas)
    {
        return GetDGEMMFunction(useBlas);
    }

    llvm::Function* IRRuntime::GetOpenBLASGetNumThreadsFunction()
    {
        // int openblas_get_num_threads();
        auto pModule = _module.GetLLVMModule();
        llvm::FunctionType* functionType = llvm::FunctionType::get(GetIntType(), {}, false);
        return static_cast<llvm::Function*>(pModule->getOrInsertFunction("openblas_get_num_threads", functionType));
    }

    llvm::Function* IRRuntime::GetOpenBLASSetNumThreadsFunction()
    {
        // void openblas_set_num_threads(int num_threads);
        auto pModule = _module.GetLLVMModule();
        auto& context = _module.GetLLVMContext();
        auto voidType = llvm::Type::getVoidTy(context);

        llvm::FunctionType* functionType = llvm::FunctionType::get(voidType, { GetIntType() }, false);
        return static_cast<llvm::Function*>(pModule->getOrInsertFunction("openblas_set_num_threads", functionType));
    }
}
}
