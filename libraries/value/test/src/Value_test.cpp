////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Value_test.cpp (value)
//  Authors:  Kern Handa, Mason Remy
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Value_test.h"
#include "TestUtil.h"

#include <value/include/ComputeContext.h>
#include <value/include/FunctionDeclaration.h>
#include <value/include/LLVMContext.h>
#include <value/include/Value.h>
#include <value/include/Vector.h>

#include <testing/include/testing.h>

#include <utilities/include/Exception.h>
#include <utilities/include/FunctionUtils.h>
#include <utilities/include/MemoryLayout.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>
#include <optional>
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
using namespace ell::value;

#define PRINT_IR 0

namespace ell
{
namespace
{
    Scalar GetTID()
    {
        if (auto result = InvokeForContext<ComputeContext>(
                [](auto&) {
#if !defined(WIN32)
                    return (int32_t)(pid_t)syscall(SYS_gettid);
#else
                    return (int32_t)GetCurrentThreadId();
#endif // !defined(WIN32)
                });
            result)
        {
            return *result;
        }

        if (auto result = InvokeForContext<LLVMContext>(
                [] {
                    return Scalar(
#if !defined(WIN32)
                        *DeclareFunction("syscall")
                             .Decorated(FunctionDecorated::No)
                             .Returns(Value({ ValueType::Int64, 0 }, ScalarLayout))
                             .Parameters(
                                 Value({ ValueType::Int64, 0 }, ScalarLayout))
                             .Call(Scalar{ (int64_t)SYS_gettid })
#else
                        *DeclareFunction("GetCurrentThreadId")
                             .Decorated(FunctionDecorated::No)
                             .Returns(Value({ ValueType::Int32, 0 }, ScalarLayout))
                             .Call()
#endif // !defined(WIN32)
                    );
                }))
        {
            return Cast<int>(*result);
        }

        throw LogicException(LogicExceptionErrors::notImplemented);
    }

} // namespace

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
        return Verify(input, actual);
    }

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

    Scalar For_test2()
    {
        Vector input(std::vector<double>{ 1, 2, 3, 4, 5 });
        Vector expected(std::vector<double>{ 0.4, 0.3, 0.2, 0.1, 0 });
        Vector output = MakeVector<double>(input.Size());
        TripleLoop(input, output);
        return Verify(output, expected);
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
                    std::cout << "### Sum_test failed for size " << i << "\n";
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
                    If(Verify(actual, expected, 1e-5) != 0, [&] {
                        ok = 1;
                        DebugPrint("Intrinsics " + fnName + " test failed\n");
                    });
                }

                // recurrsively process next item in the tuple
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

        If(Verify(data, expected) != 0, [&] {
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

    // Prefetches have no effect on the behavior of the program but can change its performance characteristics, so this
    // test just makes sure that the code compiles/runs and behavior is not affected
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
    Scalar Prefetch_test1()
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

} // namespace ell
