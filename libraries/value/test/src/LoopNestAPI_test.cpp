////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoopNestAPI_test.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoopNestAPI_test.h"
#include "LoopNest_kernels.h"
#include "TestUtil.h"

#include <value/include/CachingStrategies.h>
#include <value/include/LLVMContext.h>
#include <value/include/LoopNests.h>
#include <value/include/Matrix.h>
#include <value/include/Reference.h>
#include <value/include/Scalar.h>
#include <value/include/Value.h>
#include <value/include/loopnests/LoopNest.h>

#include <utilities/include/Logger.h>
#include <utilities/include/TunableParameters.h>

#include <llvm/ADT/Twine.h>

#include <llvm/IR/Value.h>

#if 0 // DEBUGGING
#include <value/include/loopnests/LoopNest.h>
#endif

using namespace ell::utilities;
using namespace ell::value;
using namespace ell::logging;

namespace ell
{
Scalar LoopNest_api_test1()
{
    auto matrix = MakeMatrix<int>(4, 5);
    Index i("i"), j("j");

    Using({ matrix }, ArgumentType::Output)
        .ForAll(i, 0, 4)
        .ForAll(j, 0, 5)
        .Do(loopnest_kernel)
        .Run();

    return matrix(2, 3) - 19; // will return 0 if calculation is correct
}

Scalar LoopNest_api_test2()
{
    auto matrix = MakeMatrix<int>(4, 5);
    Index i("i"), j("j");

    auto nest = Using({ matrix }, ArgumentType::Output)
                    .ForAll(i, 0, 4)
                    .ForAll(j, 0, 5)
                    .Do(loopnest_kernel);

    nest.GetSchedule().Split(i, 2);

    nest.Run();

    return matrix(2, 3) - 19; // will return 0 if calculation is correct
}

Scalar LoopNest_api_test3()
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

    Index i("i"), j("j");

    //// Use a Loopnest to call loopnest_kernel_3 for each element of the input matrix and write the result to
    //// our output.
    Using({ output }, ArgumentType::Output)
        .Using({ matrix }, ArgumentType::Input)
        .ForAll(i, 0, static_cast<int>(matrix.Rows()))
        .ForAll(j, 0, static_cast<int>(matrix.Columns()))
        .Do(loopnest_kernel_3)
        .Run();

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

Scalar LoopNest_api_test4()
{
    // Declare the output matrix and initialize its values to 0.
    auto output = MakeMatrix<int>(2, 6);

    Index i("i"), j("j");

    // Use a Loopnest to call loopnest_kernel_4 for each element of the input matrix and write the result to
    // our output.
    auto nest = Using({ output }, ArgumentType::Output)
                    .Using({ output }, ArgumentType::Input) // this isn't how you'd write in real life, hopefully (using the same memory for both input and output)
                    .ForAll(i, 0, static_cast<int>(output.Rows()))
                    .ForAll(j, 0, static_cast<int>(output.Columns()))
                    .Do(loopnest_kernel_4);

    nest.GetSchedule().Split(j, 2);

    nest.Run();

    // loopnest_kernel_4 will multiply row by 10 and add the column.
    std::vector<int> expectedValues{ 0, 1, 2, 3, 4, 5, 10, 11, 12, 13, 14, 15 };
    auto expected = Vector(expectedValues);

    // View the result as a Vector
    Vector actual = AsVector(AsFullView(output));

    // Verify that the actual result is what we expect
    return VerifySame(actual, expected);
}

Scalar LoopNest_api_test5()
{
    // Declare the output matrix and initialize its values to 0.
    auto output = MakeMatrix<int>(2, 8);

    Index i("i"), j("j");

    // Use a Loopnest to call loopnest_kernel_4 for each element of the input matrix and write the result to
    // our output.
    auto nest = Using({ output }, ArgumentType::Output)
                    .Using({ output }, ArgumentType::Input) // this isn't how you'd write in real life, hopefully (using the same memory for both input and output)
                    .ForAll(i, 0, static_cast<int>(output.Rows()))
                    .ForAll(j, 0, static_cast<int>(output.Columns()))
                    .Do(loopnest_kernel_4);

    auto& schedule = nest.GetSchedule();
    schedule.Split(j, 4);
    schedule.Split(j, 2);

    nest.Run();

    // loopnest_kernel_4 will multiply row by 10 and add the column.
    std::vector<int> expectedValues{ 0, 1, 2, 3, 4, 5, 6, 7, 10, 11, 12, 13, 14, 15, 16, 17 };
    auto expected = Vector(expectedValues);

    // View the result as a Vector
    Vector actual = AsVector(AsFullView(output));

    // Verify that the actual result is what we expect
    return VerifySame(actual, expected);
}

Scalar LoopNest_api_Parallelized_test1()
{
    Scalar ok = Allocate<int>(ScalarLayout);
    auto matrix = MakeMatrix<int>(4, 5);
    InvokeForContext<LLVMContext>([&] {
        auto v = matrix.GetValue().Get<Emittable>().GetDataAs<llvm::Value*>();
        v->setName("matrix");
    });

    Index i("i"), j("j");

    auto nest = Using({ matrix }, ArgumentType::Output)
                    .ForAll(i, 0, 4)
                    .ForAll(j, 0, 5)
                    .Do([](Matrix m, Scalar i, Scalar j) {
                        Scalar tid = GetTID();
#if 1 // Useful to turn off/on for debugging
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

    nest.GetSchedule().Parallelize(i, 2);

    nest.Run();

    ok = matrix(2, 3) - 19;
    return ok; // will return 0 if calculation is correct
}

Scalar LoopNest_api_Parallelized_test2()
{
    Scalar ok = Allocate<int>(ScalarLayout);

    auto matrix = MakeMatrix<int>(4, 5);
    Index i("i"), j("j");

    auto nest = Using({ matrix }, ArgumentType::Output)
                    .ForAll(i, 0, 4)
                    .ForAll(j, 0, 5)
                    .Do([](Matrix m, Scalar i, Scalar j) {
                        Scalar tid = GetTID();
#if 1 // Useful to turn off/on for debugging
                        InvokeForContext<ComputeContext>([&](auto&) {
                            auto iInt = i.Get<int>();
                            auto jInt = j.Get<int>();
                            Log() << "m(" << iInt << ", " << jInt << ") = " << tid.Get<int>()
                                  << " [Thread " << tid.Get<int>() << "]" << EOL;
                        });
#endif // 1
                        m(i, j) = tid;
                    });

    nest.GetSchedule().Parallelize(i, 2);

    nest.Run();

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

Scalar LoopNest_api_Unrolled_test1()
{
    auto matrix = MakeMatrix<int>(20, 5);
    Index i("i"), j("j");

    auto nest = Using({ matrix }, ArgumentType::Output)
                    .ForAll(i, 0, 4)
                    .ForAll(j, 0, 5)
                    .Do(loopnest_kernel);

    auto& schedule = nest.GetSchedule();

    schedule.Parallelize(i, 2);
    schedule.Unroll(j);

    nest.Run();

    return matrix(2, 3) - 19; // will return 0 if calculation is correct
}

Scalar LoopNest_api_SetOrder_test1()
{
    auto matrix = MakeMatrix<int>(4, 5);
    Index i("i"), j("j");

    auto nest = Using({ matrix }, ArgumentType::Output)
                    .ForAll(i, 0, 4)
                    .ForAll(j, 0, 5)
                    .Do(loopnest_kernel);

    auto& schedule = nest.GetSchedule();
    auto i_o = schedule.Split(i, 2);
    schedule.SetOrder({ i_o, j, i });

    nest.Run();

    return matrix(2, 3) - 19; // will return 0 if calculation is correct
}

Scalar LoopNest_api_CachedMatrix_test1()
{
    const int N = 4;
    auto A = MakeMatrix<int>(N, N, "A");
    For(A, [&](Scalar i, Scalar j) {
        A(i, j) = i - j;
    });

    // A:
    // [ 0, -1, -2, -3 ]
    // [ 1,  0, -1, -2 ]
    // [ 2,  1,  0, -1 ]
    // [ 3,  2,  1,  0 ]

    Index i("i"), j("j");

    auto nest = Using({ A }, ArgumentType::InputOutput)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .Do(addOne);

    nest.GetSchedule().Cache(
        CreateCacheFor(A)
            .Size({ N, N })
            .Using({ i, j })
            .Type(SubMatrixCopyInCopyOutCache{}));

    nest.Run();

    return A(2, 0) + A(0, 2) - 2; // will return 0 if calculation is correct
}

Scalar LoopNest_api_SlidingCachedMatrix_test()
{
    const int N = 8;
    const int cacheARows = N / 2;
    const int cacheACols = N / 2;

    auto A = MakeMatrix<int>(N, N, "A");
    auto B = MakeMatrix<int>(N, N, "B");

    // initialize A
    ForRange(N, [&](Scalar i) {
        ForRange(N, [&](Scalar j) {
            A(i, j) = i - j;
        });
    });

    // The input matrices:
    // A:                                 B:
    // [ 0, -1, -2, -3, -4, -5, -6, -7]   [ 0, 0, ... ]
    // [ 1,  0, -1, -2, -3, -4, -5, -6]   [ 0, 0, ... ]
    // [ 2,  1,  0, -1, -2, -3, -4, -5]   [ 0, 0, ... ]
    // [ 3,  2,  1,  0, -1, -2, -3, -4]   [ 0, 0, ... ]
    // [ 4,  3,  2,  1,  0, -1, -2, -3]   [ 0, 0, ... ]
    // [ 5,  4,  3,  2,  1,  0, -1, -2]   [ 0, 0, ... ]
    // [ 6,  5,  4,  3,  2,  1,  0, -1]   [ 0, 0, ... ]
    // [ 7,  6,  5,  4,  3,  2,  1,  0]   [ 0, 0, ... ]

    Index i("i"), j("j");
    auto nest = Using({ A }, ArgumentType::Input)
                    .Using({ B }, ArgumentType::InputOutput)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .Do([](Matrix A, Matrix B, Scalar i, Scalar j) {
                        B(i, j) = A(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto i_o = schedule.Split(i, cacheARows);
    auto j_o = schedule.Split(j, cacheACols);
    schedule.Cache(
        CreateCacheFor(A)
            .Size({ cacheARows, cacheACols })
            .Using({ i, j })
            .At({ i_o, j_o })
            .Type(SubMatrixCopyIn{}));

    nest.Run();

    Scalar ok = Allocate<int>(ScalarLayout);
    ok = 1;
    If(
        VerifySame(A, B) == 0,
        [&] {
            ok = 0;
        })
        .Else([&] {
            auto value = B.GetValue();
            value.SetLayout({ { (int)B.Size() } });
            DebugPrintVector(value);
            DebugPrint("\n");
            auto expectedValue = A.GetValue();
            expectedValue.SetLayout({ { (int)A.Size() } });
            DebugPrintVector(expectedValue);
            DebugPrint("\n");
        });
    return ok;
}

Scalar SimpleGemm_HighLevelAPI()
{
    const int N = 8;
    const int cacheARows = 4;
    const int cacheACols = 4;
    const int cacheBRows = cacheACols;
    const int cacheBCols = N;
    const int resultCacheRows = 2;
    const int resultCacheCols = 2;

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
    // [ 0, -1, -2, -3, -4, -5, -6, -7]   [ 0,  2,  4,  6,  8, 10, 12, 14]   [ 0 0 ... ]
    // [ 1,  0, -1, -2, -3, -4, -5, -6]   [ 1,  3,  5,  7,  9, 11, 13, 15]   [ 0 0 ... ]
    // [ 2,  1,  0, -1, -2, -3, -4, -5]   [ 2,  4,  6,  8, 10, 12, 14, 16]   [   ...   ]
    // [ 3,  2,  1,  0, -1, -2, -3, -4]   [ 3,  5,  7,  9, 11, 13, 15, 17]   [   ...   ]
    // [ 4,  3,  2,  1,  0, -1, -2, -3]   [ 4,  6,  8, 10, 12, 14, 16, 18]   [   ...   ]
    // [ 5,  4,  3,  2,  1,  0, -1, -2]   [ 5,  7,  9, 11, 13, 15, 17, 19]   [   ...   ]
    // [ 6,  5,  4,  3,  2,  1,  0, -1]   [ 6,  8, 10, 12, 14, 16, 18, 20]   [   ...   ]
    // [ 7,  6,  5,  4,  3,  2,  1,  0]   [ 7,  9, 11, 13, 15, 17, 19, 21]   [   ...   ]

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
    auto nest = Using({ A, B }, ArgumentType::Input)
                    .Using({ C }, ArgumentType::InputOutput)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .ForAll(k, 0, N)
                    .Do([](Matrix A, Matrix B, Matrix C, Scalar i, Scalar j, Scalar k) {
#if 0 // DEBUGGING
                        InvokeForContext<ComputeContext>([&] {
                            std::cout << "C(" << i.Get<int>() << "," << j.Get<int>() << ") pointing at (kernel): " << std::hex << reinterpret_cast<intptr_t>(std::get<int*>(C(i, j).GetValue().GetUnderlyingData())) << std::dec << std::endl;
                        });
#endif
                        C(i, j) += A(i, k) * B(k, j);
                    });

    auto& schedule = nest.GetSchedule();
    auto i_b_o = schedule.Split(i, cacheARows);
    auto k_b_o = schedule.Split(k, cacheACols);
    schedule.Cache(CreateCacheFor(A)
                       .Size({ cacheARows, cacheACols }, utilities::RowMajorMatrixOrder)
                       .Using({ i_b_o, k_b_o })
                       .Type(SubMatrixCopyIn{}));
    schedule.Cache(CreateCacheFor(B)
                       .Size({ cacheBRows, cacheBCols }, utilities::ColumnMajorMatrixOrder)
                       .Using({ k, j })
                       .At({ k_b_o })
                       .Type(SubMatrixCopyIn{}));

    auto i_o = schedule.Split(i, resultCacheRows);
    auto j_o = schedule.Split(j, resultCacheCols);
    schedule.Cache(CreateCacheFor(C)
                       .Size({ resultCacheRows, resultCacheCols }, utilities::RowMajorMatrixOrder)
                       .Using({ i_o, j_o })
                       .Type(ZeroInputCopyOutMatrixCache{}));

    schedule.SetOrder({ k_b_o, i_b_o, j_o, i_o, k, j, i });
    schedule.Unroll(i);
    schedule.Unroll(j);
    nest.Run();

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

Scalar SimpleGemm_HighLevelAPI_NoCachingHelper()
{
    const int N = 8;
    const int cacheARows = 4;
    const int cacheACols = 4;
    const int cacheBRows = cacheACols;
    const int cacheBCols = N;
    const int resultCacheRows = 2;
    const int resultCacheCols = 2;

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
    // [ 0, -1, -2, -3, -4, -5, -6, -7]   [ 0,  2,  4,  6,  8, 10, 12, 14]   [ 0 0 ... ]
    // [ 1,  0, -1, -2, -3, -4, -5, -6]   [ 1,  3,  5,  7,  9, 11, 13, 15]   [ 0 0 ... ]
    // [ 2,  1,  0, -1, -2, -3, -4, -5]   [ 2,  4,  6,  8, 10, 12, 14, 16]   [   ...   ]
    // [ 3,  2,  1,  0, -1, -2, -3, -4]   [ 3,  5,  7,  9, 11, 13, 15, 17]   [   ...   ]
    // [ 4,  3,  2,  1,  0, -1, -2, -3]   [ 4,  6,  8, 10, 12, 14, 16, 18]   [   ...   ]
    // [ 5,  4,  3,  2,  1,  0, -1, -2]   [ 5,  7,  9, 11, 13, 15, 17, 19]   [   ...   ]
    // [ 6,  5,  4,  3,  2,  1,  0, -1]   [ 6,  8, 10, 12, 14, 16, 18, 20]   [   ...   ]
    // [ 7,  6,  5,  4,  3,  2,  1,  0]   [ 7,  9, 11, 13, 15, 17, 19, 21]   [   ...   ]

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
    auto nest = Using({ A, B }, ArgumentType::Input)
                    .Using({ C }, ArgumentType::InputOutput)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .ForAll(k, 0, N)
                    .Do([](Matrix A, Matrix B, Matrix C, Scalar i, Scalar j, Scalar k) {
#if 0 // DEBUGGING
                        InvokeForContext<ComputeContext>([&] {
                            std::cout << "C(" << i.Get<int>() << "," << j.Get<int>() << ") pointing at (kernel): " << std::hex << reinterpret_cast<intptr_t>(std::get<int*>(C(i, j).GetValue().GetUnderlyingData())) << std::dec << std::endl;
                        });
#endif
                        C(i, j) += A(i, k) * B(k, j);
                    });

    auto& schedule = nest.GetSchedule();
    auto i_b_o = schedule.Split(i, cacheARows);
    auto k_b_o = schedule.Split(k, cacheACols);
    schedule.Cache<CopyInputCopyOutput>(
        A,
        { i, k },
        { cacheARows, cacheACols },
        { i_b_o, k_b_o },
        RowMajorMatrixOrder);
    schedule.Cache<CopyInputCopyOutput>(
        B,
        { k, j },
        { cacheBRows, cacheBCols },
        { k_b_o },
        ColumnMajorMatrixOrder);

    auto i_o = schedule.Split(i, resultCacheRows);
    auto j_o = schedule.Split(j, resultCacheCols);
    schedule.SetOrder({ k_b_o, i_b_o, j_o, i_o, k, j, i });
    schedule.Cache<ZeroInputReduceOutput>(
        C,
        { i, j },
        { resultCacheRows, resultCacheCols },
        { i_o, j_o },
        RowMajorMatrixOrder);

    schedule.Unroll(i);
    schedule.Unroll(j);
    nest.Run();

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

Scalar MLAS_GEMM_GeneralCachingStrategy()
{
    const int OutputRows = 16;
    const int InnerDimension = 16;
    const int OutputColumns = 16;
    const int kUnroll = 4;
    const int cacheBRows = InnerDimension / 2;
    const int cacheBCols = OutputColumns / 2;
    const int stripeSize = cacheBCols / 2;
    const int vectorSize = stripeSize / 2;
    const int NumRowsInKernel = OutputRows / 8;
    const int NumColumnsInKernel = 2 * vectorSize;

    auto A = MakeIncrementingMatrix<int>(OutputRows, InnerDimension, "A");
    auto B = MakeIncrementingMatrix<int>(InnerDimension, OutputColumns, "B");
    auto C = MakeMatrix<int>(OutputRows, OutputColumns, "C");

    auto expected = MakeMatrix<int>(OutputRows, OutputColumns, "expected");
    ForRange(OutputRows, [&](Scalar m) {
        ForRange(OutputColumns, [&](Scalar n) {
            ForRange(InnerDimension, [&](Scalar k) {
                expected(m, n) += A(m, k) * B(k, n);
            });
        });
    });

    // Declare indexes
    loopnests::Index i("i"), j("j"), k("k");
    // Define LoopNest
    auto nest = Using({ A, B }, ArgumentType::Input)
                    .Using({ C }, ArgumentType::Output)
                    .ForAll(i, 0, OutputRows)
                    .ForAll(j, 0, OutputColumns)
                    .ForAll(k, 0, InnerDimension)
                    .Do([](Matrix A_, Matrix B_, Matrix C_, Scalar i_, Scalar j_, Scalar k_) {
                        C_(i_, j_) += B_(k_, j_) * A_(i_, k_);
                    });
    auto& schedule = nest.GetSchedule();

    auto topLevelI = i;
    auto topLevelJ = j;
    auto topLevelK = k;

    // Declare splits
    auto jCache = schedule.Split(j, cacheBCols);
    auto kCache = schedule.Split(k, cacheBRows);
    auto kBlock = schedule.Split(k, kUnroll);
    auto jKernelOuter2 = schedule.Split(j, NumColumnsInKernel);
    auto jKernelOuter = schedule.Split(j, vectorSize);
    auto iKernelOuter = schedule.Split(i, NumRowsInKernel);

    // Set the order
    schedule.SetOrder({ jCache, kCache, iKernelOuter, jKernelOuter2, kBlock, k, i, jKernelOuter, j });

    // Set up caching
    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheBInput";
    size_t maxCacheElts = cacheBRows * cacheBCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    schedule.Cache<GeneralCachingStrategy>(B,
                                           { topLevelK, topLevelJ },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheParams);

    ArgumentType argTypeC = ArgumentType::Output;
    std::string cacheNameC = "cacheCOutput";
    size_t maxCacheEltsC = NumRowsInKernel * NumColumnsInKernel;
    size_t fillThresholdC = maxCacheEltsC;
    std::function<void(Scalar, Scalar)> reduceFunctionC = SumReduce;
    auto extraCacheCParams = std::make_tuple(argTypeC,
                                             cacheNameC,
                                             maxCacheEltsC,
                                             fillThresholdC,
                                             reduceFunctionC,
                                             true);
    schedule.Cache<GeneralCachingStrategy>(C,
                                           { topLevelI, topLevelJ },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheCParams);

    // Set unrolling
    schedule.Unroll(jKernelOuter);
    schedule.Unroll(i);
    schedule.Unroll(k);

#if 0 // DEBUGGING
    auto loop = nest.GetUnderlyingLoopNest();
    DebugDump(loop);
#endif
    // Run the generator
    nest.Run();

    If(
        VerifySame(C, expected) == 0,
        [&] {
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

    return VerifySame(C, expected);
}

Scalar OneSplitBoundaryTest()
{
    const int M = 4;
    const int N = 3;
    const int split = 2;
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");
    auto expectedOutput = MakeIncrementingMatrix<int>(M, N, "expectedOutput");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::InputOutput)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
#if 0 // DEBUGGING
                        InvokeForContext<ComputeContext>([&] {
                            std::cout << "inner kernel, (i,j) == (" << i.Get<int>() << ", " << j.Get<int>() << ")" << std::endl;
                        });
#endif
                        output(i, j) = input(i, j);
                    });

    auto schedule = nest.GetSchedule();

    auto topLevelI = i;
    auto topLevelJ = j;
    auto iSplit = schedule.Split(i, split);
    auto jSplit = schedule.Split(j, split);

#if 0 // DEBUGGING
    auto kernel = loopnests::Kernel("split_log_kernel")
                              .Inputs()
                              .Indices(iSplit, jSplit)
                              .Define([](Scalar i, Scalar j) {
                                    InvokeForContext<ComputeContext>([&] {
                                        std::cout << "simple boundary split log kernel, (i,j) == (" << i.Get<int>() << ", " << j.Get<int>() << ")" << std::endl;
                                    });
                              });
    nest.GetUnderlyingLoopNest().AddKernel(kernel);
#endif

    schedule.SetOrder({ iSplit, jSplit, i, j });

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    Scalar ok = Allocate<int>(ScalarLayout);
    ok = 1;
    If(
        VerifySame(output, expectedOutput) == 0,
        [&] {
            ok = 0;
        })
        .Else([&] {
            auto value = output.GetValue();
            value.SetLayout({ { (int)output.Size() } });
            DebugPrintVector(value);
            DebugPrint("\n");
            auto expectedValue = expectedOutput.GetValue();
            expectedValue.SetLayout({ { (int)expectedOutput.Size() } });
            DebugPrintVector(expectedValue);
            DebugPrint("\n");
        });
    return ok;
}

Scalar TwoSplitBoundaryTest()
{
    const int M = 8;
    const int N = 7;
    const int bigSplit = 4;
    const int smallSplit = 2;
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");
    auto expectedOutput = MakeIncrementingMatrix<int>(M, N, "expectedOutput");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::InputOutput)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
#if 0 // DEBUGGING
                        InvokeForContext<ComputeContext>([&] {
                            std::cout << "inner kernel, (i,j) == (" << i.Get<int>() << ", " << j.Get<int>() << ")" << std::endl;
                        });
#endif
                        output(i, j) = input(i, j);
                    });

    auto schedule = nest.GetSchedule();

    auto topLevelI = i;
    auto topLevelJ = j;
    auto iBigSplit = schedule.Split(i, bigSplit);
    auto jBigSplit = schedule.Split(j, bigSplit);
    auto iSmallSplit = schedule.Split(i, smallSplit);
    auto jSmallSplit = schedule.Split(j, smallSplit);

#if 0 // DEBUGGING
    auto kernel = loopnests::Kernel("big_split_log_kernel")
                              .Inputs()
                              .Indices(iBigSplit, jBigSplit)
                              .Define([](Scalar i, Scalar j) {
                                    InvokeForContext<ComputeContext>([&] {
                                        std::cout << "simple boundary big split log kernel, (i,j) == (" << i.Get<int>() << ", " << j.Get<int>() << ")" << std::endl;
                                    });
                              });
    nest.GetUnderlyingLoopNest().AddKernel(kernel);
    auto kernel2 = loopnests::Kernel("small_split_log_kernel")
                              .Inputs()
                              .Indices(iSmallSplit, jSmallSplit)
                              .Define([](Scalar i, Scalar j) {
                                    InvokeForContext<ComputeContext>([&] {
                                        std::cout << "simple boundary Small split log kernel, (i,j) == (" << i.Get<int>() << ", " << j.Get<int>() << ")" << std::endl;
                                    });
                              });
    nest.GetUnderlyingLoopNest().AddKernel(kernel2);
#endif

    schedule.SetOrder({ iBigSplit, jBigSplit, iSmallSplit, jSmallSplit, i, j });

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    Scalar ok = Allocate<int>(ScalarLayout);
    ok = 1;
    If(
        VerifySame(output, expectedOutput) == 0,
        [&] {
            ok = 0;
        })
        .Else([&] {
            auto value = output.GetValue();
            value.SetLayout({ { (int)output.Size() } });
            DebugPrintVector(value);
            DebugPrint("\n");
            auto expectedValue = expectedOutput.GetValue();
            expectedValue.SetLayout({ { (int)expectedOutput.Size() } });
            DebugPrintVector(expectedValue);
            DebugPrint("\n");
        });
    return ok;
}

Scalar SplitLargerThanSizeBoundaryTest()
{
    const int M = 8;
    const int N = 3;
    const int split = 4;
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");
    auto expectedOutput = MakeIncrementingMatrix<int>(M, N, "expectedOutput");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::InputOutput)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
#if 0 // DEBUGGING
                        InvokeForContext<ComputeContext>([&] {
                            std::cout << "inner kernel, (i,j) == (" << i.Get<int>() << ", " << j.Get<int>() << ")" << std::endl;
                        });
#endif
                        output(i, j) = input(i, j);
                    });

    auto schedule = nest.GetSchedule();

    auto topLevelI = i;
    auto topLevelJ = j;
    auto iSplit = schedule.Split(i, split);
    auto jSplit = schedule.Split(j, split);

#if 0 // DEBUGGING
    auto kernel = loopnests::Kernel("split_log_kernel")
                              .Inputs()
                              .Indices(iSplit, jSplit)
                              .Define([](Scalar i, Scalar j) {
                                    InvokeForContext<ComputeContext>([&] {
                                        std::cout << "simple boundary split log kernel, (i,j) == (" << i.Get<int>() << ", " << j.Get<int>() << ")" << std::endl;
                                    });
                              });
    nest.GetUnderlyingLoopNest().AddKernel(kernel);
#endif

    schedule.SetOrder({ iSplit, jSplit, i, j });

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    Scalar ok = Allocate<int>(ScalarLayout);
    ok = 1;
    If(
        VerifySame(output, expectedOutput) == 0,
        [&] {
            ok = 0;
        })
        .Else([&] {
            auto value = output.GetValue();
            value.SetLayout({ { (int)output.Size() } });
            DebugPrintVector(value);
            DebugPrint("\n");
            auto expectedValue = expectedOutput.GetValue();
            expectedValue.SetLayout({ { (int)expectedOutput.Size() } });
            DebugPrintVector(expectedValue);
            DebugPrint("\n");
        });
    return ok;
}

Scalar TwoSplitsLargerThanSizeBoundaryTest()
{
    const int M = 8;
    const int N = 3;
    const int bigSplit = 4;
    const int smallSplit = 2;
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");
    auto expectedOutput = MakeIncrementingMatrix<int>(M, N, "expectedOutput");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::InputOutput)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
#if 0 // DEBUGGING
                        InvokeForContext<ComputeContext>([&] {
                            std::cout << "inner kernel, (i,j) == (" << i.Get<int>() << ", " << j.Get<int>() << ")" << std::endl;
                        });
#endif
                        output(i, j) = input(i, j);
                    });

    auto schedule = nest.GetSchedule();

    auto topLevelI = i;
    auto topLevelJ = j;
    auto iBigSplit = schedule.Split(i, bigSplit);
    auto jBigSplit = schedule.Split(j, bigSplit);
    auto iSmallSplit = schedule.Split(i, smallSplit);
    auto jSmallSplit = schedule.Split(j, smallSplit);

#if 0 // DEBUGGING
    auto kernel = loopnests::Kernel("big_split_log_kernel")
                              .Inputs()
                              .Indices(iBigSplit, jBigSplit)
                              .Define([](Scalar i, Scalar j) {
                                    InvokeForContext<ComputeContext>([&] {
                                        std::cout << "simple boundary big split log kernel, (i,j) == (" << i.Get<int>() << ", " << j.Get<int>() << ")" << std::endl;
                                    });
                              });
    nest.GetUnderlyingLoopNest().AddKernel(kernel);
    auto kernel2 = loopnests::Kernel("small_split_log_kernel")
                              .Inputs()
                              .Indices(iSmallSplit, jSmallSplit)
                              .Define([](Scalar i, Scalar j) {
                                    InvokeForContext<ComputeContext>([&] {
                                        std::cout << "simple boundary Small split log kernel, (i,j) == (" << i.Get<int>() << ", " << j.Get<int>() << ")" << std::endl;
                                    });
                              });
    nest.GetUnderlyingLoopNest().AddKernel(kernel2);
#endif

    schedule.SetOrder({ iBigSplit, jBigSplit, iSmallSplit, jSmallSplit, i, j });

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    Scalar ok = Allocate<int>(ScalarLayout);
    ok = 1;
    If(
        VerifySame(output, expectedOutput) == 0,
        [&] {
            ok = 0;
        })
        .Else([&] {
            auto value = output.GetValue();
            value.SetLayout({ { (int)output.Size() } });
            DebugPrintVector(value);
            DebugPrint("\n");
            auto expectedValue = expectedOutput.GetValue();
            expectedValue.SetLayout({ { (int)expectedOutput.Size() } });
            DebugPrintVector(expectedValue);
            DebugPrint("\n");
        });
    return ok;
}

Scalar LoopNest_api_tunable_parameters_test1()
{
    auto ok = MakeScalar<int>();

    // loopnest_kernel_4 will multiply row by 10 and add the column.
    std::vector<int> expectedValues{ 0, 1, 2, 3, 4, 5, 6, 7, 10, 11, 12, 13, 14, 15, 16, 17 };
    auto expected = Vector(expectedValues);

    TunableParameter j_o{ std::vector{ 2, 4 }, "j_o" };
    TunableParameter j_o_o{ std::vector{ 1, 2 }, "j_o_o" };
    TuningEngine engine(j_o, j_o_o);
    do
    {
        // Declare the output matrix and initialize its values to 0.
        auto output = MakeMatrix<int>(2, 8);

        (void)DeclareFunction("LoopNext_tunable_" + engine.ToString())
            .Decorated(false)
            .Parameters(output)
            .Define([&](Matrix matrix) {
                Index i("i"), j("j");

                auto nest = Using({ matrix }, ArgumentType::InputOutput)
                                .ForAll(i, 0, static_cast<int>(output.Rows()))
                                .ForAll(j, 0, static_cast<int>(output.Columns()))
                                .Do([](Matrix m, Scalar i, Scalar j) {
                                    Scalar v = Allocate(m.Type(), ScalarLayout);

                                    v = i * 10;
                                    v += j;

                                    m(i, j) = v;
                                });

                auto& schedule = nest.GetSchedule();
                schedule.Split(j, j_o);
                schedule.Split(j, j_o_o);

                nest.Run();
            })(output);

        // View the result as a Vector
        Vector actual = AsVector(AsFullView(output));

        // Verify that the actual result is what we expect
        If(ok == 0, [&] { ok = VerifySame(actual, expected); });
    } while (engine.Next());

    return ok;
}
} // namespace ell
