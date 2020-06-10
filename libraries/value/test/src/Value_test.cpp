////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Value_test.cpp (value)
//  Authors:  Kern Handa, Mason Remy
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Value_test.h"
#include "TestUtil.h"

#include <value/include/Array.h>
#include <value/include/ComputeContext.h>
#include <value/include/CppEmitterContext.h>
#include <value/include/FunctionDeclaration.h>
#include <value/include/LLVMContext.h>
#include <value/include/Matrix.h>
#include <value/include/Tensor.h>
#include <value/include/Value.h>
#include <value/include/Vector.h>

#include <value/include/loopnests/CodeGenerator.h>
#include <value/include/loopnests/Kernel.h>
#include <value/include/loopnests/LoopNest.h>

#include <math/include/Matrix.h>
#include <math/include/Tensor.h>
#include <math/include/Vector.h>

#include <emitters/include/CompilerOptions.h>
#include <emitters/include/IRExecutionEngine.h>
#include <emitters/include/IRModuleEmitter.h>

#include <utilities/include/Exception.h>
#include <utilities/include/FunctionUtils.h>
#include <utilities/include/Logger.h>
#include <utilities/include/MemoryLayout.h>

#include <testing/include/testing.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <numeric>
#include <optional>
#include <thread>
#include <tuple>
#include <type_traits>
#include <vector>

#if !defined(WIN32)
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#else
#include <windows.h>
#endif // !defined(WIN32)

using namespace ell::utilities;
using namespace ell::logging;
using namespace ell::value;

#define PRINT_IR 0

namespace ell
{

void ValueGetTests()
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
} // namespace ell

Scalar Basic_test()
{
    return 0;
}

Scalar Array_test1()
{
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;

    constexpr int rows = 3, columns = 5, channels = 7;
    std::vector<int> arrayData(rows * columns * channels);
    std::generate(arrayData.begin(), arrayData.end(), [i = 0]() mutable { return ++i; });
    math::ChannelColumnRowTensor<int> mathTensor(3, 5, 7, arrayData);

    MemoryShape physicalSize{ rows, columns, channels };
    DimensionOrder dimensionOrder = RowMajorTensorOrder;
    MemoryLayout memoryLayout(physicalSize, dimensionOrder);
    Array array(Value(arrayData, memoryLayout));

    // Check shape
    {
        auto shape = array.GetValue().GetLayout().GetExtent();
        auto actual1 = static_cast<int>(shape[0]);
        auto expected1 = static_cast<int>(mathTensor.NumRows());
        if (actual1 != expected1)
        {
            DebugPrint("Array_test1: value::Array and math::Tensor row check failed\n");
            ok = 1;
        }

        auto actual2 = static_cast<int>(shape[1]);
        auto expected2 = static_cast<int>(mathTensor.NumColumns());
        if (actual2 != expected2)
        {
            DebugPrint("Array_test1: value::Array and math::Tensor column check failed\n");
            ok = 1;
        }

        auto actual3 = static_cast<int>(shape[2]);
        auto expected3 = static_cast<int>(mathTensor.NumChannels());
        if (actual3 != expected3)
        {
            DebugPrint("Array_test1: value::Array and math::Tensor channel check failed\n");
            ok = 1;
        }
    }

    // Check for loop iterations
    {
        Scalar count = Allocate(ValueType::Int32, ScalarLayout);

        // test we can enumerate all items of an array.
        For(array, [&](const std::vector<Scalar>& coordinates) {
            count += 1;
        });
        If(count != static_cast<int>(mathTensor.Size()), [&] {
            DebugPrint("Array_test1: for loop didn't visit all elements\n");
            ok = 1;
        });
    }

    Scalar ok2 = Allocate(ValueType::Int32, ScalarLayout);
    ok2 = 0;

    // Check operator(Scalar...)
    InvokeForContext<ComputeContext>([&](auto&) {
        // These tests use row.Get<int>() to get the actual row,col indexes as constants, which can
        // only be done during ComputeContext.

        // test we can enumerate all items of an array.
        For(array, [&](const std::vector<Scalar>& coordinates) {
            const auto& row = coordinates[0];
            const auto& col = coordinates[1];
            const auto& ch = coordinates[2];
            auto rowInt = row.Get<int>();
            auto colInt = col.Get<int>();
            auto chInt = ch.Get<int>();
            auto tensorVal = mathTensor(rowInt, colInt, chInt);
            Scalar expected = tensorVal;
            Scalar actual = array(row, col, ch);

            If(actual != expected, [&] {
                ok2 = 1;
            });
        });
        If(ok2 != 0, [&] {
            DebugPrint("Array_test1: value::Array and math::Tensor equality check failed\n");
            ok = 1;
        });
    });

    ok2 = 0;
    // Check operator(vector<Scalar>)
    InvokeForContext<ComputeContext>([&](auto&) {
        // These tests use row.Get<int>() to get the actual row,col indexes as constants, which can
        // only be done during ComputeContext.

        // test we can enumerate all items of an array.
        For(array, [&](const std::vector<Scalar>& coordinates) {
            const auto& row = coordinates[0];
            const auto& col = coordinates[1];
            const auto& ch = coordinates[2];
            auto rowInt = row.Get<int>();
            auto colInt = col.Get<int>();
            auto chInt = ch.Get<int>();
            Scalar expected = mathTensor(rowInt, colInt, chInt);
            Scalar actual = array(coordinates);
            If(actual != expected, [&] {
                ok2 = 1;
            });
        });
        If(ok2 != 0, [&] {
            DebugPrint("Array_test1: value::Array and math::Tensor equality check failed\n");
            ok = 1;
        });
    });

    return ok;
}

Scalar DebugPrint_test()
{
    DebugPrint("### Test that debug print is working: ");
    Vector v(std::vector<int>{ 1, 2, 3, 4 });
    DebugPrintVector(v);
    DebugPrint("\n");
    GetContext().DebugPrint("### Test that the emitter library DebugPrint is working\n");
    return 0;
}

Scalar Value_test1()
{
    Vector v(std::vector<int>{ 1, 2, 3, 4 });
    Scalar fail = 1;
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;
    For(v, [&](Scalar index) {
        If(index + 1 != v(index), [&] {
            ok = fail;
        });
    });
    If(ok != 0, [&] {
        DebugPrint("Value_test1 compare failed\n");
    });
    return ok;
}

Scalar For_test1()
{
    Vector input(std::vector<int>({ 1, 2, 3, 4 }));
    Vector actual = MakeVector<int>(input.Size());
    For(input, [&](Scalar index) {
        actual(index) = input(index);
    });
    return VerifySame(input, actual);
}

namespace
{
    void TripleLoop(value::Vector input, value::Vector output)
    {
        if (input.Size() == 0)
        {
            return;
        }

        Scalar max = Allocate(input.GetType(), ScalarLayout);
        max = Cast(0, input.GetType());
        For(input, [&](Scalar index) {
            Scalar v = input(index);
            If(v > max, [&] {
                max = v;
            });
        });

        Scalar sum = Allocate(input.GetType(), ScalarLayout);
        sum = Cast(0, input.GetType());
        For(input, [&](Scalar index) {
            Scalar v = input(index);
            v -= max;
            sum += v;
            output(index) = v;
        });

        For(output, [&](Scalar index) {
            Scalar v = input(index);
            v /= sum;
            output(index) = v;
        });
    }
} // namespace

Scalar For_test2()
{
    Vector input(std::vector<double>{ 1, 2, 3, 4, 5 });
    Vector expected(std::vector<double>{ 0.4, 0.3, 0.2, 0.1, 0 });
    Vector output = MakeVector<double>(input.Size());
    TripleLoop(input, output);
    return VerifySame(output, expected);
}

Scalar ForInsideIf_test()
{
    auto zero = MakeScalar<int>();
    auto sum = MakeScalar<int>();

    If(zero == Scalar(0), [&] {
        ForRange(10, [&](Scalar i) {
            sum += 1;
        });
    }).Else([&] {
        ForRange(10, [&](Scalar i) {
            sum += 2;
        });
    });

    Scalar ok = MakeScalar<int>();
    If(sum != 10, [&] { ok = 1; });
    return ok;
}

Scalar While_test()
{
    Scalar test = MakeScalar<Boolean>();
    Scalar count = MakeScalar<int>();

    test = (count != 5);
    While(test, [&] {
        count += 5;
        test = (count != 5);
    });

    Scalar ok = MakeScalar<int>();
    If(count != 5, [&] { ok = 1; });
    return ok;
}

Scalar WhileInsideIf_test()
{
    auto zero = MakeScalar<int>();
    auto count = MakeScalar<int>();
    count = 10;
    auto sum = MakeScalar<int>();
    Scalar notDone = MakeScalar<Boolean>();

    If(zero == Scalar(0), [&] {
        notDone = count > 0;
        While(notDone, [&] {
            sum += 1;
            count -= 1;
            notDone = count > 0;
        });
    }).Else([&] {
        notDone = count > 0;
        While(notDone, [&] {
            sum += 2;
            count -= 1;
            notDone = count > 0;
        });
    });

    Scalar ok = MakeScalar<int>();
    If(sum != 10, [&] { ok = 1; });
    return ok;
}

Scalar Casting_test1()
{
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;
    Vector floatVector = std::vector<float>{ 1.f, 2.f, 3.f };
    auto floatScalar = floatVector[1];
    Scalar intScalar = Cast<int>(floatScalar);
    Scalar globalIntScalar = GlobalAllocate("global", 3);
    intScalar += 1;
    floatScalar += 10.f;
    If(intScalar != 3, [&] {
        DebugPrint("Casting_test1 intScalar != 3\n");
        ok = 1;
    });
    If(intScalar != globalIntScalar, [&] {
        DebugPrint("Casting_test1 intScalar != globalIntScalar\n");
        ok = 1;
    });
    If(floatScalar != 12.f, [&] {
        DebugPrint("Casting_test1 floatScalar != 12.f\n");
        ok = 1;
    });
    If(floatScalar != floatVector(1), [&] {
        DebugPrint("Casting_test1 floatScalar != floatVector(1)\n");
        ok = 1;
    });
    return ok;
}

Scalar If_test1()
{
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;
    Scalar expected = 0;
    Scalar s1 = Allocate(ValueType::Int32, ScalarLayout);
    s1 = 1;
    If(s1 == 1, [&s1]() { s1 = 0; });

    If(s1 != expected, [&] {
        DebugPrint("Testing basic If expression failed\n");
        ok = 1;
    });

    s1 = 1;
    If(s1 == 0, [&s1]() { s1 = 3; }).Else([&s1]() { s1 = 0; });

    If(s1 != expected, [&] {
        DebugPrint("Testing basic If/Else expression failed\n");
        ok = 1;
    });

    s1 = 1;
    If(s1 == 3, [&s1]() { s1 = 2; }).ElseIf(s1 == 1, [&s1]() { s1 = 0; }).Else([&s1]() { s1 = 3; });

    If(s1 != expected, [&] {
        DebugPrint("Testing basic If/ElseIf/Else expression failed\n");
        ok = 1;
    });

    return ok;
}

Scalar Sum_test()
{
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;
    for (int i = 1; i < 10; ++i)
    {
        Vector v = MakeVector<float>(i);
        std::vector<float> reference(i);
        std::iota(reference.begin(), reference.end(), 0);
        auto expected = std::accumulate(reference.begin(), reference.end(), 0.f);

        v = reference;

        Scalar result = Sum(v);
        If(result != expected, [&] {
            ok = 1;
            InvokeForContext<ComputeContext>([&] {
                Log() << "### Sum_test failed for size " << i << "\n";
            });
        });
    }
    return ok;
}

Scalar Dot_test()
{
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;
    for (int i = 1; i < 10; ++i)
    {
        Vector v1 = MakeVector<float>(i), v2 = MakeVector<float>(i);
        std::vector<float> reference1(i), reference2(i);
        std::iota(reference1.begin(), reference1.end(), 0);
        std::iota(reference2.begin(), reference2.end(), reference1.back());

        v1 = reference1;
        v2 = reference2;

        Scalar result = Dot(v1, v2);
        Scalar expected = std::inner_product(reference1.begin(), reference1.end(), reference2.begin(), 0.f);
        If(result != expected, [&] {
            ok = 1;
        });
    }
    return ok;
}

namespace
{
    const std::vector<float> intrinsics_data{ 0.1f, 1.2f, 2.3f, 3.4f, 4.5f, 5.6f, 6.7f, 7.8f, 8.9f, 9.10f };

    template <typename Tuple, typename Idx = std::integral_constant<size_t, 0>>
    Scalar Intrinsics_test1_impl(Tuple tuple, Idx = {})
    {
        Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
        ok = 0;
        constexpr auto index = Idx::value;
        if constexpr (index < std::tuple_size_v<Tuple>)
        {
            auto& element = std::get<index>(tuple);
            auto fnName = std::string{ "Intrinsics_test1_" } + std::to_string(index);

            {
                auto f = element.first;
                std::vector<float> expected_data(intrinsics_data.size());
                std::transform(intrinsics_data.begin(), intrinsics_data.end(), expected_data.begin(), [f = element.second](float n) { return f(n); });
                Vector input = intrinsics_data;
                Vector actual = f(input);
                Vector expected = expected_data;
                If(VerifySame(actual, expected, 1e-5) != 0, [&] {
                    ok = 1;
                    DebugPrint("Intrinsics " + fnName + " test failed\n");
                });
            }

            // recursively process next item in the tuple
            Scalar r = Intrinsics_test1_impl(tuple, std::integral_constant<size_t, index + 1>{});

            If(r != 0, [&] {
                ok = 1; // bubble up the error
            });
        }
        return ok;
    }
} // namespace

Scalar Intrinsics_test1()
{
    return Intrinsics_test1_impl(
        std::tuple{
            std::pair{
                [](Vector v) { return Round(v); },
                [](float f) { return std::round(f); } },
            std::pair{
                [](Vector v) { return Abs(v); },
                [](float f) { return std::abs(f); } },
            std::pair{
                [](Vector v) { return Cos(v); },
                [](float f) { return std::cos(f); } },
            std::pair{
                [](Vector v) { return Exp(v); },
                [](float f) { return std::exp(f); } },
            std::pair{
                [](Vector v) { return Log(v); },
                [](float f) { return std::log(f); } },
            std::pair{
                [](Vector v) { return Log10(v); },
                [](float f) { return std::log10(f); } },
            std::pair{
                [](Vector v) { return Log2(v); },
                [](float f) { return std::log2(f); } },
            std::pair{
                [](Vector v) { return Pow(v, 3.14f); },
                [](float f) { return std::pow(f, 3.14f); } },
            std::pair{
                [](Vector v) { return Sin(v); },
                [](float f) { return std::sin(f); } },
            std::pair{
                [](Vector v) { return Sqrt(v); },
                [](float f) { return std::sqrt(f); } },
            std::pair{
                [](Vector v) { return Tanh(v); },
                [](float f) { return std::tanh(f); } },
        });
}

namespace
{
    template <typename Tuple, typename Idx = std::integral_constant<size_t, 0>>
    Scalar Intrinsics_test2_impl(Tuple tuple, Idx = {})
    {
        Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
        ok = 0;
        constexpr auto index = Idx::value;
        if constexpr (index < std::tuple_size_v<Tuple>)
        {
            int size = static_cast<int>(intrinsics_data.size());
            auto& element = std::get<index>(tuple);
            auto fnName = std::string{ "Intrinsics_test2_" } + std::to_string(index);

            auto fn = DeclareFunction(fnName)
                          .Parameters(Value(ValueType::Float, MemoryLayout{ { size } }))
                          .Returns(Value(ValueType::Float, ScalarLayout))
                          .Define([f = element.first](Vector v) {
                              v = intrinsics_data;
                              return f(v);
                          });

            auto f = element.second;
            Scalar expected = *f(intrinsics_data);
            Scalar actual = fn(MakeVector<float>(size));

            If(actual != expected, [&] {
                ok = 1;
                DebugPrint("Intrinsics " + fnName + " test 2 failed\n");
            });

            // recursively process next item in the tuple
            Scalar r = Intrinsics_test2_impl(tuple, std::integral_constant<size_t, index + 1>{});

            If(r != 0, [&] {
                ok = 1; // bubble up the error
            });
        }
        return ok;
    }

} // namespace

Scalar Intrinsics_test2()
{
    return Intrinsics_test2_impl(
        std::tuple{
            std::pair{
                [](Vector v) { return Max(v); },
                [](const std::vector<float>& v) { return std::max_element(v.begin(), v.end()); } },
            std::pair{
                [](Vector v) { return Min(v); },
                [](const std::vector<float>& v) { return std::min_element(v.begin(), v.end()); } },
            std::pair{
                [](Vector v) { return Max(v[0], v[1]); },
                [](const std::vector<float>& v) { return std::max_element(v.begin(), v.begin() + 2); } },
            std::pair{
                [](Vector v) { return Min(v[0], v[1]); },
                [](const std::vector<float>& v) { return std::min_element(v.begin(), v.begin() + 2); } },
        });
}

namespace
{
    template <typename T>
    Scalar ForRangeCasting_test_impl()
    {
        Scalar ok = Allocate<int>(ScalarLayout);

        Scalar acc = Allocate<T>(ScalarLayout);
        ForRange(4, [&](Scalar index) {
            auto casted = Cast<T>(index);
            acc += casted;
        });

        If(acc != static_cast<T>(6), [&] {
            ok = 1;
        });

        return ok;
    }
} // namespace

Scalar ForRangeCasting_test1()
{
    return ForRangeCasting_test_impl<int64_t>();
}

Scalar ForRangeCasting_test2()
{
    return ForRangeCasting_test_impl<float>();
}

Scalar Parallelized_test1()
{
    constexpr int NumThreads = 2;
    constexpr int DataPerThread = 3;
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    auto data = MakeVector<int>(NumThreads * DataPerThread);
    Parallelize(
        NumThreads,
        std::tuple{ data },
        std::function<void(Scalar, Vector)>{ [&](Scalar id, Vector capturedData) {
            ForRange(DataPerThread, [&](Scalar index) {
                capturedData[id * DataPerThread + index] = id;
            });
        } });

    auto expected = MakeVector<int>(data.Size());
    for (auto thread = 0; thread < NumThreads; ++thread)
    {
        for (auto dataIndex = 0; dataIndex < DataPerThread; ++dataIndex)
        {
            expected[thread * DataPerThread + dataIndex] = thread;
        }
    }

    If(VerifySame(data, expected) != 0, [&] {
        ok = 1;
    });

    return ok;
}

Scalar Parallelized_test2()
{
    constexpr int NumThreads = 2;
    constexpr int DataPerThread = 3;
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    auto data = MakeVector<int>(NumThreads * DataPerThread);
    Parallelize(
        NumThreads,
        std::tuple{ data },
        std::function<void(Scalar, Vector)>{ [&](Scalar id, Vector capturedData) {
            ForRange(DataPerThread, [&](Scalar index) {
                capturedData[id * DataPerThread + index] = GetTID();
            });
        } });

    auto expected = MakeVector<int>(data.Size());

    DebugPrint("  Parallelized_test2 result:   ");
    DebugPrintVector(data);
    DebugPrint("\n");

    If(VerifyDifferent(data, expected) == 1, [&] {
        ok = 1;
    });

    return ok;
}

Scalar Parallelized_test3()
{
    constexpr int DataPerThread = 8;
    constexpr int NumThreads = 4;
    constexpr int VectorSize = DataPerThread * NumThreads;
    auto A = MakeVector<int>(VectorSize);
    auto B = MakeVector<int>(VectorSize);

    Parallelize(
        NumThreads,
        std::tuple{ A, B },
        std::function<void(Scalar, Vector, Vector)>{ [&](Scalar id, Vector A, Vector B) {
            ForRange(DataPerThread, [&](Scalar index) {
                A[id * DataPerThread + index] = GetTID();
                B[id * DataPerThread + index] = id;
            });
        } });

    Scalar ok = Allocate<int>(ScalarLayout);
    ForRange(NumThreads, [&](Scalar i) {
        ForRange(DataPerThread, [&](Scalar j) {
            If(B[i * DataPerThread + j] != i, [&] { ok = 1; });
        });
    });

    return ok;
}

// Prefetches have no effect on the behavior of the program but can change its performance characteristics, so this
// test just makes sure that the code compiles/runs and behavior is not affected
// This test is just Sum_test with prefetching added in
Scalar Prefetch_test1()
{
    Scalar ok = Allocate<int>(ScalarLayout);
    for (int i = 1; i < 10; ++i)
    {
        Vector v = MakeVector<float>(i);
        std::vector<float> reference(i);
        std::iota(reference.begin(), reference.end(), 0);
        auto expected = std::accumulate(reference.begin(), reference.end(), 0.f);

        v = reference;

        Prefetch(v);
        Scalar result = Sum(v);
        If(result != expected, [&] {
            ok = 1;
            InvokeForContext<ComputeContext>([&] {
                Log() << "### Sum_test failed for size " << i << "\n";
            });
        });
    }
    return ok;
}

// Prefetches have no effect on the behavior of the program but can change its performance characteristics, so this
// test just makes sure that the code compiles/runs and behavior is not affected
Scalar Prefetch_parallelized_test1()
{
    constexpr int DataPerThread = 8;
    constexpr int NumThreads = 4;
    constexpr int VectorSize = DataPerThread * NumThreads;
    auto A = MakeVector<int>(VectorSize);
    auto B = MakeVector<int>(VectorSize);

    Parallelize(
        NumThreads,
        std::tuple{ A, B },
        std::function<void(Scalar, Vector, Vector)>{ [&](Scalar id, Vector A, Vector B) {
            ForRange(DataPerThread, [&](Scalar index) {
                A[id * DataPerThread + index] = GetTID();
                B[id * DataPerThread + index] = id;
            });
        } });

    Prefetch(A);
    Prefetch(B);

    Scalar ok = Allocate<int>(ScalarLayout);
    ForRange(NumThreads, [&](Scalar i) {
        ForRange(DataPerThread, [&](Scalar j) {
            If(B[i * DataPerThread + j] != i, [&] { ok = 1; });
        });
    });

    return ok;
}

Scalar Fma_test1()
{
    Scalar ok = Allocate<int>(ScalarLayout);

    constexpr float a_ = 3.14f, b_ = 1.8f, c_ = 8.1f, expected_ = a_ * b_ + c_;

    Scalar a = Allocate<float>(ScalarLayout);
    Scalar b = Allocate<float>(ScalarLayout);
    Scalar c = Allocate<float>(ScalarLayout);
    Scalar result = Allocate<float>(ScalarLayout);
    Scalar expected = Allocate<float>(ScalarLayout);

    a = a_;
    b = b_;
    c = c_;
    expected = expected_;
    result = FusedMultiplyAdd(a, b, c);

    If(NotEqualEpsilon(result, expected, 1e-5) == 1, [&] { ok = 1; });
    return ok;
}

Scalar Fma_test2()
{
    Scalar ok = Allocate<int>(ScalarLayout);

    constexpr double a_ = 1.763, b_ = 6.182, c_ = 9.1029, expected_ = a_ * b_ + c_;

    Scalar a = Allocate<double>(ScalarLayout);
    Scalar b = Allocate<double>(ScalarLayout);
    Scalar c = Allocate<double>(ScalarLayout);
    Scalar result = Allocate<double>(ScalarLayout);
    Scalar expected = Allocate<double>(ScalarLayout);

    a = a_;
    b = b_;
    c = c_;
    expected = expected_;
    result = FusedMultiplyAdd(a, b, c);

    If(NotEqualEpsilon(result, expected, 1e-7) == 1, [&] { ok = 1; });
    return ok;
}

Scalar Fma_test3()
{
    Scalar ok = Allocate<int>(ScalarLayout);

    constexpr int a_ = 8, b_ = 5, c_ = 2, expected_ = a_ * b_ + c_;

    Scalar a = Allocate<int>(ScalarLayout);
    Scalar b = Allocate<int>(ScalarLayout);
    Scalar c = Allocate<int>(ScalarLayout);
    Scalar result = Allocate<int>(ScalarLayout);
    Scalar expected = Allocate<int>(ScalarLayout);

    a = a_;
    b = b_;
    c = c_;
    expected = expected_;
    result = FusedMultiplyAdd(a, b, c);

    If(expected != result, [&] { ok = 1; });
    return ok;
}

Scalar UniqueName_test1()
{
    Scalar ok = Allocate<int>(ScalarLayout);

    ell::testing::IsEqual(UniqueName(""), "_0");
    ell::testing::IsEqual(UniqueName(""), "_1");

    ell::testing::IsEqual(UniqueName("foo"), "foo_0");
    ell::testing::IsEqual(UniqueName("foo"), "foo_1");

    return ok;
}

Scalar Parallelized_ComputeContext_test1()
{
    Scalar ok = Allocate<int>(ScalarLayout);

    InvokeForContext<ComputeContext>([] {
        constexpr auto numItems = 100000;
        constexpr auto numThreads = 16;

        bool ready = false;
        std::mutex m1, m2;
        std::condition_variable cv1, cv2;
        std::atomic_int atomicIndex = 0;

        std::array<std::thread, numThreads> threads;
        std::generate(std::begin(threads), std::end(threads), [&] {
            return std::thread([&] {
                std::unique_lock lock{ m1 };
                cv1.wait(lock, [&] { return ready; });
                int index{};
                while ((index = atomicIndex.fetch_add(1)) < numItems)
                {
                    [[maybe_unused]] Scalar s{ index };
                }

                cv2.notify_one();
            });
        });

        {
            std::unique_lock lock{ m1 };
            ready = true;
        }
        cv1.notify_all();

        {
            std::unique_lock lock{ m2 };
            cv2.wait(lock, [&] { return atomicIndex < numItems; });
        }
        std::for_each(std::begin(threads), std::end(threads), [](std::thread& thread) { thread.join(); });
    });

    return ok;
}

Scalar MemCopy_test1()
{
    auto vec = MakeVector<int>(4);

    std::vector expected{ 10, 20, 30, 40 };
    MemCopy(vec, Vector(expected));

    return VerifySame(vec, expected);
}

Scalar MemSet_test1()
{
    auto vec = MakeVector<int>(4);
    constexpr auto fill = char{ 0x3D };

    union
    {
        char c[sizeof(int)];
        int i;
    } expected;
    std::memset(&expected.c, fill, sizeof(expected.c));

    MemSet(vec, fill);

    auto ok = MakeScalar<int>();
    For(vec, [&](Scalar index) {
        If(vec[index] != expected.i, [&] {
            ok = 1;
        });
    });

    return ok;
}

Scalar NamedLoops_test1()
{
    {
        auto accum = MakeScalar<int>();
        ForRange(std::string{ "ForRangeLoop" }, 10, [&](Scalar index) { accum += index; });
    }

    {
        auto v = MakeVector<int>(10);
        For("ForVectorLoop", v, [&](Scalar index) { v[index] = index; });
    }

    {
        auto m = MakeMatrix<int>(10, 10);
        For("ForMatrixLoop", m, [&](Scalar row, Scalar col) { m(row, col) = row + row * col; });
    }

    {
        auto t = MakeTensor<int>(10, 10, 10);
        For("ForTensorLoop", t, [&](Scalar row, Scalar col, Scalar ch) { t(row, col, ch) = row + col + ch + ch * col * row; });
    }

    return MakeScalar<int>();
}

Scalar ThreadLocalAllocation_test1()
{
    auto ok = MakeScalar<int>("ok");

#ifdef WIN32
    // This thread is disabled for windows + LLVM due to issues with threading and TLS
    if (dynamic_cast<LLVMContext*>(&GetContext()) != nullptr)
    {
        return ok;
    }
#endif // WIN32

    constexpr int NumWorkItems = 40;
    auto threadIds = MakeVector<int>(NumWorkItems, "threadIds");
    Parallelize(
        NumWorkItems,
        std::tuple{ threadIds },
        std::function<void(Scalar, Vector)>{ [](Scalar threadId, Vector threadIds) {
            Scalar alreadySeen = StaticAllocate("AlreadySeen", ValueType::Int64, ScalarLayout, AllocateFlags::ThreadLocal);
            auto tid = Cast<int64_t>(GetTID());
            If(
                alreadySeen == int64_t{ 0 },
                [&] {
                    alreadySeen = tid;
                    threadIds[threadId] = 1;
                })
                .ElseIf(
                    alreadySeen != tid,
                    [&] {
                        threadIds[threadId] = -1;
                    });
        } });

    auto totalThreadsRan = MakeScalar<int>("totalThreadsRan");
    auto totalErrors = MakeScalar<int>("totalErrors");
    For(threadIds, [&](Scalar index) {
        If(threadIds[index] == 1, [&] {
            ++totalThreadsRan;
        }).ElseIf(threadIds[index] == -1, [&] {
            ++totalErrors;
        });
    });

    DebugPrint("Number of errors detected in TLS code: ");
    DebugPrintVector(AsVector(totalErrors));
    DebugPrint("\n");
    DebugPrint("Number of actual threads used to complete " + std::to_string(NumWorkItems) + " work items: ");
    DebugPrintVector(AsVector(totalThreadsRan));
    DebugPrint("\n");

    If(totalThreadsRan < 1, [&] { ok = 1; });
    If(totalErrors > 0, [&] { ok = 1; });

    return ok;
}

Scalar FunctionPointer_test1()
{
    auto ok = MakeScalar<int>("ok");

    // This thread is disabled CppEmitterContext for now
    if (dynamic_cast<CppEmitterContext*>(&GetContext()) != nullptr)
    {
        return ok;
    }

    auto realFnDecl = DeclareFunction("foo")
                          .Returns(Scalar(0))
                          .Parameters(Scalar(0));
    auto realFn = realFnDecl
                      .Define([](Scalar x) -> Scalar {
                          auto r = MakeScalar(x.GetType());
                          r = x + 10;
                          return r;
                      });

    auto fnPtr = DeclareFunction("bar").Returns(Scalar(0)).Parameters(Scalar(0));
    fnPtr.SetPointer(realFnDecl.GetPointer());

    auto in1 = MakeScalar<int>();
    in1 = 100;
    Scalar y = realFn(in1);

    If(y != 110, [&] { ok = 1; });

    in1 = 200;
    Scalar z = *fnPtr.Call(in1);

    If(z != 210, [&] { ok = 1; });

    return ok;
}
} // namespace ell
