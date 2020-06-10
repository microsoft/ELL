////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoopNest_kernels.h (value)
//  Authors:  Kern Handa, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace value
{
    class Scalar;
    class Matrix;
    class Tensor;
    class Vector;
    struct ViewAdapter;
} // namespace value

void loopnest_passthrough(value::ViewAdapter, value::Scalar i, value::Scalar j);
void loopnest_kernel(value::Matrix m, value::Scalar i, value::Scalar j);
void loopnest_kernel_2(value::Matrix m, value::Scalar i, value::Scalar j);
void loopnest_kernel_3(value::Matrix c, value::Matrix a, value::Scalar i, value::Scalar j);
void loopnest_kernel_4(value::Matrix c, value::Matrix a, value::Scalar i, value::Scalar j);
void matmul_kernel(value::Matrix A, value::Matrix B, value::Matrix C, value::Scalar i, value::Scalar j, value::Scalar k);
void initToZero(value::Matrix m, value::Scalar i, value::Scalar j);
void copyToCache(value::Matrix A, value::Matrix cache, value::Scalar i, value::Scalar j);
void copyFromCache(value::Matrix A, value::Matrix cache, value::Scalar i, value::Scalar j);
void copyToSmallCache(value::Matrix A, value::Matrix cache, value::Scalar i, value::Scalar j);
void copyFromSmallCache(value::Matrix A, value::Matrix cache, value::Scalar i, value::Scalar j);
void addOne(value::Matrix m, value::Scalar i, value::Scalar j);
void addTwo(value::Matrix m, value::Scalar i, value::Scalar j);
void set_vector_kernel(value::Vector v, value::Scalar i);
void increment_vector_kernel(value::Vector v, value::Scalar i);
void copy_vector_kernel(value::Vector v1, value::Vector v2, value::Scalar i);
void reorder_vector_kernel(value::Vector v, value::Matrix m, value::Scalar splitParam, value::Scalar i, value::Scalar iOuter, value::Scalar iInner);
void addCachedMatrixToUnchachedMatrix(value::Matrix A, value::Matrix B, value::Scalar Ai, value::Scalar Aj, value::Scalar Bi, value::Scalar Bj);
void addCachedMatrixToUnchachedMatrixUnrolled(value::Matrix A, value::Matrix B, value::Scalar Ai, value::Scalar Aj, value::Scalar Bi, value::Scalar Bj);

} // namespace ell
