////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Matrix_test.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Matrix_test.h"
#include "TestUtil.h"

#include <value/include/Matrix.h>
#include <value/include/Reference.h>
#include <value/include/Value.h>
#include <value/include/Vector.h>

#include <math/include/Matrix.h>
#include <math/include/Vector.h>

#include <utilities/include/MemoryLayout.h>

#include <algorithm>
#include <type_traits>
#include <vector>

using namespace ell::utilities;
using namespace ell::value;
namespace math = ell::math;
using math::MatrixLayout;

template <MatrixLayout layout>
using LayoutType = std::integral_constant<MatrixLayout, layout>;

namespace ell
{

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
            If(VerifySame(rowVector, expected) != 0, [&] {
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
            If(VerifySame(columnVector, expected) != 0, [&] {
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
        If(0 != VerifySame(actual, expected), [&] {
            DebugPrint("Matrix_test3 matrix scalar addition failed \n");
            ok = 1;
        });
    }
    {
        Matrix expected(std::vector<std::vector<float>>{
            std::vector<float>{ 1.2f - 3.4f, 2.3f - 3.4f },
            std::vector<float>{ 3.4f - 3.4f, 4.5f - 3.4f } });
        Matrix actual = m - testScalar;
        If(0 != VerifySame(actual, expected), [&] {
            DebugPrint("Matrix_test3 matrix scalar subtraction failed \n");
            ok = 1;
        });
    }
    {
        Matrix expected(std::vector<std::vector<float>>{
            std::vector<float>{ 1.2f * 3.4f, 2.3f * 3.4f },
            std::vector<float>{ 3.4f * 3.4f, 4.5f * 3.4f } });
        Matrix actual = m * testScalar;
        If(0 != VerifySame(actual, expected), [&] {
            DebugPrint("Matrix_test3 matrix scalar multiplication failed \n");
            ok = 1;
        });
    }
    {
        Matrix expected(std::vector<std::vector<float>>{
            std::vector<float>{ 1.2f / 3.4f, 2.3f / 3.4f },
            std::vector<float>{ 3.4f / 3.4f, 4.5f / 3.4f } });
        Matrix actual = m / testScalar;
        If(0 != VerifySame(actual, expected), [&] {
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
        If(0 != VerifySame(actual, expected), [&] {
            DebugPrint("Matrix_test3 matrix + matrix failed \n");
            ok = 1;
        });
    }
    {
        Matrix expected(std::vector<std::vector<float>>{
            std::vector<float>{ 1.2f - 0.1f, 2.3f - 1.2f },
            std::vector<float>{ 3.4f - 2.3f, 4.5f - 3.4f } });
        Matrix actual = m - testMatrix;
        If(0 != VerifySame(actual, expected), [&] {
            DebugPrint("Matrix_test3 matrix - matrix failed \n");
            ok = 1;
        });
    }
    return ok;
}

// This test verifies:
// - "For" with Matrix
// - Assignment from Matrix of one dimension order to another
// NOTE: This test currently passes for Compute but FAILS for Compile
Scalar Matrix_test4()
{
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;

    std::vector<std::vector<int>> dt{
        std::vector<int>{ 1, 2, 3 },
        std::vector<int>{ 4, 5, 6 },
    };
    auto source = Matrix(dt);
    auto destValue = Allocate(value::ValueType::Int32, source.GetValue().GetLayout().ReorderedCopy(DimensionOrder{ 1, 0 }));
    auto dest = Matrix(destValue);

    For(source, [&](value::Scalar row, value::Scalar column) {
        dest(row, column) = source(row, column);
    });

    std::vector<int> expectedValues{ 1, 4, 2, 5, 3, 6 };
    auto expected = Vector(expectedValues);

    Vector actual = AsVector(AsFullView(dest));

    If(VerifySame(actual, expected) == 1, [&] {
        DebugPrint("Matrix_test4 matrix assignment to different dimension order failed \n");
        ok = 1;
    });

    return ok;
}

Scalar Reshape_test()
{
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;

    Matrix m = std::vector<std::vector<float>>{
        std::vector<float>{ 1, 2, 3 },
        std::vector<float>{ 4, 5, 6 }
    };

    Vector v = std::vector<float>{ 1, 2, 3, 4, 5, 6 };

    If(0 != VerifySame(ToVector(m.GetValue()), v), [&] {
        DebugPrint("Reshape_test matrix into a vector failed \n");
        ok = 1;
    });

    If(0 != VerifySame(ToMatrix(v.GetValue(), 2, 3), m), [&] {
        DebugPrint("Reshape_test vector into a matrix failed \n");
        ok = 1;
    });

    return ok;
}

Scalar GEMV_test()
{
    Scalar ok = Allocate(ValueType::Int32, ScalarLayout);
    ok = 0;

    Matrix m = std::vector<std::vector<float>>{
        std::vector<float>{ 1.2f, 2.3f },
        std::vector<float>{ 3.4f, 4.5f }
    };

    Vector v(std::vector<float>{ 2.0f, 3.0f });

    Vector actual = GEMV(m, v);

    Vector expected(std::vector<float>{ 9.3f, 20.3f });

    If(0 != VerifySame(actual, expected, 1e-5), [&] {
        DebugPrint("GEMV_test - failed \n");
        ok = 1;
    });
    return ok;
}

Scalar MatrixReferenceTest()
{
    const int N = 4;
    const int kernelSize = 2;
    const Scalar offsetRows = 0;
    const Scalar offsetCols = 1;

    auto A = MakeMatrix<int>(N, N);

    // Initialize A to this matrix:
    // [ 0, -1, -2, -3 ]
    // [ 1,  0, -1, -2 ]
    // [ 2,  1,  0, -1 ]
    // [ 3,  2,  1,  0 ]

    // Expected matrix is the upper right quadrant of A:
    // [ -1, -2 ]
    // [  0, -1 ]
    auto expected = MakeMatrix<int>(kernelSize, kernelSize);

    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
        });
    });
    ForRange(offsetRows, offsetRows + kernelSize, [&](Scalar i) {
        ForRange(offsetCols, offsetCols + kernelSize, [&](Scalar j) {
            expected(i - offsetRows, j - offsetCols) = i - j;
        });
    });

    auto tmpA = A.GetValue();
    tmpA.SetLayout({ { (int)A.Size() } });
    DebugPrintVector(tmpA);

    Scalar kernelSizeScalar = kernelSize;
    Scalar zeroScalar = 0;
    auto temp = A.SubMatrix(offsetRows, offsetCols, kernelSize, kernelSize).GetValue();
    auto valueCachePtrPtr = temp.Reference();

    Matrix valueCachePtr = valueCachePtrPtr.Dereference();

    Scalar ok = Allocate<int>(ScalarLayout);
    ok = 1;
    If(
        VerifySame(valueCachePtr, expected) == 0,
        [&] {
            ok = 0;
        })
        .Else([&] {
            auto value = valueCachePtr.GetValue();
            value.SetLayout({ { (int)valueCachePtr.Size() } });
            DebugPrintVector(value);
            DebugPrint("\n");
            auto expectedValue = expected.GetValue();
            expectedValue.SetLayout({ { (int)expected.Size() } });
            DebugPrintVector(expectedValue);
            DebugPrint("\n");
        });
    return ok;
}

Scalar RefMatrixReferenceTest()
{
    const int N = 4;
    const int kernelSize = 2;
    const Scalar offsetRows = 0;
    const Scalar offsetCols = 1;

    auto A = MakeMatrix<int>(N, N, "A");

    // Initialize A to this matrix:
    // [ 0, -1, -2, -3 ]
    // [ 1,  0, -1, -2 ]
    // [ 2,  1,  0, -1 ]
    // [ 3,  2,  1,  0 ]

    // Expected matrix is the upper right quadrant of A:
    // [ -1, -2 ]
    // [  0, -1 ]
    auto expected = MakeMatrix<int>(kernelSize, kernelSize, "expected");

    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
        });
    });
    ForRange(offsetRows, offsetRows + kernelSize, [&](Scalar i) {
        ForRange(offsetCols, offsetCols + kernelSize, [&](Scalar j) {
            expected(i - offsetRows, j - offsetCols) = i - j;
        });
    });

    auto tmpA = A.GetValue();
    tmpA.SetLayout({ { (int)A.Size() } });
    DebugPrintVector(tmpA);

    Scalar kernelSizeScalar = kernelSize;
    Scalar zeroScalar = 0;
    auto temp = A.SubMatrix(offsetRows, offsetCols, kernelSize, kernelSize);
    Ref<Matrix> valueCachePtrPtr = temp;

    Matrix valueCachePtr = *valueCachePtrPtr;

    Scalar ok = MakeScalar<int>("ok");
    ok = 1;
    If(
        VerifySame(valueCachePtr, expected) == 0,
        [&] {
            ok = 0;
        })
        .Else([&] {
            auto value = valueCachePtr.GetValue();
            value.SetLayout({ { (int)valueCachePtr.Size() } });
            DebugPrintVector(value);
            DebugPrint("\n");
            auto expectedValue = expected.GetValue();
            expectedValue.SetLayout({ { (int)expected.Size() } });
            DebugPrintVector(expectedValue);
            DebugPrint("\n");
        });
    return ok;
}

} // namespace ell
