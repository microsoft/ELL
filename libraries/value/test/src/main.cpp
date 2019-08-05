////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (value)
//  Authors:  Kern Handa, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Matrix_test.h"
#include "Scalar_test.h"
#include "Tensor_test.h"
#include "Value_test.h"
#include "Vector_test.h"

#include <value/include/ComputeContext.h>
#include <value/include/FunctionDeclaration.h>
#include <value/include/LLVMContext.h>
#include <value/include/Vector.h>

#include <emitters/include/CompilerOptions.h>
#include <emitters/include/IRExecutionEngine.h>
#include <emitters/include/IRModuleEmitter.h>

#include <utilities/include/Exception.h>
#include <utilities/include/StringUtil.h>
#include <utilities/include/TypeAliases.h>

#include <testing/include/testing.h>

#include <functional>
#include <iostream>
#include <type_traits>

using namespace ell::utilities;
using namespace ell::value;

#define PRINT_IR 0

struct TestLLVMContext : public LLVMContext
{
    TestLLVMContext(ell::emitters::IRModuleEmitter& emitter) :
        LLVMContext(emitter),
        _emitter(emitter) {}

    void DebugDump() { _emitter.DebugDump(); }

    ell::emitters::IRFunctionEmitter& GetFunctionEmitter() const
    {
        return _emitter.GetCurrentFunction();
    }

private:
    ell::emitters::IRModuleEmitter& _emitter;
};

void PrintIR(TestLLVMContext& context)
{
#if PRINT_IR
    context.DebugDump();
#endif // PRINT_IR
}

extern "C"
{
void JittedDebugPrintInts(int* ints, int* len)
{
    std::cout << std::setprecision(6);
    for (int i = 0; i < *len; i++)
    {
        if (i > 0)
        {
            std::cout << ", ";
        }
        std::cout << ints[i];
    }
}

void JittedDebugPrintFloats(float* f, int* len)
{
    std::cout << std::setprecision(6);
    for (int i = 0; i < *len; i++)
    {
        if (i > 0)
        {
            std::cout << ", ";
        }
        std::cout << f[i];
    }
}

void JittedDebugPrintDoubles(double* f, int* len)
{
    std::cout << std::setprecision(10);
    for (int i = 0; i < *len; i++)
    {
        if (i > 0)
        {
            std::cout << ", ";
        }
        std::cout << f[i];
    }
}
}

ell::emitters::LLVMFunction printFunction;
ell::emitters::LLVMFunction printDoublesFunction;
ell::emitters::LLVMFunction printFloatsFunction;
ell::emitters::LLVMFunction printIntsFunction;

void DeclareDebugPrintFunctions(ell::emitters::IRModuleEmitter& module)
{
    auto& context = module.GetIREmitter().GetContext();

    printFunction = module.DeclareDebugPrint();

    auto type2 = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(context),
        { llvm::Type::getFloatPtrTy(context), llvm::Type::getInt32PtrTy(context) },
        false);
    printFloatsFunction = module.DeclareFunction("DebugPrintFloats", type2);

    auto type3 = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(context),
        { llvm::Type::getDoublePtrTy(context), llvm::Type::getInt32PtrTy(context) },
        false);
    printDoublesFunction = module.DeclareFunction("DebugPrintDoubles", type3);

    auto type4 = llvm::FunctionType::get(
        llvm::Type::getInt32Ty(context),
        { llvm::Type::getInt32PtrTy(context), llvm::Type::getInt32PtrTy(context) },
        false);
    printIntsFunction = module.DeclareFunction("DebugPrintInts", type4);
}

void DefineDebugPrintFunctions(ell::emitters::IRExecutionEngine& jitter)
{
    jitter.DefineFunction(printFloatsFunction, reinterpret_cast<UIntPtrT>(&JittedDebugPrintFloats));
    jitter.DefineFunction(printDoublesFunction, reinterpret_cast<UIntPtrT>(&JittedDebugPrintDoubles));
    jitter.DefineFunction(printIntsFunction, reinterpret_cast<UIntPtrT>(&JittedDebugPrintInts));
}

namespace ell
{
void DebugPrintVector(Vector message)
{
    int size = static_cast<int>(message.Size());

    InvokeForContext<ComputeContext>([&] {
        std::visit(
            [size = message.Size()](auto&& data) {
                using Type = std::decay_t<decltype(data)>;
                if constexpr (IsOneOf<Type, Emittable, Boolean*>)
                {
                    throw LogicException(LogicExceptionErrors::notImplemented);
                }
                else
                {
                    std::copy(
                        data,
                        data + size,
                        std::ostream_iterator<std::remove_pointer_t<Type>>(std::cout, ", "));
                }
            },
            message.GetValue().GetUnderlyingData());
    });

    InvokeForContext<LLVMContext>([&](auto& context) {
        std::string fnName = "DebugPrint";
        switch (message.GetType())
        {
        case ValueType::Float:
            fnName += "Floats";
            break;
        case ValueType::Double:
            fnName += "Doubles";
            break;
        case ValueType::Int32:
            fnName += "Ints";
            break;
        default:
            context.DebugPrint("DebugPrintVector not implemented on type: " + ToString(message.GetType()));
            return;
        }
        auto printFunction = FunctionDeclaration(fnName)
                                 .Parameters(
                                     Value(message.GetType(), MemoryLayout{ { size } }),
                                     Value(ValueType::Int32, ScalarLayout))
                                 .Decorated(FunctionDecorated::No);
        printFunction.Call(message, Scalar{ size });
    });
}

void DebugPrintScalar(Scalar value)
{
    InvokeForContext<ComputeContext>([&] {
        std::visit(
            [](auto&& data) {
            using Type = std::decay_t<decltype(data)>;
            if constexpr (IsOneOf<Type, Emittable, Boolean*>)
            {
                throw LogicException(LogicExceptionErrors::notImplemented);
            }
            else
            {
                std::copy(
                    data,
                    data + 1,
                    std::ostream_iterator<std::remove_pointer_t<Type>>(std::cout, ", "));
            }
        },
            value.GetValue().GetUnderlyingData());
    });

    InvokeForContext<LLVMContext>([&](auto& context) {
        std::string fnName = "DebugPrint";
        Vector vector = Allocate(value.GetType(), 1);
        vector[0] = value;
        switch (value.GetType())
        {
        case ValueType::Float:
            fnName += "Floats";
            break;
        case ValueType::Double:
            fnName += "Doubles";
            break;
        case ValueType::Int32:
            fnName += "Ints";
            break;
        default:
            context.DebugPrint("DebugPrintScalar not implemented on type: " + ToString(value.GetType()));
            return;
        }
        auto printFunction = FunctionDeclaration(fnName)
            .Parameters(
                Value(value.GetType(), MemoryLayout{ { 1 } }),
                Value(ValueType::Int32, ScalarLayout))
            .Decorated(FunctionDecorated::No);
        printFunction.Call(vector, Scalar{ 1 });
    });
}
} // namespace ell

void ComputeTest(std::string testName, std::function<Scalar()> defineFunction)
{
    // Run the test in the ComputeContext
    ContextGuard<ComputeContext> guard("Value_test_compute");

    auto fnDecl = DeclareFunction(testName)
                      .Returns(Value(ValueType::Int32, ScalarLayout));
    auto fn = fnDecl.Define(defineFunction);

#if 0 // Useful for debugging, dumps to stderr
    DebugDump(fn);
#endif // 0

    auto rc = fn().Get<int>();
    std::string msg = testName + ": Compute returned %d";
    ell::testing::ProcessTest(ell::utilities::FormatString(msg.c_str(), rc), rc == 0);
}

void LLVMJitTest(std::string testName, std::function<Scalar()> defineFunction)
{
    // Run the test in the LLVM context
    ell::emitters::CompilerOptions compilerSettings;
    compilerSettings.useBlas = false;
    compilerSettings.parallelize = true;
    compilerSettings.useThreadPool = false;
    ell::emitters::IRModuleEmitter moduleEmitter("Value_test_llvm", compilerSettings);
    DeclareDebugPrintFunctions(moduleEmitter);
    ContextGuard<TestLLVMContext> guard(moduleEmitter);

    auto fn = DeclareFunction(testName)
                  .Returns(Value(ValueType::Int32, ScalarLayout));

    fn.Define(defineFunction);

#if 0 // Useful for debugging, dumps to stderr
    DebugDump(fn);
#endif // 0

    bool save = false;
    if (save)
    {
        // Useful for debugging, saves to file
        std::string llFilename = testName + ".ll";
        moduleEmitter.WriteToFile(llFilename, ell::emitters::ModuleOutputFormat::ir);
    }

    // moduleEmitter.DebugDump();
    ell::emitters::IRExecutionEngine engine(std::move(moduleEmitter), true);
    DefineDebugPrintFunctions(engine);
    auto functionPointer = engine.ResolveFunctionAddress(fn.GetFunctionName());
    auto jitFn = reinterpret_cast<int* (*)(void)>(functionPointer);
    int rc = *jitFn();
    std::string msg = testName + ": Jitted LLVM returned %d";
    ell::testing::ProcessTest(ell::utilities::FormatString(msg.c_str(), rc), rc == 0);
}

void RunTest(std::string testName, std::function<Scalar()> defineFunction)
{
    try
    {
        ComputeTest(testName, defineFunction);
    }
    catch (const std::exception& e)
    {
        ell::testing::ProcessTest(testName + " Compute failed with exception, " + e.what(), false);
    }

    try
    {
        LLVMJitTest(testName, defineFunction);
    }
    catch (const std::exception& e)
    {
        ell::testing::ProcessTest(testName + " Jitted LLVM failed with exception, " + e.what(), false);
    }
}

int main()
{
    using namespace ell;
    using namespace utilities;
    try
    {
#define ADD_TEST_FUNCTION(a) testFunctions.push_back({ #a, a });
        std::vector<std::pair<std::string, std::function<Scalar()>>> testFunctions;

        // Value tests
        ADD_TEST_FUNCTION(Basic_test);
        ADD_TEST_FUNCTION(DebugPrint_test);
        ADD_TEST_FUNCTION(If_test1);
        ADD_TEST_FUNCTION(Value_test1);

        ADD_TEST_FUNCTION(Scalar_test1);
        ADD_TEST_FUNCTION(Scalar_test2);

        ADD_TEST_FUNCTION(Vector_test1);
        ADD_TEST_FUNCTION(Vector_test2);
        ADD_TEST_FUNCTION(Vector_test3); // bugbug: work item 2335, fix subvector assignment.
        ADD_TEST_FUNCTION(Vector_test4);
        ADD_TEST_FUNCTION(Vector_test5);
        
        ADD_TEST_FUNCTION(Matrix_test1);
        ADD_TEST_FUNCTION(Matrix_test2);
        ADD_TEST_FUNCTION(Matrix_test3);
        ADD_TEST_FUNCTION(Reshape_test);
        ADD_TEST_FUNCTION(GEMV_test);
        ADD_TEST_FUNCTION(Tensor_test1);
        ADD_TEST_FUNCTION(Tensor_test2);
        ADD_TEST_FUNCTION(Tensor_test3);
        ADD_TEST_FUNCTION(Tensor_slice_test1);

        ADD_TEST_FUNCTION(Casting_test1);
        ADD_TEST_FUNCTION(Sum_test);
        ADD_TEST_FUNCTION(Dot_test);
        ADD_TEST_FUNCTION(Intrinsics_test1);
        ADD_TEST_FUNCTION(Intrinsics_test2);
        ADD_TEST_FUNCTION(For_test1);
        ADD_TEST_FUNCTION(For_test2);
        ADD_TEST_FUNCTION(ForRangeCasting_test1);
        ADD_TEST_FUNCTION(ForRangeCasting_test2);
        ADD_TEST_FUNCTION(Parallelized_test1);
        ADD_TEST_FUNCTION(Parallelized_test2);
        ADD_TEST_FUNCTION(Parallelized_test3);
        ADD_TEST_FUNCTION(Prefetch_test1);
        ADD_TEST_FUNCTION(ScalarRefTest);
        ADD_TEST_FUNCTION(ScalarRefRefTest);
        ADD_TEST_FUNCTION(ScalarRefRefRefTest);
        ADD_TEST_FUNCTION(MatrixReferenceTest);
        ADD_TEST_FUNCTION(RefScalarRefTest);
        ADD_TEST_FUNCTION(RefScalarRefCtorsTest);
        ADD_TEST_FUNCTION(RefScalarRefRefTest);
        ADD_TEST_FUNCTION(RefScalarRefRefRefTest);
        ADD_TEST_FUNCTION(RefMatrixReferenceTest);

        for (auto [name, fn] : testFunctions)
        {
            RunTest(name, fn);
        }

#undef ADD_TEST_FUNCTION
    }
    catch (const std::exception& exception)
    {
        std::cerr << "ERROR, got exception. Message: " << exception.what() << std::endl;
        throw;
    }

    if (testing::DidTestFail())
    {
        return 1;
    }
    return 0;
}
