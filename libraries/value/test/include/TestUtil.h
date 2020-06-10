////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TestUtil.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <utilities/include/MemoryLayout.h>

#include <value/include/Array.h>
#include <value/include/Matrix.h>
#include <value/include/Scalar.h>
#include <value/include/Tensor.h>
#include <value/include/Vector.h>
#include <value/include/loopnests/LoopNest.h>

#include <string>

namespace ell
{
value::Scalar NotEqualEpsilon(value::Scalar x, value::Scalar y, double epsilon = 1e-7);
value::Scalar EqualEpsilon(value::Scalar x, value::Scalar y, double epsilon = 1e-7);

value::Scalar VerifySame(value::Vector actual, value::Vector expected, double epsilon = 1e-7);
value::Scalar VerifySame(value::Matrix actual, value::Matrix expected, double epsilon = 1e-7);
value::Scalar VerifySame(value::Tensor actual, value::Tensor expected, double epsilon = 1e-7);
value::Scalar VerifySame(value::Array actual, value::Array expected, double epsilon = 1e-7);

value::Scalar VerifyDifferent(value::Vector actual, value::Vector expected, double epsilon = 1e-7);
value::Scalar VerifyDifferent(value::Matrix actual, value::Matrix expected, double epsilon = 1e-7);
value::Scalar VerifyDifferent(value::Tensor actual, value::Tensor expected, double epsilon = 1e-7);
value::Scalar VerifyDifferent(value::Array actual, value::Array expected, double epsilon = 1e-7);

void PrintMatrix(std::string indent, value::Matrix e);
void DebugPrintVector(value::Vector data);
void PrintLoops(const value::loopnests::LoopNest& loop, std::string tag);

value::Scalar GetTID();

template <typename ValueType>
value::Array MakeIncrementingArray(std::vector<int> size, const std::string& name)
{
    auto array = value::MakeArray<ValueType>(utilities::MemoryShape(size), name);
    int counter = 0;
    value::For(array, [&](const std::vector<value::Scalar>& indices) {
        array(indices) = counter++;
    });
    return array;
}

template <typename ValueType>
value::Tensor MakeIncrementingTensor(int rows, int columns, int channels, const std::string& name)
{
    auto tensor = value::MakeTensor<ValueType>(rows, columns, channels, name);
    int counter = 0;
    value::ForRange(channels, [&](value::Scalar channel) {
        value::ForRange(rows, [&](value::Scalar row) {
            value::ForRange(columns, [&](value::Scalar column) {
                tensor(row, column, channel) = counter++;
            });
        });
    });
    return tensor;
}

template <typename ValueType>
value::Matrix MakeIncrementingMatrix(int rows, int cols, const std::string& name)
{
    auto matrix = value::MakeMatrix<ValueType>(rows, cols, name);
    value::ForRange(rows, [&](value::Scalar row) {
        value::ForRange(cols, [&](value::Scalar col) {
            matrix(row, col) = row * cols + col;
        });
    });
    return matrix;
}

template <typename ValueType>
value::Vector MakeIncrementingVector(int elements, const std::string& name)
{
    auto vec = value::MakeVector<ValueType>(elements, name);
    value::ForRange(elements, [&](value::Scalar element) {
        vec(element) = element;
    });
    return vec;
}

//
// Matrix-multiply example helpers
//
void MultiplyMatrices(value::Matrix& A, value::Matrix& B, value::Matrix& C);

struct MatMul3TestCaseParameters
{
    int M;
    int N;
    int K;
    int L;
    value::Matrix A;
    value::Matrix B;
    value::Matrix C;
    value::Matrix D;
    value::Matrix E;
    value::Matrix expectedC;
    value::Matrix expectedE;
};

MatMul3TestCaseParameters GetMatMul3TestCaseParameters(int M, int N, int K, int L);
} // namespace ell
