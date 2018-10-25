////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Value_test.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Value_test.h"

#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>

#include "ComputeContext.h"
#include "LLVMContext.h"
#include "Matrix.h"
#include "Tensor.h"
#include "Value.h"
#include "Vector.h"

#include "IRModuleEmitter.h"

// testing
#include "testing.h"

using namespace ell::emitters;
using namespace ell::utilities;
using namespace ell::value;

namespace
{
// Copied from ConvolutionTestData.cpp in dsp/test
//
// NOTE: The examples below were randomly-generated using
//       the following script:
//
// ```
// import scipy
// import scipy.signal
// import numpy as np
//
// input = np.random.rand(16)
// filter = [0.25, 0.5, 0.25]
// reference = scipy.signal.correlate(input, filter, mode="valid", method="direct")
// ```
auto Get1DReferenceFilter()
{
    return std::vector<double>{ 0.25, 0.5, 0.25 };
}

auto Get1DReferenceSignal()
{
    return std::vector<double>{ 0.42929697, 0.90317845, 0.84490289, 0.66174327, 0.10820399, 0.3511343, 0.58248869, 0.62674724, 0.11014194, 0.00132073, 0.58431646, 0.39873614, 0.40304155, 0.79139607, 0.97710827, 0.21268128 };
}

auto Get1DReferenceConvolutionResult()
{
    return std::vector<double>{ 0.77013919, 0.81368187, 0.56914835, 0.30732139, 0.34824032, 0.53571473, 0.48653128, 0.21208796, 0.17427497, 0.39217245, 0.44620757, 0.49905383, 0.74073549, 0.73957347 };
}

struct TestLLVMContext : public LLVMContext
{
    TestLLVMContext(std::unique_ptr<IRModuleEmitter> emitter)
        : LLVMContext(*emitter), _emitter(std::move(emitter))
    {
    }

    void DebugDump()
    {
        _emitter->DebugDump();
    }

private:
    std::unique_ptr<IRModuleEmitter> _emitter;
};

} // anonymous

namespace ell
{

void VarGetTests()
{
    using namespace std;
    // Value::Get<IsArithmetic<T>> -> T*
    static_assert(is_same_v<decltype(declval<Value>().Get<Boolean*>()), Boolean*>);
    static_assert(is_same_v<decltype(declval<Value>().Get<char*>()), char*>);
    static_assert(is_same_v<decltype(declval<Value>().Get<uint8_t*>()), uint8_t*>);
    static_assert(is_same_v<decltype(declval<Value>().Get<short*>()), short*>);
    static_assert(is_same_v<decltype(declval<Value>().Get<int*>()), int*>);
    static_assert(is_same_v<decltype(declval<Value>().Get<int64_t*>()), int64_t*>);
    static_assert(is_same_v<decltype(declval<Value>().Get<float*>()), float*>);
    static_assert(is_same_v<decltype(declval<Value>().Get<double*>()), double*>);

    // Value::Get<IsArithmetic<T>> const -> T*
    static_assert(is_same_v<decltype(declval<const Value>().Get<Boolean*>()), Boolean*>);
    static_assert(is_same_v<decltype(declval<const Value>().Get<char*>()), char*>);
    static_assert(is_same_v<decltype(declval<const Value>().Get<uint8_t*>()), uint8_t*>);
    static_assert(is_same_v<decltype(declval<const Value>().Get<short*>()), short*>);
    static_assert(is_same_v<decltype(declval<const Value>().Get<int*>()), int*>);
    static_assert(is_same_v<decltype(declval<const Value>().Get<int64_t*>()), int64_t*>);
    static_assert(is_same_v<decltype(declval<const Value>().Get<float*>()), float*>);
    static_assert(is_same_v<decltype(declval<const Value>().Get<double*>()), double*>);

    // Value::TryGet<IsArithmetic<T>*> -> std::optional<T*>
    static_assert(is_same_v<decltype(declval<Value>().TryGet<Boolean*>()), std::optional<Boolean*>>);
    static_assert(is_same_v<decltype(declval<Value>().TryGet<char*>()), std::optional<char*>>);
    static_assert(is_same_v<decltype(declval<Value>().TryGet<uint8_t*>()), std::optional<uint8_t*>>);
    static_assert(is_same_v<decltype(declval<Value>().TryGet<short*>()), std::optional<short*>>);
    static_assert(is_same_v<decltype(declval<Value>().TryGet<int*>()), std::optional<int*>>);
    static_assert(is_same_v<decltype(declval<Value>().TryGet<int64_t*>()), std::optional<int64_t*>>);
    static_assert(is_same_v<decltype(declval<Value>().TryGet<float*>()), std::optional<float*>>);
    static_assert(is_same_v<decltype(declval<Value>().TryGet<double*>()), std::optional<double*>>);

    // Value::TryGet<IsArithmetic<T>*> const -> std::optional<T* const>
    static_assert(is_same_v<decltype(declval<const Value>().TryGet<Boolean*>()), std::optional<Boolean* const>>);
    static_assert(is_same_v<decltype(declval<const Value>().TryGet<char*>()), std::optional<char* const>>);
    static_assert(is_same_v<decltype(declval<const Value>().TryGet<uint8_t*>()), std::optional<uint8_t* const>>);
    static_assert(is_same_v<decltype(declval<const Value>().TryGet<short*>()), std::optional<short* const>>);
    static_assert(is_same_v<decltype(declval<const Value>().TryGet<int*>()), std::optional<int* const>>);
    static_assert(is_same_v<decltype(declval<const Value>().TryGet<int64_t*>()), std::optional<int64_t* const>>);
    static_assert(is_same_v<decltype(declval<const Value>().TryGet<float*>()), std::optional<float* const>>);
    static_assert(is_same_v<decltype(declval<const Value>().TryGet<double*>()), std::optional<double* const>>);
}

void Value_test1()
{
    CreateFunction("Value_test1", [] {
        Value v(std::vector<int>{ 1, 2, 3, 4 });
        For(v, [&](Scalar index)
        {
            InvokeForContext<ComputeContext>([&](auto&) { std::cout << *v.Offset(index).Get<int*>() << " "; });
        });

        InvokeForContext<ComputeContext>([](auto&) { std::cout << std::endl; });
        InvokeForContext<TestLLVMContext>([](auto& context) { context.DebugDump(); });
    })();
}

void Scalar_test1()
{
    CreateFunction("Scalar_test1", [] {
        Scalar s1 = 1;
        InvokeForContext<ComputeContext>([&](auto&) { testing::IsEqual(s1.Get<int>(), 1); });

        s1 += 2;
        InvokeForContext<ComputeContext>([&](auto&) { testing::IsEqual(s1.Get<int>(), 3); });

        Scalar s2 = s1 + 3;
        InvokeForContext<ComputeContext>([&](auto&) { testing::IsEqual(s1.Get<int>(), 3); });
        InvokeForContext<ComputeContext>([&](auto&) { testing::IsEqual(s2.Get<int>(), 6); });

        InvokeForContext<TestLLVMContext>([](auto& context) { context.DebugDump(); });
    })();
}

Vector testConvolve1D(Vector signal, Vector filter)
{
    size_t resultSize = signal.Size() - filter.Size() + 1;
    Vector result(Allocate(signal.GetType(), resultSize));

    For(result, [&](Scalar index) {
        Scalar accum;
        For(filter, [&](Scalar filterIndex) {
            accum += filter(filterIndex) * signal(index + filterIndex);
        });

        result(index) = accum;
    });

    return result;
}

void Vector_test1()
{
    auto signal = Get1DReferenceSignal();
    auto filter = Get1DReferenceFilter();
    auto referenceResult = Get1DReferenceConvolutionResult();
    auto valueType = GetValueType<decltype(signal)::value_type>();
    auto convolve1D = CreateFunction("testConvolve1D",
                                     Value{ valueType, MemoryLayout({ (int)referenceResult.size() }) },
                                     { Value{ valueType, MemoryLayout({ (int)signal.size() }) },
                                       Value{ valueType, MemoryLayout({ (int)filter.size() }) } },
                                     testConvolve1D);

    InvokeForContext<ComputeContext>([&](auto&)
    {
        Vector result = convolve1D(signal, filter);
         For(result, [&](Scalar index)
         {
             testing::IsEqual(referenceResult[index.Get<int>()], result[index].Get<double>());
         });
    });

    InvokeForContext<TestLLVMContext>([](auto& context) { context.DebugDump(); });
}

std::vector<std::unique_ptr<EmitterContext>> GetContexts()
{
    std::vector<std::unique_ptr<EmitterContext>> contexts;
    contexts.push_back(std::make_unique<ComputeContext>("Value_test"));
    contexts.push_back(std::make_unique<TestLLVMContext>(std::make_unique<IRModuleEmitter>("Value_test", CompilerOptions{})));
    return contexts;
}

} // ell
