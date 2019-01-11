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

#include <emitters/include/IRModuleEmitter.h>

#include <math/include/Matrix.h>
#include <math/include/Tensor.h>
#include <math/include/Vector.h>

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
    return std::vector<double>{ 0.77013919, 0.81368187, 0.56914835, 0.30732139, 0.34824032, 0.53571473, 0.48653128, 0.21208796, 0.17427497, 0.39217245, 0.44620757, 0.49905383, 0.74073549, 0.73957347 };
}

struct TestLLVMContext : public LLVMContext
{
    TestLLVMContext(std::unique_ptr<IRModuleEmitter> emitter) :
        LLVMContext(*emitter),
        _emitter(std::move(emitter)) {}

    void DebugDump() { _emitter->DebugDump(); }

private:
    std::unique_ptr<IRModuleEmitter> _emitter;
};

void PrintIR(TestLLVMContext& context)
{
#if PRINT_IR
    context.DebugDump();
#endif // PRINT_IR
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

} // namespace

namespace ell
{

namespace
{
    template <typename T = void>
    bool Verify(Vector actual, Vector expected)
    {
        bool ok = true;
        For(actual, [&](Scalar index) {
            if constexpr (std::is_same_v<T, void>)
            {
                If(actual(index) == expected(index), [&] {
                    InvokeForContext<ComputeContext>([&](ComputeContext&) {
                        ok &= true;
                    });
                })
                    .Else([&] {
                        InvokeForContext<ComputeContext>([&](ComputeContext&) {
                            ok &= false;
                        });
                    });
            }
            else
            {
                InvokeForContext<ComputeContext>([&](ComputeContext&) {
                    ok &= testing::IsEqual(actual(index).Get<T>(), expected(index).Get<T>());
                });
            }
        });
        return ok;
    }

    template <typename T = void>
    bool Verify(Matrix actual, Matrix expected)
    {
        bool ok = true;
        For(actual, [&](Scalar row, Scalar col) {
            if constexpr (std::is_same_v<T, void>)
            {
                If(actual(row, col) == expected(row, col), [&] {
                    InvokeForContext<ComputeContext>([&](ComputeContext&) {
                        ok &= true;
                    });
                })
                    .Else([&] {
                        InvokeForContext<ComputeContext>([&](ComputeContext&) {
                            ok &= false;
                        });
                    });
            }
            else
            {
                InvokeForContext<ComputeContext>([&](ComputeContext&) {
                    ok &= testing::IsEqual(actual(row, col).Get<T>(), expected(row, col).Get<T>());
                });
            }
        });
        return ok;
    }

    template <typename T = void>
    bool Verify(Tensor actual, Tensor expected)
    {
        bool ok = true;
        For(actual, [&](Scalar row, Scalar col, Scalar ch) {
            if constexpr (std::is_same_v<T, void>)
            {
                If(actual(row, col, ch) == expected(row, col, ch), [&] {
                    InvokeForContext<ComputeContext>([&](ComputeContext&) {
                        ok &= true;
                    });
                })
                    .Else([&] {
                        InvokeForContext<ComputeContext>([&](ComputeContext&) {
                            ok &= false;
                        });
                    });
            }
            else
            {
                InvokeForContext<ComputeContext>([&](ComputeContext&) {
                    ok &= testing::IsEqual(actual(row, col, ch).Get<T>(), expected(row, col, ch).Get<T>());
                });
            }
        });
        return ok;
    }
} // namespace

std::vector<std::unique_ptr<EmitterContext>> GetContexts()
{
    std::vector<std::unique_ptr<EmitterContext>> contexts;
    contexts.push_back(std::make_unique<ComputeContext>("Value_test"));
    contexts.push_back(
        std::make_unique<TestLLVMContext>(std::make_unique<IRModuleEmitter>("Value_test", CompilerOptions{})));
    return contexts;
}

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

void Value_test1()
{
    DeclareFunction("Value_test1").Define([] {
        Value v(std::vector<int>{ 1, 2, 3, 4 });
        For(v, [&](Scalar index) {
            InvokeForContext<ComputeContext>([&](auto&) { std::cout << *v.Offset(index).Get<int*>() << " "; });
        });

        InvokeForContext<ComputeContext>([](auto&) { std::cout << std::endl; });
        InvokeForContext<TestLLVMContext>(PrintIR);
    })();
}

void Scalar_test1()
{
    DeclareFunction("Scalar_test1").Define([] {
        bool ok = true;
        Scalar s1 = 1;
        InvokeForContext<ComputeContext>([&](auto&) { ok &= testing::IsEqual(s1.Get<int>(), 1); });

        s1 += 2;
        InvokeForContext<ComputeContext>([&](auto&) { ok &= testing::IsEqual(s1.Get<int>(), 3); });

        Scalar s2 = s1 + 3;
        InvokeForContext<ComputeContext>([&](auto&) { ok &= testing::IsEqual(s1.Get<int>(), 3); });
        InvokeForContext<ComputeContext>([&](auto&) { ok &= testing::IsEqual(s2.Get<int>(), 6); });

        InvokeForContext<TestLLVMContext>(PrintIR);
        testing::ProcessTest("Testing basic semantics for Scalar", ok);
    })();
}

Vector testConvolve1D(Vector signal, Vector filter)
{
    size_t resultSize = signal.Size() - filter.Size() + 1;
    Vector result(Allocate(signal.GetType(), resultSize));

    For(result, [&](Scalar index) {
        Scalar accum;
        For(filter, [&](Scalar filterIndex) { accum += filter(filterIndex) * signal(index + filterIndex); });

        result(index) = accum;
    });

    math::Vector<double, math::VectorOrientation::column> mv = std::vector<double>{ 1, 2, 3, 4 };

    return result;
}

void Vector_test1()
{
    auto signal = Get1DReferenceSignal();
    auto filter = Get1DReferenceFilter();
    auto referenceResult = Get1DReferenceConvolutionResult();
    auto valueType = GetValueType<decltype(signal)::value_type>();

    auto convolve1D = DeclareFunction("testConvolve1D")
                          .Returns({ valueType, MemoryLayout({ (int)referenceResult.size() }) })
                          .Parameters(
                              Value{ valueType, MemoryLayout({ (int)signal.size() }) },
                              Value{ valueType, MemoryLayout({ (int)filter.size() }) })
                          .Define(testConvolve1D);

    InvokeForContext<ComputeContext>([&](auto&) {
        bool ok = true;
        Vector result = convolve1D(signal, filter);
        For(result, [&](Scalar index) {
            auto indexInt = index.Get<int>();
            ok &= testing::IsEqual(referenceResult[indexInt], result[index].Get<double>());
        });
        testing::ProcessTest("Testing 1D convolution with Vector", ok);
    });

    InvokeForContext<TestLLVMContext>(PrintIR);
}

void Vector_test2()
{
    auto fn = DeclareFunction("Vector_test2")
                  .Parameters(Value{ ValueType::Float, MemoryLayout{ { 2 } } })
                  .Define([](Vector v) {
                      bool ok = true;
                      v = std::vector<float>{ 1.2f, 2.3f };
                      Vector testVector(std::vector<float>{ 0.1f, 1.2f });
                      Scalar testScalar{ 3.4f };

                      {
                          Vector expected(std::vector<float>{ 4.6f, 5.7f });
                          Vector actual = v + testScalar;
                          ok &= Verify<float>(actual, expected);
                      }
                      {
                          Vector expected(std::vector<float>{ -2.2f, -1.1f });
                          Vector actual = v - testScalar;
                          ok &= Verify<float>(actual, expected);
                      }
                      {
                          Vector expected(std::vector<float>{ 4.08f, 7.82f });
                          Vector actual = v * testScalar;
                          ok &= Verify<float>(actual, expected);
                      }
                      {
                          Vector expected(std::vector<float>{ 1.2f / 3.4f, 2.3f / 3.4f });
                          Vector actual = v / testScalar;
                          ok &= Verify<float>(actual, expected);
                      }

                      // Vector +- Vector -> Vector
                      {
                          Vector expected(std::vector<float>{ 1.3f, 3.5f });
                          Vector actual = v + testVector;
                          ok &= Verify<float>(actual, expected);
                      }
                      {
                          Vector expected(std::vector<float>{ 1.1f, 1.1f });
                          Vector actual = v - testVector;
                          ok &= Verify<float>(actual, expected);
                      }

                      InvokeForContext<ComputeContext>([&](auto&) { testing::ProcessTest("Vector binary operations", ok); });
                  });

    InvokeForContext<ComputeContext>([&](auto&) { fn(MakeVector<float>(2)); });

    InvokeForContext<TestLLVMContext>(PrintIR);
}

namespace
{
    template <MatrixLayout layout>
    void Matrix_test1Impl(std::integral_constant<MatrixLayout, layout>)
    {
        constexpr int rows = 3, columns = 4;
        std::vector<int> matrixData(rows * columns);
        std::generate(matrixData.begin(), matrixData.end(), [i = 0]() mutable { return ++i; });
        math::Matrix<int, layout> mathMatrix(rows, columns, matrixData);

        auto dimensionOrder = layout == MatrixLayout::rowMajor ? DimensionOrder{ 0, 1 } : DimensionOrder{ 1, 0 };
        auto physicalSize =
            layout == MatrixLayout::rowMajor ? std::vector({ rows, columns }) : std::vector({ columns, rows });
        MemoryLayout memoryLayout(physicalSize, dimensionOrder);
        Matrix matrix(Value(matrixData, memoryLayout));

        testing::ProcessTest("value::Matrix and math::Matrix column check",
                             matrix.Columns() == mathMatrix.NumColumns());
        testing::ProcessTest("value::Matrix and math::Matrix row check", matrix.Rows() == mathMatrix.NumRows());

        std::cout << "      Expected    Actual" << std::endl;
        bool ok = true;
        // Need to explicitly capture mathMatrix because of a weird VC++ bug
        For(matrix, [&, &mathMatrix = mathMatrix](Scalar row, Scalar col) {
            auto rowInt = row.Get<int>();
            auto colInt = col.Get<int>();
            auto matrixVal = matrix(row, col).Get<int>();
            auto mathMatrixVal = mathMatrix(rowInt, colInt);
            std::cout << "@(" << rowInt << ", " << colInt << ") = " << mathMatrixVal << "\t" << matrixVal << std::endl;
            ok &= testing::IsEqual(matrixVal, mathMatrixVal);
        });
        testing::ProcessTest("value::Matrix and math::Matrix equality check", ok);

        ok = true;
        for (size_t rowIndex = 0; rowIndex < matrix.Rows(); ++rowIndex)
        {
            auto mathRowVector = mathMatrix.GetRow(rowIndex);
            auto rowVector = matrix.Row((int)rowIndex);
            testing::IsEqual(rowVector.Size(), mathRowVector.Size());

            std::cout << "Row Vector " << rowIndex << ": Expected    Actual" << std::endl;
            For(rowVector, [&](Scalar index) {
                auto indexInt = index.Get<int>();
                std::cout << "@(" << indexInt << ") = " << mathRowVector[indexInt] << "\t"
                          << rowVector(index).Get<int>() << std::endl;
                ok &= testing::IsEqual(rowVector(index).Get<int>(), mathRowVector[indexInt]);
            });
        }
        testing::ProcessTest("value::Matrix and math::Matrix row slice equality", ok);

        ok = true;
        for (size_t columnIndex = 0; columnIndex < matrix.Rows(); ++columnIndex)
        {
            auto mathColumnVector = mathMatrix.GetColumn(columnIndex);
            auto columnVector = matrix.Column((int)columnIndex);
            testing::IsEqual(columnVector.Size(), mathColumnVector.Size());

            std::cout << "Column Vector " << columnIndex << ": Expected    Actual" << std::endl;
            For(columnVector, [&](Scalar index) {
                auto indexInt = index.Get<int>();
                std::cout << "@(" << indexInt << ") = " << mathColumnVector[indexInt] << "\t"
                          << columnVector(index).Get<int>() << std::endl;
                testing::IsEqual(columnVector(index).Get<int>(), mathColumnVector[indexInt]);
            });
        }
        testing::ProcessTest("value::Matrix and math::Matrix column slice equality", ok);
    }
} // namespace

void Matrix_test1()
{
    // Test only enabled for ComputeContext for now
    InvokeForContext<ComputeContext>([](auto&) {
        DeclareFunction("Matrix_test1").Define([]() -> void {
            ApplyToEach([](auto layout) { Matrix_test1Impl(layout); },
                        LayoutType<MatrixLayout::rowMajor>{},
                        LayoutType<MatrixLayout::columnMajor>{});
        })();
    });
}

void Matrix_test2()
{
    InvokeForContext<ComputeContext>([](auto&) {
        std::vector<std::vector<int>> data{
            std::vector<int>{ 1, 2, 3 },
            std::vector<int>{ 4, 5, 6 },
        };
        Matrix m(data);

        testing::ProcessTest("Tensor construction test",
                             testing::IsEqual(m.Rows(), 2u) && testing::IsEqual(m.Columns(), 3u) &&
                                 testing::IsEqual(m(1, 2).Get<int>(), data[1][2]));
    });
}

void Matrix_test3()
{
    auto fn = DeclareFunction("Matrix_test3")
                  .Parameters(Value{ ValueType::Float, MemoryLayout{ { 2, 2 } } })
                  .Define([](Matrix m) {
                      bool ok = true;
                      m = std::vector<std::vector<float>>{
                          std::vector<float>{ 1.2f, 2.3f },
                          std::vector<float>{ 3.4f, 4.5f }
                      };
                      Matrix testMatrix(std::vector<std::vector<float>>{
                          std::vector<float>{ 0.1f, 1.2f },
                          std::vector<float>{ 2.3f, 3.4f } });
                      Scalar testScalar{ 3.4f };

                      {
                          Matrix expected(std::vector<std::vector<float>>{
                              std::vector<float>{ 4.6f, 5.7f },
                              std::vector<float>{ 6.8f, 7.9f } });
                          Matrix actual = m + testScalar;
                          ok &= Verify<float>(actual, expected);
                      }
                      {
                          Matrix expected(std::vector<std::vector<float>>{
                              std::vector<float>{ -2.2f, -1.1f },
                              std::vector<float>{ 0.f, 1.1f } });
                          Matrix actual = m - testScalar;
                          ok &= Verify<float>(actual, expected);
                      }
                      {
                          Matrix expected(std::vector<std::vector<float>>{
                              std::vector<float>{ 4.08f, 7.82f },
                              std::vector<float>{ 11.56f, 15.3f } });
                          Matrix actual = m * testScalar;
                          ok &= Verify<float>(actual, expected);
                      }
                      {
                          Matrix expected(std::vector<std::vector<float>>{
                              std::vector<float>{ 1.2f / 3.4f, 2.3f / 3.4f },
                              std::vector<float>{ 3.4f / 3.4f, 4.5f / 3.4f } });
                          Matrix actual = m / testScalar;
                          ok &= Verify<float>(actual, expected);
                      }

                      // Vector +- Vector -> Vector
                      {
                          Matrix expected(std::vector<std::vector<float>>{
                              std::vector<float>{ 1.3f, 3.5f },
                              std::vector<float>{ 5.7f, 7.9f } });
                          Matrix actual = m + testMatrix;
                          ok &= Verify<float>(actual, expected);
                      }
                      {
                          Matrix expected(std::vector<std::vector<float>>{
                              std::vector<float>{ 1.1f, 1.1f },
                              std::vector<float>{ 1.1f, 1.1f } });
                          Matrix actual = m - testMatrix;
                          ok &= Verify<float>(actual, expected);
                      }

                      InvokeForContext<ComputeContext>([&](auto&) { testing::ProcessTest("Matrix binary operations", ok); });
                  });

    InvokeForContext<ComputeContext>([&](auto&) { fn(MakeMatrix<float>(2, 2)); });

    InvokeForContext<TestLLVMContext>(PrintIR);
}

void Tensor_test1()
{
    // Test only enabled for ComputeContext for now
    InvokeForContext<ComputeContext>([](auto&) {
        DeclareFunction("Tensor_test1").Define([]() -> void {
            constexpr int rows = 3, columns = 5, channels = 7;
            std::vector<int> tensorData(rows * columns * channels);
            std::generate(tensorData.begin(), tensorData.end(), [i = 0]() mutable { return ++i; });
            math::ChannelColumnRowTensor<int> mathTensor(3, 5, 7, tensorData);

            MemoryShape physicalSize{ rows, columns, channels };
            DimensionOrder dimensionOrder = RowMajorTensorOrder;
            MemoryLayout memoryLayout(physicalSize, dimensionOrder);
            Tensor tensor(Value(tensorData, memoryLayout));

            testing::ProcessTest("value::Tensor and math::Tensor column check",
                                 tensor.Columns() == mathTensor.NumColumns());
            testing::ProcessTest("value::Tensor and math::Tensor row check", tensor.Rows() == mathTensor.NumRows());
            testing::ProcessTest("value::Tensor and math::Tensor channel check",
                                 tensor.Channels() == mathTensor.NumChannels());

            std::cout << "      Expected    Actual" << std::endl;
            bool ok = true;
            For(tensor, [&](Scalar row, Scalar col, Scalar ch) {
                auto rowInt = row.Get<int>();
                auto colInt = col.Get<int>();
                auto chInt = ch.Get<int>();
                std::cout << "@(" << rowInt << ", " << colInt << ", " << chInt
                          << ") = " << mathTensor(rowInt, colInt, chInt) << "\t" << tensor(row, col, ch).Get<int>()
                          << std::endl;
                ok &= testing::IsEqual(tensor(row, col, ch).Get<int>(), mathTensor(rowInt, colInt, chInt));
            });
            testing::ProcessTest("value::Tensor and math::Tensor equality check", ok);

            for (int column = 0; column < (int)mathTensor.NumColumns(); ++column)
            {
                auto mathSlicedMatrix1 = math::GetSlice<Dimension::row, Dimension::channel>(mathTensor, column);
                auto slicedMatrix1 = tensor.Slice(Slice::All, column, Slice::All);

                ok = true;
                For(slicedMatrix1, [&](Scalar row, Scalar col) {
                    auto rowInt = row.Get<int>();
                    auto colInt = col.Get<int>();
                    std::cout << "@(" << rowInt << ", " << colInt << ") = " << mathSlicedMatrix1(rowInt, colInt) << "\t"
                              << slicedMatrix1(row, col).Get<int>() << std::endl;
                    ok &= testing::IsEqual(slicedMatrix1(row, col).Get<int>(), mathSlicedMatrix1(rowInt, colInt));
                });
                testing::ProcessTest("value::Tensor and math::Tensor matrix slice equality check", ok);
            }

            for (int row = 0; row < (int)mathTensor.NumRows(); ++row)
            {
                auto mathSlicedMatrix1 = math::GetSlice<Dimension::column, Dimension::channel>(mathTensor, row);
                auto slicedMatrix1 = tensor.Slice(row, Slice::All, Slice::All);

                ok = true;
                For(slicedMatrix1, [&](Scalar row, Scalar col) {
                    auto rowInt = row.Get<int>();
                    auto colInt = col.Get<int>();
                    std::cout << "@(" << rowInt << ", " << colInt << ") = " << mathSlicedMatrix1(rowInt, colInt) << "\t"
                              << slicedMatrix1(row, col).Get<int>() << std::endl;
                    ok &= testing::IsEqual(slicedMatrix1(row, col).Get<int>(), mathSlicedMatrix1(rowInt, colInt));
                });
                testing::ProcessTest("value::Tensor and math::Tensor matrix slice equality check", ok);
            }

            ok = true;
            for (int row = 0; row < (int)mathTensor.NumRows(); ++row)
            {
                for (int column = 0; column < (int)mathTensor.NumColumns(); ++column)
                {
                    for (int channel = 0; channel < (int)mathTensor.NumChannels(); ++channel)
                    {
                        {
                            auto mathSlicedVector = math::GetSlice<Dimension::row>(mathTensor, column, channel);
                            auto slicedVector = tensor.Slice(Slice::All, column, channel);

                            For(slicedVector, [&](Scalar index) {
                                auto indexInt = index.Get<int>();
                                ok &= testing::IsEqual(slicedVector(index).Get<int>(), mathSlicedVector[indexInt]);
                            });
                        }
                        {
                            auto mathSlicedVector = math::GetSlice<Dimension::column>(mathTensor, row, channel);
                            auto slicedVector = tensor.Slice(row, Slice::All, channel);

                            For(slicedVector, [&](Scalar index) {
                                auto indexInt = index.Get<int>();
                                ok &= testing::IsEqual(slicedVector(index).Get<int>(), mathSlicedVector[indexInt]);
                            });
                        }
                    }
                    auto mathSlicedVector = math::GetSlice<Dimension::channel>(mathTensor, row, column);
                    auto slicedVector = tensor.Slice(row, column, Slice::All);

                    For(slicedVector, [&](Scalar index) {
                        auto indexInt = index.Get<int>();
                        ok &= testing::IsEqual(slicedVector(index).Get<int>(), mathSlicedVector[indexInt]);
                    });
                }
            }
            testing::ProcessTest("value::Tensor and math::Tensor vector slice equality check", ok);
        })();
    });
}

void Tensor_test2()
{
    InvokeForContext<ComputeContext>([](auto&) {
        std::vector<std::vector<std::vector<int>>> data{ std::vector<std::vector<int>>{
                                                             std::vector<int>{ 1, 2, 3 },
                                                             std::vector<int>{ 4, 5, 6 },
                                                         },
                                                         std::vector<std::vector<int>>{
                                                             std::vector<int>{ 7, 8, 9 },
                                                             std::vector<int>{ 10, 11, 12 },
                                                         } };
        Tensor t(data);

        testing::ProcessTest("Tensor construction test",
                             testing::IsEqual(t.Rows(), 2u) && testing::IsEqual(t.Columns(), 2u) &&
                                 testing::IsEqual(t.Channels(), 3u) &&
                                 testing::IsEqual(t(1, 0, 2).Get<int>(), data[1][0][2]));
    });
}

void Tensor_test3()
{
    auto fn = DeclareFunction("Tensor_test3")
                  .Parameters(Value{ ValueType::Float, MemoryLayout{ { 2, 2, 2 } } })
                  .Define([](Tensor t) {
                      bool ok = true;
                      t =
                          std::vector<std::vector<std::vector<float>>>{
                              std::vector<std::vector<float>>{
                                  std::vector<float>{ 1.2f, 2.3f },
                                  std::vector<float>{ 3.4f, 4.5f } },
                              std::vector<std::vector<float>>{
                                  std::vector<float>{ 5.4f, 4.3f },
                                  std::vector<float>{ 3.2f, 2.1f } },
                          };
                      Scalar testScalar{ 3.4f };

                      {
                          Tensor expected(
                              std::vector<std::vector<std::vector<float>>>{
                                  std::vector<std::vector<float>>{
                                      std::vector<float>{ 4.6f, 5.7f },
                                      std::vector<float>{ 6.8f, 7.9f } },
                                  std::vector<std::vector<float>>{
                                      std::vector<float>{ 8.8f, 7.7f },
                                      std::vector<float>{ 6.6f, 5.5f } } });
                          Tensor actual = t + testScalar;
                          ok &= Verify<float>(actual, expected);
                      }
                      {
                          Tensor expected(
                              std::vector<std::vector<std::vector<float>>>{
                                  std::vector<std::vector<float>>{
                                      std::vector<float>{ -2.2f, -1.1f },
                                      std::vector<float>{ 0.f, 1.1f } },
                                  std::vector<std::vector<float>>{
                                      std::vector<float>{ 2.f, 0.9f },
                                      std::vector<float>{ -0.2f, -1.3f } } });
                          Tensor actual = t - testScalar;
                          ok &= Verify<float>(actual, expected);
                      }
                      {
                          Tensor expected(
                              std::vector<std::vector<std::vector<float>>>{
                                  std::vector<std::vector<float>>{
                                      std::vector<float>{ 4.08f, 7.82f },
                                      std::vector<float>{ 11.56f, 15.3f } },
                                  std::vector<std::vector<float>>{
                                      std::vector<float>{ 18.36f, 14.62f },
                                      std::vector<float>{ 10.88f, 7.14f } } });
                          Tensor actual = t * testScalar;
                          ok &= Verify<float>(actual, expected);
                      }
                      {
                          Tensor expected(
                              std::vector<std::vector<std::vector<float>>>{
                                  std::vector<std::vector<float>>{
                                      std::vector<float>{ 1.2f / 3.4f, 2.3f / 3.4f },
                                      std::vector<float>{ 3.4f / 3.4f, 4.5f / 3.4f } },
                                  std::vector<std::vector<float>>{
                                      std::vector<float>{ 5.4f / 3.4f, 4.3f / 3.4f },
                                      std::vector<float>{ 3.2f / 3.4f, 2.1f / 3.4f } } });
                          Tensor actual = t / testScalar;
                          ok &= Verify<float>(actual, expected);
                      }

                      InvokeForContext<ComputeContext>([&](auto&) { testing::ProcessTest("Tensor binary operations", ok); });
                  });

    InvokeForContext<ComputeContext>([&](auto&) { fn(MakeTensor<float>(2, 2, 2)); });

    InvokeForContext<TestLLVMContext>(PrintIR);
}

void Tensor_slice_test1()
{
    InvokeForContext<ComputeContext>([](auto&) {
        auto input =
            std::vector<double>{ 11, 22, 33, 44, 55, 66, 77, 88, 99, 111, 222, 333, 444, 555, 666, 777, 888, 999 };
        /*
            channel major order:
            input[:, :, ch] will be in canonical order row-major matrix order
            input[:, :, 0] =
                [[11, 22, 33],
                [44, 55, 66],
                [77, 88, 99]]
            input[:, :, 1] =
                [[111, 222, 333],
                [444, 555, 666],
                [777, 888, 999]]
            */

        constexpr int rows = 3, cols = 3, chs = 2;
        Tensor inputTensor({ input, MemoryLayout({ chs, rows, cols }, DimensionOrder(ChannelMajorTensorOrder)) });
        math::ColumnRowChannelTensor<double> mathTensor(cols, rows, chs, input);

        const double* mathFirstElement = &mathTensor(0, 0, 0);
        const double* valueFirstElement = inputTensor(0, 0, 0).GetValue().Get<double*>();

        {
            bool ok = true;
            For(inputTensor, [&](Scalar row, Scalar col, Scalar ch) {
                int rowInt = row.Get<int>(), colInt = col.Get<int>(), chInt = ch.Get<int>();
                const double* mathElement = &mathTensor(rowInt, colInt, chInt);
                const double* valueElement = inputTensor(row, col, ch).GetValue().Get<double*>();
                ok &= testing::IsEqual(*mathElement, *valueElement);
                auto mathElementDifference = mathElement - mathFirstElement;
                auto valueElementDifference = valueElement - valueFirstElement;
                ok &= testing::IsEqual(mathElementDifference, valueElementDifference);
            });
            testing::ProcessTest("Tensor_slice_test1 channel-major order", ok);
        }

        {
            {
                auto mathMatrix = mathTensor.GetSlice<Dimension::column, Dimension::row>(0);
                auto matrix = inputTensor.Slice(Slice::All, Slice::All, 0);

                testing::ProcessTest("Tensor row-column GetSlice dimension",
                                     mathMatrix.NumColumns() == matrix.Columns() &&
                                         mathMatrix.NumRows() == matrix.Rows());
            }

            for (int ch = 0; ch < chs; ++ch)
            {
                auto mathMatrix = mathTensor.GetSlice<Dimension::row, Dimension::column>(ch);
                auto matrix = inputTensor.Slice(Slice::All, Slice::All, ch);

                bool ok = true;
                For(matrix, [&](Scalar row, Scalar col) {
                    auto rowInt = row.Get<int>(), colInt = col.Get<int>();

                    const double* mathElement = &mathMatrix(rowInt, colInt);
                    const double* valueElement = matrix(row, col).GetValue().Get<double*>();
                    ok &= testing::IsEqual(*mathElement, *valueElement);

                    auto mathElementDifference = mathElement - mathFirstElement;
                    auto valueElementDifference = valueElement - valueFirstElement;
                    ok &= testing::IsEqual(mathElementDifference, valueElementDifference);
                });

                testing::ProcessTest("Tensor row-column GetSlice", ok);
            }
        }

        {
            {
                auto mathMatrix = mathTensor.GetSlice<Dimension::column, Dimension::channel>(0);
                auto matrix = inputTensor.Slice(0, Slice::All, Slice::All);

                testing::ProcessTest("Tensor column-channel GetSlice dimension",
                                     mathMatrix.NumColumns() == matrix.Columns() &&
                                         mathMatrix.NumRows() == matrix.Rows());
            }

            for (int row = 0; row < rows; ++row)
            {
                auto mathMatrix = mathTensor.GetSlice<Dimension::column, Dimension::channel>(row);
                auto matrix = inputTensor.Slice(row, Slice::All, Slice::All);

                bool ok = true;
                For(matrix, [&](Scalar row, Scalar col) {
                    auto rowInt = row.Get<int>(), colInt = col.Get<int>();

                    const double* mathElement = &mathMatrix(rowInt, colInt);
                    const double* valueElement = matrix(row, col).GetValue().Get<double*>();
                    ok &= testing::IsEqual(*mathElement, *valueElement);

                    auto mathElementDifference = mathElement - mathFirstElement;
                    auto valueElementDifference = valueElement - valueFirstElement;
                    ok &= testing::IsEqual(mathElementDifference, valueElementDifference);
                });

                testing::ProcessTest("Tensor column-channel GetSlice", ok);
            }
        }

        {
            {
                auto mathVector = mathTensor.GetSlice<Dimension::channel>(0, 0);
                auto vector = inputTensor.Slice(0, 0, Slice::All);

                testing::ProcessTest("Tensor channel GetSlice length", mathVector.Size() == vector.Size());
            }
            for (int row = 0; row < static_cast<int>(mathTensor.NumRows()); ++row)
            {
                for (int col = 0; col < static_cast<int>(mathTensor.NumColumns()); ++col)
                {
                    auto mathVector = mathTensor.GetSlice<Dimension::channel>(row, col);
                    auto vector = inputTensor.Slice(row, col, Slice::All);

                    bool ok = true;
                    For(vector, [&](Scalar index) {
                        auto indexInt = index.Get<int>();
                        const double* mathElement = &mathVector[indexInt];
                        const double* valueElement = vector(index).GetValue().Get<double*>();
                        ok &= testing::IsEqual(*mathElement, *valueElement);
                        auto mathElementDifference = mathElement - mathFirstElement;
                        auto valueElementDifference = valueElement - valueFirstElement;
                        ok &= testing::IsEqual(mathElementDifference, valueElementDifference);
                    });

                    testing::ProcessTest("Tensor channel GetSlice", ok);
                }
            }
        }

        {
            {
                auto mathVector = mathTensor.GetSlice<Dimension::column>(0, 0);
                auto vector = inputTensor.Slice(0, Slice::All, 0);

                testing::ProcessTest("Tensor column GetSlice length", mathVector.Size() == vector.Size());
            }
            for (int row = 0; row < static_cast<int>(mathTensor.NumRows()); ++row)
            {
                for (int ch = 0; ch < static_cast<int>(mathTensor.NumChannels()); ++ch)
                {
                    auto mathVector = mathTensor.GetSlice<Dimension::column>(row, ch);
                    auto vector = inputTensor.Slice(row, Slice::All, ch);

                    bool ok = true;
                    For(vector, [&](Scalar index) {
                        auto indexInt = index.Get<int>();
                        const double* mathElement = &mathVector[indexInt];
                        const double* valueElement = vector(index).GetValue().Get<double*>();
                        ok &= testing::IsEqual(*mathElement, *valueElement);
                        auto mathElementDifference = mathElement - mathFirstElement;
                        auto valueElementDifference = valueElement - valueFirstElement;
                        ok &= testing::IsEqual(mathElementDifference, valueElementDifference);
                    });

                    testing::ProcessTest("Tensor column GetSlice", ok);
                }
            }
        }

        {
            {
                auto mathVector = mathTensor.GetSlice<Dimension::row>(0, 0);
                auto vector = inputTensor.Slice(Slice::All, 0, 0);

                testing::ProcessTest("Tensor row GetSlice length", mathVector.Size() == vector.Size());
            }
            for (int col = 0; col < static_cast<int>(mathTensor.NumColumns()); ++col)
            {
                for (int ch = 0; ch < static_cast<int>(mathTensor.NumChannels()); ++ch)
                {
                    auto mathVector = mathTensor.GetSlice<Dimension::row>(col, ch);
                    auto vector = inputTensor.Slice(Slice::All, col, ch);

                    bool ok = true;
                    For(vector, [&](Scalar index) {
                        auto indexInt = index.Get<int>();
                        const double* mathElement = &mathVector[indexInt];
                        const double* valueElement = vector(index).GetValue().Get<double*>();
                        ok &= testing::IsEqual(*mathElement, *valueElement);
                        auto mathElementDifference = mathElement - mathFirstElement;
                        auto valueElementDifference = valueElement - valueFirstElement;
                        ok &= testing::IsEqual(mathElementDifference, valueElementDifference);
                    });

                    testing::ProcessTest("Tensor row GetSlice", ok);
                }
            }
        }
    });
}

void Casting_test1()
{
    InvokeForContext<ComputeContext>([](auto&) {
        Vector floatVector = std::vector<float>{ 1.f, 2.f, 3.f };
        auto floatScalar = floatVector[1];
        Scalar intScalar = Cast<int>(floatScalar);
        Scalar globalIntScalar = GlobalAllocate("global", 3);
        intScalar += 1;
        floatScalar += 10.f;
        testing::ProcessTest("Cast test",
                             intScalar.Get<int>() == 3 && intScalar.Get<int>() == globalIntScalar.Get<int>() &&
                                 floatScalar.Get<float>() == 12.f && floatVector[1].Get<float>() == 12.f);
    });
}

void If_test1()
{
    InvokeForContext<ComputeContext>([](auto&) {
        DeclareFunction("If_test1").Define([]() -> void {
            Scalar s1 = 1;
            If(s1 == 1, [&s1]() { s1 = 0; });

            testing::ProcessTest("Testing basic If expression ", testing::IsEqual(s1.Get<int>(), 0));

            s1 = 1;
            If(s1 == 0, [&s1]() { s1 = 3; }).Else([&s1]() { s1 = 0; });

            testing::ProcessTest("Testing basic If/Else expression ", testing::IsEqual(s1.Get<int>(), 0));

            s1 = 1;
            If(s1 == 3, [&s1]() { s1 = 2; }).ElseIf(s1 == 1, [&s1]() { s1 = 0; }).Else([&s1]() { s1 = 3; });

            testing::ProcessTest("Testing basic If/ElseIf/Else expression ", testing::IsEqual(s1.Get<int>(), 0));
        })(); // invoke function
    });
}

void Accumulate_test()
{
    auto fn = DeclareFunction("Accumulate_test").Define([]() -> void {
        bool ok = true;
        for (int i = 1; i < 10; ++i)
        {
            Vector v = MakeVector<float>(i);
            std::vector<float> reference(i);
            std::iota(reference.begin(), reference.end(), 0);

            v = reference;

            Scalar result = Accumulate(v, Cast(0, v.GetType()));
            If(result != std::accumulate(reference.begin(), reference.end(), 0.f),
               [&] { InvokeForContext<ComputeContext>([&](auto&) { ok = false; }); });
        }
        testing::ProcessTest("Accumulate test", ok);
    });

    InvokeForContext<ComputeContext>([&](auto&) { fn(); });
}

void Dot_test()
{
    auto fn = DeclareFunction("Dot_test").Define([]() -> void {
        bool ok = true;
        for (int i = 1; i < 10; ++i)
        {
            Vector v1 = MakeVector<float>(i), v2 = MakeVector<float>(i);
            std::vector<float> reference1(i), reference2(i);
            std::iota(reference1.begin(), reference1.end(), 0);
            std::iota(reference2.begin(), reference2.end(), reference1.back());

            v1 = reference1;
            v2 = reference2;

            Scalar result = Dot(v1, v2);
            If(result != std::inner_product(reference1.begin(), reference1.end(), reference2.begin(), 0.f),
               [&] { InvokeForContext<ComputeContext>([&](auto&) { ok = false; }); });
        }
        testing::ProcessTest("Dot test", ok);
    });

    InvokeForContext<ComputeContext>([&](auto&) { fn(); });
}

namespace
{
    const std::vector<float> intrinsics_data{ 0.1f, 1.2f, 2.3f, 3.4f, 4.5f, 5.6f, 6.7f, 7.8f, 8.9f, 9.10f };

    template <typename Tuple, typename Idx = std::integral_constant<size_t, 0>>
    void Intrinsics_test1_impl(Tuple tuple, Idx = {})
    {
        constexpr auto index = Idx::value;
        if constexpr (index < std::tuple_size_v<Tuple>)
        {
            auto& element = std::get<index>(tuple);
            auto fnName = std::string{ "Intrinsics_test1_" } + std::to_string(index);
            auto fn = DeclareFunction(fnName)
                          .Parameters(Value(ValueType::Float, MemoryLayout{ { 10 } }))
                          .Returns(Value(ValueType::Float, MemoryLayout{ { 10 } }))
                          .Define([f = element.first](Vector v) -> Vector {
                              v = intrinsics_data;
                              return f(v);
                          });

            InvokeForContext<ComputeContext>([&](ComputeContext&) {
                std::vector<float> expected(intrinsics_data.size());
                std::transform(intrinsics_data.begin(), intrinsics_data.end(), expected.begin(), [f = element.second](float n) { return f(n); });
                auto result = fn(MakeVector<float>(intrinsics_data.size()));
                auto ok = true;
                // have to explicitly capture because vc++ has issues
                For(result, [&result, &ok, &expected](Scalar index) {
                    auto expectedValue = expected[index.Get<int>()];
                    auto computedValue = result(index).template Get<float>();

                    // We can't use absolute difference because VC++'s math functions
                    // are optimized for 6 digits of precision, which can result in differences
                    // that are relatively negligible, but absolutely greater than the epsilon
                    // used by the testing library
                    auto difference = RelativeDifference(expectedValue, computedValue);
                    auto testResult = difference < 1e-6f;
                    if (!testResult)
                    {
                        auto precision = std::cerr.precision();

                        std::cerr << std::setprecision(std::numeric_limits<float>::digits10 + 1)
                                  << "Computed: " << computedValue << "\tExpected: " << expectedValue
                                  << "\tDifference: " << (expectedValue - computedValue) << std::endl
                                  << std::setprecision(precision);
                    }
                    ok &= testResult;
                });
                testing::ProcessTest(fnName, ok);
            });

            InvokeForContext<TestLLVMContext>(PrintIR);

            Intrinsics_test1_impl(tuple, std::integral_constant<size_t, index + 1>{});
        }
    }
} // namespace

void Intrinsics_test1()
{
    Intrinsics_test1_impl(
        std::tuple{
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
    void Intrinsics_test2_impl(Tuple tuple, Idx = {})
    {
        constexpr auto index = Idx::value;
        if constexpr (index < std::tuple_size_v<Tuple>)
        {
            auto& element = std::get<index>(tuple);
            auto fnName = std::string{ "Intrinsics_test2_" } + std::to_string(index);

            auto fn = DeclareFunction(fnName)
                          .Parameters(Value(ValueType::Float, MemoryLayout{ { 10 } }))
                          .Returns(Value(ValueType::Float, ScalarLayout))
                          .Define([f = element.first](Vector v) {
                              v = intrinsics_data;
                              return f(v);
                          });

            InvokeForContext<ComputeContext>([&](auto&) {
                auto f = element.second;
                auto expected = *f(intrinsics_data);
                auto result = fn(MakeVector<float>(intrinsics_data.size()));
                auto ok = result.template Get<float>() == expected;
                testing::ProcessTest(fnName, ok);
            });

            InvokeForContext<TestLLVMContext>(PrintIR);

            Intrinsics_test2_impl(tuple, std::integral_constant<size_t, index + 1>{});
        }
    }

} // namespace

void Intrinsics_test2()
{
    Intrinsics_test2_impl(
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

} // namespace ell
