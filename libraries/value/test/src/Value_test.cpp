////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Value_test.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Value_test.h"

#include <value/include/ComputeContext.h>
#include <value/include/FunctionDeclaration.h>
#include <value/include/LLVMContext.h>
#include <value/include/Matrix.h>
#include <value/include/Tensor.h>
#include <value/include/Value.h>
#include <value/include/Vector.h>

#include <math/include/Matrix.h>
#include <math/include/Tensor.h>
#include <math/include/Vector.h>

#include <utilities/include/Exception.h>
#include <utilities/include/FunctionUtils.h>

#include <testing/include/testing.h>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <limits>
#include <memory>
#include <numeric>
#include <optional>
#include <type_traits>
#include <vector>

#if !defined(WIN32)
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#else
#include <windows.h>
#endif // !defined(WIN32)

using namespace ell::emitters;
using namespace ell::utilities;
using namespace ell::value;
namespace math = ell::math;
using math::Dimension;
using math::MatrixLayout;

template <MatrixLayout layout>
using LayoutType = std::integral_constant<MatrixLayout, layout>;

using namespace ell::emitters;
using namespace ell::utilities;
using namespace ell::value;

#define PRINT_IR 0

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
    auto signal = Get1DReferenceSignal();
    auto filter = Get1DReferenceFilter();
    size_t resultSize = signal.size() - filter.size() + 1;
    std::vector<double> result(resultSize);

    for (size_t i = 0; i < resultSize; i++)
    {
        double accum = 0;
        for (size_t j = 0; j < filter.size(); j++)
        {
            accum += filter[j] * signal[i + j];
        }
        result[i] = accum;
    }
    return result;
}

template <
    typename T1,
    typename T2,
    typename T3 = std::conditional_t<sizeof(T1) >= sizeof(T2), T1, T2>>
inline std::enable_if_t<std::is_floating_point<T1>::value && std::is_floating_point<T2>::value, T3>
RelativeDifference(T1 a, T2 b)
{
    return std::fabs((a - b) / std::min<T3>(a, b));
}

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

namespace ell
{
namespace
{
    void PrintMatrix(std::string indent, Matrix e)
    {
        if (!e.GetValue().IsConstant())
        {
            std::cout << "cannot print non constant matrix";
            return;
        }
        if (e.Type() == ValueType::Undefined)
        {
            std::cout << "Undefined";
            return;
        }
        else if (e.Type() == ValueType::Void)
        {
            std::cout << "void";
            return;
        }
        std::cout << std::setprecision(10);
        int rows = static_cast<int>(e.Rows());
        int cols = static_cast<int>(e.Columns());
        for (int i = 0; i < rows; i++)
        {
            std::cout << indent;
            for (int j = 0; j < cols; j++)
            {
                if (j > 0)
                {
                    std::cout << ", ";
                }
                Scalar s = e(i, j);
                switch (s.GetType())
                {
                case ValueType::Undefined:
                    break;
                case ValueType::Void:
                    break;
                case ValueType::Boolean:
                    std::cout << (bool)s.Get<Boolean>();
                    break;
                case ValueType::Char8:
                    std::cout << s.Get<char>();
                    break;
                case ValueType::Byte:
                    std::cout << s.Get<uint8_t>();
                    break;
                case ValueType::Int16:
                    std::cout << s.Get<int16_t>();
                    break;
                case ValueType::Int32:
                    std::cout << s.Get<int32_t>();
                    break;
                case ValueType::Int64:
                    std::cout << s.Get<int64_t>();
                    break;
                case ValueType::Float:
                    std::cout << s.Get<float>();
                    break;
                case ValueType::Double:
                    std::cout << s.Get<double>();
                    break;
                }
            }
            std::cout << "\n";
        }
    }

    Scalar NotEqualEpsilon(Scalar x, Scalar y, double epsilon)
    {
        if (x.GetType() == ValueType::Int32)
        {
            return x != y;
        }
        Scalar e = Allocate(ValueType::Double, ScalarLayout);
        e = epsilon;
        Scalar tens = Floor(Log10(Cast(x, ValueType::Double)));
        If(tens > 0.0, [&] {
            // then we have some precision already on the left hand side of the decimal place, so remove that from epsilon
            e *= Pow(10.0, tens);
        });
        Scalar rx = Allocate(ValueType::Double, ScalarLayout);
        Scalar ry = Allocate(ValueType::Double, ScalarLayout);
        rx = Floor(Cast(x, ValueType::Double) / e) * e;
        ry = Floor(Cast(y, ValueType::Double) / e) * e;
        InvokeForContext<ComputeContext>([&] {
            double t = tens.Get<double>();
            double dx = rx.Get<double>();
            double dy = ry.Get<double>();
            if (dx != dy)
            {
                std::cout << std::setprecision(10);
                std::cout << "  NotEqualEpsilon failed: t=" << t << ": " << dx << " != " << dy << "\n";
            }
        });
        return rx != ry;
    }

    Scalar Verify(Vector actual, Vector expected, double epsilon = 1e-7)
    {
        Scalar fail = 1;
        Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
        ok = 0;
        For(actual, [&](Scalar index) {
            Scalar x = actual(index);
            Scalar y = expected(index);
            If(NotEqualEpsilon(x, y, epsilon), [&] {
                ok = fail;
            });
        });
        If(ok != 0, [&] {
            DebugPrint("## Vector compare failed\n");
            DebugPrint("  Expected: ");
            DebugPrintVector(expected);
            DebugPrint("\n");
            DebugPrint("  Actual:   ");
            DebugPrintVector(actual);
            DebugPrint("\n");
        });
        return ok;
    }

    Scalar Verify(Matrix actual, Matrix expected, double epsilon = 1e-7)
    {
        Scalar fail = 1;
        Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
        ok = 0;
        For(actual, [&](Scalar row, Scalar col) {
            Scalar x = actual(row, col);
            Scalar y = expected(row, col);
            If(NotEqualEpsilon(x, y, epsilon), [&] {
                ok = fail;
            });
        });
        If(ok != 0, [&] {
            DebugPrint("## Matrix compare failed\n");
            InvokeForContext<ComputeContext>([&] {
                std::cout << "Expected: \n";
                PrintMatrix("   ", expected);
                std::cout << "\n";
                std::cout << "Actual:   \n";
                PrintMatrix("   ", actual);
                std::cout << "\n";
            });
        });
        return ok;
    }

    Scalar Verify(Tensor actual, Tensor expected, double epsilon = 1e-7)
    {
        Scalar fail = Cast(1, ValueType::Int32);
        Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
        ok = 0;
        For(actual, [&](Scalar row, Scalar col, Scalar ch) {
            Scalar x = actual(row, col, ch);
            Scalar y = expected(row, col, ch);
            If(NotEqualEpsilon(x, y, epsilon), [&] {
                DebugPrint("## Tensor compare failed\n");
                ok = fail;
            });
        });
        return ok;
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
}

Scalar Basic_test()
{
    DebugPrint("### Test that debug print is working: ");
    Vector v(std::vector<int>{ 1, 2, 3, 4 });
    DebugPrintVector(v);
    DebugPrint("\n");
    return 0;
}

Scalar DebugPrint_test()
{
    DebugPrint("### Test that debug print is working: ");
    Vector v(std::vector<int>{ 1, 2, 3, 4 });
    DebugPrintVector(v);
    DebugPrint("\n");
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

Scalar Scalar_test1()
{
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;
    Scalar s1 = 1;
    If(s1 != 1, [&] {
        ok = 1;
        DebugPrint("Scalar_test1 if 1 failed\n");
    });

    s1 += 2;

    If(s1 != 3, [&] {
        ok = 1;
        DebugPrint("Scalar_test1 if 2 failed\n");
    });

    Scalar s2 = s1 + 3;

    If(s1 != 3, [&] {
        ok = 1;
        DebugPrint("Scalar_test1 if 3 failed\n");
    });

    If(s2 != 6, [&] {
        ok = 1;
        DebugPrint("Scalar_test1 if 4 failed\n");
    });

    return ok;
}

Vector testConvolve1D(Vector signal, Vector filter)
{
    size_t resultSize = signal.Size() - filter.Size() + 1;
    Vector result(Allocate(signal.GetType(), resultSize));
    Scalar accum = Allocate(signal.GetType(), ScalarLayout);
    For(result, [&](Scalar index) {
        accum = Cast(0, accum.GetType());
        For(filter, [&](Scalar filterIndex) {
            accum += filter(filterIndex) * signal(index + filterIndex);
        });
        result(index) = accum;
    });

    return result;
}

Scalar Vector_test1()
{
    auto signal = Get1DReferenceSignal();
    auto filter = Get1DReferenceFilter();
    auto referenceResult = Get1DReferenceConvolutionResult();

    // this works
    Vector result = testConvolve1D(signal, filter);

    // but the following does not.
    //auto valueType = GetValueType<decltype(signal)::value_type>();
    //uto convolve1D = DeclareFunction("Convolve1D")
    //                     .Returns({ valueType, MemoryLayout({ (int)referenceResult.size() }) })
    //                     .Parameters(
    //                         Value{ valueType, MemoryLayout({ (int)signal.size() }) },
    //                         Value{ valueType, MemoryLayout({ (int)filter.size() }) })
    //                     .Define(testConvolve1D);
    // Vector result = convolve1D(signal, filter);

    Vector expected(referenceResult);
    return Verify(result, expected);
}

Scalar Vector_test2()
{
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;
    Vector v = std::vector<float>{ 1.2f, 2.3f };
    Vector testVector(std::vector<float>{ 0.1f, 1.2f });
    Scalar testScalar{ 3.4f };
    {
        Vector expected(std::vector<float>{ 1.2f + 3.4f, 2.3f + 3.4f });
        Vector actual = v + testScalar;
        If(Verify(actual, expected) != 0, [&] {
            DebugPrint("## Vector_test2 vector scalar addition failed\n");
            ok = 1;
        });
    }
    {
        Vector expected(std::vector<float>{ 1.2f - 3.4f, 2.3f - 3.4f });
        Vector actual = v - testScalar;
        If(Verify(actual, expected) != 0, [&] {
            DebugPrint("## Vector_test2 vector scalar subtraction failed\n");
            ok = 1;
        });
    }
    {
        Vector expected(std::vector<float>{ 1.2f * 3.4f, 2.3f * 3.4f });
        Vector actual = v * testScalar;
        If(Verify(actual, expected) != 0, [&] {
            DebugPrint("## Vector_test2 vector scalar multiplication failed\n");
            ok = 1;
        });
    }
    {
        Vector expected(std::vector<float>{ 1.2f / 3.4f, 2.3f / 3.4f });
        Vector actual = v / testScalar;
        If(Verify(actual, expected) != 0, [&] {
            DebugPrint("## Vector_test2 vector scalar division failed\n");
            ok = 1;
        });
    }

    // Vector +- Vector -> Vector
    {
        Vector expected(std::vector<float>{ 1.2f + 0.1f, 2.3f + 1.2f });
        Vector actual = v + testVector;
        If(Verify(actual, expected) != 0, [&] {
            DebugPrint("## Vector_test2 vector+vector failed\n");
            ok = 1;
        });
    }
    {
        Vector expected(std::vector<float>{ 1.2f - 0.1f, 2.3f - 1.2f });
        Vector actual = v - testVector;
        If(Verify(actual, expected) != 0, [&] {
            DebugPrint("## Vector_test2 vector-vector failed\n");
            ok = 1;
        });
    }
    return ok;
}

namespace
{
    template <MatrixLayout layout>
    Scalar Matrix_test1Impl(std::integral_constant<MatrixLayout, layout>)
    {
        Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
        ok = 0;
        constexpr int rows = 3, columns = 4;
        std::vector<int> matrixData(rows * columns);
        std::generate(matrixData.begin(), matrixData.end(), [i = 0]() mutable { return ++i; });
        math::Matrix<int, layout> mathMatrix(rows, columns, matrixData);

        auto dimensionOrder = layout == MatrixLayout::rowMajor ? DimensionOrder{ 0, 1 } : DimensionOrder{ 1, 0 };
        auto physicalSize =
            layout == MatrixLayout::rowMajor ? std::vector({ rows, columns }) : std::vector({ columns, rows });
        MemoryLayout memoryLayout(physicalSize, dimensionOrder);
        Matrix matrix(Value(matrixData, memoryLayout));

        if (matrix.Columns() != mathMatrix.NumColumns())
        {
            DebugPrint("## value::Matrix and math::Matrix column check failed\n");
            ok = 1;
        };
        if (matrix.Rows() != mathMatrix.NumRows())
        {
            DebugPrint("## value::Matrix and math::Matrix row check failed\n");
            ok = 1;
        };

        Scalar ok2 = Allocate(ValueType::Int32, ScalarLayout);
        ok2 = 0;
        // test each row slice is correct
        for (size_t rowIndex = 0; rowIndex < matrix.Rows(); ++rowIndex)
        {
            auto mathRowVector = mathMatrix.GetRow(rowIndex);
            auto rowVector = matrix.Row((int)rowIndex);
            Vector expected = mathRowVector.ToArray();
            If(Verify(rowVector, expected) != 0, [&] {
                ok2 = 1;
            });
        }
        If(ok2 != 0, [&] {
            DebugPrint("value::Matrix and math::Matrix row slice equality check failed\n");
            ok = 1;
        });

        ok2 = 0;
        // check each column slice is correct.
        for (size_t columnIndex = 0; columnIndex < matrix.Rows(); ++columnIndex)
        {
            auto mathColumnVector = mathMatrix.GetColumn(columnIndex);
            auto columnVector = matrix.Column((int)columnIndex);
            Vector expected = mathColumnVector.ToArray();
            If(Verify(columnVector, expected) != 0, [&] {
                ok2 = 1;
            });
        }
        If(ok2 != 0, [&] {
            DebugPrint("value::Matrix and math::Matrix column slice equality check failed\n");
            ok = 1;
        });

        return ok;
    }

} // namespace

Scalar Matrix_test1()
{
    Scalar ok = Matrix_test1Impl(LayoutType<MatrixLayout::rowMajor>{});
    Scalar ok2 = Matrix_test1Impl(LayoutType<MatrixLayout::columnMajor>{});
    If(ok2 != 0, [&] {
        ok = 1;
    });
    return ok;
}

Scalar Matrix_test2()
{
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;

    std::vector<std::vector<int>> data{
        std::vector<int>{ 1, 2, 3 },
        std::vector<int>{ 4, 5, 6 },
    };
    Matrix m(data);

    Scalar actual = static_cast<int>(m.Rows());
    Scalar expected = 2;

    If(actual != expected, [&] {
        DebugPrint("Matrix_test2 should have 2 rows\n");
        ok = 1;
    });

    Scalar actual2 = static_cast<int>(m.Columns());
    Scalar expected2 = 3;

    If(actual2 != expected2, [&] {
        DebugPrint("Matrix_test2 should have 3 columns\n");
        ok = 1;
    });

    Scalar actual3 = m(1, 2);
    Scalar expected3 = data[1][2];

    If(actual3 != expected3, [&] {
        DebugPrint("Matrix_test2 item at (1,2) has incorrect value\n");
        ok = 1;
    });

    return ok;
}

Scalar Matrix_test3()
{
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;

    Matrix m = std::vector<std::vector<float>>{
        std::vector<float>{ 1.2f, 2.3f },
        std::vector<float>{ 3.4f, 4.5f }
    };

    Matrix testMatrix(std::vector<std::vector<float>>{
        std::vector<float>{ 0.1f, 1.2f },
        std::vector<float>{ 2.3f, 3.4f } });
    Scalar testScalar{ 3.4f };

    {
        Matrix expected(std::vector<std::vector<float>>{
            std::vector<float>{ 1.2f + 3.4f, 2.3f + 3.4f },
            std::vector<float>{ 3.4f + 3.4f, 4.5f + 3.4f } });
        Matrix actual = m + testScalar;
        If(0 != Verify(actual, expected), [&] {
            DebugPrint("Matrix_test3 matrix scalar addition failed \n");
            ok = 1;
        });
    }
    {
        Matrix expected(std::vector<std::vector<float>>{
            std::vector<float>{ 1.2f - 3.4f, 2.3f - 3.4f },
            std::vector<float>{ 3.4f - 3.4f, 4.5f - 3.4f } });
        Matrix actual = m - testScalar;
        If(0 != Verify(actual, expected), [&] {
            DebugPrint("Matrix_test3 matrix scalar subtraction failed \n");
            ok = 1;
        });
    }
    {
        Matrix expected(std::vector<std::vector<float>>{
            std::vector<float>{ 1.2f * 3.4f, 2.3f * 3.4f },
            std::vector<float>{ 3.4f * 3.4f, 4.5f * 3.4f } });
        Matrix actual = m * testScalar;
        If(0 != Verify(actual, expected), [&] {
            DebugPrint("Matrix_test3 matrix scalar multiplication failed \n");
            ok = 1;
        });
    }
    {
        Matrix expected(std::vector<std::vector<float>>{
            std::vector<float>{ 1.2f / 3.4f, 2.3f / 3.4f },
            std::vector<float>{ 3.4f / 3.4f, 4.5f / 3.4f } });
        Matrix actual = m / testScalar;
        If(0 != Verify(actual, expected), [&] {
            DebugPrint("Matrix_test3 matrix scalar division failed \n");
            ok = 1;
        });
    }

    // Vector +- Vector -> Vector
    {
        Matrix expected(std::vector<std::vector<float>>{
            std::vector<float>{ 1.2f + 0.1f, 2.3f + 1.2f },
            std::vector<float>{ 3.4f + 2.3f, 4.5f + 3.4f } });
        Matrix actual = m + testMatrix;
        If(0 != Verify(actual, expected), [&] {
            DebugPrint("Matrix_test3 matrix + matrix failed \n");
            ok = 1;
        });
    }
    {
        Matrix expected(std::vector<std::vector<float>>{
            std::vector<float>{ 1.2f - 0.1f, 2.3f - 1.2f },
            std::vector<float>{ 3.4f - 2.3f, 4.5f - 3.4f } });
        Matrix actual = m - testMatrix;
        If(0 != Verify(actual, expected), [&] {
            DebugPrint("Matrix_test3 matrix - matrix failed \n");
            ok = 1;
        });
    }
    return ok;
}
Scalar Tensor_test1()
{
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;

    constexpr int rows = 3, columns = 5, channels = 7;
    std::vector<int> tensorData(rows * columns * channels);
    std::generate(tensorData.begin(), tensorData.end(), [i = 0]() mutable { return ++i; });
    math::ChannelColumnRowTensor<int> mathTensor(3, 5, 7, tensorData);

    MemoryShape physicalSize{ rows, columns, channels };
    DimensionOrder dimensionOrder = RowMajorTensorOrder;
    MemoryLayout memoryLayout(physicalSize, dimensionOrder);
    Tensor tensor(Value(tensorData, memoryLayout));

    {
        Scalar actual = static_cast<int>(tensor.Columns());
        Scalar expected = static_cast<int>(mathTensor.NumColumns());
        If(actual != expected, [&] {
            DebugPrint("Tensor_test1: value::Tensor and math::Tensor column check failed\n");
            ok = 1;
        });

        Scalar actual2 = static_cast<int>(tensor.Rows());
        Scalar expected2 = static_cast<int>(mathTensor.NumRows());
        If(actual != expected, [&] {
            DebugPrint("Tensor_test1: value::Tensor and math::Tensor row check failed\n");
            ok = 1;
        });

        Scalar actual3 = static_cast<int>(tensor.Channels());
        Scalar expected3 = static_cast<int>(mathTensor.NumChannels());
        If(actual != expected, [&] {
            DebugPrint("Tensor_test1: value::Tensor and math::Tensor channel check failed\n");
            ok = 1;
        });
    }

    Scalar ok2 = Allocate(ValueType::Int32, ScalarLayout);
    ok2 = 0;

    InvokeForContext<ComputeContext>([&] {
        // These tests use row.Get<int>() to get the actual row,col indexes as constants, which can
        // only be done during ComputeContext.

        // test we can enummerate all items of a tensor.
        For(tensor, [&](Scalar row, Scalar col, Scalar ch) {
            auto rowInt = row.Get<int>();
            auto colInt = col.Get<int>();
            auto chInt = ch.Get<int>();
            Scalar expected = mathTensor(rowInt, colInt, chInt);
            Scalar actual = tensor(row, col, ch);
            If(actual != expected, [&] {
                ok2 = 1;
            });
        });
        If(ok2 != 0, [&] {
            DebugPrint("Tensor_test1: value::Tensor and math::Tensor equality check failed\n");
            ok = 1;
        });

        ok2 = 0;
        // test we can get matrix slices of a tensor rows and channels
        for (int column = 0; column < (int)mathTensor.NumColumns(); ++column)
        {
            auto mathSlicedMatrix1 = math::GetSlice<Dimension::row, Dimension::channel>(mathTensor, column);
            auto slicedMatrix1 = tensor.Slice(Slice::All, column, Slice::All);
            For(slicedMatrix1, [&](Scalar row, Scalar col) {
                auto rowInt = row.Get<int>();
                auto colInt = col.Get<int>();
                Scalar expected = mathSlicedMatrix1(rowInt, colInt);
                Scalar actual = slicedMatrix1(row, col);
                If(actual != expected, [&] {
                    ok2 = 1;
                });
            });
        }

        If(ok2 != 0, [&] {
            DebugPrint("Tensor_test1: value::Tensor and math::Tensor(row,channel) matrix slice equality check\n");
            ok = 1;
        });

        ok2 = 0;
        // test we can get matrix slices of a tensor columns and channels
        for (int row = 0; row < (int)mathTensor.NumRows(); ++row)
        {
            auto mathSlicedMatrix1 = math::GetSlice<Dimension::column, Dimension::channel>(mathTensor, row);
            auto slicedMatrix1 = tensor.Slice(row, Slice::All, Slice::All);

            For(slicedMatrix1, [&](Scalar row, Scalar col) {
                auto rowInt = row.Get<int>();
                auto colInt = col.Get<int>();
                Scalar expected = mathSlicedMatrix1(rowInt, colInt);
                Scalar actual = slicedMatrix1(row, col);
                If(actual != expected, [&] {
                    ok2 = 1;
                });
            });
        }
        If(ok2 != 0, [&] {
            DebugPrint("Tensor_test1: value::Tensor and math::Tensor(col,channel) matrix slice equality check\n");
            ok = 1;
        });
    });

    ok2 = 0;
    for (int row = 0; row < (int)mathTensor.NumRows(); ++row)
    {
        for (int column = 0; column < (int)mathTensor.NumColumns(); ++column)
        {
            for (int channel = 0; channel < (int)mathTensor.NumChannels(); ++channel)
            {
                {
                    Vector mathSlicedVector = math::GetSlice<Dimension::row>(mathTensor, column, channel).ToArray();
                    auto slicedVector = tensor.Slice(Slice::All, column, channel);
                    If(Verify(slicedVector, mathSlicedVector) != 0, [&] {
                        ok2 = 1;
                    });
                }
                {
                    Vector mathSlicedVector = math::GetSlice<Dimension::column>(mathTensor, row, channel).ToArray();
                    auto slicedVector = tensor.Slice(row, Slice::All, channel);
                    If(Verify(slicedVector, mathSlicedVector) != 0, [&] {
                        ok2 = 1;
                    });
                }
            }
            Vector mathSlicedVector = math::GetSlice<Dimension::channel>(mathTensor, row, column).ToArray();
            auto slicedVector = tensor.Slice(row, column, Slice::All);
            If(Verify(slicedVector, mathSlicedVector) != 0, [&] {
                ok2 = 1;
            });
        }
    }
    If(ok2 != 0, [&] {
        DebugPrint("Tensor_test1: enumerating value::Tensor and math::Tensor every which way failed\n");
        ok = 1;
    });
    return ok;
}

Scalar Tensor_test2()
{
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;
    std::vector<std::vector<std::vector<int>>> data{ std::vector<std::vector<int>>{
                                                         std::vector<int>{ 1, 2, 3 },
                                                         std::vector<int>{ 4, 5, 6 },
                                                     },
                                                     std::vector<std::vector<int>>{
                                                         std::vector<int>{ 7, 8, 9 },
                                                         std::vector<int>{ 10, 11, 12 },
                                                     } };
    Tensor t(data);
    Scalar actual = static_cast<int>(t.Rows());
    Scalar expected = 2;
    If(actual != expected, [&] {
        DebugPrint("Tensor_test2: Tensor Rows() != 2\n");
        ok = 1;
    });

    Scalar actual2 = static_cast<int>(t.Columns());
    Scalar expected2 = 2;
    If(actual2 != expected2, [&] {
        DebugPrint("Tensor_test2: Tensor Columns() != 2\n");
        ok = 1;
    });

    Scalar actual3 = static_cast<int>(t.Channels());
    Scalar expected3 = 3;
    If(actual3 != expected3, [&] {
        DebugPrint("Tensor_test2: Tensor Channels() != 3\n");
        ok = 1;
    });

    Scalar actual4 = t(1, 0, 2);
    Scalar expected4 = data[1][0][2];
    If(actual4 != expected4, [&] {
        DebugPrint("Tensor_test2: Tensor t(1, 0, 2) failed\n");
        ok = 1;
    });

    return ok;
}

Scalar Tensor_test3()
{
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;
    Tensor t =
        std::vector<std::vector<std::vector<float>>>{
            std::vector<std::vector<float>>{
                std::vector<float>{ 1.2f, 2.3f },
                std::vector<float>{ 3.4f, 4.5f } },
            std::vector<std::vector<float>>{
                std::vector<float>{ 5.4f, 4.3f },
                std::vector<float>{ 3.2f, 2.1f } },
        };
    float s = 3.4f;
    Scalar testScalar{ s };

    {
        Tensor expected =
            std::vector<std::vector<std::vector<float>>>{
                std::vector<std::vector<float>>{
                    std::vector<float>{ 1.2f + s, 2.3f + s },
                    std::vector<float>{ 3.4f + s, 4.5f + s } },
                std::vector<std::vector<float>>{
                    std::vector<float>{ 5.4f + s, 4.3f + s },
                    std::vector<float>{ 3.2f + s, 2.1f + s } },
            };
        Tensor actual = t + testScalar;
        If(Verify(actual, expected) != 0, [&] {
            ok = 1;
            DebugPrint("Tensor_test3: Tensor scalar addition failed\n");
        });
    }
    {
        Tensor expected =
            std::vector<std::vector<std::vector<float>>>{
                std::vector<std::vector<float>>{
                    std::vector<float>{ 1.2f - s, 2.3f - s },
                    std::vector<float>{ 3.4f - s, 4.5f - s } },
                std::vector<std::vector<float>>{
                    std::vector<float>{ 5.4f - s, 4.3f - s },
                    std::vector<float>{ 3.2f - s, 2.1f - s } },
            };
        Tensor actual = t - testScalar;
        If(Verify(actual, expected) != 0, [&] {
            ok = 1;
            DebugPrint("Tensor_test3: Tensor scalar subtraction failed\n");
        });
    }
    {
        Tensor expected =
            std::vector<std::vector<std::vector<float>>>{
                std::vector<std::vector<float>>{
                    std::vector<float>{ 1.2f * s, 2.3f * s },
                    std::vector<float>{ 3.4f * s, 4.5f * s } },
                std::vector<std::vector<float>>{
                    std::vector<float>{ 5.4f * s, 4.3f * s },
                    std::vector<float>{ 3.2f * s, 2.1f * s } },
            };
        Tensor actual = t * testScalar;
        If(Verify(actual, expected) != 0, [&] {
            ok = 1;
            DebugPrint("Tensor_test3: Tensor scalar multiplication failed\n");
        });
    }
    {
        Tensor expected =
            std::vector<std::vector<std::vector<float>>>{
                std::vector<std::vector<float>>{
                    std::vector<float>{ 1.2f / s, 2.3f / s },
                    std::vector<float>{ 3.4f / s, 4.5f / s } },
                std::vector<std::vector<float>>{
                    std::vector<float>{ 5.4f / s, 4.3f / s },
                    std::vector<float>{ 3.2f / s, 2.1f / s } },
            };
        Tensor actual = t / testScalar;
        If(Verify(actual, expected) != 0, [&] {
            ok = 1;
            DebugPrint("Tensor_test3: Tensor scalar division failed\n");
        });
    }

    return ok;
}

template <typename T>
Matrix ToMatrix(T mathMatrix)
{
    std::vector<double> flat;
    for (size_t i = 0; i < mathMatrix.NumRows(); i++)
    {
        for (size_t j = 0; j < mathMatrix.NumColumns(); j++)
        {
            flat.push_back(mathMatrix(i, j));
        }
    }
    return Matrix(flat, mathMatrix.NumRows(), mathMatrix.NumColumns());
}

Scalar Tensor_slice_test1()
{
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;

    auto input =
        std::vector<double>{ 11, 22, 33, 44, 55, 66, 77, 88, 99, 111, 222, 333, 444, 555, 666, 777, 888, 999 };

    constexpr int rows = 3, cols = 3, chs = 2;
    Tensor inputTensor({ input, MemoryLayout({ chs, rows, cols }, DimensionOrder(ChannelMajorTensorOrder)) });
    math::ColumnRowChannelTensor<double> mathTensor(cols, rows, chs, input);

    // channel major order:
    // input[:, :, ch] will be in canonical order row-major matrix order
    // input[:, :, 0] =
    //     [[11, 22, 33],
    //     [44, 55, 66],
    //     [77, 88, 99]]
    // input[:, :, 1] =
    //     [[111, 222, 333],
    //     [444, 555, 666],
    //     [777, 888, 999]]
    //
    /*
    // channel major enumeration of the tensor prints the above output.
    for (size_t channel = 0; channel < chs; channel++)
    {
        for (size_t row = 0; row < rows; row++)
        {
            for (size_t col = 0; col < cols; col++)
            {
                if (col > 0) std::cout << ", ";
                std::cout << mathTensor(row, col, channel);
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }
*/
    Scalar ok2 = Allocate(ValueType::Int32, ScalarLayout);
    ok2 = 0;
    InvokeForContext<ComputeContext>([&] {
        // This tests uses row.Get<int>() to get the actual row, col indexes as constants, which can
        // only be done during ComputeContext.
        For(inputTensor, [&](Scalar row, Scalar col, Scalar ch) {
            int rowInt = row.Get<int>(), colInt = col.Get<int>(), chInt = ch.Get<int>();
            Scalar mathElement = mathTensor(rowInt, colInt, chInt);
            Scalar valueElement = inputTensor(row, col, ch);
            If(mathElement != valueElement, [&] {
                ok2 = 1;
            });
        });
        If(ok2 != 0, [&] {
            DebugPrint("Tensor_slice_test1: channel-major order enumeration failed\n");
            ok = 1;
        });
    });

    {
        { Matrix mathMatrix = ToMatrix(mathTensor.GetSlice<Dimension::row, Dimension::column>(0));
    auto matrix = inputTensor.Slice(Slice::All, Slice::All, 0);

    If(Verify(matrix, mathMatrix) != 0, [&] {
        ok = 1;
        DebugPrint("Tensor_slice_test1: Tensor row-column GetSlice failed\n");
    });
}
} // namespace ell

{
    { // We can't use ToArray() on this slice because data is not stored in the same layout, we have to build flat vector manually.
      auto slice = mathTensor.GetSlice<Dimension::column, Dimension::channel>(0);
Matrix mathMatrix = ToMatrix(slice);
auto matrix = inputTensor.Slice(0, Slice::All, Slice::All);

If(Verify(matrix, mathMatrix) != 0, [&] {
    ok = 1;
    DebugPrint("Tensor_slice_test1: Tensor column-channel GetSlice failed\n");
});
}
}

{
    { Vector mathVector = mathTensor.GetSlice<Dimension::channel>(0, 0).ToArray();
auto vector = inputTensor.Slice(0, 0, Slice::All);

If(Verify(mathVector, vector) != 0, [&] {
    ok = 1;
    DebugPrint("Tensor_slice_test1: Tensor channel vector failed\n");
});
}
}

{
    { Vector mathVector = mathTensor.GetSlice<Dimension::column>(0, 0).ToArray();
auto vector = inputTensor.Slice(0, Slice::All, 0);

If(Verify(mathVector, vector) != 0, [&] {
    ok = 1;
    DebugPrint("Tensor_slice_test1: Tensor column vector failed");
});
}
}

{
    {
        Vector mathVector = mathTensor.GetSlice<Dimension::row>(0, 0).ToArray();
        auto vector = inputTensor.Slice(Slice::All, 0, 0);

        If(Verify(mathVector, vector) != 0, [&] {
            ok = 1;
            DebugPrint("Tensor_slice_test1: Tensor row vector failed");
        });
    }
}
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
        std::function{ [&](Scalar id, Vector capturedData) {
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
        std::function{ [&](Scalar id, Vector capturedData) {
            ForRange(DataPerThread, [&](Scalar index) {
                capturedData[id * DataPerThread + index] = GetTID();
            });
        } });

    auto expected = MakeVector<int>(data.Size());

    If(Verify(data, expected) == 0, [&] {
        ok = 1;
    });

    return ok;
}

} // namespace ell
