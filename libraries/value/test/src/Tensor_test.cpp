////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Tensor_test.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Tensor_test.h"
#include "TestUtil.h"

#include <value/include/ComputeContext.h>
#include <value/include/FunctionDeclaration.h>
#include <value/include/Matrix.h>
#include <value/include/Tensor.h>
#include <value/include/Value.h>
#include <value/include/Vector.h>

#include <math/include/Matrix.h>
#include <math/include/Tensor.h>
#include <math/include/Vector.h>

#include <utilities/include/MemoryLayout.h>

#include <algorithm>
#include <type_traits>
#include <vector>

using namespace ell::utilities;
using namespace ell::value;
namespace math = ell::math;
using math::Dimension;
using math::MatrixLayout;

template <MatrixLayout layout>
using LayoutType = std::integral_constant<MatrixLayout, layout>;

namespace ell
{

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
        Matrix mathMatrix = ToMatrix(mathTensor.GetSlice<Dimension::row, Dimension::column>(0));
        auto matrix = inputTensor.Slice(Slice::All, Slice::All, 0);

        If(Verify(matrix, mathMatrix) != 0, [&] {
            ok = 1;
            DebugPrint("Tensor_slice_test1: Tensor row-column GetSlice failed\n");
        });
    }

    {
        // We can't use ToArray() on this slice because data is not stored in the same layout, we have to build flat vector manually.
        auto slice = mathTensor.GetSlice<Dimension::column, Dimension::channel>(0);
        Matrix mathMatrix = ToMatrix(slice);
        auto matrix = inputTensor.Slice(0, Slice::All, Slice::All);

        If(Verify(matrix, mathMatrix) != 0, [&] {
            ok = 1;
            DebugPrint("Tensor_slice_test1: Tensor column-channel GetSlice failed\n");
        });
    }

    {
        Vector mathVector = mathTensor.GetSlice<Dimension::channel>(0, 0).ToArray();
        auto vector = inputTensor.Slice(0, 0, Slice::All);

        If(Verify(mathVector, vector) != 0, [&] {
            ok = 1;
            DebugPrint("Tensor_slice_test1: Tensor channel vector failed\n");
        });
    }

    {
        Vector mathVector = mathTensor.GetSlice<Dimension::column>(0, 0).ToArray();
        auto vector = inputTensor.Slice(0, Slice::All, 0);

        If(Verify(mathVector, vector) != 0, [&] {
            ok = 1;
            DebugPrint("Tensor_slice_test1: Tensor column vector failed");
        });
    }

    {
        Vector mathVector = mathTensor.GetSlice<Dimension::row>(0, 0).ToArray();
        auto vector = inputTensor.Slice(Slice::All, 0, 0);

        If(Verify(mathVector, vector) != 0, [&] {
            ok = 1;
            DebugPrint("Tensor_slice_test1: Tensor row vector failed");
        });
    }
    return ok;
}
} // namespace ell
