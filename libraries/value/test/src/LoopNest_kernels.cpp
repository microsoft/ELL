////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoopNest_kernels.cpp (value)
//  Authors:  Kern Handa, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoopNest_kernels.h"

#include <value/include/ComputeContext.h>
#include <value/include/EmitterContext.h>
#include <value/include/FunctionDeclaration.h>
#include <value/include/LLVMContext.h>
#include <value/include/Matrix.h>
#include <value/include/Tensor.h>
#include <value/include/Value.h>
#include <value/include/Vector.h>

#include <utilities/include/Logger.h>

using namespace ell::logging;
using namespace ell::utilities;
using namespace ell::value;

namespace ell
{

void loopnest_passthrough(ViewAdapter, Scalar i, Scalar j)
{}

void loopnest_kernel(Matrix m, Scalar i, Scalar j)
{
    InvokeForContext<ComputeContext>([&] {
        auto iInt = i.Get<int>();
        auto jInt = j.Get<int>();
        Log() << "m(" << iInt << ", " << jInt << ") = " << (iInt * 2 + jInt * 5) << EOL;
    });
    m(i, j) = i * 2 + j * 5;
}

void loopnest_kernel_2(Matrix m, Scalar i, Scalar j)
{
    InvokeForContext<ComputeContext>([&] {
        auto iInt = i.Get<int>();
        auto jInt = j.Get<int>();
        Log() << "m(" << iInt << ", " << jInt << ") += " << (iInt * 10 + jInt * 2) << EOL;
    });
    m(i, j) += i * 10 + j * 2;
}

void loopnest_kernel_3(Matrix c, Matrix a, Scalar i, Scalar j)
{
    c(i, j) += a(i, j);
}

void loopnest_kernel_4(Matrix c, Matrix a, Scalar i, Scalar j)
{
    Scalar v = Allocate(c.GetValue().GetBaseType(), ScalarLayout);

    v = i * 10;
    v += j;

    c(i, j) = a(i, j) + v;
}

void matmul_kernel(Matrix A, Matrix B, Matrix C, Scalar i, Scalar j, Scalar k)
{
    InvokeForContext<ComputeContext>([&] {
        auto iInt = i.Get<int>();
        auto jInt = j.Get<int>();
        auto kInt = k.Get<int>();
        Log() << "C(" << iInt << ", " << jInt << ") += "
              << "A(" << iInt << ", " << kInt << ") * B(" << kInt << ", " << jInt << ")" << EOL;
    });
    C(i, j) += A(i, k) * B(k, j);
}

void initToZero(Matrix m, Scalar i, Scalar j)
{
    InvokeForContext<ComputeContext>([&] {
        auto iInt = i.Get<int>();
        auto jInt = j.Get<int>();
        Log() << "m(" << iInt << ", " << jInt << ") = " << 0 << EOL;
    });
    m(i, j) = 0;
}

void copyToCache(Matrix A, Matrix cache, Scalar i, Scalar j)
{
    InvokeForContext<ComputeContext>([&] {
        auto iInt = i.Get<int>();
        auto jInt = j.Get<int>();
        Log() << cache.GetValue().GetName() << "(" << iInt << "-" << (iInt + 2) << ", " << jInt << "-" << (jInt + 2) << ") = "
              << A.GetValue().GetName() << "(" << iInt << "-" << (iInt + 2) << ", " << jInt << "-" << (jInt + 2) << ")" << EOL;
    });
    cache(i, j) = A(i, j);
    cache(i + 1, j) = A(i + 1, j);
    cache(i, j + 1) = A(i, j + 1);
    cache(i + 1, j + 1) = A(i + 1, j + 1);
}

void copyFromCache(Matrix A, Matrix cache, Scalar i, Scalar j)
{
    InvokeForContext<ComputeContext>([&] {
        auto iInt = i.Get<int>();
        auto jInt = j.Get<int>();
        Log() << A.GetValue().GetName() << "(" << iInt << "-" << (iInt + 2) << ", " << jInt << "-" << (jInt + 2) << ") = "
              << cache.GetValue().GetName() << "(" << iInt << "-" << (iInt + 2) << ", " << jInt << "-" << (jInt + 2) << ")" << EOL;
    });
    A(i, j) = cache(i, j);
    A(i + 1, j) = cache(i + 1, j);
    A(i, j + 1) = cache(i, j + 1);
    A(i + 1, j + 1) = cache(i + 1, j + 1);
}

void copyToSmallCache(Matrix A, Matrix cache, Scalar i, Scalar j)
{
    InvokeForContext<ComputeContext>([&] {
        auto iInt = i.Get<int>();
        auto jInt = j.Get<int>();
        Log() << "* " << cache.GetValue().GetName() << " = "
              << A.GetValue().GetName() << "(" << iInt << "-" << (iInt + 2) << ", " << jInt << "-" << (jInt + 2) << ")" << EOL;
    });
    cache(0, 0) = A(i, j);
    cache(1, 0) = A(i + 1, j);
    cache(0, 1) = A(i, j + 1);
    cache(1, 1) = A(i + 1, j + 1);
}

void copyFromSmallCache(Matrix A, Matrix cache, Scalar i, Scalar j)
{
    InvokeForContext<ComputeContext>([&] {
        auto iInt = i.Get<int>();
        auto jInt = j.Get<int>();
        Log() << "* " << A.GetValue().GetName() << "(" << iInt << "-" << (iInt + 2) << ", " << jInt << "-" << (jInt + 2) << ") = "
              << cache.GetValue().GetName() << EOL;
    });
    A(i, j) = cache(0, 0);
    A(i + 1, j) = cache(1, 0);
    A(i, j + 1) = cache(0, 1);
    A(i + 1, j + 1) = cache(1, 1);
}

void addOne(Matrix m, Scalar i, Scalar j)
{
    InvokeForContext<ComputeContext>([&] {
        auto iInt = i.Get<int>();
        auto jInt = j.Get<int>();
        Log() << m.GetValue().GetName() << "(" << iInt << ", " << jInt << ") += " << 1 << EOL;
    });
    m(i, j) += 1;
}

void addTwo(Matrix m, Scalar i, Scalar j)
{
    InvokeForContext<ComputeContext>([&] {
        auto iInt = i.Get<int>();
        auto jInt = j.Get<int>();
        Log() << m.GetValue().GetName() << "(" << iInt << ", " << jInt << ") += " << 2 << EOL;
    });
    m(i, j) += 2;
}

void set_vector_kernel(Vector v, Scalar i)
{
    InvokeForContext<ComputeContext>([&] {
        auto iInt = i.Get<int>();
        Log() << "v(" << iInt << ") = " << iInt << EOL;
    });
    v(i) = i;
}

void increment_vector_kernel(Vector v, Scalar i)
{
    InvokeForContext<ComputeContext>([&] {
        auto iInt = i.Get<int>();
        Log() << "v(" << iInt << ") = " << iInt << EOL;
    });
    v(i) += 1;
}

void copy_vector_kernel(Vector v1, Vector v2, Scalar i)
{
    v2(i) = v1(i);
}

void reorder_vector_kernel(Vector v, Matrix m, Scalar splitParam, Scalar i, Scalar iOuter, Scalar iInner)
{
    InvokeForContext<ComputeContext>([&] {
        auto iInt = i.Get<int>();
        auto iOuterInt = iOuter.Get<int>();
        auto iInnerInt = iInner.Get<int>();
        auto splitInt = splitParam.Get<int>();
        Log() << "m(" << iOuterInt << "/" << splitInt << ", " << iInnerInt << ") = v(" << iInt << ")" << EOL;
    });
    m(iOuter / splitParam, iInner) = v(i);
}

void addCachedMatrixToUnchachedMatrix(Matrix A, Matrix B, Scalar Ai, Scalar Aj, Scalar Bi, Scalar Bj)
{
    InvokeForContext<ComputeContext>([&](auto&) {
        auto AiInt = Ai.Get<int>();
        auto AjInt = Aj.Get<int>();
        auto BiInt = Bi.Get<int>();
        auto BjInt = Bj.Get<int>();
        Log() << A.GetValue().GetName() << "(" << AiInt << ", " << AjInt << ") += " << B.GetValue().GetName() << "(" << BiInt << ", " << BjInt << ")" << EOL;
    });
    A(Ai, Aj) += B(Bi, Bj);
}

void addCachedMatrixToUnchachedMatrixUnrolled(Matrix A, Matrix B, Scalar Ai, Scalar Aj, Scalar Bi, Scalar Bj)
{
    InvokeForContext<ComputeContext>([&](auto&) {
        auto BiInt = Bi.Get<int>();
        auto BjInt = Bj.Get<int>();
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                Log() << A.GetValue().GetName() << "(" << i << ", " << j << ") += " << B.GetValue().GetName() << "(" << (BiInt + i) << ", " << (BjInt + j) << ")" << EOL;
            }
        }
    });
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            A(i, j) += B(Bi + i, Bj + j);
        }
    }
}

} // namespace ell
