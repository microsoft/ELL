////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoopNest_test.cpp (value)
//  Authors:  Kern Handa, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoopNest_test.h"
#include "LoopNest_kernels.h"
#include "TestUtil.h"

#include <value/include/ComputeContext.h>
#include <value/include/EmitterContext.h>
#include <value/include/FunctionDeclaration.h>
#include <value/include/LLVMContext.h>
#include <value/include/Matrix.h>
#include <value/include/Tensor.h>
#include <value/include/Value.h>
#include <value/include/Vector.h>

#include <value/include/loopnests/CodeGenerator.h>
#include <value/include/loopnests/Kernel.h>
#include <value/include/loopnests/LoopNest.h>
#include <value/include/loopnests/LoopNestPrinter.h>

#include <emitters/include/IRFunctionEmitter.h>

#include <math/include/Matrix.h>
#include <math/include/Tensor.h>
#include <math/include/Vector.h>

#include <utilities/include/FunctionUtils.h>
#include <utilities/include/Logger.h>

#include <testing/include/testing.h>

#include <deque>
#include <iomanip>
#include <iostream>
#include <queue>
#include <sstream>
#include <type_traits>
#include <unordered_map>
#include <vector>

using namespace ell::emitters;
using namespace ell::utilities;
using namespace ell::logging;
using namespace ell::value;
using namespace ell::value::loopnests;

template <ell::math::MatrixLayout layout>
using LayoutType = std::integral_constant<ell::math::MatrixLayout, layout>;

namespace ell
{
// LoopNest-specific test utilities
namespace
{
    template <typename ListType, typename ElementType>
    bool InList(const ListType& list, ElementType item)
    {
        return std::find(list.begin(), list.end(), item) != list.end();
    }

    void SplitAndSetOrder(LoopNest& loops, const std::vector<Index>& indices, const std::vector<int>& splitSizes, std::string order)
    {
        using IndexMap = std::unordered_map<char, std::pair<Index, std::queue<int>>>;
        IndexMap indexInfo;
        for (const auto& index : indices)
        {
            indexInfo.insert({ index.GetName()[0], { index, {} } });
        }

        std::vector<Index> splits;
        for (auto ch : order)
        {
            auto& [index, indexSplits] = indexInfo.at(ch);
            splits.push_back(index);
            if (indexSplits.empty()) // first visit for this index, copy split list over instead of splitting
            {
                indexSplits = std::queue<int>{ std::deque<int>{ splitSizes.begin(), splitSizes.end() } };
            }
            else
            {
                loops.Split(index, indexSplits.front());
                indexSplits.pop();
            }
        }

        loops.SetLoopOrder(splits);
    }

} // namespace

// Low-level tests of loop nest infrastructure
Scalar SplitIterationDomain_test1()
{
    Index i("i"), j("j");
    SplitIterationDomain domain({ { i, { 0, 120 } },
                                  { j, { 0, 200 } } });

    auto [i1, i2] = domain.Split(i, 30);
    auto [i3, i4] = domain.Split(i2, 15);
    auto [i5, i6] = domain.Split(i4, 5);

    auto [j1, j2] = domain.Split(j, 50);
    auto [j3, j4] = domain.Split(j2, 10);

    if (domain.NumDimensions() != 2)
    {
        return 1;
    }

    // `NumSplits` returns the number of loops, not splits. It should be 4 for `i` and 3 for `j`
    if (domain.NumSplits(i) != 4 || domain.NumSplits(j) != 3)
    {
        return 1;
    }

    if (!domain.IsPrimaryDimension(i) || !domain.IsPrimaryDimension(j))
        return 1;

    for (Index index : { i1, i2, i3, i4, i5, i6 })
    {
        if (domain.GetBaseIndex(index) != i)
            return 1;
        if (domain.IsPrimaryDimension(index))
            return 1;
    }

    for (Index index : { j1, j2, j3, j4 })
    {
        if (domain.GetBaseIndex(index) != j)
            return 1;
        if (domain.IsPrimaryDimension(index))
            return 1;
    }

    const auto iRange = domain.GetDimensionRange(i);
    for (Index index : { i1, i3, i5, i6 })
    {
        if (!iRange.IsLoopIndex(index))
            return 1;
    }
    for (Index index : { i, i2, i4 })
    {
        if (!iRange.IsComputedIndex(index))
            return 1;
    }
    auto parents = iRange.GetAllParentIndices(i4); // should be i, i_0, i_2
    if (!InList(parents, i) || !InList(parents, i2))
        return 1;

    auto dependents = iRange.GetDependentIndices(i4); // /should be i5, i6
    if (!InList(dependents, i5) || !InList(dependents, i6))
        return 1;

    return 0;
}

// Tests of actual loop nests
Scalar LoopNest_test1()
{
    auto matrix = MakeMatrix<int>(4, 5);
    IndexRange i("i", { 0, 4 }), j("j", { 0, 5 });

    auto kernel = Kernel("kernel")
                      .Inputs(matrix.GetValue())
                      .Indices(i.GetIndex(), j.GetIndex())
                      .Define(loopnest_kernel);

    LoopNest loop(std::vector<IndexRange>{ i, j });
    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);

    CodeGenerator generator;
    generator.Run(loop);

#if 0
    PrintLoops(loop, "LoopNest_test1");
#endif

    return matrix(2, 3) - 19; // will return 0 if calculation is correct
}

Scalar LoopNest_test2()
{
    auto matrix = MakeMatrix<int>(4, 5);
    IndexRange i("i", { 0, 4 }), j("j", { 0, 5 });

    auto kernel = Kernel("kernel")
                      .Inputs(matrix.GetValue())
                      .Indices(i.GetIndex(), j.GetIndex())
                      .Define(loopnest_kernel);

    LoopNest loop(std::vector<IndexRange>{ i, j });
    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);
    loop.Split(i.GetIndex(), 2);

    CodeGenerator generator;
    generator.Run(loop);

    return matrix(2, 3) - 19; // will return 0 if calculation is correct
}

// This tests that the loopnest works with a degenerate (1x1) kernel, both for compute and compile
Scalar LoopNest_test3()
{
    // Declare the input matrix
    std::vector<std::vector<int>> dt{
        std::vector<int>{ 1, 2, 3 },
        std::vector<int>{ 4, 5, 6 },
    };
    auto matrix = Matrix(dt);
    // Declare the output matrix and initialize its values to 10.
    auto output = MakeMatrix<int>(static_cast<int>(matrix.Rows()), static_cast<int>(matrix.Columns()));
    For(output, [&](Scalar row, Scalar column) {
        output(row, column) = 10;
    });

    // Use a Loopnest to call loopnest_kernel_3 for each element of the input matrix and write the result to
    // our output.
    loopnests::IndexRange i("i", { 0, static_cast<int>(matrix.Rows()) }), j("j", { 0, static_cast<int>(matrix.Columns()) });

    auto kernel = loopnests::Kernel("kernel")
                      .Inputs(output.GetValue(), matrix.GetValue())
                      .Indices(i.GetIndex(), j.GetIndex())
                      .Define(loopnest_kernel_3);

    loopnests::LoopNest loop(std::vector<loopnests::IndexRange>{ i, j });
    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);

    loopnests::CodeGenerator generator;
    generator.Run(loop);

    // loopnest_kernel_3 will add the input element to the output element.
    // Since we initialized the output to 10, we expect the result to be
    // 10 greater than the input.
    std::vector<int> expectedValues{ 11, 12, 13, 14, 15, 16 };
    auto expected = Vector(expectedValues);

    // View the result as a Vector
    Vector actual = AsVector(AsFullView(output));

    // Verify that the actual result is what we expect
    return VerifySame(actual, expected);
}

// This tests that the loopnest works with a degenerate (1x1) kernel, both for compute and compile,
// when the kernel has non-trival assignment code in it.
Scalar LoopNest_test4()
{
    // Declare the output matrix and initialize its values to 0.
    auto output = MakeMatrix<int>(2, 6);
    For(output, [&](Scalar row, Scalar column) {
        output(row, column) = 0;
    });

    // Use a Loopnest to call loopnest_kernel_3 for each element of the input matrix and write the result to
    // our output.
    loopnests::IndexRange i("i", { 0, static_cast<int>(output.Rows()) }), j("j", { 0, static_cast<int>(output.Columns()) });

    auto kernel = loopnests::Kernel("kernel")
                      .Inputs(output.GetValue(), output.GetValue())
                      .Indices(i.GetIndex(), j.GetIndex())
                      .Define(loopnest_kernel_4);

    loopnests::LoopNest loop(std::vector<loopnests::IndexRange>{ i, j });
    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);
    loop.Split(j.GetIndex(), 2);

    loopnests::CodeGenerator generator;
    generator.Run(loop);

    // loopnest_kernel_4 will multiply row by 10 and add the column.
    std::vector<int> expectedValues{ 0, 1, 2, 3, 4, 5, 10, 11, 12, 13, 14, 15 };
    auto expected = Vector(expectedValues);

    // View the result as a Vector
    Vector actual = AsVector(AsFullView(output));

    // Verify that the actual result is what we expect
    return VerifySame(actual, expected);
}

// Simple loopnest test using variable-length inputs and indices APIs
Scalar LoopNest_test5()
{
    auto matrix = MakeMatrix<int>(4, 5);
    Index i("i"), j("j");
    LoopNest loop({ { i, { 0, 4 } },
                    { j, { 0, 5 } } });

    auto kernel = Kernel("kernel")
                      .Inputs({ matrix.GetValue() })
                      .Indices({ i, j })
                      .Define(loopnest_kernel);

    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);
    loop.Split(i, 2);

    CodeGenerator generator;
    generator.Run(loop);

    return matrix(2, 3) - 19; // will return 0 if calculation is correct
}

// Simple loopnest test that loops from X to N where N > X > 0
Scalar LoopNest_test6()
{
    int N = 4;
    int X = 2;
    auto matrix = MakeMatrix<int>(N, N, "matrix");

    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            matrix(i, j) = i + j;
        });
    });
    // matrix:
    // [ 0,  1,  2,  3 ]
    // [ 1,  2,  3,  4 ]
    // [ 2,  3,  4,  5 ]
    // [ 3,  4,  5,  6 ]

    // Sum the bottom right quadrant of the matrix and store the value in position (0, 0)

    Index i("i"), j("j");
    LoopNest loop({ { i, { X, N } },
                    { j, { X, N } } });
    auto kernel = Kernel("kernel")
                      .Inputs(matrix.GetValue())
                      .Indices(i, j)
                      .Define([](Matrix mat, Scalar i, Scalar j) {
                          mat(0, 0) += mat(i, j);
                      });
    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);
    CodeGenerator generator;
    generator.Run(loop);
    return matrix(0, 0) - 20; // Will return 0 if calculation is correct
}

Scalar LoopNestNonzeroStart_test()
{
    const int size = 12;
    const int begin = 2;
    const int end = 10;
    auto vector = MakeVector<int>(size);
    for (int i = 0; i < size; ++i)
    {
        vector(i) = 100 * i;
    }
    std::vector<int> expectedValues(size);
    for (int i = 0; i < size; ++i)
    {
        expectedValues[i] = 100 * i;
    }
    for (int i = begin; i < end; ++i)
    {
        expectedValues[i] = i;
    }
    auto expected = Vector(expectedValues);

    Index i("i");
    LoopNest loop({ { i, { begin, end } } });
    auto kernel = Kernel("k")
                      .Inputs(vector.GetValue())
                      .Indices(i)
                      .Define(set_vector_kernel);

    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);

    CodeGenerator generator;
    generator.Run(loop);

#if 0
    PrintLoops(loop, "LoopNestNonzeroStart_test");
#endif

    // Verify that the actual result is what we expect
    return VerifySame(vector, expected);
}

Scalar LoopNestBoundary_test1()
{
    const int size = 12;
    const int n = 10;
    auto vector = MakeVector<int>(size);
    for (int i = n; i < size; ++i)
    {
        vector(i) = 100 * i;
    }
    std::vector<int> expectedValues(size);
    for (int i = 0; i < n; ++i)
    {
        expectedValues[i] = i;
    }
    for (int i = n; i < size; ++i)
    {
        expectedValues[i] = 100 * i;
    }
    auto expected = Vector(expectedValues);

    Index i("i");
    LoopNest loop({ { i, { 0, n } } });
    auto split_i = loop.Split(i, 4);

    auto kernel = Kernel("k")
                      .Inputs(vector.GetValue())
                      .Indices(i)
                      .Define(set_vector_kernel);

    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);

    CodeGenerator generator;
    generator.Run(loop);

#if 0
    PrintLoops(loop, "LoopNestBoundary_test1");
#endif

    // Verify that the actual result is what we expect
    return VerifySame(vector, expected);
}

Scalar LoopNestBoundary_test2()
{
    const int size = 12;
    const int n = 10;
    auto vector = MakeVector<int>(size);
    for (int i = n; i < size; ++i)
    {
        vector(i) = 100 * i;
    }
    std::vector<int> expectedValues(size);
    for (int i = 0; i < n; ++i)
    {
        expectedValues[i] = i;
    }
    for (int i = n; i < size; ++i)
    {
        expectedValues[i] = 100 * i;
    }
    auto expected = Vector(expectedValues);

    Index i("i");
    LoopNest loop({ { i, { 0, n } } });
    auto split_i = loop.Split(i, 4);
    auto split_i2 = loop.Split(i, 2);

    auto kernel = Kernel("k")
                      .Inputs(vector.GetValue())
                      .Indices(i)
                      .Define(set_vector_kernel);

    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);

    CodeGenerator generator;
    generator.Run(loop);

#if 0
    PrintLoops(loop, "LoopNestBoundary_test2");
#endif

    // Verify that the actual result is what we expect
    return VerifySame(vector, expected);
}

Scalar LoopNestBoundary_test3()
{
    const int size = 12;
    const int n = 8;
    auto vector = MakeVector<int>(size);
    for (int i = n; i < size; ++i)
    {
        vector(i) = 100 * i;
    }
    std::vector<int> expectedValues(size);
    for (int i = 0; i < n; ++i)
    {
        expectedValues[i] = i;
    }
    for (int i = n; i < size; ++i)
    {
        expectedValues[i] = 100 * i;
    }
    auto expected = Vector(expectedValues);

    Index i("i"), j("j");
    LoopNest loop({ { i, { 0, n } }, { j, { 0, n } } });
    loop.Split(i, 4);
    loop.Split(i, 2);
    loop.Split(j, 4);

    loop.SetLoopOrder({ i, j, i, j, i });

    auto kernel = Kernel("k")
                      .Inputs(vector.GetValue())
                      .Indices(i)
                      .Define(set_vector_kernel);

    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);

    CodeGenerator generator;
    generator.Run(loop);

#if 0
    PrintLoops(loop, "LoopNestBoundary_test3");
#endif

    // Verify that the actual result is what we expect
    return VerifySame(vector, expected);
}

Scalar LoopNestBoundary_test4()
{
    // The input matrices:
    // A:                                 B:                                 C:
    // [ 0, -1, -2, -3, -4, -5, -6, -7]   [ 0,  2,  4,  6,  8, 10, 12, 14]   [ 100 100 100 ... ]
    // [ 1,  0, -1, -2, -3, -4, -5, -6]   [ 1,  3,  5,  7,  9, 11, 13, 15]   [ 100 100 100 ... ]
    // [ 2,  1,  0, -1, -2, -3, -4, -5]   [ 2,  4,  6,  8, 10, 12, 14, 16]   [      ...        ]
    // [ 3,  2,  1,  0, -1, -2, -3, -4]   [ 3,  5,  7,  9, 11, 13, 15, 17]   [      ...        ]
    // [ 4,  3,  2,  1,  0, -1, -2, -3]   [ 4,  6,  8, 10, 12, 14, 16, 18]   [      ...        ]
    // [ 5,  4,  3,  2,  1,  0, -1, -2]   [ 5,  7,  9, 11, 13, 15, 17, 19]   [      ...        ]
    // [ 6,  5,  4,  3,  2,  1,  0, -1]   [ 6,  8, 10, 12, 14, 16, 18, 20]   [      ...        ]
    // [ 7,  6,  5,  4,  3,  2,  1,  0]   [ 7,  9, 11, 13, 15, 17, 19, 21]   [      ...        ]

    // (A * B) (the desired result):
    // [-140, -196, -252, -308, -364, -420, -476, -532]
    // [-112, -152, -192, -232, -272, -312, -352, -392]
    // [ -84, -108, -132, -156, -180, -204, -228, -252]
    // [ -56,  -64,  -72,  -80,  -88,  -96, -104, -112]
    // [ -28,  -20,  -12,   -4,    4,   12,   20,   28]
    // [   0,   24,   48,   72,   96,  120,  144,  168]
    // [  28,   68,  108,  148,  188,  228,  268,  308]
    // [  56,  112,  168,  224,  280,  336,  392,  448]

    const int N = 8;
    const int M = N;
    const int K = N;

    auto A = MakeMatrix<int>(M, K, "A");
    auto B = MakeMatrix<int>(K, N, "B");
    auto C = MakeMatrix<int>(M, N, "C");
    auto expected = MakeMatrix<int>(M, N, "expected");

    // initialize A, B, and C
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
            B(i, j) = i + 2 * j;
            C(i, j) = 100;
            expected(i, j) = 0;
        });
    });

    // fill out expected with a simple for-loop gemm
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            ForRange(N, [&](Scalar k) {
                expected(i, j) += A(i, k) * B(k, j);
            });
        });
    });

    // Do computation in blocks of k_r x k_c
    const int k_r = 3;
    const int k_c = 4;
    Matrix temp = MakeMatrix<int>(k_r, k_c, "temp");

    loopnests::Index i("i"), j("j"), k("k");
    loopnests::LoopNest loop({ { i, { 0, M } },
                               { j, { 0, N } },
                               { k, { 0, K } } });

    auto [i_outer, i_inner] = loop.Split(i, k_r);
    auto [j_outer, j_inner] = loop.Split(j, k_c);

    auto prologueKernel = loopnests::Kernel("prologue")
                              .Inputs(temp.GetValue())
                              .Indices(i_inner, j_inner)
                              .Define([](Matrix temp, Scalar i_inner, Scalar j_inner) {
                                  temp(i_inner, j_inner) = 0;
                              });

    auto bodyKernel = loopnests::Kernel("body")
                          .Inputs(A.GetValue(), B.GetValue(), C.GetValue(), temp.GetValue())
                          .Indices(i, j, i_inner, j_inner, k)
                          .Define([](Matrix A, Matrix B, Matrix C, Matrix temp, Scalar i, Scalar j, Scalar i_inner, Scalar j_inner, Scalar k) {
                              temp(i_inner, j_inner) += A(i, k) * B(k, j);
                          });
    auto epilogueKernel = loopnests::Kernel("epilogue")
                              .Inputs(C.GetValue(), temp.GetValue())
                              .Indices(i, j, j_outer, i_inner, j_inner)
                              .Define([](Matrix C, Matrix temp, Scalar i, Scalar j, Scalar j_outer, Scalar i_inner, Scalar j_inner) {
                                  C(i, j) = temp(i_inner, j_inner);
                              });

    loop.SetLoopOrder({ i_outer, j_outer, k, j_inner, i_inner });

    loop.AddKernel(prologueKernel, First(k));
    loop.AddKernel(bodyKernel, LoopNest::ConstraintType::predicate);
    loop.AddKernel(epilogueKernel, Last(k));

    loopnests::CodeGenerator generator;
    generator.Run(loop);

#if 0
    PrintLoops(loop, "LoopNestBoundary_test4");
#endif

    return VerifySame(C, expected);
}

Scalar LoopNestBoundary_test5()
{
    const int M = 9;
    const int N = 10;
    const int K = 11;

    // Computes A*B + 1
    auto A = MakeMatrix<int>(M, K, "A");
    auto B = MakeMatrix<int>(K, N, "B");
    auto C = MakeMatrix<int>(M, N, "C");
    auto expected = MakeMatrix<int>(M, N, "expected");

    // initialize A, B, and C
    ForRange(M, [&](Scalar i) {
        ForRange(K, [&](Scalar j) {
            // A(i, j) = i - j;
            A(i, j) = 1;
        });
    });

    ForRange(K, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            // B(i, j) = i + 2 * j;
            B(i, j) = 1;
        });
    });

    ForRange(M, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            C(i, j) = 100;
            expected(i, j) = 0;
        });
    });

    // fill out expected with a simple for-loop gemm (plus 1)
    ForRange(M, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            ForRange(K, [&](Scalar k) {
                expected(i, j) += A(i, k) * B(k, j);
            });
            expected(i, j) += 1;
        });
    });

    loopnests::Index i("i"), j("j"), k("k");
    loopnests::LoopNest loop({ { i, { 0, M } },
                               { j, { 0, N } },
                               { k, { 0, K } } });

    auto prologueKernel = loopnests::Kernel("Prologue")
                              .Inputs(C.GetValue())
                              .Indices(i, j)
                              .Define([](Matrix C, Scalar i, Scalar j) {
                                  C(i, j) = 0;
                              });
    auto bodyKernel = loopnests::Kernel("Body")
                          .Inputs(A.GetValue(), B.GetValue(), C.GetValue())
                          .Indices(i, j, k)
                          .Define([](Matrix A, Matrix B, Matrix C, Scalar i, Scalar j, Scalar k) {
                              C(i, j) += A(i, k) * B(k, j);
                          });
    auto epilogueKernel = loopnests::Kernel("Epilogue")
                              .Inputs(C.GetValue())
                              .Indices(i, j)
                              .Define([](Matrix C, Scalar i, Scalar j) {
                                  C(i, j) += 1;
                              });

    loop.AddKernel(prologueKernel, First(k));
    loop.AddKernel(bodyKernel, LoopNest::ConstraintType::predicate);
    loop.AddKernel(epilogueKernel, Last(k), PlacementPredicate{ Placement::after });

    auto [i_outer, i_inner] = loop.Split(i, 4);
    auto [j_outer, j_inner] = loop.Split(j, 4);
    auto [k_outer, k_inner] = loop.Split(k, 4);

    loopnests::CodeGenerator generator;
    generator.Run(loop);

#if 0
    PrintLoops(loop, "LoopNestBoundary_test5");
#endif

    return VerifySame(C, expected);
}

Scalar LoopNestReorder_test1()
{
    auto matrix = MakeMatrix<int>(4, 5);
    IndexRange i("i", { 0, 4 }), j("j", { 0, 5 });
    auto iIndex = i.GetIndex();
    auto jIndex = j.GetIndex();

    auto kernel = Kernel("kernel")
                      .Inputs(matrix.GetValue())
                      .Indices(iIndex, jIndex)
                      .Define(loopnest_kernel);

    LoopNest loop(IterationDomain({ i, j }));
    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);
    loop.Split(i.GetIndex(), 2);
    loop.SetLoopOrder({ iIndex, jIndex, iIndex });

    CodeGenerator generator;
    generator.Run(loop);

#if 0
    PrintLoops(loop, "LoopNestReorder_test1");
#endif

    return matrix(2, 3) - 19; // will return 0 if calculation is correct
}

Scalar LoopNestReorder_test2()
{
    auto matrix = MakeMatrix<int>(4, 5);
    Index i("i"), j("j");

    auto kernel = Kernel("kernel")
                      .Inputs(matrix.GetValue())
                      .Indices(i, j)
                      .Define(loopnest_kernel);

    LoopNest loop(IterationDomain({ { i, { 0, 4 } }, { j, { 0, 5 } } }));
    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);
    auto [iOuter, iInner] = loop.Split(i, 2);
    loop.SetLoopOrder({ iInner, j, iOuter });

    CodeGenerator generator;
    generator.Run(loop);

#if 0
    PrintLoops(loop, "LoopNestReorder_test2");
#endif

    return matrix(2, 3) - 19; // will return 0 if calculation is correct
}

Scalar TwoKernel_test()
{
    auto matrix = MakeMatrix<int>(4, 5);
    IndexRange i("i", { 0, 4 }), j("j", { 0, 5 });
    auto iIndex = i.GetIndex();
    auto jIndex = j.GetIndex();

    auto kernel1 = Kernel("kernel1")
                       .Inputs(matrix.GetValue())
                       .Indices(iIndex, jIndex)
                       .Define(loopnest_kernel);

    auto kernel2 = Kernel("kernel2")
                       .Inputs(matrix.GetValue())
                       .Indices(iIndex, jIndex)
                       .Define(loopnest_kernel_2);

    LoopNest loop(IterationDomain({ i, j }));
    loop.AddKernel(kernel1, LoopNest::ConstraintType::predicate);
    loop.AddKernel(kernel2, LoopNest::ConstraintType::predicate);
    loop.Split(i.GetIndex(), 2);
    loop.SetLoopOrder({ iIndex, jIndex, iIndex });

    CodeGenerator generator;
    generator.Run(loop);

#if 0
    PrintLoops(loop, "TwoKernel_test");
#endif

    auto expected = 19 + 26; // 19 == 1st kernel (2*i + 5*j), 26 == 2nd kernel (10*i+2*j)
    return matrix(2, 3) - expected; // will return 0 if calculation is correct
}

// Prototype for test with a kernel that runs on the last iteration of an index
// split: where to split the loop (0 if no split)
// id: id to use for body and "last" kernels ("" if they should not share an ID)
Scalar LoopNestLastPredicate_test(std::string tag, int split, std::string id)
{
    const int n = 32;
    std::vector<int> expectedValues(n);
    for (int i = 0; i < n; ++i)
    {
        expectedValues[i] = i;
    }
    if (id.empty())
    {
        expectedValues[n - 1] += 1;
    }
    else
    {
        expectedValues[n - 1] = 1;
    }
    auto expected = Vector(expectedValues);

    auto vector = MakeVector<int>(n);
    Index i("i");
    LoopNest loop({ { i, { 0, n } } });

    if (split != 0)
    {
        loop.Split(i, split);
    }

    auto kernel = Kernel("k", id)
                      .Inputs(vector.GetValue())
                      .Indices(i)
                      .Define(set_vector_kernel);

    auto boundaryKernel = Kernel("boundary", id)
                              .Inputs(vector.GetValue())
                              .Indices(i)
                              .Define(increment_vector_kernel);

    if (id.empty())
    {
        loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);
        loop.AddKernel(boundaryKernel, { Last(i) });
    }
    else
    {
        loop.AddKernel(boundaryKernel, { Last(i) });
        loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);
    }

#if 0
    PrintLoops(loop, "LoopNestLastPredicate_test_" + tag);
#endif

    CodeGenerator generator;
    generator.Run(loop);

    // Verify that the actual result is what we expect
    return VerifySame(vector, expected);
}

// Test with a kernel that runs on the last iteration of an index
Scalar LoopNestLastPredicate_test1()
{
    return LoopNestLastPredicate_test("1", 0, "");
}

// Test with a kernel that runs on the last iteration of an index, with a split loop
Scalar LoopNestLastPredicate_test2()
{
    return LoopNestLastPredicate_test("2", 4, "");
}

// Test with an alternate kernel that runs on the last iteration of an index (instead of the main kernel)
Scalar LoopNestLastPredicate_test3()
{
    return LoopNestLastPredicate_test("3", 0, "k");
}
// Test with an alternate kernel that runs on the last iteration of an index (instead of the main kernel), with a split loop
Scalar LoopNestLastPredicate_test4()
{
    return LoopNestLastPredicate_test("4", 4, "k");
}

Scalar LoopNestBoundaryPredicate_test1()
{
    const int size = 12;
    const int n = 10;
    auto vector = MakeVector<int>(size);
    for (int i = n; i < size; ++i)
    {
        vector(i) = 100 * i;
    }

    std::vector<int> expectedValues(size);
    int mainEnd = 4 * (n / 4);
    for (int i = 0; i < mainEnd; ++i)
    {
        expectedValues[i] = i;
    }
    for (int i = mainEnd; i < n; ++i)
    {
        expectedValues[i] = 1;
    }
    for (int i = n; i < size; ++i)
    {
        expectedValues[i] = 100 * i; // same as initialized vector, untouched
    }
    auto expected = Vector(expectedValues);

    Index i("i");
    LoopNest loop({ { i, { 0, n } } });
    auto split_i = loop.Split(i, 4);

    auto kernel = Kernel("k")
                      .Inputs(vector.GetValue())
                      .Indices(i)
                      .Define(set_vector_kernel);

    auto boundaryKernel = Kernel("boundary", kernel.GetId())
                              .Inputs(vector.GetValue())
                              .Indices(i)
                              .Define(increment_vector_kernel);

    loop.AddKernel(boundaryKernel, { EndBoundary(i) });
    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);

    CodeGenerator generator;
    generator.Run(loop);

#if 0
    PrintLoops(loop, "LoopNestBoundaryPredicate_test1");
#endif

    // Verify that the actual result is what we expect
    return VerifySame(vector, expected);
}

Scalar MissingIndex_test()
{
    const int n = 12;
    const int splitAmount = 4;
    auto vector = MakeVector<int>(n);

    ForRange(n, [&](Scalar i) {
        vector(i) = Scalar(100);
    });

    std::vector<int> expectedValues(n);
    for (int i = 0; i < n; ++i)
    {
        expectedValues[i] = 100;
    }
    for (int i = 0; i < n; i += splitAmount)
    {
        expectedValues[i] = i;
    }
    auto expected = Vector(expectedValues);

    Index i("i");
    LoopNest loop({ { i, { 0, n } } });
    auto [i_outer, i_inner] = loop.Split(i, splitAmount);

    auto kernel = Kernel("k")
                      .Inputs(vector.GetValue())
                      .Indices(i)
                      .Define(set_vector_kernel); // v[i] = i

    CodePositionConstraints constraint(LoopFragmentType::body, { i_outer }, {});
    loop.AddKernel(kernel, constraint);

    CodeGenerator generator;
    generator.Run(loop);

#if 0
    PrintLoops(loop, "MissingIndex_test");
#endif

    // Verify that the actual result is what we expect
    return VerifySame(vector, expected);
}

Scalar RequiredIndex_test()
{
    std::string loopOrder = "ijk";
    const int N = 8;
    auto A = MakeMatrix<int>(N, N, "A");
    auto B = MakeMatrix<int>(N, N, "B");
    auto C = MakeMatrix<int>(N, N, "C");

    // initialize A, B, and C
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
            B(i, j) = i + 2 * j;
            C(i, j) = 100;
        });
    });

    // The input matrices:
    // A:                                 B:                                 C:
    // [ 0, -1, -2, -3, -4, -5, -6, -7]   [ 0,  2,  4,  6,  8, 10, 12, 14]   [ 100 100 100 ... ]
    // [ 1,  0, -1, -2, -3, -4, -5, -6]   [ 1,  3,  5,  7,  9, 11, 13, 15]   [ 100 100 100 ... ]
    // [ 2,  1,  0, -1, -2, -3, -4, -5]   [ 2,  4,  6,  8, 10, 12, 14, 16]   [      ...        ]
    // [ 3,  2,  1,  0, -1, -2, -3, -4]   [ 3,  5,  7,  9, 11, 13, 15, 17]   [      ...        ]
    // [ 4,  3,  2,  1,  0, -1, -2, -3]   [ 4,  6,  8, 10, 12, 14, 16, 18]   [      ...        ]
    // [ 5,  4,  3,  2,  1,  0, -1, -2]   [ 5,  7,  9, 11, 13, 15, 17, 19]   [      ...        ]
    // [ 6,  5,  4,  3,  2,  1,  0, -1]   [ 6,  8, 10, 12, 14, 16, 18, 20]   [      ...        ]
    // [ 7,  6,  5,  4,  3,  2,  1,  0]   [ 7,  9, 11, 13, 15, 17, 19, 21]   [      ...        ]

    // (A * B) + 1  (the desired result):
    // [-139, -195, -251, -307, -363, -419, -475, -531]
    // [-111, -151, -191, -231, -271, -311, -351, -391]
    // [ -83, -107, -131, -155, -179, -203, -227, -251]
    // [ -55,  -63,  -71,  -79,  -87,  -95, -103, -111]
    // [ -27,  -19,  -11,   -3,    5,   13,   21,   29]
    // [   1,   25,   49,   73,   97,  121,  145,  169]
    // [  29,   69,  109,  149,  189,  229,  269,  309]
    // [  57,  113,  169,  225,  281,  337,  393,  449]

    Index i("i"), j("j"), k("k");

    auto initCKernel = Kernel("init")
                           .Inputs(C.GetValue())
                           .Indices(i, j)
                           .Define(initToZero);
    auto innerKernel = Kernel("matmul")
                           .Inputs(A.GetValue(), B.GetValue(), C.GetValue())
                           .Indices(i, j, k)
                           .Define(matmul_kernel);
    auto postProcessCKernel = Kernel("post")
                                  .Inputs(C.GetValue())
                                  .Indices(i, j)
                                  .Define(addOne);

    LoopNest loop({ { i, { 0, N } },
                    { j, { 0, N } },
                    { k, { 0, N } } });

    CodePositionConstraints preConstraint{ LoopFragmentType::prologue, { i, j }, {} };
    loop.AddKernel(initCKernel, preConstraint);

    loop.AddKernel(innerKernel, LoopNest::ConstraintType::constraint);

    CodePositionConstraints postConstraint{ LoopFragmentType::epilogue, { i, j }, {} };
    loop.AddKernel(postProcessCKernel, postConstraint);

    // SplitAndSetOrder(loop, { i, j, k }, { 4, 2 }, loopOrder);

    CodeGenerator generator;
    generator.Run(loop);

#if 0 // DEBUGGING
    PrintLoops(loop, "RequiredIndex_test");

    InvokeForContext<ComputeContext>([&](auto&) {
        ForRange(N, [&](Scalar i) {
            ForRange(N, [&](Scalar j) {
                auto val = C(i, j).Get<int>();
                Log() << std::setw(5) << val;
            });
            Log() << EOL;
        });
    });
#endif

    return C(1, 2) + C(2, 1) - (-191 + -107); // will return 0 if calculation is correct
}

Scalar SimpleImperfectNest_test()
{
    const int N = 4;
    Vector A = MakeVector<int>(N);
    Vector B = MakeVector<int>(N);

    // initialize A, B, and C
    ForRange(N, [&](Scalar i) {
        A(i) = 10;
        B(i) = 20;
    });
    Index i("i");

    auto prologueKernel = Kernel("prologue")
                              .Inputs(A.GetValue())
                              .Indices(i)
                              .Define(set_vector_kernel);
    auto bodyKernel = Kernel("body")
                          .Inputs(A.GetValue())
                          .Indices(i)
                          .Define(increment_vector_kernel);
    auto epilogueKernel = Kernel("epilogue")
                              .Inputs(A.GetValue(), B.GetValue())
                              .Indices(i)
                              .Define(copy_vector_kernel);

    LoopNest loop({ { i, { 0, N } } });

    CodePositionConstraints prologueConstraint{ LoopFragmentType::prologue, {}, { i } };
    loop.AddKernel(prologueKernel, prologueConstraint);

    loop.AddKernel(bodyKernel, LoopNest::ConstraintType::constraint);

    CodePositionConstraints epilogueConstraint{ LoopFragmentType::epilogue, {}, { i } };
    loop.AddKernel(epilogueKernel, epilogueConstraint);

    CodeGenerator generator;
    generator.Run(loop);

    // DEBUGGING
#if 0
    PrintLoops(loop, "SimpleImperfectNest_test");
#endif

#if 0
    InvokeForContext<ComputeContext>([&](auto&) {
        ForRange(N, [&](Scalar i) {
            ForRange(N, [&](Scalar j) {
                auto val = C(i, j).Get<int>();
                Log() << std::setw(5) << val;
            });
            Log() << EOL;
        });
    });
#endif

    std::vector<int> expectedValues{ 20, 20, 20, 11 };
    auto expected = Vector(expectedValues);

    // Verify that the actual result is what we expect
    return VerifySame(B, expected);
}

Scalar ImperfectNest_test(std::string loopOrder)
{
    const int N = 8;
    auto A = MakeMatrix<int>(N, N, "A");
    auto B = MakeMatrix<int>(N, N, "B");
    auto C = MakeMatrix<int>(N, N, "C");

    // initialize A, B, and C
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
            B(i, j) = i + 2 * j;
            C(i, j) = 100;
        });
    });

    // The input matrices:
    // A:                                 B:                                 C:
    // [ 0, -1, -2, -3, -4, -5, -6, -7]   [ 0,  2,  4,  6,  8, 10, 12, 14]   [ 100 100 100 ... ]
    // [ 1,  0, -1, -2, -3, -4, -5, -6]   [ 1,  3,  5,  7,  9, 11, 13, 15]   [ 100 100 100 ... ]
    // [ 2,  1,  0, -1, -2, -3, -4, -5]   [ 2,  4,  6,  8, 10, 12, 14, 16]   [      ...        ]
    // [ 3,  2,  1,  0, -1, -2, -3, -4]   [ 3,  5,  7,  9, 11, 13, 15, 17]   [      ...        ]
    // [ 4,  3,  2,  1,  0, -1, -2, -3]   [ 4,  6,  8, 10, 12, 14, 16, 18]   [      ...        ]
    // [ 5,  4,  3,  2,  1,  0, -1, -2]   [ 5,  7,  9, 11, 13, 15, 17, 19]   [      ...        ]
    // [ 6,  5,  4,  3,  2,  1,  0, -1]   [ 6,  8, 10, 12, 14, 16, 18, 20]   [      ...        ]
    // [ 7,  6,  5,  4,  3,  2,  1,  0]   [ 7,  9, 11, 13, 15, 17, 19, 21]   [      ...        ]

    // (A * B) + 1  (the desired result):
    // [-139, -195, -251, -307, -363, -419, -475, -531]
    // [-111, -151, -191, -231, -271, -311, -351, -391]
    // [ -83, -107, -131, -155, -179, -203, -227, -251]
    // [ -55,  -63,  -71,  -79,  -87,  -95, -103, -111]
    // [ -27,  -19,  -11,   -3,    5,   13,   21,   29]
    // [   1,   25,   49,   73,   97,  121,  145,  169]
    // [  29,   69,  109,  149,  189,  229,  269,  309]
    // [  57,  113,  169,  225,  281,  337,  393,  449]

    Index i("i"), j("j"), k("k");

    auto innerKernel = Kernel("matmul")
                           .Inputs(A.GetValue(), B.GetValue(), C.GetValue())
                           .Indices(i, j, k)
                           .Define(matmul_kernel);
    auto initCKernel = Kernel("init")
                           .Inputs(C.GetValue())
                           .Indices(i, j)
                           .Define(initToZero);
    auto postProcessCKernel = Kernel("post")
                                  .Inputs(C.GetValue())
                                  .Indices(i, j)
                                  .Define(addOne);

    LoopNest loop({ { i, { 0, N } },
                    { j, { 0, N } },
                    { k, { 0, N } } });

    CodePositionConstraints preConstraint{ LoopFragmentType::prologue, { i, j }, { k } };
    loop.AddKernel(initCKernel, preConstraint);

    loop.AddKernel(innerKernel, LoopNest::ConstraintType::constraint);

    CodePositionConstraints postConstraint{ LoopFragmentType::epilogue, { i, j }, { k } };
    loop.AddKernel(postProcessCKernel, postConstraint);

    SplitAndSetOrder(loop, { i, j, k }, { 4, 2 }, loopOrder);

    CodeGenerator generator;
    generator.Run(loop);

#if 0
    PrintLoops(loop, "ImperfectNest_test_" + loopOrder);
#endif

#if 0
    InvokeForContext<ComputeContext>([&](auto&) {
        ForRange(N, [&](Scalar i) {
            ForRange(N, [&](Scalar j) {
                auto val = C(i, j).Get<int>();
                Log() << std::setw(5) << val;
            });
            Log() << EOL;
        });
    });
#endif

    return C(1, 2) + C(2, 1) - (-191 + -107); // will return 0 if calculation is correct
}

Scalar ImperfectNest_test_ijk()
{
    return ImperfectNest_test("ijk");
}

Scalar ImperfectNest_test_ikj()
{
    return ImperfectNest_test("ikj");
}

Scalar ImperfectNest_test_kij()
{
    return ImperfectNest_test("kij");
}

Scalar ImperfectNest_test_ijkijk()
{
    return ImperfectNest_test("ijkijk");
}

Scalar ImperfectNest_test_kijijk()
{
    return ImperfectNest_test("kijijk");
}

Scalar ImperfectNest_test_ijkkij()
{
    return ImperfectNest_test("ijkkij");
}

Scalar SplitIndex_test1_old()
{
    auto vector = MakeVector<int>(4 * 5);
    auto matrix = MakeMatrix<int>(4, 5);
    auto splitParam = Scalar(Allocate<int>(utilities::ScalarLayout));
    splitParam = 5;
    IndexRange i("i", { 0, 4 * 5 });
    auto iIndex = i.GetIndex();
    LoopNest loop(std::vector<IndexRange>{ i });
    auto [i_outer, i_inner] = loop.Split(iIndex, 10);

    auto kernel1 = Kernel("set_vector")
                       .Inputs(vector.GetValue())
                       .Indices(iIndex)
                       .Define(set_vector_kernel);

    auto kernel2 = Kernel("reorder_vector")
                       .Inputs(vector.GetValue(), matrix.GetValue(), splitParam.GetValue())
                       .Indices(iIndex, i_outer, i_inner)
                       .Define(reorder_vector_kernel);

    loop.AddKernel(kernel1, LoopNest::ConstraintType::constraint);
    loop.AddKernel(kernel2, LoopNest::ConstraintType::constraint);

    CodeGenerator generator;
    generator.Run(loop);

#if 0
    PrintLoops(loop, "SplitIndex_test1_old");
#endif

    return (vector(0) - matrix(0, 0)) + (vector(13) - matrix(2, 3)) + (matrix(3, 2) - (3 * 5 + 2));
}

Scalar SplitIndex_test1()
{
    auto vector = MakeVector<int>(4 * 5);
    auto matrix = MakeMatrix<int>(4, 5);
    auto splitParam = Scalar(Allocate<int>(utilities::ScalarLayout));
    splitParam = 5;
    IndexRange i("i", { 0, 4 * 5 });
    auto iIndex = i.GetIndex();
    LoopNest loop(std::vector<IndexRange>{ i });
    auto [i_outer, i_inner] = loop.Split(iIndex, 10);

    auto kernel1 = Kernel("set_vector")
                       .Inputs(vector.GetValue())
                       .Indices(iIndex)
                       .Define(set_vector_kernel);

    auto kernel2 = Kernel("reorder_vector")
                       .Inputs(vector.GetValue(), matrix.GetValue(), splitParam.GetValue())
                       .Indices(iIndex, i_outer, i_inner)
                       .Define(reorder_vector_kernel);

    loop.AddKernel(kernel1, LoopNest::ConstraintType::predicate);
    loop.AddKernel(kernel2, LoopNest::ConstraintType::predicate);

    CodeGenerator generator;
    generator.Run(loop);

#if 0
    PrintLoops(loop, "SplitIndex_test1");
#endif

    return (vector(0) - matrix(0, 0)) + (vector(13) - matrix(2, 3)) + (matrix(3, 2) - (3 * 5 + 2));
}

// Same as SplitIndex_test1, but with an extra split
Scalar SplitIndex_test2()
{
    auto vector = MakeVector<int>(4 * 5);
    auto matrix = MakeMatrix<int>(4, 5);
    auto splitParam = Scalar(Allocate<int>(utilities::ScalarLayout));
    splitParam = 5;

    Index i("i");
    LoopNest loop({ { i, { 0, 4 * 5 } } });
    auto [i_outer, temp] = loop.Split(i, 10);
    auto [i_middle, i_inner] = loop.Split(i, 5);

    auto kernel1 = Kernel("set_vector")
                       .Inputs(vector.GetValue())
                       .Indices(i)
                       .Define(set_vector_kernel);

    auto kernel2 = Kernel("reorder_vector")
                       .Inputs(vector.GetValue(), matrix.GetValue(), splitParam.GetValue())
                       .Indices(i, i_outer, temp)
                       .Define(reorder_vector_kernel);

    loop.AddKernel(kernel1, LoopNest::ConstraintType::predicate);
    loop.AddKernel(kernel2, LoopNest::ConstraintType::predicate);

#if 0
    PrintLoops(loop, "SplitIndex_test2");
#endif

    CodeGenerator generator;
    generator.Run(loop);

    return (vector(0) - matrix(0, 0)) + (vector(13) - matrix(2, 3)) + (matrix(3, 2) - (3 * 5 + 2));
}

// Same as SplitIndex_test2, but splitting an outer index
Scalar SplitIndex_test3()
{
    auto vector = MakeVector<int>(4 * 5);
    auto matrix = MakeMatrix<int>(4, 5);
    auto splitParam = Scalar(Allocate<int>(utilities::ScalarLayout));
    splitParam = 5;

    Index i("i");
    LoopNest loop({ { i, { 0, 4 * 5 } } });
    auto [temp, i_inner] = loop.Split(i, 5);
    auto [i_outer, i_middle] = loop.Split(temp, 10);
    loop.SetLoopOrder({ i_outer, i_middle, i_inner });
    auto kernel1 = Kernel("set_vector")
                       .Inputs(vector.GetValue())
                       .Indices(i)
                       .Define(set_vector_kernel);

    auto kernel2 = Kernel("reorder_vector")
                       .Inputs(vector.GetValue(), matrix.GetValue(), splitParam.GetValue())
                       .Indices(i, temp, i_inner)
                       .Define(reorder_vector_kernel);

    loop.AddKernel(kernel1, LoopNest::ConstraintType::predicate);
    loop.AddKernel(kernel2, LoopNest::ConstraintType::predicate);

#if 0
    PrintLoops(loop, "SplitIndex_test3");
#endif

    CodeGenerator generator;
    generator.Run(loop);

    return (vector(0) - matrix(0, 0)) + (vector(13) - matrix(2, 3)) + (matrix(3, 2) - (3 * 5 + 2));
}

Scalar EpilogueIndex_test()
{
    const int N = 8;
    auto vector = MakeVector<int>(N);

    Index i("i");
    LoopNest loop({ { i, { 0, N } } });
    auto [i_outer, i_inner] = loop.Split(i, 4);

    auto prologueKernel = Kernel("prologue")
                              .Inputs(vector.GetValue())
                              .Indices(i)
                              .Define([](Vector v, Scalar i) {
                                  v[i] = i;
                              });
    auto bodyKernel = Kernel("body")
                          .Inputs(vector.GetValue())
                          .Indices(i)
                          .Define([](Vector v, Scalar i) {
                              v[i] += 10;
                          });
    auto epilogueKernel = Kernel("epilogue")
                              .Inputs(vector.GetValue())
                              .Indices(i)
                              .Define([](Vector v, Scalar i) {
                                  v[i] += 1;
                              });

    loopnests::CodePositionConstraints prologueConstraints{ loopnests::LoopFragmentType::prologue, { i_outer }, {} };
    loopnests::CodePositionConstraints epilogueConstraints{ loopnests::LoopFragmentType::epilogue, { i_outer }, {} };
    loop.AddKernel(prologueKernel, prologueConstraints);
    loop.AddKernel(bodyKernel, LoopNest::ConstraintType::constraint);
    loop.AddKernel(epilogueKernel, epilogueConstraints);

#if 0
    PrintLoops(loop, "EpilogueIndex_test");
#endif

    Scalar ok = Allocate<int>(ScalarLayout);
    ok = 1;

    CodeGenerator generator;
    generator.Run(loop);

    std::vector<int> expectedValues{ 10, 10, 10, 11, 14, 10, 10, 11 };
    auto expected = Vector(expectedValues);

    If(
        VerifySame(vector, expected) == 0,
        [&] {
            ok = 0;
        })
        .Else([&] {
            ok = 1;
        });

    return ok;
}

Scalar RenameKernelArg_test()
{
    auto matrix = MakeMatrix<int>(4, 5);
    IndexRange i("i", { 0, 4 }), j("j", { 0, 5 });
    LoopNest loop(std::vector<IndexRange>{ i, j });

    auto kernel = Kernel("kernel")
                      .Inputs(matrix.GetValue())
                      .Indices(i.GetIndex(), j.GetIndex())
                      .Define(loopnest_kernel);

    loop.AddKernel(kernel, LoopNest::ConstraintType::constraint);

    auto newMatrix = MakeMatrix<int>(4, 5);
    auto [outer, inner] = loop.Split(i.GetIndex(), 2);
    loop.RenameVariable(matrix, newMatrix, { inner });

    CodeGenerator generator;
    generator.Run(loop);

    return newMatrix(2, 3) - 19; // will return 0 if calculation is correct
}

Scalar NonInnermostKernel_test1()
{
    auto matrix = MakeMatrix<int>(4, 4);
    ForRange(4, [&](Scalar i) {
        ForRange(4, [&](Scalar j) {
            matrix(i, j) = i - j;
        });
    });

    // matrix:
    //  0  -1  -2  -3
    //  1   0  -1  -2
    //  2   1   0  -1
    //  3   2   1   0

    // result with 1x1 only:
    //  1   0  -1  -2
    //  2   1   0  -1
    //  3   2   1   0
    //  4   3   2   1

    // (correct) result with 2x2 only ("before"):
    //  2  -1   0  -3
    //  1   0  -1  -2
    //  4   1   2  -1
    //  3   2   1   0

    // result with 2x2 invoked at the outer level, and 1x1 invoked inside ("first"):
    //  2   0   0  -2
    //  2   1   0  -1
    //  4   2   2   0
    //  4   3   2   1

    Index i("i"), j("j");
    LoopNest loop({ { i, { 0, 4 } },
                    { j, { 0, 4 } } });

    auto kernel1x1 = Kernel("kernel_1x1")
                         .Inputs(matrix.GetValue())
                         .Indices(i, j)
                         .Define(addOne);

    auto [iOuter, iInner] = loop.Split(i, 2);
    auto [jOuter, jInner] = loop.Split(j, 2);

    auto kernel2x2 = Kernel("kernel_2x2", kernel1x1.GetId())
                         .Inputs(matrix.GetValue())
                         .Indices(iOuter, jOuter)
                         .Define(addTwo);
    loop.AddKernel(kernel2x2, LoopNest::ConstraintType::constraint);
    loop.AddKernel(kernel1x1, LoopNest::ConstraintType::constraint);

#if 0
    PrintLoops(loop, "NonInnermostKernel_test1");
#endif

    CodeGenerator generator;
    generator.Run(loop);

#if 0 // DEBUGGING
    InvokeForContext<ComputeContext>([&](auto&) {
        ForRange(4, [&](Scalar i) {
            ForRange(4, [&](Scalar j) {
                auto val = matrix(i, j).Get<int>();
                Log() << std::setw(4) << val;
            });
            Log() << EOL;
        });
    });
#endif
    return matrix(1, 1).Copy(); // will return 0 if calculation is correct
}

Scalar NonInnermostKernel_test2()
{
    auto matrix = MakeMatrix<int>(4, 4, "matrix");
    auto expected = MakeMatrix<int>(4, 4, "matrix");
    ForRange(4, [&](Scalar i) {
        ForRange(4, [&](Scalar j) {
            matrix(i, j) = i - j;
            expected(i, j) = i - j;
        });
    });
    ForRange(2, [&](Scalar i) {
        ForRange(2, [&](Scalar j) {
            expected(2 * i, 2 * j) += 2;
        });
    });

    // matrix:
    //  0  -1  -2  -3
    //  1   0  -1  -2
    //  2   1   0  -1
    //  3   2   1   0

    // result with 1x1:
    //  1   0  -1  -2
    //  2   1   0  -1
    //  3   2   1   0
    //  4   3   2   1

    // result with 2x2:
    //  2  -1   0  -3
    //  1   0  -1  -2
    //  4   1   2  -1
    //  3   2   1   0

    Index i("i"), j("j");
    LoopNest loop({ { i, { 0, 4 } },
                    { j, { 0, 4 } } });

    auto kernel1x1 = Kernel("kernel_1x1")
                         .Inputs(matrix.GetValue())
                         .Indices(i, j)
                         .Define(addOne);

    auto [iOuter, iInner] = loop.Split(i, 2);
    auto [jOuter, jInner] = loop.Split(j, 2);

    auto kernel2x2 = Kernel("kernel_2x2", kernel1x1.GetId())
                         .Inputs(matrix.GetValue())
                         .Indices(iOuter, jOuter)
                         .Define(addTwo);

    CodePositionConstraints bodyConstraint{ LoopFragmentType::body, { iOuter, jOuter }, {} };
    loop.AddKernel(kernel2x2, bodyConstraint);
    loop.AddKernel(kernel1x1, LoopNest::ConstraintType::constraint);

#if 0
    PrintLoops(loop, "NonInnermostKernel_test2");
#endif

    CodeGenerator generator;
    generator.Run(loop);

#if 0 // DEBUGGING
    InvokeForContext<ComputeContext>([&](auto&) {
        ForRange(4, [&](Scalar i) {
            ForRange(4, [&](Scalar j) {
                auto val = matrix(i, j).Get<int>();
                Log() << std::setw(4) << val;
            });
            Log() << EOL;
        });
    });
#endif

    Scalar ok = Allocate<int>(ScalarLayout);
    If(
        VerifySame(matrix, expected) == 0,
        [&] {
            ok = 0;
        })
        .Else([&] {
            ok = 1;
        });

    return matrix(1, 1).Copy(); // will return 0 if calculation is correct
}

Scalar NonInnermostKernel_test3()
{
    auto matrix = MakeMatrix<int>(4, 4, "matrix");
    ForRange(4, [&](Scalar i) {
        ForRange(4, [&](Scalar j) {
            matrix(i, j) = i - j;
        });
    });

    // matrix:
    //  0  -1  -2  -3
    //  1   0  -1  -2
    //  2   1   0  -1
    //  3   2   1   0

    // result with 1x1:
    //  1   0  -1  -2
    //  2   1   0  -1
    //  3   2   1   0
    //  4   3   2   1

    // result with 2x2:
    //  2  -1   0  -3
    //  1   0  -1  -2
    //  4   1   2  -1
    //  3   2   1   0

    Index i("i"), j("j");
    LoopNest loop({ { i, { 0, 4 } },
                    { j, { 0, 4 } } });

    auto kernel1x1 = Kernel("kernel_1x1")
                         .Inputs(matrix.GetValue())
                         .Indices(i, j)
                         .Define(addOne);

    auto [iOuter, iInner] = loop.Split(i, 2);
    auto [jOuter, jInner] = loop.Split(j, 2);

    auto kernel2x2 = Kernel("kernel_2x2", kernel1x1.GetId())
                         .Inputs(matrix.GetValue())
                         .Indices(iOuter, jOuter)
                         .Define(addTwo);

    loop.AddKernel(kernel2x2, LoopFragmentType::body);
    loop.AddKernel(kernel1x1, LoopFragmentType::body);

#if 0
    PrintLoops(loop, "NonInnermostKernel_test3");
#endif

    CodeGenerator generator;
    generator.Run(loop);

#if 0 // DEBUGGING
    InvokeForContext<ComputeContext>([&](auto&) {
        ForRange(4, [&](Scalar i) {
            ForRange(4, [&](Scalar j) {
                auto val = matrix(i, j).Get<int>();
                Log() << std::setw(4) << val;
            });
            Log() << EOL;
        });
    });
#endif
    return matrix(1, 1).Copy(); // will return 0 if calculation is correct
}

Scalar NonInnermostKernel_test4()
{
    auto matrix = MakeMatrix<int>(4, 4);
    ForRange(4, [&](Scalar i) {
        ForRange(4, [&](Scalar j) {
            matrix(i, j) = i - j;
        });
    });

    // matrix:
    //  0  -1  -2  -3
    //  1   0  -1  -2
    //  2   1   0  -1
    //  3   2   1   0

    // result with 1x1 only:
    //  1   0  -1  -2
    //  2   1   0  -1
    //  3   2   1   0
    //  4   3   2   1

    // result with 2x2 only ("before"):
    //  2  -1   0  -3
    //  1   0  -1  -2
    //  4   1   2  -1
    //  3   2   1   0

    // result with 2x2 invoked at the outer level, and 1x1 invoked inside ("first"):
    //  2   0   0  -2
    //  2   1   0  -1
    //  4   2   2   0
    //  4   3   2   1

    Index i("i"), j("j");
    LoopNest loop({ { i, { 0, 4 } },
                    { j, { 0, 4 } } });

    auto kernel1x1 = Kernel("kernel_1x1")
                         .Inputs(matrix.GetValue())
                         .Indices(i, j)
                         .Define(addOne);

    auto [iOuter, iInner] = loop.Split(i, 2);
    auto [jOuter, jInner] = loop.Split(j, 2);

    auto kernel2x2 = Kernel("kernel_2x2", kernel1x1.GetId())
                         .Inputs(matrix.GetValue())
                         .Indices(iOuter, jOuter)
                         .Define(addTwo);

    loop.AddKernel(kernel1x1, LoopNest::ConstraintType::predicate);
    loop.AddKernel(kernel2x2, {}, IsDefined(iOuter) && IsDefined(jOuter));

#if 0
    PrintLoops(loop, "NonInnermostKernel_test4");
#endif

    CodeGenerator generator;
    generator.Run(loop);

#if 0 // DEBUGGING
    InvokeForContext<ComputeContext>([&](auto&) {
        ForRange(4, [&](Scalar i) {
            ForRange(4, [&](Scalar j) {
                auto val = matrix(i, j).Get<int>();
                Log() << std::setw(4) << val;
            });
            Log() << EOL;
        });
    });
#endif
    return matrix(1, 1).Copy(); // will return 0 if calculation is correct
}

// This test adds 1 to each element in a 4x4 matrix, but does all the work in on a cached piece of the matrix.
// The i and j dimensions are subdivided into 2x2 tiles, then each tile is copied into the cache, operated on,
// and copied back.
// In this version of the test, the cache is the same size as the original matrix. The next test shows a more useful
// scenario, where the cache is the size of a single tile.
Scalar CachedMatrix_test1()
{
    const int N = 4;
    // auto A = MakeMatrix<int>(N, N);
    // A.GetValue().SetName("A"); // BUG(?): this doesn't work because GetValue() returns a copy
    auto A = MakeMatrix<int>(N, N, "A");

    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
        });
    });

    // A:
    // [ 0, -1, -2, -3 ]
    // [ 1,  0, -1, -2 ]
    // [ 2,  1,  0, -1 ]
    // [ 3,  2,  1,  0 ]

    const int cacheSize = 2;
    auto cache = MakeMatrix<int>(N, N, "cache");

    Index i("i"), j("j");
    LoopNest loop({ { i, { 0, N } },
                    { j, { 0, N } } });
    auto computeKernel = Kernel("compute")
                             .Inputs(A.GetValue())
                             .Indices(i, j)
                             .Define(addOne);

    auto [iOuter, iInner] = loop.Split(i, cacheSize);
    auto [jOuter, jInner] = loop.Split(j, cacheSize);

    auto initCacheKernel = Kernel("cache")
                               .Inputs(A.GetValue(), cache.GetValue())
                               .Indices(iOuter, jOuter)
                               .Define(copyToCache);

    auto copybackKernel = Kernel("uncache")
                              .Inputs(A.GetValue(), cache.GetValue())
                              .Indices(iOuter, jOuter)
                              .Define(copyFromCache);

    // inside iInner, jInner loop (and inside them), "cache" is used instead of "A"
    loop.RenameVariable(A, cache, { iInner, jInner });

    loop.AddKernel(initCacheKernel, LoopFragmentType::prologue);
    loop.AddKernel(computeKernel, LoopNest::ConstraintType::constraint);
    loop.AddKernel(copybackKernel, LoopFragmentType::epilogue);

    CodeGenerator generator;
    generator.Run(loop);

#if 0 // DEBUGGING
    PrintLoops(loop, "CachedMatrix_test1");

    InvokeForContext<ComputeContext>([&](auto&) {
        ForRange(N, [&](Scalar i) {
            ForRange(N, [&](Scalar j) {
                auto val = A(i, j).Get<int>();
                Log() << std::setw(4) << val;
            });
            Log() << EOL;
        });
    });
#endif
    return A(2, 0) + A(0, 2) - 2; // will return 0 if calculation is correct
}

// This test adds 1 to each element in a 4x4 matrix, but does all the work in on a cached piece of the matrix.
// The i and j dimensions are subdivided into 2x2 tiles, then each tile is copied into the cache, operated on,
// and copied back.
// In this version of the test, the cache is the same size as the original matrix. The next test shows a more useful
// scenario, where the cache is the size of a single tile.
Scalar CachedMatrix_test1_new()
{
    const int N = 4;
    // auto A = MakeMatrix<int>(N, N);
    // A.GetValue().SetName("A"); // BUG(?): this doesn't work because GetValue() returns a copy
    auto A = MakeMatrix<int>(N, N, "A");

    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
        });
    });

    // A:
    // [ 0, -1, -2, -3 ]
    // [ 1,  0, -1, -2 ]
    // [ 2,  1,  0, -1 ]
    // [ 3,  2,  1,  0 ]

    const int cacheSize = 2;
    auto cache = MakeMatrix<int>(N, N, "cache");

    Index i("i"), j("j");
    LoopNest loop({ { i, { 0, N } },
                    { j, { 0, N } } });
    auto computeKernel = Kernel("compute")
                             .Inputs(A.GetValue())
                             .Indices(i, j)
                             .Define(addOne);

    auto [iOuter, iInner] = loop.Split(i, cacheSize);
    auto [jOuter, jInner] = loop.Split(j, cacheSize);

    auto initCacheKernel = Kernel("cache")
                               .Inputs(A.GetValue(), cache.GetValue())
                               .Indices(iOuter, jOuter)
                               .Define(copyToCache);

    auto copybackKernel = Kernel("uncache")
                              .Inputs(A.GetValue(), cache.GetValue())
                              .Indices(iOuter, jOuter)
                              .Define(copyFromCache);

    // inside iInner, jInner loop (and inside them), "cache" is used instead of "A"
    loop.RenameVariable(A, cache, { iInner, jInner });

    // loop.AddKernel(initCacheKernel, {}, Before(iInner) || Before(jInner));
    loop.AddKernel(initCacheKernel, {}, Before(iInner));
    loop.AddKernel(computeKernel, LoopNest::ConstraintType::predicate);
    loop.AddKernel(copybackKernel, {}, After(iInner) || After(jInner));

    CodeGenerator generator;
    generator.Run(loop);

#if 0 // DEBUGGING
    PrintLoops(loop, "CachedMatrix_test1_new");
#endif

#if 0 // DEBUGGING
    InvokeForContext<ComputeContext>([&](auto&) {
        ForRange(N, [&](Scalar i) {
            ForRange(N, [&](Scalar j) {
                auto val = A(i, j).Get<int>();
                Log() << std::setw(4) << val;
            });
            Log() << EOL;
        });
    });
#endif
    return A(2, 0) + A(0, 2) - 2; // will return 0 if calculation is correct
}

// This test adds 1 to each element in a 4x4 matrix, but does all the work in on a cached piece of the matrix.
// The i and j dimensions are subdivided into 2x2 tiles, then each tile is copied into the cache, operated on,
// and copied back.
// The cache here is a 2x2 matrix that gets reused for each tile. In this version of the test, we need to add the
// compute kernel after the loops are split, so that we can have it use the inner tile indices instead of
// the full matrix indices.
Scalar CachedMatrix_test2()
{
    // Create the 'A' matrix
    const int N = 4;
    auto A = MakeMatrix<int>(N, N, "A");

    // Create the small cache matrix
    const int cacheSize = 2;
    auto cache = MakeMatrix<int>(cacheSize, cacheSize, "cache");

    // Initialize A to this matrix:
    // [ 0, -1, -2, -3 ]
    // [ 1,  0, -1, -2 ]
    // [ 2,  1,  0, -1 ]
    // [ 3,  2,  1,  0 ]
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
        });
    });

    // Create a loop nest to iterate over A's domain
    Index i("i"), j("j");
    LoopNest loop({ { i, { 0, N } },
                    { j, { 0, N } } });

    // Split the loops into tiles the size of the cache
    auto [iOuter, iInner] = loop.Split(i, cacheSize);
    auto [jOuter, jInner] = loop.Split(j, cacheSize);

    // Tell the loop nest that kernels that run on the individual tiles should use 'cache' in place of 'A'
    loop.RenameVariable(A, cache, { iInner, jInner });

    // Add the code to initialize the cache with a tile of 'A'
    auto initCacheKernel = Kernel("init")
                               .Inputs(A.GetValue(), cache.GetValue())
                               .Indices(iOuter, jOuter)
                               .Define(copyToSmallCache);

    // We use "prologue" as the placement to tell the system to run this kernel before the inner tile loops
    loop.AddKernel(initCacheKernel, LoopFragmentType::prologue);

    // Add the compute kernel, using the inner, tile-relative indices
    auto kernel = Kernel("kernel")
                      .Inputs(A.GetValue())
                      .Indices(iInner, jInner)
                      .Define(addOne);
    loop.AddKernel(kernel, LoopNest::ConstraintType::constraint);

    // ...and the code to copy the processed data back from the kernel into 'A'
    auto copybackKernel = Kernel("copyback")
                              .Inputs(A.GetValue(), cache.GetValue())
                              .Indices(iOuter, jOuter)
                              .Define(copyFromSmallCache);

    // Here, we use "epilogue" as the placement to tell the system to run this kernel after the inner tile loops
    loop.AddKernel(copybackKernel, LoopFragmentType::epilogue);

    CodeGenerator generator;
    generator.Run(loop);

#if 0 // DEBUGGING
    InvokeForContext<ComputeContext>([&](auto&) {
        ForRange(N, [&](Scalar i) {
            ForRange(N, [&](Scalar j) {
                auto val = A(i, j).Get<int>();
                Log() << std::setw(4) << val;
            });
            Log() << EOL;
        });
    });
#endif
    return A(2, 0) + A(0, 2) - 2; // will return 0 if calculation is correct
}

// This test adds 1 to each element in a 4x4 matrix, but does all the work in on a cached piece of the matrix.
// The i and j dimensions are subdivided into 2x2 tiles, then each tile is copied into the cache, operated on,
// and copied back.
// The cache here is a 2x2 matrix that gets reused for each tile. In this version of the test, we need to add the
// compute kernel after the loops are split, so that we can have it use the inner tile indices instead of
// the full matrix indices.
//
// This version differs from CachedMatrix_test2 only in how the cached matrix is given to the kernel
Scalar CachedMatrix_test3()
{
    const int N = 4;

    // Create and initialize the 'A' matrix
    auto A = MakeMatrix<int>(N, N, "A");
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
        });
    });

    // Create the small cache matrix
    const int cacheSize = 2;
    auto cache = MakeMatrix<int>(cacheSize, cacheSize, "cache");

    // Create a loop nest to iterate over A's domain
    Index i("i"), j("j");
    LoopNest loop({ { i, { 0, N } },
                    { j, { 0, N } } });

    // Split the loops into tiles the size of the cache
    auto [iOuter, iInner] = loop.Split(i, cacheSize);
    auto [jOuter, jInner] = loop.Split(j, cacheSize);

    // Add the code to initialize the cache with a tile of 'A'
    auto initCacheKernel = Kernel("init")
                               .Inputs(A.GetValue(), cache.GetValue())
                               .Indices(iOuter, jOuter)
                               .Define(copyToSmallCache);

    // We use "prologue" as the placement to tell the system to run this kernel before the inner tile loops
    loop.AddKernel(initCacheKernel, LoopFragmentType::prologue);

    // Add the compute kernel, using the inner, tile-relative indices, and the cached matrix
    auto kernel = Kernel("kernel")
                      .Inputs(cache.GetValue())
                      .Indices(iInner, jInner)
                      .Define(addOne);
    loop.AddKernel(kernel, LoopNest::ConstraintType::constraint);

    // ...and the code to copy the processed data back from the kernel into 'A'
    auto copybackKernel = Kernel("copyback")
                              .Inputs(A.GetValue(), cache.GetValue())
                              .Indices(iOuter, jOuter)
                              .Define(copyFromSmallCache);

    // Here, we use "epilogue" as the placement to tell the system to run this kernel after the inner tile loops
    loop.AddKernel(copybackKernel, LoopFragmentType::epilogue);

    CodeGenerator generator;
    generator.Run(loop);

#if 0 // DEBUGGING
    InvokeForContext<ComputeContext>([&](auto&) {
        ForRange(N, [&](Scalar i) {
            ForRange(N, [&](Scalar j) {
                auto val = A(i, j).Get<int>();
                Log() << std::setw(4) << val;
            });
            Log() << EOL;
        });
    });
#endif
    return A(2, 0) + A(0, 2) - 2; // will return 0 if calculation is correct
}

// This test does an element-wise sum of two 4x4 matrices, storing the result in the left matrix,
// where the left matrix is cached in 2x2 tiles and the right matrix is not.
// The i and j dimensions are subdivided into 2x2 tiles, then each tile of the left matrix is copied into the cache,
// operated on with the right matrix, and copied back.
// The cache here is a 2x2 matrix that gets reused for each tile.
// In this version, we need to pass in both the split indices and the global indices into the kernel since one matrix
// is a cache using the split indices, while the other is uncached and needs the global indices
Scalar CachedMatrix_test4()
{
    const int N = 4;
    const int cacheSize = 2;
    auto A = MakeMatrix<int>(N, N, "A");
    auto B = MakeMatrix<int>(N, N, "B");
    auto expected = MakeMatrix<int>(N, N);
    auto cacheA = MakeMatrix<int>(cacheSize, cacheSize, "cacheA");

    // Initialize the 'A' matrix
    // A:
    // [ 0, -1, -2, -3 ]
    // [ 1,  0, -1, -2 ]
    // [ 2,  1,  0, -1 ]
    // [ 3,  2,  1,  0 ]
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
        });
    });

    // Initialize the 'B' matrix
    // B:
    // [ 0,  1,  2,  3 ]
    // [ 1,  2,  3,  4 ]
    // [ 2,  3,  4,  5 ]
    // [ 3,  4,  5,  6 ]
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            B(i, j) = i + j;
        });
    });

    // Initialize the 'expected' matrix = A + B
    // A + B:
    // [ 0,  0,  0,  0 ]
    // [ 2,  2,  2,  2 ]
    // [ 4,  4,  4,  4 ]
    // [ 6,  6,  6,  6 ]
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            expected(i, j) = (i - j) + (i + j);
        });
    });

    // Create a loop nest to iterate over A's and B's domains
    Index i("i"), j("j");
    LoopNest loop({ { i, { 0, N } },
                    { j, { 0, N } } });

    // Split the loops into tiles the size of the cache
    auto [iOuter, iInner] = loop.Split(i, cacheSize);
    auto [jOuter, jInner] = loop.Split(j, cacheSize);

    // Add the code to initialize the cache with a tile of 'A'
    auto initCacheKernel = Kernel("init")
                               .Inputs(A.GetValue(), cacheA.GetValue())
                               .Indices(iOuter, jOuter)
                               .Define(copyToSmallCache);

    // We use "prologue" as the placement to tell the system to run this kernel before the inner tile loops
    loop.AddKernel(initCacheKernel, LoopFragmentType::prologue);

    // Add the compute kernel, using the inner, tile-relative indices, and the cached matrix
    auto kernel = Kernel("kernel")
                      .Inputs(cacheA.GetValue(), B.GetValue())
                      .Indices(iInner, jInner, i, j)
                      .Define(addCachedMatrixToUnchachedMatrix);
    CodePositionConstraints constraints{ LoopFragmentType::body, { iInner, jInner }, {} };
    loop.AddKernel(kernel, constraints);

    // ...and the code to copy the processed data back from the kernel into 'A'
    auto copybackKernel = Kernel("copyback")
                              .Inputs(A.GetValue(), cacheA.GetValue())
                              .Indices(iOuter, jOuter)
                              .Define(copyFromSmallCache);

    // Here, we use "epilogue" as the placement to tell the system to run this kernel after the inner tile loops
    loop.AddKernel(copybackKernel, LoopFragmentType::epilogue);

    CodeGenerator generator;
    generator.Run(loop);

    Scalar ok = Allocate<int>(ScalarLayout);
    ok = 1;
    If(
        VerifySame(A, expected) == 0,
        [&] {
            ok = 0;
        })
        .Else([&] {
            auto value = A.GetValue();
            value.SetLayout({ { (int)A.Size() } });
            DebugPrintVector(value);
            DebugPrint("\n");
        });
    return ok;
}

// This test does an element-wise sum of two 4x4 matrices, storing the result in the left matrix,
// where the left matrix is cached in 2x2 tiles and the right matrix is not.
// The i and j dimensions are subdivided into 2x2 tiles, then each tile of the left matrix is copied into the cache,
// operated on with the right matrix, and copied back.
// The cache here is a 2x2 matrix that gets reused for each tile.
// In this version, we need to pass in both the split indices and the global indices into the kernel since one matrix
// is a cache using the split indices, while the other is uncached and needs the global indices
//
// The difference with the previous test is that the kernel is unrolled and operates on a panel rather than individual indices
Scalar CachedMatrix_test5()
{
    const int N = 4;
    const int cacheSize = 2;
    auto A = MakeMatrix<int>(N, N, "A");
    auto B = MakeMatrix<int>(N, N, "B");
    auto expected = MakeMatrix<int>(N, N);
    auto cacheA = MakeMatrix<int>(cacheSize, cacheSize, "cacheA");

    // Initialize the 'A' matrix
    // A:
    // [ 0, -1, -2, -3 ]
    // [ 1,  0, -1, -2 ]
    // [ 2,  1,  0, -1 ]
    // [ 3,  2,  1,  0 ]
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
        });
    });

    // Initialize the 'B' matrix
    // B:
    // [ 0,  1,  2,  3 ]
    // [ 1,  2,  3,  4 ]
    // [ 2,  3,  4,  5 ]
    // [ 3,  4,  5,  6 ]
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            B(i, j) = i + j;
        });
    });

    // A + B:
    // [ 0,  0,  0,  0 ]
    // [ 2,  2,  2,  2 ]
    // [ 4,  4,  4,  4 ]
    // [ 6,  6,  6,  6 ]
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            expected(i, j) = i * 2;
        });
    });

    // Create a loop nest to iterate over A's and B's domains
    Index i("i"), j("j");
    LoopNest loop({ { i, { 0, N } },
                    { j, { 0, N } } });

    // Split the loops into tiles the size of the cache
    auto [iOuter, iInner] = loop.Split(i, cacheSize);
    auto [jOuter, jInner] = loop.Split(j, cacheSize);

    // Add the code to initialize the cache with a tile of 'A'
    auto initCacheKernel = Kernel("init")
                               .Inputs(A.GetValue(), cacheA.GetValue())
                               .Indices(iOuter, jOuter)
                               .Define(copyToSmallCache);

    // We use "prologue" as the placement to tell the system to run this kernel before the inner tile loops
    loop.AddKernel(initCacheKernel, LoopFragmentType::prologue);

    // Add the compute kernel, using the inner, tile-relative indices, and the cached matrix
    auto kernel = Kernel("kernel")
                      .Inputs(cacheA.GetValue(), B.GetValue())
                      .Indices(iOuter, jOuter, i, j)
                      .Define(addCachedMatrixToUnchachedMatrixUnrolled);
    CodePositionConstraints constraints{ LoopFragmentType::body, { iOuter, jOuter }, {} };
    loop.AddKernel(kernel, constraints);

    // ...and the code to copy the processed data back from the kernel into 'A'
    auto copybackKernel = Kernel("copyback")
                              .Inputs(A.GetValue(), cacheA.GetValue())
                              .Indices(iOuter, jOuter)
                              .Define(copyFromSmallCache);

    // Here, we use "epilogue" as the placement to tell the system to run this kernel after the inner tile loops
    loop.AddKernel(copybackKernel, LoopFragmentType::epilogue);

    CodeGenerator generator;
    generator.Run(loop);

    Scalar ok = Allocate<int>(ScalarLayout);
    ok = 1;
    If(
        VerifySame(A, expected) == 0,
        [&] {
            ok = 0;
        })
        .Else([&] {
            auto value = A.GetValue();
            value.SetLayout({ { (int)A.Size() } });
            DebugPrintVector(value);
            DebugPrint("\n");
        });
    return ok;
}

Scalar LoopNest_Parallelized_test1()
{
    Scalar ok = Allocate<int>(ScalarLayout);
    auto matrix = MakeMatrix<int>(4, 5);
    InvokeForContext<LLVMContext>([&] {
        auto v = matrix.GetValue().Get<Emittable>().GetDataAs<LLVMValue>();
        v->setName("matrix");
    });
    loopnests::IndexRange i("i", { 0, 4 }), j("j", { 0, 5 });

    auto kernel = loopnests::Kernel("kernel")
                      .Inputs(matrix.GetValue())
                      .Indices(i.GetIndex(), j.GetIndex())
                      .Define([](Matrix m, Scalar i, Scalar j) {
                          Scalar tid = GetTID();
#if 0 // Useful to turn off/on for debugging
                          InvokeForContext<ComputeContext>([&](auto&) {
                              auto iInt = i.Get<int>();
                              auto jInt = j.Get<int>();
                              Log() << "m(" << iInt << ", " << jInt << ") = " << (iInt * 2 + jInt * 5)
                                    << " [Thread " << tid.Get<int>() << "]"
                                    << EOL;
                          });
#endif // 1
                          m(i, j) = i * 2 + j * 5;
                      });

    loopnests::LoopNest loop(std::vector<loopnests::IndexRange>{ i, j });
    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);
    loop.Parallelize(i.GetIndex(), 2);

    loopnests::CodeGenerator generator;
    generator.Run(loop);

    ok = matrix(2, 3) - 19;
    return ok; // will return 0 if calculation is correct
}

Scalar LoopNest_Parallelized_test2()
{
    Scalar ok = Allocate<int>(ScalarLayout);

    auto matrix = MakeMatrix<int>(4, 5);
    loopnests::IndexRange i("i", { 0, 4 }), j("j", { 0, 5 });

    auto kernel = loopnests::Kernel("kernel")
                      .Inputs(matrix.GetValue())
                      .Indices(i.GetIndex(), j.GetIndex())
                      .Define([](Matrix m, Scalar i, Scalar j) {
                          Scalar tid = GetTID();
#if 0 // Useful to turn off/on for debugging
                          InvokeForContext<ComputeContext>([&](auto&) {
                              auto iInt = i.Get<int>();
                              auto jInt = j.Get<int>();
                              Log() << "m(" << iInt << ", " << jInt << ") = " << tid.Get<int>()
                                    << " [Thread " << tid.Get<int>() << "]" << EOL;
                          });
#endif // 1
                          m(i, j) = tid;
                      });

    loopnests::LoopNest loop(std::vector<loopnests::IndexRange>{ i, j });
    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);
    loop.Parallelize(i.GetIndex(), 2);

    loopnests::CodeGenerator generator;
    generator.Run(loop);

    auto expected = MakeMatrix<int>(4, 5);
    If(
        VerifySame(matrix, expected) == 0,
        [&] {
            ok = 1;
        })
        .Else([&] {
            auto value = matrix.GetValue();
            value.SetLayout({ { (int)matrix.Size() } });
            DebugPrintVector(value);
            DebugPrint("\n");
        });
    return ok;
}

Scalar LoopNest_Unrolled_test1()
{
    auto matrix = MakeMatrix<int>(20, 5);
    IndexRange i("i", { 0, 20 }), j("j", { 0, 5 });

    auto kernel = Kernel("kernel")
                      .Inputs(matrix.GetValue())
                      .Indices(i.GetIndex(), j.GetIndex())
                      .Define(loopnest_kernel);

    LoopNest loop(std::vector<IndexRange>{ i, j });
    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);
    loop.Unroll(j.GetIndex());

    CodeGenerator generator;
    generator.Run(loop);

    return matrix(2, 3) - 19; // will return 0 if calculation is correct
}

Scalar LoopNest_DebugDump_test1()
{
    auto matrix = MakeMatrix<int>(4, 5);
    Index i("i"), j("j");
    LoopNest loop({ { i, { 0, 4 } },
                    { j, { 0, 5 } } });

    auto kernel = Kernel("kernel")
                      .Inputs(matrix.GetValue())
                      .Indices(i, j)
                      .Define(loopnest_kernel);

    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);
    loop.Split(i, 2);
    loop.Unroll(j);
    loop.SetLoopOrder({ i, j, i });

    InvokeForContext<ComputeContext>([&](auto&) {
        std::stringstream ss;
        DebugDump(loop, "DebugDump test", &ss);
        Log() << ss.str() << EOL;
        // TODO: verify somehow that the printing worked
    });

    return 0;
}

Scalar LoopNest_DebugDump_test2()
{
    const int N = 8;
    auto matrix = MakeMatrix<int>(N, N);
    Index i("i"), j("j");
    LoopNest loop({ { i, { 0, N } },
                    { j, { 0, N } } });

    auto kernel = Kernel("kernel")
                      .Inputs(matrix.GetValue())
                      .Indices(i, j)
                      .Define(loopnest_kernel);

    loop.AddKernel(kernel, LoopNest::ConstraintType::predicate);
    SplitAndSetOrder(loop, { i, j }, { 4, 2 }, "ijij");

    InvokeForContext<ComputeContext>([&](auto&) {
        std::stringstream ss;
        DebugDump(loop, "DebugDump test", &ss);
        Log() << ss.str() << EOL;
        // TODO: verify somehow that the printing worked
    });

    return 0;
}

Scalar SimpleMatMult_test()
{
    const int N = 8;
    auto A = MakeMatrix<int>(N, N, "A");
    auto B = MakeMatrix<int>(N, N, "B");
    auto C = MakeMatrix<int>(N, N, "C");
    auto expected = MakeMatrix<int>(N, N, "expected");

    // initialize A, B, and C
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
            B(i, j) = i + 2 * j;
            C(i, j) = 100;
        });
    });

    // fill out expected with a simple for-loop gemm
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            ForRange(N, [&](Scalar k) {
                expected(i, j) += A(i, k) * B(k, j);
            });
        });
    });

    // The input matrices:
    // A:                                 B:                                 C:
    // [ 0, -1, -2, -3, -4, -5, -6, -7]   [ 0,  2,  4,  6,  8, 10, 12, 14]   [ 100 100 100 ... ]
    // [ 1,  0, -1, -2, -3, -4, -5, -6]   [ 1,  3,  5,  7,  9, 11, 13, 15]   [ 100 100 100 ... ]
    // [ 2,  1,  0, -1, -2, -3, -4, -5]   [ 2,  4,  6,  8, 10, 12, 14, 16]   [      ...        ]
    // [ 3,  2,  1,  0, -1, -2, -3, -4]   [ 3,  5,  7,  9, 11, 13, 15, 17]   [      ...        ]
    // [ 4,  3,  2,  1,  0, -1, -2, -3]   [ 4,  6,  8, 10, 12, 14, 16, 18]   [      ...        ]
    // [ 5,  4,  3,  2,  1,  0, -1, -2]   [ 5,  7,  9, 11, 13, 15, 17, 19]   [      ...        ]
    // [ 6,  5,  4,  3,  2,  1,  0, -1]   [ 6,  8, 10, 12, 14, 16, 18, 20]   [      ...        ]
    // [ 7,  6,  5,  4,  3,  2,  1,  0]   [ 7,  9, 11, 13, 15, 17, 19, 21]   [      ...        ]

    // (A * B) (the desired result):
    // [-140, -196, -252, -308, -364, -420, -476, -532]
    // [-112, -152, -192, -232, -272, -312, -352, -392]
    // [ -84, -108, -132, -156, -180, -204, -228, -252]
    // [ -56,  -64,  -72,  -80,  -88,  -96, -104, -112]
    // [ -28,  -20,  -12,   -4,    4,   12,   20,   28]
    // [   0,   24,   48,   72,   96,  120,  144,  168]
    // [  28,   68,  108,  148,  188,  228,  268,  308]
    // [  56,  112,  168,  224,  280,  336,  392,  448]

    Index i("i"), j("j"), k("k");

    LoopNest loop({ { i, { 0, N } },
                    { j, { 0, N } },
                    { k, { 0, N } } });

    int cacheARows = 4;
    int cacheACols = 4;
    int resultCacheRows = 2;
    int resultCacheCols = 2;

    auto [i_panel_outer, i_panel_inner] = loop.Split(i, cacheARows);
    auto [k_panel_outer, k_panel_inner] = loop.Split(k, cacheACols);
    auto [i_kernel_outer, i_kernel_inner] = loop.Split(i, resultCacheRows);
    auto [j_kernel_outer, j_kernel_inner] = loop.Split(j, resultCacheCols);

    auto initCKernel = Kernel("init")
                           .Inputs(C.GetValue())
                           .Indices(i, j)
                           .Define([](Matrix C, Scalar i, Scalar j) {
                               C(i, j) = 0;
                           });

    auto innerKernel = Kernel("matmul")
                           .Inputs(A.GetValue(), B.GetValue(), C.GetValue())
                           .Indices(i, j, k)
                           .Define([](Matrix A, Matrix B, Matrix C, Scalar i, Scalar j, Scalar k) {
                               C(i, j) += A(i, k) * B(k, j);
                           });

    loopnests::CodePositionConstraints initConstraints{ loopnests::LoopFragmentType::prologue, { i, j }, { k } };
    loop.AddKernel(initCKernel, initConstraints);
    loop.AddKernel(innerKernel, LoopNest::ConstraintType::constraint);

    loop.SetLoopOrder({ k, i, j, i, k, j, i });

    CodeGenerator generator;
    generator.Run(loop);

#if 0
    PrintLoops(loop, "SimpleMatMult_test");
#endif

    Scalar ok = Allocate<int>(ScalarLayout);
    ok = 1;
    If(
        VerifySame(C, expected) == 0,
        [&] {
            ok = 0;
        })
        .Else([&] {
            auto value = C.GetValue();
            value.SetLayout({ { (int)C.Size() } });
            DebugPrintVector(value);
            DebugPrint("\n");
            auto expectedValue = expected.GetValue();
            expectedValue.SetLayout({ { (int)expected.Size() } });
            DebugPrintVector(expectedValue);
            DebugPrint("\n");
        });
    return ok;
}

Scalar GotoBLASGemm_LowLevelAPI()
{
    const int N = 8;
    auto A = MakeMatrix<int>(N, N, "A");
    auto B = MakeMatrix<int>(N, N, "B");
    auto C = MakeMatrix<int>(N, N, "C");
    auto expected = MakeMatrix<int>(N, N, "expected");

    // initialize A, B, and C
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
            B(i, j) = i + 2 * j;
        });
    });

    // fill out expected with a simple for-loop gemm
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            ForRange(N, [&](Scalar k) {
                expected(i, j) += A(i, k) * B(k, j);
            });
        });
    });

    // The input matrices:
    // A:                                 B:                                 C:
    // [ 0, -1, -2, -3, -4, -5, -6, -7]   [ 0,  2,  4,  6,  8, 10, 12, 14]   [ 0 0 0 ... ]
    // [ 1,  0, -1, -2, -3, -4, -5, -6]   [ 1,  3,  5,  7,  9, 11, 13, 15]   [ 0 0 0 ... ]
    // [ 2,  1,  0, -1, -2, -3, -4, -5]   [ 2,  4,  6,  8, 10, 12, 14, 16]   [    ...    ]
    // [ 3,  2,  1,  0, -1, -2, -3, -4]   [ 3,  5,  7,  9, 11, 13, 15, 17]   [    ...    ]
    // [ 4,  3,  2,  1,  0, -1, -2, -3]   [ 4,  6,  8, 10, 12, 14, 16, 18]   [    ...    ]
    // [ 5,  4,  3,  2,  1,  0, -1, -2]   [ 5,  7,  9, 11, 13, 15, 17, 19]   [    ...    ]
    // [ 6,  5,  4,  3,  2,  1,  0, -1]   [ 6,  8, 10, 12, 14, 16, 18, 20]   [    ...    ]
    // [ 7,  6,  5,  4,  3,  2,  1,  0]   [ 7,  9, 11, 13, 15, 17, 19, 21]   [    ...    ]

    // (A * B) (the desired result):
    // [-140, -196, -252, -308, -364, -420, -476, -532]
    // [-112, -152, -192, -232, -272, -312, -352, -392]
    // [ -84, -108, -132, -156, -180, -204, -228, -252]
    // [ -56,  -64,  -72,  -80,  -88,  -96, -104, -112]
    // [ -28,  -20,  -12,   -4,    4,   12,   20,   28]
    // [   0,   24,   48,   72,   96,  120,  144,  168]
    // [  28,   68,  108,  148,  188,  228,  268,  308]
    // [  56,  112,  168,  224,  280,  336,  392,  448]

    Index i("i"), j("j"), k("k");

    LoopNest loop({ { i, { 0, N } },
                    { j, { 0, N } },
                    { k, { 0, N } } });

    int cacheARows = 4;
    int cacheACols = 4;
    int cacheBRows = cacheACols;
    int cacheBCols = N;
    int resultCacheRows = 2;
    int resultCacheCols = 2;

    auto [i_panel_outer, i_panel_inner] = loop.Split(i, cacheARows);
    auto [k_panel_outer, k_panel_inner] = loop.Split(k, cacheACols);
    auto [i_kernel_outer, i_kernel_inner] = loop.Split(i, resultCacheRows);
    auto [j_kernel_outer, j_kernel_inner] = loop.Split(j, resultCacheCols);

    auto cachedResult = MakeMatrix<int>(resultCacheRows, resultCacheCols, "cachedResult");

    auto cacheA = MakeMatrix<int>(cacheARows, cacheACols, "cacheA");
    auto transposeCacheB = MakeMatrix<int>(cacheBCols, cacheBRows, "transposeCacheB");

    auto cacheAKernel = Kernel("cacheAKernel")
                            .Inputs(A.GetValue(), cacheA.GetValue())
                            .Indices(i_panel_outer, k_panel_outer)
                            .Define([&](Matrix A, Matrix cache, Scalar iPanel, Scalar kPanel) {
                                for (int i = 0; i < cacheARows; ++i)
                                {
                                    for (int k = 0; k < cacheACols; ++k)
                                    {
                                        cache(i, k) = A(iPanel + i, kPanel + k);
                                    }
                                }
                            });

    auto transposeCacheBKernel = Kernel("transposeCacheBKernel")
                                     .Inputs(B.GetValue(), transposeCacheB.GetValue())
                                     .Indices(k_panel_outer)
                                     .Define([&](Matrix B, Matrix transposeCache, Scalar kPanel) {
                                         for (int k = 0; k < cacheBRows; ++k)
                                         {
                                             for (int j = 0; j < cacheBCols; ++j)
                                             {
                                                 transposeCache(j, k) = B(kPanel + k, j);
                                             }
                                         }
                                     });

    auto innerKernel = Kernel("matmul")
                           .Inputs(cacheA.GetValue(), transposeCacheB.GetValue(), cachedResult.GetValue())
                           .Indices(i, j, k, i_kernel_inner, j_kernel_inner, i_kernel_outer, k_panel_inner)
                           .Define([](Matrix A, Matrix B, Matrix C, Scalar i, Scalar j, Scalar k, Scalar iInner, Scalar jInner, Scalar iOuter, Scalar kPanelInner) {
                               C(iInner, jInner) += A(iOuter + iInner, kPanelInner) * B(j, kPanelInner);
                           });

    auto clearCacheKernel = Kernel("clearCacheKernel")
                                .Inputs(cachedResult.GetValue())
                                .Indices(i_kernel_outer, j_kernel_outer)
                                .Define([&](Matrix cache, Scalar iOuter, Scalar jOuter) {
                                    Scalar zeroValue = Allocate<int>(utilities::ScalarLayout);
                                    for (int i = 0; i < resultCacheRows; ++i)
                                    {
                                        for (int j = 0; j < resultCacheCols; ++j)
                                        {
                                            cache(i, j) = zeroValue;
                                        }
                                    }
                                });

    auto uncacheKernel = Kernel("uncacheKernel")
                             .Inputs(C.GetValue(), cachedResult.GetValue())
                             .Indices(i_panel_outer, i_kernel_outer, j_kernel_outer)
                             .Define([&](Matrix C, Matrix cache, Scalar iPanelOuter, Scalar iOuter, Scalar jOuter) {
                                 for (int i = 0; i < resultCacheRows; ++i)
                                 {
                                     for (int j = 0; j < resultCacheCols; ++j)
                                     {
                                         C(iPanelOuter + iOuter + i, jOuter + j) += cache(i, j);
                                     }
                                 }
                             });

    CodePositionConstraints cacheAConstraint{ LoopFragmentType::prologue, { i_panel_outer, k_panel_outer }, {} };
    loop.AddKernel(cacheAKernel, cacheAConstraint);

    CodePositionConstraints cacheBConstraint{ LoopFragmentType::prologue, { k_panel_outer }, {} };
    loop.AddKernel(transposeCacheBKernel, cacheBConstraint);

    CodePositionConstraints constraint{ LoopFragmentType::body, { k, i_kernel_inner, j_kernel_inner }, {} };
    loop.AddKernel(innerKernel, constraint);

    CodePositionConstraints preConstraint{ LoopFragmentType::prologue, { i_kernel_outer, j_kernel_outer }, {} };
    loop.AddKernel(clearCacheKernel, preConstraint);

    CodePositionConstraints postConstraint{ LoopFragmentType::epilogue, { i_kernel_outer, j_kernel_outer }, {} };
    loop.AddKernel(uncacheKernel, postConstraint);
    loop.SetLoopOrder({ k, i, j, i, k, j, i });

    loop.Unroll(i_kernel_inner);
    loop.Unroll(j_kernel_inner);
    CodeGenerator generator;
    generator.Run(loop);

#if 0
    PrintLoops(loop, "GotoBLASGemm_LowLevelAPI");
#endif

    Scalar ok = Allocate<int>(ScalarLayout);
    ok = 1;
    If(
        VerifySame(C, expected) == 0,
        [&] {
            ok = 0;
        })
        .Else([&] {
            auto value = C.GetValue();
            value.SetLayout({ { (int)C.Size() } });
            DebugPrintVector(value);
            DebugPrint("\n");
            auto expectedValue = expected.GetValue();
            expectedValue.SetLayout({ { (int)expected.Size() } });
            DebugPrintVector(expectedValue);
            DebugPrint("\n");
        });
    return ok;
}

Scalar GotoBLASGemmWithRefDeref()
{
    const int N = 8;
    auto A = MakeMatrix<int>(N, N, "A");
    auto B = MakeMatrix<int>(N, N, "B");
    auto C = MakeMatrix<int>(N, N, "C");
    auto expected = MakeMatrix<int>(N, N, "expected");

    // initialize A, B, and C
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
            B(i, j) = i + 2 * j;
        });
    });

    // fill out expected with a simple for-loop gemm
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            ForRange(N, [&](Scalar k) {
                expected(i, j) += A(i, k) * B(k, j);
            });
        });
    });

    // The input matrices:
    // A:                                 B:                                 C:
    // [ 0, -1, -2, -3, -4, -5, -6, -7]   [ 0,  2,  4,  6,  8, 10, 12, 14]   [ 100 100 100 ... ]
    // [ 1,  0, -1, -2, -3, -4, -5, -6]   [ 1,  3,  5,  7,  9, 11, 13, 15]   [ 100 100 100 ... ]
    // [ 2,  1,  0, -1, -2, -3, -4, -5]   [ 2,  4,  6,  8, 10, 12, 14, 16]   [      ...        ]
    // [ 3,  2,  1,  0, -1, -2, -3, -4]   [ 3,  5,  7,  9, 11, 13, 15, 17]   [      ...        ]
    // [ 4,  3,  2,  1,  0, -1, -2, -3]   [ 4,  6,  8, 10, 12, 14, 16, 18]   [      ...        ]
    // [ 5,  4,  3,  2,  1,  0, -1, -2]   [ 5,  7,  9, 11, 13, 15, 17, 19]   [      ...        ]
    // [ 6,  5,  4,  3,  2,  1,  0, -1]   [ 6,  8, 10, 12, 14, 16, 18, 20]   [      ...        ]
    // [ 7,  6,  5,  4,  3,  2,  1,  0]   [ 7,  9, 11, 13, 15, 17, 19, 21]   [      ...        ]

    // (A * B) (the desired result):
    // [-140, -196, -252, -308, -364, -420, -476, -532]
    // [-112, -152, -192, -232, -272, -312, -352, -392]
    // [ -84, -108, -132, -156, -180, -204, -228, -252]
    // [ -56,  -64,  -72,  -80,  -88,  -96, -104, -112]
    // [ -28,  -20,  -12,   -4,    4,   12,   20,   28]
    // [   0,   24,   48,   72,   96,  120,  144,  168]
    // [  28,   68,  108,  148,  188,  228,  268,  308]
    // [  56,  112,  168,  224,  280,  336,  392,  448]

    Index i("i"), j("j"), k("k");

    loopnests::LoopNest loop({ { i, { 0, N } },
                               { j, { 0, N } },
                               { k, { 0, N } } });

    int cacheARows = 4;
    int cacheACols = 4;
    int cacheBRows = cacheACols;
    int cacheBCols = N;
    int resultCacheRows = 2;
    int resultCacheCols = 2;

    auto [i_panel_outer, i_panel_inner] = loop.Split(i, cacheARows);
    auto [k_panel_outer, k_panel_inner] = loop.Split(k, cacheACols);
    auto [i_kernel_outer, i_kernel_inner] = loop.Split(i, resultCacheRows);
    auto [j_kernel_outer, j_kernel_inner] = loop.Split(j, resultCacheCols);

    auto cachedResult = MakeMatrix<int>(resultCacheRows, resultCacheCols, "cachedResult");

    auto cacheA = MakeMatrix<int>(cacheARows, cacheACols, "cacheA");
    auto cacheARef = cacheA.GetValue().Reference();

    auto transposeCacheB = MakeMatrix<int>(cacheBCols, cacheBRows, "transposeCacheB");

    auto cacheAKernel = loopnests::Kernel("cacheAKernel")
                            .Inputs(A.GetValue(), cacheARef)
                            .Indices(i_panel_outer, k_panel_outer)
                            .Define([cacheARows, cacheACols](Matrix A, Value cacheRef, Scalar iPanel, Scalar kPanel) {
                                auto cache = Matrix(cacheRef.Dereference());
                                for (int i = 0; i < cacheARows; ++i)
                                {
                                    for (int k = 0; k < cacheACols; ++k)
                                    {
                                        cache(i, k) = A(iPanel + i, kPanel + k);
                                    }
                                }
                                // Update cacheRef so that global (i, k) index into the corect spot in the cache
                                auto cacheTmp = cacheRef.Dereference();
                                cacheTmp.SetLayout(MemoryShape{ cacheARows, cacheACols });
                                auto cacheTmpOffset = cacheTmp.Offset({ -1 * iPanel, -1 * kPanel });
                                cacheTmpOffset.SetLayout(MemoryShape{ cacheARows, cacheACols });
                                cacheRef = cacheTmpOffset.Reference();
                            });

    auto resetCacheAKernel = loopnests::Kernel("resetCacheAKernel")
                                 .Inputs(cacheARef)
                                 .Indices(i_panel_outer, k_panel_outer)
                                 .Define([cacheARows, cacheACols](Value cacheRef, Scalar iPanel, Scalar kPanel) {
                                     // Reset cacheRef to point to the cache while we have iPanel and kPanel in hand
                                     auto offsetCache = cacheRef.Dereference();
                                     offsetCache.SetLayout(MemoryShape{ cacheARows, cacheACols });
                                     auto realCache = offsetCache.Offset({ iPanel, kPanel });
                                     realCache.SetLayout(MemoryShape{ cacheARows, cacheACols });
                                     cacheRef = realCache.Reference();
                                 });

    auto transposeCacheBKernel = loopnests::Kernel("transposeCacheBKernel")
                                     .Inputs(B.GetValue(), transposeCacheB.GetValue())
                                     .Indices(k_panel_outer)
                                     .Define([cacheBRows, cacheBCols](Matrix B, Matrix transposeCache, Scalar kPanel) {
                                         for (int k = 0; k < cacheBRows; ++k)
                                         {
                                             for (int j = 0; j < cacheBCols; ++j)
                                             {
                                                 transposeCache(j, k) = B(kPanel + k, j);
                                             }
                                         }
                                     });

    auto innerKernel = loopnests::Kernel("matmul")
                           .Inputs(cacheARef, transposeCacheB.GetValue(), cachedResult.GetValue())
                           .Indices(i, j, k, i_kernel_inner, j_kernel_inner, i_kernel_outer, k_panel_inner, i_panel_outer, k_panel_outer)
                           .Define([cacheARows, cacheACols](Value Aref, Matrix B, Matrix C, Scalar i, Scalar j, Scalar k, Scalar iInner, Scalar jInner, Scalar iOuter, Scalar kPanelInner, Scalar iPanel, Scalar kPanel) {
                               auto offsetA = Aref.Dereference();
                               offsetA.SetLayout(MemoryShape{ cacheARows, cacheACols });
                               auto A = Matrix(offsetA);
                               C(iInner, jInner) += A(i, k) * B(j, kPanelInner);
                           });

    auto clearCacheKernel = loopnests::Kernel("clearCacheKernel")
                                .Inputs(cachedResult.GetValue())
                                .Indices(i_kernel_outer, j_kernel_outer)
                                .Define([resultCacheRows, resultCacheCols](Matrix cache, Scalar iOuter, Scalar jOuter) {
                                    Scalar zeroValue = Allocate<int>(utilities::ScalarLayout);
                                    for (int i = 0; i < resultCacheRows; ++i)
                                    {
                                        for (int j = 0; j < resultCacheCols; ++j)
                                        {
                                            cache(i, j) = zeroValue;
                                        }
                                    }
                                });

    auto uncacheKernel = loopnests::Kernel("uncacheKernel")
                             .Inputs(C.GetValue(), cachedResult.GetValue())
                             .Indices(i_panel_outer, i_kernel_outer, j_kernel_outer)
                             .Define([resultCacheRows, resultCacheCols](Matrix C, Matrix cache, Scalar iPanelOuter, Scalar iOuter, Scalar jOuter) {
                                 for (int i = 0; i < resultCacheRows; ++i)
                                 {
                                     for (int j = 0; j < resultCacheCols; ++j)
                                     {
                                         C(iPanelOuter + iOuter + i, jOuter + j) += cache(i, j);
                                     }
                                 }
                             });

    loopnests::CodePositionConstraints cacheAConstraint{ loopnests::LoopFragmentType::prologue, { i_panel_outer, k_panel_outer }, {} };
    loop.AddKernel(cacheAKernel, cacheAConstraint);

    loopnests::CodePositionConstraints resetCacheAConstraint{ loopnests::LoopFragmentType::epilogue, { i_panel_outer, k_panel_outer }, {} };
    loop.AddKernel(resetCacheAKernel, resetCacheAConstraint);

    loopnests::CodePositionConstraints cacheBConstraint{ loopnests::LoopFragmentType::prologue, { k_panel_outer }, {} };
    loop.AddKernel(transposeCacheBKernel, cacheBConstraint);

    loopnests::CodePositionConstraints constraint{ loopnests::LoopFragmentType::body, { k, i_kernel_inner, j_kernel_inner }, {} };
    loop.AddKernel(innerKernel, constraint);

    loopnests::CodePositionConstraints preConstraint{ loopnests::LoopFragmentType::prologue, { i_kernel_outer, j_kernel_outer }, {} };
    loop.AddKernel(clearCacheKernel, preConstraint);

    loopnests::CodePositionConstraints postConstraint{ loopnests::LoopFragmentType::epilogue, { i_kernel_outer, j_kernel_outer }, {} };
    loop.AddKernel(uncacheKernel, postConstraint);
    loop.SetLoopOrder({ k, i, j, i, k, j, i });

    loop.Unroll(i_kernel_inner);
    loop.Unroll(j_kernel_inner);
    loopnests::CodeGenerator generator;
    generator.Run(loop);
#if 0
    PrintLoops(loop, "GotoBLASGemmWithRefDeref");
#endif

    Scalar ok = Allocate<int>(ScalarLayout);
    ok = 1;
    If(
        VerifySame(C, expected) == 0,
        [&] {
            ok = 0;
        })
        .Else([&] {
            auto value = C.GetValue();
            value.SetLayout({ { (int)C.Size() } });
            DebugPrintVector(value);
            DebugPrint("\n");
            auto expectedValue = expected.GetValue();
            expectedValue.SetLayout({ { (int)expected.Size() } });
            DebugPrintVector(expectedValue);
            DebugPrint("\n");
        });
    return ok;
}

Scalar YG12LowLevel_TestBoundary()
{
    const int N = 8;
    auto A = MakeMatrix<int>(N, N, "A");
    auto B = MakeMatrix<int>(N, N, "B");
    auto C = MakeMatrix<int>(N, N, "C");
    auto expected = MakeMatrix<int>(N, N, "expected");

    // initialize A, B, and C
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
            B(i, j) = i + 2 * j;
        });
    });

    // fill out expected with a simple for-loop gemm
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            ForRange(N, [&](Scalar k) {
                expected(i, j) += A(i, k) * B(k, j);
            });
        });
    });

    static const int k_r = 3;
    static const int k_c = 4;

    // Cache B into a columnMajor matrix
    auto transposeBVal = MakeMatrix<int>(N, N, "transposeB");
    auto transposeB = Matrix(transposeBVal);

    Index transposeK("transposeK"), transposeN("transposeN");
    LoopNest transposeLoop({ { transposeK, { 0, N } },
                             { transposeN, { 0, N } } });
    static const int transposeRows = N;
    static const int transposeCols = N;

    auto [k_transpose_outer, k_transpose_inner] = transposeLoop.Split(transposeK, transposeRows);
    auto [n_transpose_outer, n_transpose_inner] = transposeLoop.Split(transposeN, transposeCols);

    auto transposeKernel = Kernel("transpose_kernel")
                               .Inputs(B.GetValue(), transposeB.GetValue())
                               .Indices(transposeK, transposeN)
                               .Define([](Matrix input, Matrix output, Scalar row, Scalar col) {
                                   output(col, row) = input(row, col);
                               });
    transposeLoop.Unroll(n_transpose_inner);
    transposeLoop.SetLoopOrder({ transposeK, transposeN, transposeK, transposeN });
    loopnests::CodePositionConstraints transposeConstraints{ loopnests::LoopFragmentType::body, { k_transpose_inner, n_transpose_inner }, {} };
    transposeLoop.AddKernel(transposeKernel, loopnests::LoopFragmentType::body);

    loopnests::CodeGenerator transposeGenerator;
    transposeGenerator.Run(transposeLoop);

    // Do computation in blocks of k_r x k_c
    {
        Matrix temp = MakeMatrix<int>(k_r, k_c);

        loopnests::Index m("m"), n("n"), k("k");
        loopnests::LoopNest loop({ { m, { 0, N - (N % k_r) } },
                                   { n, { 0, N } },
                                   { k, { 0, N } } });

        auto [n_block_outer, n_block_inner] = loop.Split(n, 4);

        auto [m_outer, m_inner] = loop.Split(m, k_r);
        auto [n_outer, n_inner] = loop.Split(n, k_c);

        auto kernel = loopnests::Kernel("MatrixMatrixMultiplyNode_Kernel")
                          .Inputs(A.GetValue(), transposeB.GetValue(), C.GetValue(), temp.GetValue())
                          .Indices(m, n, m_inner, n_inner, k)
                          .Define([](Matrix A, Matrix B, Matrix C, Matrix temp, Scalar i, Scalar j, Scalar i_inner, Scalar j_inner, Scalar k) {
                              temp(i_inner, j_inner) += A(i, k) * B(j, k);
                          });
        auto kernel2 = loopnests::Kernel("MatrixMatrixMultiplyNode_Reduce")
                           .Inputs(C.GetValue(), temp.GetValue())
                           .Indices(m_outer, n_outer, n_block_outer)
                           .Define([](Matrix C, Matrix temp, Scalar i, Scalar j, Scalar j_outer) {
                               For(temp, [&](Scalar i_inner, Scalar j_inner) {
                                   C(i + i_inner, j_outer + j + j_inner) = temp(i_inner, j_inner);
                               });
                           });
        auto kernel3 = loopnests::Kernel("MatrixMatrixMultiplyNode_InitializeCache")
                           .Inputs(temp.GetValue())
                           .Indices(m_outer, n_outer)
                           .Define([](Matrix temp, Scalar i, Scalar j) {
                               For(temp, [&](Scalar i_inner, Scalar j_inner) {
                                   temp(i_inner, j_inner) = static_cast<int>(0);
                               });
                           });
        loop.Unroll(m_inner);
        loop.Unroll(n_inner);

        loop.Unroll(n_outer);

        loop.SetLoopOrder({ n, m, n, k, n, m });
        loopnests::CodePositionConstraints constraints2{ loopnests::LoopFragmentType::epilogue, { m_outer, n_outer }, {} };
        loopnests::CodePositionConstraints constraints3{ loopnests::LoopFragmentType::prologue, { m_outer, n_outer }, {} };

        loop.AddKernel(kernel3, constraints3);
        loop.AddKernel(kernel, LoopNest::ConstraintType::constraint);
        loop.AddKernel(kernel2, constraints2);
        loopnests::CodeGenerator generator;
#if 0
        PrintLoops(loop, "YG12_Boundary_test_first");
#endif
        generator.Run(loop);
    }

    // Do remainder
    {
        auto remainderRows = N % k_r;
        auto startM = N - remainderRows;
        Matrix temp = MakeMatrix<int>(remainderRows, k_c);

        loopnests::Index m("m"), n("n"), k("k");
        loopnests::LoopNest loop({ { m, { startM, N } },
                                   { n, { 0, N } },
                                   { k, { 0, N } } });

        auto [n_block_outer, n_block_inner] = loop.Split(n, 4);
        auto [m_outer, m_inner] = loop.Split(m, remainderRows);
        auto [n_outer, n_inner] = loop.Split(n, k_c);

        auto kernel = loopnests::Kernel("MatrixMatrixMultiplyNode_Kernel_remainder")
                          .Inputs(A.GetValue(), transposeB.GetValue(), C.GetValue(), temp.GetValue())
                          .Indices(m, n, m_inner, n_inner, k)
                          .Define([](Matrix A, Matrix B, Matrix C, Matrix temp, Scalar i, Scalar j, Scalar i_inner, Scalar j_inner, Scalar k) {
                              temp(i_inner, j_inner) += A(i, k) * B(j, k);
                          });
        auto kernel2 = loopnests::Kernel("MatrixMatrixMultiplyNode_Reduce_remainder")
                           .Inputs(C.GetValue(), temp.GetValue())
                           .Indices(m, m_outer, n_outer, n_block_outer)
                           .Define([](Matrix C, Matrix temp, Scalar i, Scalar i_outer, Scalar j_outer, Scalar j_block_outer) {
                               For(temp, [&](Scalar i_inner, Scalar j_inner) {
                                   C(i + i_inner, j_block_outer + j_outer + j_inner) = temp(i_inner, j_inner);
                               });
                           });
        auto kernel3 = loopnests::Kernel("MatrixMatrixMultiplyNode_InitializeCache")
                           .Inputs(temp.GetValue())
                           .Indices()
                           .Define([](Matrix temp) {
                               For(temp, [&](Scalar i_inner, Scalar j_inner) {
                                   temp(i_inner, j_inner) = static_cast<int>(0);
                               });
                           });
        loop.Unroll(n_inner);
        loop.Unroll(n_outer);

        loop.SetLoopOrder({ n, m, n, k, n, m });
        loopnests::CodePositionConstraints constraints2{ loopnests::LoopFragmentType::epilogue, { n_outer, m_outer }, {} };
        loopnests::CodePositionConstraints constraints3{ loopnests::LoopFragmentType::prologue, { n_outer, m_outer }, {} };
        loop.AddKernel(kernel3, constraints3);
        loop.AddKernel(kernel, LoopNest::ConstraintType::constraint);
        loop.AddKernel(kernel2, constraints2);
        loopnests::CodeGenerator generator;
#if 0
        PrintLoops(loop, "YG12_Boundary_test_remainder");
#endif
        generator.Run(loop);
    }

    Scalar ok = Allocate<int>(ScalarLayout);
    ok = 1;
    If(
        VerifySame(C, expected) == 0,
        [&] {
            ok = 0;
        })
        .Else([&] {
            auto value = C.GetValue();
            value.SetLayout({ { (int)C.Size() } });
            DebugPrintVector(value);
            DebugPrint("\n");
            auto expectedValue = expected.GetValue();
            expectedValue.SetLayout({ { (int)expected.Size() } });
            DebugPrintVector(expectedValue);
            DebugPrint("\n");
        });
    return ok;
}

Scalar KernelPredicate_test()
{
    const int M = 8;
    const int N = M;
    const int K = M;
    auto A = MakeMatrix<int>(M, K, "A");
    auto B = MakeMatrix<int>(K, N, "B");
    auto C = MakeMatrix<int>(M, N, "C");
    auto expected = MakeMatrix<int>(M, N, "expected");

    // initialize A, B, and C
    ForRange(M, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
            B(i, j) = i + 2 * j;
            C(i, j) = 100;
        });
    });

    // fill out expected with a simple for-loop gemm (plus 1)
    ForRange(M, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            ForRange(K, [&](Scalar k) {
                expected(i, j) += A(i, k) * B(k, j);
            });
            expected(i, j) += 1;
        });
    });

    // The input matrices:
    // A:                                 B:                                 C:
    // [ 0, -1, -2, -3, -4, -5, -6, -7]   [ 0,  2,  4,  6,  8, 10, 12, 14]   [ 100 100 100 ... ]
    // [ 1,  0, -1, -2, -3, -4, -5, -6]   [ 1,  3,  5,  7,  9, 11, 13, 15]   [ 100 100 100 ... ]
    // [ 2,  1,  0, -1, -2, -3, -4, -5]   [ 2,  4,  6,  8, 10, 12, 14, 16]   [      ...        ]
    // [ 3,  2,  1,  0, -1, -2, -3, -4]   [ 3,  5,  7,  9, 11, 13, 15, 17]   [      ...        ]
    // [ 4,  3,  2,  1,  0, -1, -2, -3]   [ 4,  6,  8, 10, 12, 14, 16, 18]   [      ...        ]
    // [ 5,  4,  3,  2,  1,  0, -1, -2]   [ 5,  7,  9, 11, 13, 15, 17, 19]   [      ...        ]
    // [ 6,  5,  4,  3,  2,  1,  0, -1]   [ 6,  8, 10, 12, 14, 16, 18, 20]   [      ...        ]
    // [ 7,  6,  5,  4,  3,  2,  1,  0]   [ 7,  9, 11, 13, 15, 17, 19, 21]   [      ...        ]

    // (A * B) (the desired result):
    // [-140, -196, -252, -308, -364, -420, -476, -532]
    // [-112, -152, -192, -232, -272, -312, -352, -392]
    // [ -84, -108, -132, -156, -180, -204, -228, -252]
    // [ -56,  -64,  -72,  -80,  -88,  -96, -104, -112]
    // [ -28,  -20,  -12,   -4,    4,   12,   20,   28]
    // [   0,   24,   48,   72,   96,  120,  144,  168]
    // [  28,   68,  108,  148,  188,  228,  268,  308]
    // [  56,  112,  168,  224,  280,  336,  392,  448]

    Index i("i"), j("j"), k("k");

    LoopNest loop({ { i, { 0, N } },
                    { j, { 0, N } },
                    { k, { 0, N } } });

    auto initCKernel = Kernel("init")
                           .Inputs(C.GetValue())
                           .Indices(i, j, k)
                           .Define([](Matrix C, Scalar i, Scalar j, Scalar k) {
                               C(i, j) = 0;
                           });

    auto innerKernel = Kernel("matmul")
                           .Inputs(A.GetValue(), B.GetValue(), C.GetValue())
                           .Indices(i, j, k)
                           .Define([](Matrix A, Matrix B, Matrix C, Scalar i, Scalar j, Scalar k) {
                               C(i, j) += A(i, k) * B(k, j);
                           });

    auto postKernel = Kernel("addone")
                          .Inputs(A.GetValue(), B.GetValue(), C.GetValue())
                          .Indices(i, j, k)
                          .Define([](Matrix A, Matrix B, Matrix C, Scalar i, Scalar j, Scalar k) {
                              C(i, j) += 1;
                          });

    loop.AddKernel(initCKernel, { First(k) });
    loop.AddKernel(innerKernel, LoopNest::ConstraintType::predicate);
    loop.AddKernel(postKernel, { Last(k) });

#if 1
    auto [i_panel_outer, i_panel_inner] = loop.Split(i, 2);
    auto [j_kernel_outer, j_kernel_inner] = loop.Split(j, 4);
    auto [k_panel_outer, k_panel_inner] = loop.Split(k, 2);

    loop.SetLoopOrder({ k, j, i, j, i, k });
#endif

#if 0
    PrintLoops(loop, "KernelPredicate_test");
#endif

    CodeGenerator generator;
    generator.Run(loop);

    Scalar ok = Allocate<int>(ScalarLayout);
    ok = 1;

    If(
        VerifySame(C, expected) == 0,
        [&] {
            ok = 0;
        })
        .Else([&] {
#if 0
            auto value = C.GetValue();
            value.SetLayout({ { (int)C.Size() } });
            DebugPrintVector(value);
            DebugPrint("\n");
            auto expectedValue = expected.GetValue();
            expectedValue.SetLayout({ { (int)expected.Size() } });
            DebugPrintVector(expectedValue);
            DebugPrint("\n");
#endif
        });

    return ok;
}

Scalar MatMul3_test1()
{
    auto p = GetMatMul3TestCaseParameters(8, 8, 8, 8);
    const auto M = p.M;
    const auto N = p.N;
    const auto K = p.K;
    const auto L = p.L;

    Index i("i"), j("j"), k("k"), l("l");
    LoopNest loop({ { i, { 0, M } },
                    { j, { 0, N } },
                    { k, { 0, K } },
                    { l, { 0, L } } });

    auto initCKernel = Kernel("initC")
                           .Inputs(p.C.GetValue())
                           .Indices(i, j)
                           .Define([](Matrix C, Scalar i, Scalar j) {
                               C(i, j) = 0;
                           });

    auto computeCKernel = Kernel("matmulC")
                              .Inputs(p.A.GetValue(), p.B.GetValue(), p.C.GetValue())
                              .Indices(i, j, k)
                              .Define([](Matrix A, Matrix B, Matrix C, Scalar i, Scalar j, Scalar k) {
                                  C(i, j) += A(i, k) * B(k, j);
                              });

    auto initEKernel = Kernel("initE")
                           .Inputs(p.E.GetValue())
                           .Indices(i, l)
                           .Define([](Matrix E, Scalar i, Scalar l) {
                               E(i, l) = 0;
                           });

    auto computeEKernel = Kernel("matmulE")
                              .Inputs(p.C.GetValue(), p.D.GetValue(), p.E.GetValue())
                              .Indices(i, j, l)
                              .Define([](Matrix C, Matrix D, Matrix E, Scalar i, Scalar j, Scalar l) {
                                  E(i, l) += C(i, j) * D(j, l);
                              });

    loop.AddKernel(initCKernel, { First(k) && First(l) });
    loop.AddKernel(computeCKernel, { First(l) });

    loop.AddKernel(initEKernel, { Last(k) && First(j) });
    loop.AddKernel(computeEKernel, { Last(k) });

#if 0
    PrintLoops(loop, "MatMul3_test1");
#endif

    CodeGenerator generator;
    generator.Run(loop);

    return VerifySame(p.E, p.expectedE);
}

Scalar MatMul3_test2()
{
    auto p = GetMatMul3TestCaseParameters(8, 8, 8, 8);
    const auto M = p.M;
    const auto N = p.N;
    const auto K = p.K;
    const auto L = p.L;

    Index i("i"), j("j"), k("k"), l("l");
    LoopNest loop({ { i, { 0, M } },
                    { j, { 0, N } },
                    { k, { 0, K } },
                    { l, { 0, L } } });

    int stepI = 4;
    int stepJ = 4;
    // int stepK = 4;
    auto [iOuter, iInner] = loop.Split(i, stepI);
    auto [jOuter, jInner] = loop.Split(j, stepJ);
    // auto [kOuter, kInner] = loop.Split(k, stepK);

    loop.SetLoopOrder({ iOuter, jOuter, k, l, iInner, jInner });

    auto initCKernel = Kernel("initC")
                           .Inputs(p.C.GetValue())
                           .Indices(i, j)
                           .Define([&](Matrix C, Scalar i, Scalar j) {
                               ForRange(stepI, [&](Scalar ii) {
                                   ForRange(stepJ, [&](Scalar jj) {
                                       C(i + ii, j + jj) = 0;
                                   });
                               });
                           });

    auto computeCKernel = Kernel("matmulC")
                              .Inputs(p.A.GetValue(), p.B.GetValue(), p.C.GetValue())
                              .Indices(i, j, k)
                              .Define([&](Matrix A, Matrix B, Matrix C, Scalar i, Scalar j, Scalar k) {
                                  // accum into C(I,J) via GEMM
                                  ForRange(stepI, [&](Scalar ii) {
                                      ForRange(stepJ, [&](Scalar jj) {
                                          C(i + ii, j + jj) += A(i + ii, k) * B(k, j + jj);
                                      });
                                  });
                              });

    auto initEKernel = Kernel("initE")
                           .Inputs(p.E.GetValue())
                           .Indices(i, l)
                           .Define([&](Matrix E, Scalar i, Scalar l) {
                               ForRange(stepI, [&](Scalar ii) {
                                   E(i + ii, l) = 0;
                               });
                           });

    auto computeEKernel = Kernel("matmulE")
                              .Inputs(p.C.GetValue(), p.D.GetValue(), p.E.GetValue())
                              .Indices(i, j, l)
                              .Define([&](Matrix C, Matrix D, Matrix E, Scalar i, Scalar j, Scalar l) {
                                  ForRange(stepI, [&](Scalar ii) {
                                      ForRange(stepJ, [&](Scalar jj) {
                                          // accum into E(I,L) via GEMM
                                          E(i + ii, l) += C(i + ii, j + jj) * D(j + jj, l);
                                      });
                                  });
                              });

    loop.AddKernel(initCKernel, { First(k) && First(l) }, { Before(iInner) || Before(jInner) });
    loop.AddKernel(computeCKernel, { First(l) }, { Before(iInner) || Before(jInner) });

    loop.AddKernel(initEKernel, { Last(k) && First(j) }, { Before(iInner) || Before(jInner) });
    loop.AddKernel(computeEKernel, { Last(k) }, { Before(iInner) || Before(jInner) });

#if 0
    PrintLoops(loop, "MatMul3_test2");
#endif

    CodeGenerator generator;
    generator.Run(loop);

    return VerifySame(p.E, p.expectedE);
}

Scalar LoopNestFuse_test1()
{
    auto p = GetMatMul3TestCaseParameters(8, 8, 8, 8);
    const auto M = p.M;
    const auto N = p.N;
    const auto K = p.K;
    const auto L = p.L;

    Index i("i"), j("j"), k("k"), l("l");
    LoopNest loopC({ { i, { 0, M } },
                     { j, { 0, N } },
                     { k, { 0, K } } });
    LoopNest loopE({ { i, { 0, M } },
                     { j, { 0, N } },
                     { l, { 0, L } } });

    auto initCKernel = Kernel("initC")
                           .Inputs(p.C.GetValue())
                           .Indices(i, j)
                           .Define([](Matrix C, Scalar i, Scalar j) {
                               C(i, j) = 0;
                           });

    auto computeCKernel = Kernel("matmulC")
                              .Inputs(p.A.GetValue(), p.B.GetValue(), p.C.GetValue())
                              .Indices(i, j, k)
                              .Define([](Matrix A, Matrix B, Matrix C, Scalar i, Scalar j, Scalar k) {
                                  C(i, j) += A(i, k) * B(k, j);
                              });

    auto initEKernel = Kernel("initE")
                           .Inputs(p.E.GetValue())
                           .Indices(i, l)
                           .Define([](Matrix E, Scalar i, Scalar l) {
                               E(i, l) = 0;
                           });

    auto computeEKernel = Kernel("matmulE")
                              .Inputs(p.C.GetValue(), p.D.GetValue(), p.E.GetValue())
                              .Indices(i, j, l)
                              .Define([](Matrix C, Matrix D, Matrix E, Scalar i, Scalar j, Scalar l) {
                                  E(i, l) += C(i, j) * D(j, l);
                              });

    loopC.AddKernel(initCKernel, { First(k) });
    loopC.AddKernel(computeCKernel, LoopNest::ConstraintType::predicate);

    loopE.AddKernel(initEKernel, { First(j) });
    loopE.AddKernel(computeEKernel, LoopNest::ConstraintType::predicate);

    // Now fuse the loops
    auto fusedLoops = Fuse(loopC, loopE, { l }, { k });
    fusedLoops.SetLoopOrder({ i, j, k, l });

#if 0
    PrintLoops(fusedLoops, "LoopNestFuse_test1: fusedLoops");
#endif

    Scalar ok = Allocate<int>(ScalarLayout);
    ok = 1;

    CodeGenerator generator;
    generator.Run(fusedLoops);

    return VerifySame(p.E, p.expectedE);
}

LoopNest GetMatMulLoopNest(std::string name, const Matrix& A, const Matrix& B, const Matrix& C, const Index& i, const Index& j, const Index& k, bool initResult = true)
{
    const int M = static_cast<int>(C.Rows());
    const int N = static_cast<int>(C.Columns());
    const int K = static_cast<int>(A.Columns());

    LoopNest loop({ { i, { 0, M } },
                    { j, { 0, N } },
                    { k, { 0, K } } });

    auto initCKernel = Kernel("init_" + name)
                           .Inputs(C.GetValue())
                           .Indices(i, j)
                           .Define([](Matrix C, Scalar i, Scalar j) {
                               C(i, j) = 0;
                           });

    auto innerKernel = Kernel("matmul_" + name)
                           .Inputs(A.GetValue(), B.GetValue(), C.GetValue())
                           .Indices(i, j, k)
                           .Define([](Matrix A, Matrix B, Matrix C, Scalar i, Scalar j, Scalar k) {
                               C(i, j) += A(i, k) * B(k, j);
                           });

    if (initResult)
        loop.AddKernel(initCKernel, { First(k) });
    loop.AddKernel(innerKernel, LoopNest::ConstraintType::predicate);

    return loop;
}

Scalar LoopNestFuse_test2()
{
    auto p = GetMatMul3TestCaseParameters(8, 8, 8, 8);

    Index i("i"), j("j"), k("k"), l("l");

    LoopNest loopC = GetMatMulLoopNest("C", p.A, p.B, p.C, i, j, k); // C = A * B
    LoopNest loopE = GetMatMulLoopNest("E", p.C, p.D, p.E, i, l, j); // E = C * D

#if 0
    PrintLoops(loopC, "LoopNestFuse_test: loopC");
    PrintLoops(loopE, "LoopNestFuse_test: loopE");
#endif

    // Now fuse the loops
    auto fusedLoops = Fuse(loopC, loopE, { l }, { k });

#if 0
    PrintLoops(fusedLoops, "LoopNestFuse_test2: fusedLoops");
#endif

    Scalar ok = Allocate<int>(ScalarLayout);
    ok = 1;

    CodeGenerator generator;
    generator.Run(fusedLoops);

    If(
        VerifySame(p.E, p.expectedE) == 0,
        [&] {
            ok = 0;
        });

    return ok;
}

Scalar LoopNestFuse_test3()
{
    auto p = GetMatMul3TestCaseParameters(8, 8, 8, 8);

    Index i("i"), j("j"), k("k"), l("l");

    LoopNest loopC = GetMatMulLoopNest("C", p.A, p.B, p.C, i, j, k); // C = A * B
    LoopNest loopE = GetMatMulLoopNest("E", p.C, p.D, p.E, i, l, j); // E = C * D

#if 0
    PrintLoops(loopC, "LoopNestFuse_test: loopC");
    PrintLoops(loopE, "LoopNestFuse_test: loopE");
#endif

    // Now fuse the loops
    auto fusedLoops = Fuse(loopC, loopE);

#if 0
    PrintLoops(fusedLoops, "LoopNestFuse_test2: fusedLoops");
#endif

    Scalar ok = Allocate<int>(ScalarLayout);
    ok = 1;

    CodeGenerator generator;
    generator.Run(fusedLoops);

    If(
        VerifySame(p.E, p.expectedE) == 0,
        [&] {
            ok = 0;
        });

    return ok;
}

Scalar ConvertedConstraint_test1()
{
    std::string loopOrder = "ijk";
    const int N = 8;
    auto A = MakeMatrix<int>(N, N, "A");
    auto B = MakeMatrix<int>(N, N, "B");
    auto C = MakeMatrix<int>(N, N, "C");

    // initialize A, B, and C
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
            B(i, j) = i + 2 * j;
            C(i, j) = 100;
        });
    });

    // The input matrices:
    // A:                                 B:                                 C:
    // [ 0, -1, -2, -3, -4, -5, -6, -7]   [ 0,  2,  4,  6,  8, 10, 12, 14]   [ 100 100 100 ... ]
    // [ 1,  0, -1, -2, -3, -4, -5, -6]   [ 1,  3,  5,  7,  9, 11, 13, 15]   [ 100 100 100 ... ]
    // [ 2,  1,  0, -1, -2, -3, -4, -5]   [ 2,  4,  6,  8, 10, 12, 14, 16]   [      ...        ]
    // [ 3,  2,  1,  0, -1, -2, -3, -4]   [ 3,  5,  7,  9, 11, 13, 15, 17]   [      ...        ]
    // [ 4,  3,  2,  1,  0, -1, -2, -3]   [ 4,  6,  8, 10, 12, 14, 16, 18]   [      ...        ]
    // [ 5,  4,  3,  2,  1,  0, -1, -2]   [ 5,  7,  9, 11, 13, 15, 17, 19]   [      ...        ]
    // [ 6,  5,  4,  3,  2,  1,  0, -1]   [ 6,  8, 10, 12, 14, 16, 18, 20]   [      ...        ]
    // [ 7,  6,  5,  4,  3,  2,  1,  0]   [ 7,  9, 11, 13, 15, 17, 19, 21]   [      ...        ]

    // (A * B) + 1  (the desired result):
    // [-139, -195, -251, -307, -363, -419, -475, -531]
    // [-111, -151, -191, -231, -271, -311, -351, -391]
    // [ -83, -107, -131, -155, -179, -203, -227, -251]
    // [ -55,  -63,  -71,  -79,  -87,  -95, -103, -111]
    // [ -27,  -19,  -11,   -3,    5,   13,   21,   29]
    // [   1,   25,   49,   73,   97,  121,  145,  169]
    // [  29,   69,  109,  149,  189,  229,  269,  309]
    // [  57,  113,  169,  225,  281,  337,  393,  449]

    Index i("i"), j("j"), k("k");

    auto innerKernel = Kernel("matmul")
                           .Inputs(A.GetValue(), B.GetValue(), C.GetValue())
                           .Indices(i, j, k)
                           .Define(matmul_kernel);
    auto initCKernel = Kernel("init")
                           .Inputs(C.GetValue())
                           .Indices(i, j)
                           .Define(initToZero);
    auto postProcessCKernel = Kernel("post")
                                  .Inputs(C.GetValue())
                                  .Indices(i, j)
                                  .Define(addOne);

    LoopNest loop({ { i, { 0, N } },
                    { j, { 0, N } },
                    { k, { 0, N } } });

    CodePositionConstraints preConstraint{ LoopFragmentType::prologue, { i, j }, {} };
    loop.AddKernel(initCKernel, preConstraint);

    loop.AddKernel(innerKernel, LoopNest::ConstraintType::constraint);

    CodePositionConstraints postConstraint{ LoopFragmentType::epilogue, { i, j }, {} };
    loop.AddKernel(postProcessCKernel, postConstraint);

    SplitAndSetOrder(loop, { i, j, k }, { 4, 2 }, loopOrder);

    CodeGenerator generator;
    generator.Run(loop);

#if 0 // DEBUGGING
    PrintLoops(loop, "ConvertedConstraint_test1");
#endif

#if 0 // DEBUGGING
    InvokeForContext<ComputeContext>([&](auto&) {
        ForRange(N, [&](Scalar i) {
            ForRange(N, [&](Scalar j) {
                auto val = C(i, j).Get<int>();
                Log() << std::setw(5) << val;
            });
            Log() << EOL;
        });
    });
#endif

    return C(1, 2) + C(2, 1) - (-191 + -107); // will return 0 if calculation is correct
}

Scalar ConvertedConstraint_test2()
{
    auto matrix = MakeMatrix<int>(4, 5);
    IndexRange i("i", { 0, 4 }), j("j", { 0, 5 });

    auto kernel = Kernel("kernel")
                      .Inputs(matrix.GetValue())
                      .Indices(i.GetIndex(), j.GetIndex())
                      .Define(loopnest_kernel);

    LoopNest loop(std::vector<IndexRange>{ i, j });
    loop.AddKernel(kernel, LoopNest::ConstraintType::constraint);
    loop.Split(i.GetIndex(), 2);

    CodeGenerator generator;
    generator.Run(loop);

#if 0 // DEBUGGING
    PrintLoops(loop, "ConvertedConstraint_test2");
#endif

#if 0 // DEBUGGING
    InvokeForContext<ComputeContext>([&](auto&) {
        ForRange(4, [&](Scalar i) {
            ForRange(5, [&](Scalar j) {
                auto val = matrix(i, j).Get<int>();
                Log() << std::setw(5) << val;
            });
            Log() << EOL;
        });
    });
#endif

    return matrix(2, 3) - 19; // will return 0 if calculation is correct
}
} // namespace ell
