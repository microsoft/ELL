////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Value_test.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Value_test.h"

#include "ComputeContext.h"
#include "LLVMContext.h"
#include "Value.h"
#include "ValueMatrix.h"
#include "ValueTensor.h"
#include "ValueVector.h"

// emitters
#include "IRModuleEmitter.h"

// math
#include "Matrix.h"
#include "Tensor.h"
#include "Vector.h"

// utilities
#include "FunctionUtils.h"

// testing
#include "testing.h"

#include <iostream>
#include <memory>
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
    return std::vector<double>{ 0.42929697, 0.90317845, 0.84490289, 0.66174327, 0.10820399, 0.3511343,
                                0.58248869, 0.62674724, 0.11014194, 0.00132073, 0.58431646, 0.39873614,
                                0.40304155, 0.79139607, 0.97710827, 0.21268128 };
}

auto Get1DReferenceConvolutionResult()
{
    return std::vector<double>{ 0.77013919, 0.81368187, 0.56914835, 0.30732139, 0.34824032, 0.53571473, 0.48653128,
                                0.21208796, 0.17427497, 0.39217245, 0.44620757, 0.49905383, 0.74073549, 0.73957347 };
}

struct TestLLVMContext : public LLVMContext
{
    TestLLVMContext(std::unique_ptr<IRModuleEmitter> emitter) : LLVMContext(*emitter), _emitter(std::move(emitter)) {}

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

} // namespace

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
        For(v, [&](Scalar index) {
            InvokeForContext<ComputeContext>([&](auto&) { std::cout << *v.Offset(index).Get<int*>() << " "; });
        });

        InvokeForContext<ComputeContext>([](auto&) { std::cout << std::endl; });
        InvokeForContext<TestLLVMContext>(PrintIR);
    })();
}

void Scalar_test1()
{
    CreateFunction("Scalar_test1", [] {
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
    auto convolve1D = CreateFunction("testConvolve1D",
                                     Value{ valueType, MemoryLayout({ (int)referenceResult.size() }) },
                                     { Value{ valueType, MemoryLayout({ (int)signal.size() }) },
                                       Value{ valueType, MemoryLayout({ (int)filter.size() }) } },
                                     testConvolve1D);

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

std::vector<std::unique_ptr<EmitterContext>> GetContexts()
{
    std::vector<std::unique_ptr<EmitterContext>> contexts;
    contexts.push_back(std::make_unique<ComputeContext>("Value_test"));
    contexts.push_back(
        std::make_unique<TestLLVMContext>(std::make_unique<IRModuleEmitter>("Value_test", CompilerOptions{})));
    return contexts;
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
        CreateFunction("Matrix_test1", []() -> void {
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

void Tensor_test1()
{
    // Test only enabled for ComputeContext for now
    InvokeForContext<ComputeContext>([](auto&) {
        CreateFunction("Tensor_test1", []() -> void {
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

} // namespace ell
