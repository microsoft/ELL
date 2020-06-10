////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CachingStrategy_test.cpp (value)
//  Authors:  Mason Remy
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CachingStrategy_test.h"
#include "TestUtil.h"

#include <value/include/CachingStrategies.h>
#include <value/include/ComputeContext.h>
#include <value/include/EmitterContext.h>
#include <value/include/FunctionDeclaration.h>
#include <value/include/LLVMContext.h>
#include <value/include/LoopNests.h>
#include <value/include/Matrix.h>
#include <value/include/Scalar.h>
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

namespace ell
{
// Tests of LoopNest caching strategies

Scalar BLASTCOPY_ValidateOutput_Test1()
{
    int N = 8;
    int cacheRows = N;
    int cacheCols = N;
    int stripeSize = 4;

    auto input = MakeIncrementingMatrix<int>(N, N, "input");
    auto output = MakeMatrix<int>(N, N, "output");
    auto expectedOutput = MakeIncrementingMatrix<int>(N, N, "expectedOutput");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);

    schedule.SetOrder({ iCache, jCache, jStripe, i, j });

    BLASTCopy cachingProvider{};
    std::tuple<int, Index, BoundaryConditionHandling> blasTCopyExtras = { stripeSize, jStripe, BoundaryConditionHandling::ZeroPadding };
    schedule.Cache(cachingProvider,
                   input,
                   { i, j },
                   { cacheRows, cacheCols },
                   { iCache, jCache },
                   std::nullopt, // Order isn't used by BLASTCopy
                   blasTCopyExtras);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    return VerifySame(output, expectedOutput);
}

// Test with smaller cache and stripe size than previous test
Scalar BLASTCOPY_ValidateOutput_Test2()
{
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input, expectedOutput
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7]
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // [16, 17, 18, 19, 20, 21, 22, 23]
    // [24, 25, 26, 27, 28, 29, 30, 31]
    // [32, 33, 34, 35, 36, 37, 38, 39]
    // [40, 41, 42, 43, 44, 45, 46, 47]
    // [48, 49, 50, 51, 52, 53, 54, 55]
    // [56, 57, 58, 59, 60, 61, 62, 63]
    auto input = MakeIncrementingMatrix<int>(N, N, "input");
    auto output = MakeMatrix<int>(N, N, "output");
    auto expectedOutput = MakeIncrementingMatrix<int>(N, N, "expectedOutput");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);

    schedule.SetOrder({ iCache, jCache, jStripe, i, j });

    BLASTCopy cachingProvider{};
    std::tuple<int, Index, BoundaryConditionHandling> blasTCopyExtras = { stripeSize, jStripe, BoundaryConditionHandling::ZeroPadding };
    schedule.Cache(cachingProvider,
                   input,
                   { i, j },
                   { cacheRows, cacheCols },
                   { iCache, jCache },
                   std::nullopt, // Order isn't used by BLASTCopy
                   blasTCopyExtras);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    return VerifySame(output, expectedOutput);
}

Scalar BLASTCOPY_ValidateMemory_Test1()
{
    int N = 8;
    int cacheRows = N;
    int cacheCols = N;
    int stripeSize = 4;

    auto input = MakeIncrementingMatrix<int>(N, N, "input");
    auto output = MakeMatrix<int>(N, N, "output");

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7]
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // [16, 17, 18, 19, 20, 21, 22, 23]
    // [24, 25, 26, 27, 28, 29, 30, 31]
    // [32, 33, 34, 35, 36, 37, 38, 39]
    // [40, 41, 42, 43, 44, 45, 46, 47]
    // [48, 49, 50, 51, 52, 53, 54, 55]
    // [56, 57, 58, 59, 60, 61, 62, 63]
    // clang-format off
    Vector expectedCached =
        {
             0,  1,  2,  3,
             8,  9, 10, 11,
            16, 17, 18, 19,
            24, 25, 26, 27,
            32, 33, 34, 35,
            40, 41, 42, 43,
            48, 49, 50, 51,
            56, 57, 58, 59,

             4,  5,  6,  7,
            12, 13, 14, 15,
            20, 21, 22, 23,
            28, 29, 30, 31,
            36, 37, 38, 39,
            44, 45, 46, 47,
            52, 53, 54, 55,
            60, 61, 62, 63
        };
    // clang-format on

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);

    schedule.SetOrder({ iCache, jCache, jStripe, i, j });

    BLASTCopy cachingProvider{};
    std::tuple<int, Index, BoundaryConditionHandling> blasTCopyExtras = { stripeSize, jStripe, BoundaryConditionHandling::ZeroPadding };
    schedule.Cache(cachingProvider,
                   input,
                   { i, j },
                   { cacheRows, cacheCols },
                   { iCache, jCache },
                   std::nullopt, // Order isn't used by BLASTCopy
                   blasTCopyExtras);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    // Examine the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    rawCacheValue.SetLayout({ { (int)rawCacheValue.GetLayout().GetMemorySize() } });
    auto cacheVector = Vector(rawCacheValue);

    return VerifySame(cacheVector, expectedCached);
}

// Smaller stripe size than previous test
Scalar BLASTCOPY_ValidateMemory_Test2()
{
    int N = 8;
    int cacheRows = N;
    int cacheCols = N;
    int stripeSize = 2;

    auto input = MakeIncrementingMatrix<int>(N, N, "input");
    auto output = MakeMatrix<int>(N, N, "output");

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7]
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // [16, 17, 18, 19, 20, 21, 22, 23]
    // [24, 25, 26, 27, 28, 29, 30, 31]
    // [32, 33, 34, 35, 36, 37, 38, 39]
    // [40, 41, 42, 43, 44, 45, 46, 47]
    // [48, 49, 50, 51, 52, 53, 54, 55]
    // [56, 57, 58, 59, 60, 61, 62, 63]
    // clang-format off
    Vector expectedCached =
        {
             0,  1,
             8,  9,
            16, 17,
            24, 25,
            32, 33,
            40, 41,
            48, 49,
            56, 57,

             2,  3,
            10, 11,
            18, 19,
            26, 27,
            34, 35,
            42, 43,
            50, 51,
            58, 59,

             4,  5,
            12, 13,
            20, 21,
            28, 29,
            36, 37,
            44, 45,
            52, 53,
            60, 61,

             6,  7,
            14, 15,
            22, 23,
            30, 31,
            38, 39,
            46, 47,
            54, 55,
            62, 63
        };
    // clang-format on

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);

    schedule.SetOrder({ iCache, jCache, jStripe, i, j });

    BLASTCopy cachingProvider{};
    std::tuple<int, Index, BoundaryConditionHandling> blasTCopyExtras = { stripeSize, jStripe, BoundaryConditionHandling::ZeroPadding };
    schedule.Cache(cachingProvider,
                   input,
                   { i, j },
                   { cacheRows, cacheCols },
                   { iCache, jCache },
                   std::nullopt, // Order isn't used by BLASTCopy
                   blasTCopyExtras);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    // Examine the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    rawCacheValue.SetLayout({ { (int)rawCacheValue.GetLayout().GetMemorySize() } });
    auto cacheVector = Vector(rawCacheValue);

    return VerifySame(cacheVector, expectedCached);
}

// Same stripe size as previous test, but don't cache entire matrix at once
Scalar BLASTCOPY_ValidateMemory_Test3()
{
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    auto input = MakeIncrementingMatrix<int>(N, N, "input");
    auto output = MakeMatrix<int>(N, N, "output");

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7]
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // [16, 17, 18, 19, 20, 21, 22, 23]
    // [24, 25, 26, 27, 28, 29, 30, 31]
    // [32, 33, 34, 35, 36, 37, 38, 39]
    // [40, 41, 42, 43, 44, 45, 46, 47]
    // [48, 49, 50, 51, 52, 53, 54, 55]
    // [56, 57, 58, 59, 60, 61, 62, 63]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  1,
             8,  9,
            16, 17,
            24, 25,

             2,  3,
            10, 11,
            18, 19,
            26, 27,
        };
    Vector expectedCachedUpperRight =
        {
             4,  5,
            12, 13,
            20, 21,
            28, 29,

             6,  7,
            14, 15,
            22, 23,
            30, 31
        };
    Vector expectedCachedLowerLeft =
        {
            32, 33,
            40, 41,
            48, 49,
            56, 57,

            34, 35,
            42, 43,
            50, 51,
            58, 59,
        };
    Vector expectedCachedLowerRight =
        {
            36, 37,
            44, 45,
            52, 53,
            60, 61,

            38, 39,
            46, 47,
            54, 55,
            62, 63
        };
    // clang-format on

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto topLevelI = i;
    auto topLevelJ = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);

    schedule.SetOrder({ iCache, jCache, jStripe, i, j });

    BLASTCopy cachingProvider{};
    std::tuple<int, Index, BoundaryConditionHandling> blasTCopyExtras = { stripeSize, jStripe, BoundaryConditionHandling::ZeroPadding };
    schedule.Cache(cachingProvider,
                   input,
                   { i, j },
                   { cacheRows, cacheCols },
                   { iCache, jCache },
                   std::nullopt, // Order isn't used by BLASTCopy
                   blasTCopyExtras);

    // Get a handle to the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    int rawCacheSize = (int)rawCacheValue.GetLayout().NumElements();

    auto cachedUpperLeft = MakeVector<int>(rawCacheSize);
    auto cachedUpperRight = MakeVector<int>(rawCacheSize);
    auto cachedLowerLeft = MakeVector<int>(rawCacheSize);
    auto cachedLowerRight = MakeVector<int>(rawCacheSize);

    // Add a low level API kernel to access the underlying cache after it has been filled
    auto cacheSpyKernel = loopnests::Kernel("cache_spy_kernel")
                              .Inputs(rawCacheValue, cachedUpperLeft, cachedUpperRight, cachedLowerLeft, cachedLowerRight)
                              .Indices(topLevelI, topLevelJ)
                              .Define([cacheRows, cacheCols](Value rawCacheValue, Vector cachedUpperLeft, Vector cachedUpperRight, Vector cachedLowerLeft, Vector cachedLowerRight, Scalar i, Scalar j) {
                                  auto cacheView = rawCacheValue;
                                  cacheView.SetLayout({ { (int)rawCacheValue.GetLayout().NumElements() } });
                                  auto vectorCacheView = Vector(cacheView);
                                  If(i == 0,
                                     [&]() {
                                         // TODO : remove nested if's
                                         If(j == 0,
                                            [&]() {
                                                cachedUpperLeft = vectorCacheView;
                                            })
                                             .ElseIf(j == cacheCols,
                                                     [&]() {
                                                         cachedUpperRight = vectorCacheView;
                                                     });
                                     })
                                      .ElseIf(i == cacheRows,
                                              [&]() {
                                                  If(j == 0, [&]() {
                                                      cachedLowerLeft = vectorCacheView;
                                                  }).ElseIf(j == cacheCols, [&]() {
                                                      cachedLowerRight = vectorCacheView;
                                                  });
                                              });
                              });
    auto cacheSpyPosition = loopnests::CodePositionConstraints{ loopnests::LoopFragmentType::body, { iCache, jCache }, {} };
    nest.GetUnderlyingLoopNest().AddKernel(cacheSpyKernel, cacheSpyPosition);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    auto ok = MakeScalar<int>("ok");
    ok = 1;
    auto printError = [&] {
        DebugPrint("Upper Left:");
        DebugPrintVector(cachedUpperLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Upper Right:");
        DebugPrintVector(cachedUpperRight);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperRight);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Lower Left:");
        DebugPrintVector(cachedLowerLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedLowerLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Lower Right:");
        DebugPrintVector(cachedLowerRight);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedLowerRight);
        DebugPrint("\n");
        DebugPrint("\n");
    };
    // TODO : replace nested if's
    If(VerifySame(cachedUpperLeft, expectedCachedUpperLeft) == 0, [&]() {
        If(VerifySame(cachedUpperRight, expectedCachedUpperRight) == 0, [&]() {
            If(VerifySame(cachedLowerLeft, expectedCachedLowerLeft) == 0, [&]() {
                If(VerifySame(cachedLowerRight, expectedCachedLowerRight) == 0, [&]() {
                    ok = 0;
                }).Else(printError);
            }).Else(printError);
        }).Else(printError);
    }).Else(printError);
    return ok;
}

Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(int M, int N, int cacheRows, int cacheCols, int stripeSize)
{
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");
    auto expectedOutput = MakeIncrementingMatrix<int>(M, N, "expectedOutput");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);

    schedule.SetOrder({ iCache, jCache, jStripe, i, j });

    BLASTCopy cachingProvider{};
    std::tuple<int, Index, BoundaryConditionHandling> blasTCopyExtras = { stripeSize, jStripe, BoundaryConditionHandling::ZeroPadding };
    schedule.Cache(cachingProvider,
                   input,
                   { i, j },
                   { cacheRows, cacheCols },
                   { iCache, jCache },
                   std::nullopt, // Order isn't used by BLASTCopy
                   blasTCopyExtras);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    return VerifySame(output, expectedOutput);
}

// input matrix rows evenly divides cache rows
// input matrix cols doesn't evenly divide cache cols
Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test1()
{
    int M = 8;
    int N = 7; // N doesn't evenly divide the number of cache columns
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

// input matrix rows evenly divides cache rows
// input matrix cols doesn't evenly divide cache cols but does evenly divide stripeSize
Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test2()
{
    int M = 8;
    int N = 6; // N doesn't evenly divide the number of cache columns, but does evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

// input matrix rows doesn't evenly divides cache rows
// input matrix cols doesn't evenly divide cache cols
Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test3()
{
    int M = 6;
    int N = 7; // N doesn't evenly divide the number of cache columns
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

// input matrix rows doesn't evenly divides cache rows
// input matrix cols doesn't evenly divide cache cols but does evenly divide stripe size
Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test4()
{
    int M = 6;
    int N = 6; // N doesn't evenly divide the number of cache columns, but does evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

// input matrix rows evenly divides cache rows
// input matrix cols < cache cols, doesn't evenly divide stripe size
Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test5()
{
    int M = 8;
    int N = 3; // N < cache columns, doesn't evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

// input matrix rows evenly divides cache rows
// input matrix cols < cache cols, evenly divides stripe size
Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test6()
{
    int M = 8;
    int N = 2; // N < cache columns, does evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

// input matrix rows < cache rows
// input matrix cols < cache cols, doesn't evenly divides stripe size
Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test7()
{
    int M = 3;
    int N = 3; // N < cache columns, doesn't evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

// input matrix rows < cache rows
// input matrix cols < cache cols, does evenly divides stripe size
Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test8()
{
    int M = 2;
    int N = 2; // N < cache columns, does evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

// input matrix rows < cache rows
// input matrix cols multiple of cache cols
Scalar BLASTCOPY_ValidateOutput_BoundaryCondition_Test9()
{
    int M = 2;
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Runner(int M, int N, int cacheRows, int cacheCols, int stripeSize, Vector expectedCachedUpperLeft, Vector expectedCachedUpperRight, Vector expectedCachedLowerLeft, Vector expectedCachedLowerRight)
{
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto topLevelI = i;
    auto topLevelJ = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);

    schedule.SetOrder({ iCache, jCache, jStripe, i, j });

    BLASTCopy cachingProvider{};
    std::tuple<int, Index, BoundaryConditionHandling> blasTCopyExtras = { stripeSize, jStripe, BoundaryConditionHandling::ZeroPadding };
    schedule.Cache(cachingProvider,
                   input,
                   { i, j },
                   { cacheRows, cacheCols },
                   { iCache, jCache },
                   std::nullopt, // Order isn't used by BLASTCopy
                   blasTCopyExtras);

    // Get a handle to the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    int rawCacheSize = (int)rawCacheValue.GetLayout().NumElements();

    auto cachedUpperLeft = MakeVector<int>(rawCacheSize);
    auto cachedUpperRight = MakeVector<int>(rawCacheSize);
    auto cachedLowerLeft = MakeVector<int>(rawCacheSize);
    auto cachedLowerRight = MakeVector<int>(rawCacheSize);

    // Add a low level API kernel to access the underlying cache after it has been filled
    auto cacheSpyKernel = loopnests::Kernel("cache_spy_kernel")
                              .Inputs(rawCacheValue, cachedUpperLeft, cachedUpperRight, cachedLowerLeft, cachedLowerRight)
                              .Indices(topLevelI, topLevelJ)
                              .Define([cacheRows, cacheCols](Value rawCacheValue, Vector cachedUpperLeft, Vector cachedUpperRight, Vector cachedLowerLeft, Vector cachedLowerRight, Scalar i, Scalar j) {
                                  auto cacheView = rawCacheValue;
                                  cacheView.SetLayout({ { (int)rawCacheValue.GetLayout().NumElements() } });
                                  auto vectorCacheView = Vector(cacheView);
                                  If(i == 0,
                                     [&]() {
                                         // TODO : remove nested if's
                                         If(j == 0,
                                            [&]() {
                                                cachedUpperLeft = vectorCacheView;
                                            })
                                             .ElseIf(j == cacheCols,
                                                     [&]() {
                                                         cachedUpperRight = vectorCacheView;
                                                     });
                                     })
                                      .ElseIf(i == cacheRows,
                                              [&]() {
                                                  If(j == 0, [&]() {
                                                      cachedLowerLeft = vectorCacheView;
                                                  }).ElseIf(j == cacheCols, [&]() {
                                                      cachedLowerRight = vectorCacheView;
                                                  });
                                              });
                              });
    auto cacheSpyPosition = loopnests::CodePositionConstraints{ loopnests::LoopFragmentType::body, { iCache, jCache }, {} };
    nest.GetUnderlyingLoopNest().AddKernel(cacheSpyKernel, cacheSpyPosition);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    auto ok = MakeScalar<int>("ok");
    ok = 1;
    auto printError = [&] {
        DebugPrint("Upper Left:");
        DebugPrintVector(cachedUpperLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Upper Right:");
        DebugPrintVector(cachedUpperRight);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperRight);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Lower Left:");
        DebugPrintVector(cachedLowerLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedLowerLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Lower Right:");
        DebugPrintVector(cachedLowerRight);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedLowerRight);
        DebugPrint("\n");
        DebugPrint("\n");
    };
    // TODO : replace nested if's
    If(VerifySame(cachedUpperLeft, expectedCachedUpperLeft) == 0, [&]() {
        If(VerifySame(cachedUpperRight, expectedCachedUpperRight) == 0, [&]() {
            If(VerifySame(cachedLowerLeft, expectedCachedLowerLeft) == 0, [&]() {
                If(VerifySame(cachedLowerRight, expectedCachedLowerRight) == 0, [&]() {
                    ok = 0;
                }).Else(printError);
            }).Else(printError);
        }).Else(printError);
    }).Else(printError);
    return ok;
}

Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Runner_LeftCachesOnly(int M, int N, int cacheRows, int cacheCols, int stripeSize, Vector expectedCachedUpperLeft, Vector expectedCachedLowerLeft)
{
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto topLevelI = i;
    auto topLevelJ = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);

    schedule.SetOrder({ iCache, jCache, jStripe, i, j });

    BLASTCopy cachingProvider{};
    std::tuple<int, Index, BoundaryConditionHandling> blasTCopyExtras = { stripeSize, jStripe, BoundaryConditionHandling::ZeroPadding };
    schedule.Cache(cachingProvider,
                   input,
                   { i, j },
                   { cacheRows, cacheCols },
                   { iCache, jCache },
                   std::nullopt, // Order isn't used by BLASTCopy
                   blasTCopyExtras);

    // Get a handle to the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    int rawCacheSize = (int)rawCacheValue.GetLayout().NumElements();

    // No right caches when N < cacheCols
    auto cachedUpperLeft = MakeVector<int>(rawCacheSize);
    auto cachedLowerLeft = MakeVector<int>(rawCacheSize);

    // Add a low level API kernel to access the underlying cache after it has been filled
    auto cacheSpyKernel = loopnests::Kernel("cache_spy_kernel")
                              .Inputs(rawCacheValue, cachedUpperLeft, cachedLowerLeft)
                              .Indices(topLevelI, topLevelJ)
                              .Define([cacheRows](Value rawCacheValue, Vector cachedUpperLeft, Vector cachedLowerLeft, Scalar i, Scalar j) {
                                  auto cacheView = rawCacheValue;
                                  cacheView.SetLayout({ { (int)rawCacheValue.GetLayout().NumElements() } });
                                  auto vectorCacheView = Vector(cacheView);
                                  If(i == 0,
                                     [&]() {
                                         // TODO : remove nested if's
                                         If(j == 0,
                                            [&]() {
                                                cachedUpperLeft = vectorCacheView;
                                            });
                                     })
                                      .ElseIf(i == cacheRows,
                                              [&]() {
                                                  If(j == 0, [&]() {
                                                      cachedLowerLeft = vectorCacheView;
                                                  });
                                              });
                              });
    auto cacheSpyPosition = loopnests::CodePositionConstraints{ loopnests::LoopFragmentType::body, { iCache, jCache }, {} };
    nest.GetUnderlyingLoopNest().AddKernel(cacheSpyKernel, cacheSpyPosition);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    auto ok = MakeScalar<int>("ok");
    ok = 1;
    auto printError = [&] {
        DebugPrint("Upper Left:");
        DebugPrintVector(cachedUpperLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Lower Left:");
        DebugPrintVector(cachedLowerLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedLowerLeft);
        DebugPrint("\n");
        DebugPrint("\n");
    };
    // TODO : replace nested if's
    If(VerifySame(cachedUpperLeft, expectedCachedUpperLeft) == 0, [&]() {
        If(VerifySame(cachedLowerLeft, expectedCachedLowerLeft) == 0, [&]() {
            ok = 0;
        }).Else(printError);
    }).Else(printError);
    return ok;
}

Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Runner_UpperCachesOnly(int M, int N, int cacheRows, int cacheCols, int stripeSize, Vector expectedCachedUpperLeft, Vector expectedCachedUpperRight)
{
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto topLevelI = i;
    auto topLevelJ = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);

    schedule.SetOrder({ iCache, jCache, jStripe, i, j });

    BLASTCopy cachingProvider{};
    std::tuple<int, Index, BoundaryConditionHandling> blasTCopyExtras = { stripeSize, jStripe, BoundaryConditionHandling::ZeroPadding };
    schedule.Cache(cachingProvider,
                   input,
                   { i, j },
                   { cacheRows, cacheCols },
                   { iCache, jCache },
                   std::nullopt, // Order isn't used by BLASTCopy
                   blasTCopyExtras);

    // Get a handle to the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    int rawCacheSize = (int)rawCacheValue.GetLayout().NumElements();

    auto cachedUpperLeft = MakeVector<int>(rawCacheSize);
    auto cachedUpperRight = MakeVector<int>(rawCacheSize);

    // Add a low level API kernel to access the underlying cache after it has been filled
    auto cacheSpyKernel = loopnests::Kernel("cache_spy_kernel")
                              .Inputs(rawCacheValue, cachedUpperLeft, cachedUpperRight)
                              .Indices(topLevelI, topLevelJ)
                              .Define([cacheCols](Value rawCacheValue, Vector cachedUpperLeft, Vector cachedUpperRight, Scalar i, Scalar j) {
                                  auto cacheView = rawCacheValue;
                                  cacheView.SetLayout({ { (int)rawCacheValue.GetLayout().NumElements() } });
                                  auto vectorCacheView = Vector(cacheView);
                                  If(i == 0,
                                     [&]() {
                                         // TODO : remove nested if's
                                         If(j == 0,
                                            [&]() {
                                                cachedUpperLeft = vectorCacheView;
                                            })
                                             .ElseIf(j == cacheCols,
                                                     [&]() {
                                                         cachedUpperRight = vectorCacheView;
                                                     });
                                     });
                              });
    auto cacheSpyPosition = loopnests::CodePositionConstraints{ loopnests::LoopFragmentType::body, { iCache, jCache }, {} };
    nest.GetUnderlyingLoopNest().AddKernel(cacheSpyKernel, cacheSpyPosition);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    auto ok = MakeScalar<int>("ok");
    ok = 1;
    auto printError = [&] {
        DebugPrint("Upper Left:");
        DebugPrintVector(cachedUpperLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Upper Right:");
        DebugPrintVector(cachedUpperRight);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperRight);
        DebugPrint("\n");
        DebugPrint("\n");
    };
    // TODO : replace nested if's
    If(VerifySame(cachedUpperLeft, expectedCachedUpperLeft) == 0, [&]() {
        If(VerifySame(cachedUpperRight, expectedCachedUpperRight) == 0, [&]() {
            ok = 0;
        }).Else(printError);
    }).Else(printError);
    return ok;
}

Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Runner_UpperLeftCacheOnly(int M, int N, int cacheRows, int cacheCols, int stripeSize, Vector expectedCachedUpperLeft)
{
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto topLevelI = i;
    auto topLevelJ = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);

    schedule.SetOrder({ iCache, jCache, jStripe, i, j });

    BLASTCopy cachingProvider{};
    std::tuple<int, Index, BoundaryConditionHandling> blasTCopyExtras = { stripeSize, jStripe, BoundaryConditionHandling::ZeroPadding };
    schedule.Cache(cachingProvider,
                   input,
                   { i, j },
                   { cacheRows, cacheCols },
                   { iCache, jCache },
                   std::nullopt, // Order isn't used by BLASTCopy
                   blasTCopyExtras);

    // Get a handle to the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    int rawCacheSize = (int)rawCacheValue.GetLayout().NumElements();

    // No right caches when N < cacheCols
    auto cachedUpperLeft = MakeVector<int>(rawCacheSize);

    // Add a low level API kernel to access the underlying cache after it has been filled
    auto cacheSpyKernel = loopnests::Kernel("cache_spy_kernel")
                              .Inputs(rawCacheValue, cachedUpperLeft)
                              .Indices(topLevelI, topLevelJ)
                              .Define([](Value rawCacheValue, Vector cachedUpperLeft, Scalar i, Scalar j) {
                                  auto cacheView = rawCacheValue;
                                  cacheView.SetLayout({ { (int)rawCacheValue.GetLayout().NumElements() } });
                                  auto vectorCacheView = Vector(cacheView);
                                  If(i == 0,
                                     [&]() {
                                         // TODO : remove nested if's
                                         If(j == 0,
                                            [&]() {
                                                cachedUpperLeft = vectorCacheView;
                                            });
                                     });
                              });
    auto cacheSpyPosition = loopnests::CodePositionConstraints{ loopnests::LoopFragmentType::body, { iCache, jCache }, {} };
    nest.GetUnderlyingLoopNest().AddKernel(cacheSpyKernel, cacheSpyPosition);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    return VerifySame(cachedUpperLeft, expectedCachedUpperLeft);
}

Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test1()
{
    int M = 8; // M does evenly divide cache rows
    int N = 7; // N doesn't evenly divide cache columns
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6],
    // [ 7,  8,  9, 10, 11, 12, 13],
    // [14, 15, 16, 17, 18, 19, 20],
    // [21, 22, 23, 24, 25, 26, 27],
    // [28, 29, 30, 31, 32, 33, 34],
    // [35, 36, 37, 38, 39, 40, 41],
    // [42, 43, 44, 45, 46, 47, 48],
    // [49, 50, 51, 52, 53, 54, 55]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  1,
             7,  8,
            14, 15,
            21, 22,

             2,  3,
             9, 10,
            16, 17,
            23, 24,
        };
    Vector expectedCachedUpperRight =
        {
             4,  5,
            11, 12,
            18, 19,
            25, 26,

             6,  0,
            13,  0,
            20,  0,
            27,  0
        };
    Vector expectedCachedLowerLeft =
        {
            28, 29,
            35, 36,
            42, 43,
            49, 50,

            30, 31,
            37, 38,
            44, 45,
            51, 52,
        };
    Vector expectedCachedLowerRight =
        {
            32, 33,
            39, 40,
            46, 47,
            53, 54,

            34,  0,
            41,  0,
            48,  0,
            55,  0
        };
    // clang-format on

    return BLASTCOPY_ValidateMemory_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight, expectedCachedLowerLeft, expectedCachedLowerRight);
}

Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test2()
{
    int M = 8; // M does evenly divide cache rows
    int N = 6; // N doesn't evenly divide cache columns, but does evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5],
    // [ 6,  7,  8,  9, 10, 11],
    // [12, 13, 14, 15, 16, 17],
    // [18, 19, 20, 21, 22, 23],
    // [24, 25, 26, 27, 28, 29],
    // [30, 31, 32, 33, 34, 35],
    // [36, 37, 38, 39, 40, 41],
    // [42, 43, 44, 45, 46, 47]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  1,
             6,  7,
            12, 13,
            18, 19,

             2,  3,
             8,  9,
            14, 15,
            20, 21,
        };
    Vector expectedCachedUpperRight =
        {
             4,  5,
            10, 11,
            16, 17,
            22, 23,

             0,  0,
             0,  0,
             0,  0,
             0,  0
        };
    Vector expectedCachedLowerLeft =
        {
            24, 25,
            30, 31,
            36, 37,
            42, 43,

            26, 27,
            32, 33,
            38, 39,
            44, 45,
        };
    Vector expectedCachedLowerRight =
        {
            28, 29,
            34, 35,
            40, 41,
            46, 47,

             0,  0,
             0,  0,
             0,  0,
             0,  0
        };
    // clang-format on

    return BLASTCOPY_ValidateMemory_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight, expectedCachedLowerLeft, expectedCachedLowerRight);
}

// input matrix rows doesn't evenly divides cache rows
// input matrix cols doesn't evenly divide cache cols
Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test3()
{
    int M = 6;
    int N = 7; // N doesn't evenly divide the number of cache columns
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6],
    // [ 7,  8,  9, 10, 11, 12, 13],
    // [14, 15, 16, 17, 18, 19, 20],
    // [21, 22, 23, 24, 25, 26, 27],
    // [28, 29, 30, 31, 32, 33, 34],
    // [35, 36, 37, 38, 39, 40, 41],
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  1,
             7,  8,
            14, 15,
            21, 22,

             2,  3,
             9, 10,
            16, 17,
            23, 24,
        };
    Vector expectedCachedUpperRight =
        {
             4,  5,
            11, 12,
            18, 19,
            25, 26,

             6,  0,
            13,  0,
            20,  0,
            27,  0
        };
    
    // Check that it gets reviewed correctly to keep the cached data contiguous
    Vector expectedCachedLowerLeft =
        {
            28, 29,
            35, 36,
            30, 31,
            37, 38,

             0,  0,
             0,  0,            
             0,  0,
             0,  0,
        };
    Vector expectedCachedLowerRight =
        {
            32, 33,
            39, 40,
            34,  0,
            41,  0,

             0,  0,
             0,  0,
             0,  0,
             0,  0
        };
    // clang-format on

    return BLASTCOPY_ValidateMemory_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight, expectedCachedLowerLeft, expectedCachedLowerRight);
}

// input matrix rows doesn't evenly divides cache rows
// input matrix cols doesn't evenly divide cache cols but does evenly divide stripe size
Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test4()
{
    int M = 6;
    int N = 6; // N doesn't evenly divide the number of cache columns, but does evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5],
    // [ 6,  7,  8,  9, 10, 11],
    // [12, 13, 14, 15, 16, 17],
    // [18, 19, 20, 21, 22, 23],
    // [24, 25, 26, 27, 28, 29],
    // [30, 31, 32, 33, 34, 35]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  1,
             6,  7,
            12, 13,
            18, 19,

             2,  3,
             8,  9,
            14, 15,
            20, 21,
        };
    Vector expectedCachedUpperRight =
        {
             4,  5,
            10, 11,
            16, 17,
            22, 23,

             0,  0,
             0,  0,
             0,  0,
             0,  0
        };
    Vector expectedCachedLowerLeft =
        {
            24, 25,
            30, 31,
            26, 27,
            32, 33,

             0,  0,
             0,  0,
             0,  0,
             0,  0,
        };
    Vector expectedCachedLowerRight =
        {
            28, 29,
            34, 35,
             0,  0,
             0,  0,

             0,  0,
             0,  0,
             0,  0,
             0,  0
        };
    // clang-format on
    return BLASTCOPY_ValidateMemory_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight, expectedCachedLowerLeft, expectedCachedLowerRight);
}

// input matrix rows evenly divides cache rows
// input matrix cols < cache cols, doesn't evenly divide stripe size
Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test5()
{
    int M = 8;
    int N = 3; // N < cache columns, doesn't evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1,  2],
    // [ 3,  4,  5],
    // [ 6,  7,  8],
    // [ 9, 10, 11],
    // [12, 13, 14],
    // [15, 16, 17],
    // [18, 19, 20],
    // [21, 22, 23]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  1,
             3,  4,
             6,  7,
             9, 10,

             2, 0,
             5, 0,
             8, 0,
            11, 0,
        };
    Vector expectedCachedLowerLeft =
        {
            12, 13,
            15, 16,
            18, 19,
            21, 22,

            14,  0,
            17,  0,
            20,  0,
            23,  0,
        };
    // clang-format on

    return BLASTCOPY_ValidateMemory_BoundaryCondition_Runner_LeftCachesOnly(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft, expectedCachedLowerLeft);
}

// input matrix rows evenly divides cache rows
// input matrix cols < cache cols, evenly divides stripe size
Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test6()
{
    int M = 8;
    int N = 2; // N < cache columns, does evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1],
    // [ 2,  3],
    // [ 4,  5],
    // [ 6,  7],
    // [ 8,  9],
    // [10, 11],
    // [12, 13],
    // [14, 15]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  1,
             2,  3,
             4,  5,
             6,  7,

             0,  0,
             0,  0,
             0,  0,
             0,  0,
        };
    Vector expectedCachedLowerLeft =
        {
             8,  9,
            10, 11,
            12, 13,
            14, 15,

             0,  0,
             0,  0,
             0,  0,
             0,  0,
        };
    // clang-format on
    return BLASTCOPY_ValidateMemory_BoundaryCondition_Runner_LeftCachesOnly(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft, expectedCachedLowerLeft);
}

// input matrix rows < cache rows
// input matrix cols < cache cols, doesn't evenly divides stripe size
Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test7()
{
    int M = 3;
    int N = 3; // N < cache columns, doesn't evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input
    // A:
    // [0, 1, 2],
    // [3, 4, 5],
    // [6, 7, 8]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
            0, 1,
            3, 4,
            6, 7,
            2, 0,

            5, 0,
            8, 0,
            0, 0,
            0, 0
        };
    // clang-format on
    return BLASTCOPY_ValidateMemory_BoundaryCondition_Runner_UpperLeftCacheOnly(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft);
}

// input matrix rows < cache rows
// input matrix cols < cache cols, does evenly divides stripe size
Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test8()
{
    int M = 2;
    int N = 2; // N < cache columns, does evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0, 1],
    // [ 2, 3]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
            0, 1,
            2, 3,
            0, 0,
            0, 0,

            0, 0,
            0, 0,
            0, 0,
            0, 0,
        };
    // clang-format on
    return BLASTCOPY_ValidateMemory_BoundaryCondition_Runner_UpperLeftCacheOnly(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft);
}

// input matrix rows < cache rows
// input matrix cols multiple of cache cols
Scalar BLASTCOPY_ValidateMemory_BoundaryCondition_Test9()
{
    int M = 2;
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7],
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  1,
             8,  9,
             2,  3,
            10, 11,

            0, 0,
            0, 0,
            0, 0,
            0, 0,
        };
    Vector expectedCachedUpperRight =
        {
             4,  5,
            12, 13,
             6,  7,
            14, 15,

             0,  0,
             0,  0,
             0,  0,
             0,  0
        };
    // clang-format on
    return BLASTCOPY_ValidateMemory_BoundaryCondition_Runner_UpperCachesOnly(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight);
}

// Convolution caching tests

// General Caching Strategy

Scalar GeneralCachingStrategy_ValidateOutput_Test1()
{
    // Square matrix tiling
    loopnests::Index i("i"), j("j");

    const int Rows = 8;
    const int Columns = 8;
    const int SplitSize = 4;

    auto input = MakeIncrementingMatrix<int>(Rows, Columns, "input");
    auto output = MakeMatrix<int>(Rows, Columns, "output");

    // Define LoopNest
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, Rows)
                    .ForAll(j, 0, Columns)
                    .Do([=](Matrix input_, Matrix output_, Scalar i_, Scalar j_) {
                        output_(i_, j_) = input_(i_, j_);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iBlock = schedule.Split(i, SplitSize);
    auto jBlock = schedule.Split(j, SplitSize);

    std::vector<Index> orderedIndices = { iBlock, jBlock, i, j };
    schedule.SetOrder(orderedIndices);

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = SplitSize * SplitSize;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);

    schedule.Cache<GeneralCachingStrategy>(input,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheParams);

#if 0 // DEBUGGING
    auto loop = nest.GetUnderlyingLoopNest();
    DebugDump(loop);
#endif

    nest.Run();

    return VerifySame(output, input);
}

Scalar GeneralCachingStrategy_ValidateMemory_Test1()
{
    // Square matrix tiling
    loopnests::Index i("i"), j("j");

    const int Rows = 8;
    const int Columns = 8;
    const int SplitSize = 4;

    // input
    // [ 0,  1,  2,  3,  4,  5,  6,  7]
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // [16, 17, 18, 19, 20, 21, 22, 23]
    // [24, 25, 26, 27, 28, 29, 30, 31]
    // [32, 33, 34, 35, 36, 37, 38, 39]
    // [40, 41, 42, 43, 44, 45, 46, 47]
    // [48, 49, 50, 51, 52, 53, 54, 55]
    // [56, 57, 58, 59, 60, 61, 62, 63]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  1,  2,  3,
             8,  9, 10, 11,
            16, 17, 18, 19,
            24, 25, 26, 27
        };
    Vector expectedCachedLowerLeft =
        {
            32, 33, 34, 35,
            40, 41, 42, 43,
            48, 49, 50, 51,
            56, 57, 58, 59
        };
    Vector expectedCachedUpperRight =
        {
             4,  5,  6,  7,
            12, 13, 14, 15,
            20, 21, 22, 23,
            28, 29, 30, 31
        };
    Vector expectedCachedLowerRight =
        {
            36, 37, 38, 39,
            44, 45, 46, 47,
            52, 53, 54, 55,
            60, 61, 62, 63
        };
    // clang-format on
    auto input = MakeIncrementingMatrix<int>(Rows, Columns, "input");
    auto output = MakeMatrix<int>(Rows, Columns, "output");

    // Define LoopNest
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, Rows)
                    .ForAll(j, 0, Columns)
                    .Do([=](Matrix input_, Matrix output_, Scalar i_, Scalar j_) {
                        output_(i_, j_) = input_(i_, j_);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iBlock = schedule.Split(i, SplitSize);
    auto jBlock = schedule.Split(j, SplitSize);

    schedule.SetOrder({ iBlock, jBlock, i, j });

    GeneralCachingStrategy cachingProvider{};
    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = SplitSize * SplitSize;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

    // Get a handle to the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    int rawCacheSize = (int)rawCacheValue.GetLayout().NumElements();

    auto cachedUpperLeft = MakeVector<int>(rawCacheSize);
    auto cachedUpperRight = MakeVector<int>(rawCacheSize);
    auto cachedLowerLeft = MakeVector<int>(rawCacheSize);
    auto cachedLowerRight = MakeVector<int>(rawCacheSize);

    // Add a low level API kernel to access the underlying cache after it has been filled
    auto cacheSpyKernel = loopnests::Kernel("cache_spy_kernel")
                              .Inputs(rawCacheValue, cachedUpperLeft, cachedUpperRight, cachedLowerLeft, cachedLowerRight)
                              .Indices(iTopLevel, jTopLevel)
                              .Define([=](Value rawCacheValue, Vector cachedUpperLeft, Vector cachedUpperRight, Vector cachedLowerLeft, Vector cachedLowerRight, Scalar i, Scalar j) {
                                  auto cacheView = rawCacheValue;
                                  cacheView.SetLayout({ { (int)rawCacheValue.GetLayout().NumElements() } });
                                  auto vectorCacheView = Vector(cacheView);
                                  If(i == 0,
                                     [&]() {
                                         // TODO : remove nested if's
                                         If(j == 0,
                                            [&]() {
                                                cachedUpperLeft = vectorCacheView;
                                            })
                                             .ElseIf(j == SplitSize,
                                                     [&]() {
                                                         cachedUpperRight = vectorCacheView;
                                                     });
                                     })
                                      .ElseIf(i == SplitSize,
                                              [&]() {
                                                  If(j == 0, [&]() {
                                                      cachedLowerLeft = vectorCacheView;
                                                  }).ElseIf(j == SplitSize, [&]() {
                                                      cachedLowerRight = vectorCacheView;
                                                  });
                                              });
                              });
    auto cacheSpyPosition = loopnests::CodePositionConstraints{ loopnests::LoopFragmentType::epilogue, { iBlock, jBlock }, {} };
    nest.GetUnderlyingLoopNest().AddKernel(cacheSpyKernel, cacheSpyPosition);

    nest.Run();

    auto ok = MakeScalar<int>("ok");
    ok = 1;
    auto printError = [&] {
        DebugPrint("Upper Left:");
        DebugPrintVector(cachedUpperLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Upper Right:");
        DebugPrintVector(cachedUpperRight);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperRight);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Lower Left:");
        DebugPrintVector(cachedLowerLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedLowerLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Lower Right:");
        DebugPrintVector(cachedLowerRight);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedLowerRight);
        DebugPrint("\n");
        DebugPrint("\n");
    };
    // TODO : replace nested if's
    If(VerifySame(cachedUpperLeft, expectedCachedUpperLeft) == 0, [&]() {
        If(VerifySame(cachedUpperRight, expectedCachedUpperRight) == 0, [&]() {
            If(VerifySame(cachedLowerLeft, expectedCachedLowerLeft) == 0, [&]() {
                If(VerifySame(cachedLowerRight, expectedCachedLowerRight) == 0, [&]() {
                    ok = 0;
                }).Else(printError);
            }).Else(printError);
        }).Else(printError);
    }).Else(printError);
    return ok;
}

Scalar GeneralCachingStrategy_ValidateOutput_Test2()
{
    // BLASTCopy caching
    loopnests::Index i("i"), j("j");

    const int Rows = 16;
    const int Columns = 16;
    const int InputCacheRows = 8;
    const int InputCacheCols = 8;
    const int StripeSize = 4;
    const int VecSize = 2;

    auto input = MakeIncrementingMatrix<int>(Rows, Columns, "input");
    auto output = MakeMatrix<int>(Rows, Columns, "output");

    // Define LoopNest
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, Rows)
                    .ForAll(j, 0, Columns)
                    .Do([=](Matrix input_, Matrix output_, Scalar i_, Scalar j_) {
                        output_(i_, j_) = input_(i_, j_);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iBlock = schedule.Split(i, InputCacheRows);
    auto jBlock = schedule.Split(j, InputCacheCols);
    auto jStripe = schedule.Split(j, StripeSize);
    auto jVec = schedule.Split(j, VecSize);

    std::vector<Index> orderedIndices = { jBlock,
                                          iBlock,
                                          jStripe,
                                          i,
                                          jVec,
                                          j };
    schedule.SetOrder(orderedIndices);

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = InputCacheRows * InputCacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    schedule.Cache<GeneralCachingStrategy>(input,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheParams);

#if 0 // DEBUGGING
    auto loop = nest.GetUnderlyingLoopNest();
    DebugDump(loop);
#endif

    nest.Run();

    return VerifySame(output, input);
}

Scalar GeneralCachingStrategy_ValidateOutput_Test3()
{
    // Progressive BLASTCopy caching
    loopnests::Index i("i"), j("j");

    const int Rows = 16;
    const int Columns = 16;
    const int InputCacheRows = 8;
    const int InputCacheCols = 8;
    const int StripeSize = 4;
    const int VecSize = 2;

    auto input = MakeIncrementingMatrix<int>(Rows, Columns, "input");
    auto output = MakeMatrix<int>(Rows, Columns, "output");

    // Define LoopNest
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, Rows)
                    .ForAll(j, 0, Columns)
                    .Do([=](Matrix input_, Matrix output_, Scalar i_, Scalar j_) {
                        output_(i_, j_) = input_(i_, j_);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iBlock = schedule.Split(i, InputCacheRows);
    auto jBlock = schedule.Split(j, InputCacheCols);
    auto jStripe = schedule.Split(j, StripeSize);
    auto jVec = schedule.Split(j, VecSize);

    std::vector<Index> orderedIndices = { jBlock,
                                          iBlock,
                                          jStripe,
                                          i,
                                          jVec,
                                          j };
    schedule.SetOrder(orderedIndices);

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = InputCacheRows * InputCacheCols;
    size_t fillThreshold = InputCacheRows * StripeSize;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    schedule.Cache<GeneralCachingStrategy>(input,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheParams);

#if 0 // DEBUGGING
    auto loop = nest.GetUnderlyingLoopNest();
    DebugDump(loop);
#endif

    nest.Run();

    return VerifySame(output, input);
}

Scalar GeneralCachingStrategy_ValidateOutput_Test4()
{
    // BLASTCopy caching with boundary condition on rows
    loopnests::Index i("i"), j("j");

    const int Rows = 15;
    const int Columns = 16;
    const int InputCacheRows = 8;
    const int InputCacheCols = 8;
    const int StripeSize = 4;
    const int VecSize = 2;

    auto input = MakeIncrementingMatrix<int>(Rows, Columns, "input");
    auto output = MakeMatrix<int>(Rows, Columns, "output");

    // Define LoopNest
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, Rows)
                    .ForAll(j, 0, Columns)
                    .Do([=](Matrix input_, Matrix output_, Scalar i_, Scalar j_) {
                        output_(i_, j_) = input_(i_, j_);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iBlock = schedule.Split(i, InputCacheRows);
    auto jBlock = schedule.Split(j, InputCacheCols);
    auto jStripe = schedule.Split(j, StripeSize);
    auto jVec = schedule.Split(j, VecSize);

    std::vector<Index> orderedIndices = { jBlock,
                                          iBlock,
                                          jStripe,
                                          i,
                                          jVec,
                                          j };
    schedule.SetOrder(orderedIndices);

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = InputCacheRows * InputCacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    schedule.Cache<GeneralCachingStrategy>(input,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheParams);

#if 0 // DEBUGGING
    auto loop = nest.GetUnderlyingLoopNest();
    DebugDump(loop);
#endif

    nest.Run();

    return VerifySame(output, input);
}

Scalar GeneralCachingStrategy_ValidateOutput_Test5()
{
    // Square output cache
    loopnests::Index i("i"), j("j");

    const int Rows = 8;
    const int Columns = 8;
    const int OutputCacheRows = 2;
    const int OutputCacheCols = 2;

    auto input = MakeIncrementingMatrix<int>(Rows, Columns, "input");
    auto output = MakeMatrix<int>(Rows, Columns, "output");

    // Define LoopNest
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, Rows)
                    .ForAll(j, 0, Columns)
                    .Do([=](Matrix input_, Matrix output_, Scalar i_, Scalar j_) {
                        output_(i_, j_) = input_(i_, j_);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iOutput = schedule.Split(i, OutputCacheRows);
    auto jOutput = schedule.Split(j, OutputCacheCols);

    std::vector<Index> orderedIndices = { iOutput, jOutput, i, j };
    schedule.SetOrder(orderedIndices);

    ArgumentType argType = ArgumentType::Output;
    std::string cacheName = "cacheOutput";

    size_t maxCacheElts = OutputCacheRows * OutputCacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    schedule.Cache<GeneralCachingStrategy>(output,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheParams);

#if 0 // DEBUGGING
    auto loop = nest.GetUnderlyingLoopNest();
    DebugDump(loop);
#endif

    nest.Run();

    return VerifySame(output, input);
}

Scalar GeneralCachingStrategy_ValidateOutput_Test6()
{
    // Rectangular output cache
    loopnests::Index i("i"), j("j");

    const int Rows = 8;
    const int Columns = 8;
    const int OutputCacheRows = 4;
    const int OutputCacheCols = 2;

    auto input = MakeIncrementingMatrix<int>(Rows, Columns, "input");
    auto output = MakeMatrix<int>(Rows, Columns, "output");

    // Define LoopNest
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, Rows)
                    .ForAll(j, 0, Columns)
                    .Do([=](Matrix input_, Matrix output_, Scalar i_, Scalar j_) {
                        output_(i_, j_) = input_(i_, j_);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iOutput = schedule.Split(i, OutputCacheRows);
    auto jOutput = schedule.Split(j, OutputCacheCols);

    std::vector<Index> orderedIndices = { iOutput, jOutput, i, j };
    schedule.SetOrder(orderedIndices);

    ArgumentType argType = ArgumentType::Output;
    std::string cacheName = "cacheOutput";

    size_t maxCacheElts = OutputCacheRows * OutputCacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    schedule.Cache<GeneralCachingStrategy>(output,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheParams);

#if 0 // DEBUGGING
    auto loop = nest.GetUnderlyingLoopNest();
    DebugDump(loop);
#endif

    nest.Run();

    return VerifySame(output, input);
}

Scalar GeneralCachingStrategy_ValidateOutput_Test7()
{
    // Square matrix tiling with square output cache
    loopnests::Index i("i"), j("j");

    const int Rows = 8;
    const int Columns = 8;
    const int InputCacheRows = 4;
    const int InputCacheCols = 4;
    const int OutputCacheRows = 2;
    const int OutputCacheCols = 2;

    auto input = MakeIncrementingMatrix<int>(Rows, Columns, "input");
    auto output = MakeMatrix<int>(Rows, Columns, "output");

    // Define LoopNest
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, Rows)
                    .ForAll(j, 0, Columns)
                    .Do([=](Matrix input_, Matrix output_, Scalar i_, Scalar j_) {
                        output_(i_, j_) = input_(i_, j_);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iBlock = schedule.Split(i, InputCacheRows);
    auto jBlock = schedule.Split(j, InputCacheCols);
    auto iOutput = schedule.Split(i, OutputCacheRows);
    auto jOutput = schedule.Split(j, OutputCacheCols);

    std::vector<Index> orderedIndices = { iBlock, jBlock, iOutput, jOutput, i, j };
    schedule.SetOrder(orderedIndices);

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";

    size_t maxCacheElts = InputCacheRows * InputCacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    schedule.Cache<GeneralCachingStrategy>(input,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheParams);

    ArgumentType output_argType = ArgumentType::Output;
    std::string output_cacheName = "cacheOutput";

    size_t output_maxCacheElts = OutputCacheRows * OutputCacheCols;
    size_t output_fillThreshold = output_maxCacheElts;
    std::function<void(Scalar, Scalar)> output_reduceFunction = CopyReduce;
    auto output_extraCacheParams = std::make_tuple(output_argType,
                                                   output_cacheName,
                                                   output_maxCacheElts,
                                                   output_fillThreshold,
                                                   output_reduceFunction,
                                                   false);
    schedule.Cache<GeneralCachingStrategy>(output,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           output_extraCacheParams);

#if 0 // DEBUGGING
    auto loop = nest.GetUnderlyingLoopNest();
    DebugDump(loop);
#endif

    nest.Run();

    return VerifySame(output, input);
}

Scalar GeneralCachingStrategy_ValidateOutput_Test8()
{
    // Rectangular matrix input tiling with different rectangular output cache
    loopnests::Index i("i"), j("j");

    const int Rows = 8;
    const int Columns = 8;
    const int InputCacheRows = 4;
    const int InputCacheCols = 2;
    const int OutputCacheRows = 2;
    const int OutputCacheCols = 4;

    auto input = MakeIncrementingMatrix<int>(Rows, Columns, "input");
    auto output = MakeMatrix<int>(Rows, Columns, "output");

    // Define LoopNest
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, Rows)
                    .ForAll(j, 0, Columns)
                    .Do([=](Matrix input_, Matrix output_, Scalar i_, Scalar j_) {
                        output_(i_, j_) = input_(i_, j_);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iBlock = schedule.Split(i, InputCacheRows);
    auto iOutput = schedule.Split(i, OutputCacheRows);
    auto jOutput = schedule.Split(j, OutputCacheCols);
    auto jBlock = schedule.Split(j, InputCacheCols);

    std::vector<Index> orderedIndices = { iBlock, iOutput, jOutput, jBlock, i, j };
    schedule.SetOrder(orderedIndices);

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";

    size_t maxCacheElts = InputCacheRows * InputCacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    schedule.Cache<GeneralCachingStrategy>(input,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheParams);

    ArgumentType output_argType = ArgumentType::Output;
    std::string output_cacheName = "cacheOutput";

    size_t output_maxCacheElts = OutputCacheRows * OutputCacheCols;
    size_t output_fillThreshold = output_maxCacheElts;
    std::function<void(Scalar, Scalar)> output_reduceFunction = CopyReduce;
    auto output_extraCacheParams = std::make_tuple(output_argType,
                                                   output_cacheName,
                                                   output_maxCacheElts,
                                                   output_fillThreshold,
                                                   output_reduceFunction,
                                                   false);
    schedule.Cache<GeneralCachingStrategy>(output,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           output_extraCacheParams);

#if 0 // DEBUGGING
    auto loop = nest.GetUnderlyingLoopNest();
    DebugDump(loop);
#endif

    nest.Run();

    return VerifySame(output, input);
}

Scalar GeneralCachingStrategy_ValidateOutput_Test9()
{
    // BLASTCopy input caching with square output cache
    loopnests::Index i("i"), j("j");

    const int Rows = 16;
    const int Columns = 16;
    const int InputCacheRows = 8;
    const int InputCacheCols = 8;
    const int StripeSize = 4;
    const int VecSize = 2;
    const int OutputCacheRows = 2;
    const int OutputCacheCols = 2;

    auto input = MakeIncrementingMatrix<int>(Rows, Columns, "input");
    auto output = MakeMatrix<int>(Rows, Columns, "output");

    // Define LoopNest
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, Rows)
                    .ForAll(j, 0, Columns)
                    .Do([=](Matrix input_, Matrix output_, Scalar i_, Scalar j_) {
                        output_(i_, j_) = input_(i_, j_);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iBlock = schedule.Split(i, InputCacheRows);
    auto iOutput = schedule.Split(i, OutputCacheRows);
    auto jBlock = schedule.Split(j, InputCacheCols);
    auto jStripe = schedule.Split(j, StripeSize);
    auto jOutput = schedule.Split(j, OutputCacheCols);
    auto jVec = schedule.Split(j, VecSize);

    std::vector<Index> orderedIndices = { jBlock,
                                          iBlock,
                                          jStripe,
                                          iOutput,
                                          jOutput,
                                          i,
                                          jVec,
                                          j };
    schedule.SetOrder(orderedIndices);

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = InputCacheRows * InputCacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    schedule.Cache<GeneralCachingStrategy>(input,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheParams);

    ArgumentType output_argType = ArgumentType::Output;
    std::string output_cacheName = "cacheOutput";

    size_t output_maxCacheElts = OutputCacheRows * OutputCacheCols;
    size_t output_fillThreshold = output_maxCacheElts;
    std::function<void(Scalar, Scalar)> output_reduceFunction = CopyReduce;
    auto output_extraCacheParams = std::make_tuple(output_argType,
                                                   output_cacheName,
                                                   output_maxCacheElts,
                                                   output_fillThreshold,
                                                   output_reduceFunction,
                                                   false);
    schedule.Cache<GeneralCachingStrategy>(output,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           output_extraCacheParams);

#if 0 // DEBUGGING
    auto loop = nest.GetUnderlyingLoopNest();
    DebugDump(loop);
#endif

    nest.Run();

    return VerifySame(output, input);
}

Scalar GeneralCachingStrategy_ValidateOutput_Test10()
{
    // BLASTCopy input caching with rectangular output cache
    loopnests::Index i("i"), j("j");

    const int Rows = 16;
    const int Columns = 16;
    const int InputCacheRows = 8;
    const int InputCacheCols = 8;
    const int StripeSize = 4;
    const int VecSize = 2;
    const int OutputCacheRows = 2;
    const int OutputCacheCols = 4;

    auto input = MakeIncrementingMatrix<int>(Rows, Columns, "input");
    auto output = MakeMatrix<int>(Rows, Columns, "output");

    // Define LoopNest
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, Rows)
                    .ForAll(j, 0, Columns)
                    .Do([=](Matrix input_, Matrix output_, Scalar i_, Scalar j_) {
                        output_(i_, j_) = input_(i_, j_);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iBlock = schedule.Split(i, InputCacheRows);
    auto iOutput = schedule.Split(i, OutputCacheRows);
    auto jBlock = schedule.Split(j, InputCacheCols);
    auto jStripe = schedule.Split(j, StripeSize);
    auto jOutput = schedule.Split(j, OutputCacheCols);
    auto jVec = schedule.Split(j, VecSize);

    std::vector<Index> orderedIndices = { jBlock,
                                          iBlock,
                                          jStripe,
                                          iOutput,
                                          jOutput,
                                          i,
                                          jVec,
                                          j };
    schedule.SetOrder(orderedIndices);

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = InputCacheRows * InputCacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    schedule.Cache<GeneralCachingStrategy>(input,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheParams);

    ArgumentType output_argType = ArgumentType::Output;
    std::string output_cacheName = "cacheOutput";

    size_t output_maxCacheElts = OutputCacheRows * OutputCacheCols;
    size_t output_fillThreshold = output_maxCacheElts;
    std::function<void(Scalar, Scalar)> output_reduceFunction = CopyReduce;
    auto output_extraCacheParams = std::make_tuple(output_argType,
                                                   output_cacheName,
                                                   output_maxCacheElts,
                                                   output_fillThreshold,
                                                   output_reduceFunction,
                                                   false);
    schedule.Cache<GeneralCachingStrategy>(output,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           { iOutput },
                                           std::nullopt,
                                           output_extraCacheParams);

#if 0 // DEBUGGING
    auto loop = nest.GetUnderlyingLoopNest();
    DebugDump(loop);
#endif

    nest.Run();

    return VerifySame(output, input);
}

Scalar GeneralCachingStrategy_ValidateOutput_Test11()
{
    // BLASTCopy output caching
    loopnests::Index i("i"), j("j");

    const int Rows = 16;
    const int Columns = 16;
    const int CacheRows = 8;
    const int CacheCols = 8;
    const int StripeSize = 4;
    const int VecSize = 2;

    auto input = MakeIncrementingMatrix<int>(Rows, Columns, "input");
    auto output = MakeMatrix<int>(Rows, Columns, "output");

    // Define LoopNest
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, Rows)
                    .ForAll(j, 0, Columns)
                    .Do([=](Matrix input_, Matrix output_, Scalar i_, Scalar j_) {
                        output_(i_, j_) = input_(i_, j_);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iBlock = schedule.Split(i, CacheRows);
    auto jBlock = schedule.Split(j, CacheCols);
    auto jStripe = schedule.Split(j, StripeSize);
    auto jVec = schedule.Split(j, VecSize);

    std::vector<Index> orderedIndices = { jBlock,
                                          iBlock,
                                          jStripe,
                                          i,
                                          jVec,
                                          j };
    schedule.SetOrder(orderedIndices);

    ArgumentType argType = ArgumentType::Output;
    std::string cacheName = "cacheOutput";
    size_t maxCacheElts = CacheRows * CacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    schedule.Cache<GeneralCachingStrategy>(output,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheParams);

#if 0 // DEBUGGING
    auto loop = nest.GetUnderlyingLoopNest();
    DebugDump(loop);
#endif

    nest.Run();

    return VerifySame(output, input);
}

Scalar GeneralCachingStrategy_ValidateOutput_Test12()
{
    // BLASTCopy input caching with same BLASTCopy output caching
    loopnests::Index i("i"), j("j");

    const int Rows = 16;
    const int Columns = 16;
    const int CacheRows = 8;
    const int CacheCols = 8;
    const int StripeSize = 4;
    const int VecSize = 2;

    auto input = MakeIncrementingMatrix<int>(Rows, Columns, "input");
    auto output = MakeMatrix<int>(Rows, Columns, "output");

    // Define LoopNest
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, Rows)
                    .ForAll(j, 0, Columns)
                    .Do([=](Matrix input_, Matrix output_, Scalar i_, Scalar j_) {
                        output_(i_, j_) = input_(i_, j_);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iBlock = schedule.Split(i, CacheRows);

    auto jBlock = schedule.Split(j, CacheCols);
    auto jStripe = schedule.Split(j, StripeSize);
    auto jVec = schedule.Split(j, VecSize);

    std::vector<Index> orderedIndices = { jBlock,
                                          iBlock,
                                          jStripe,
                                          i,
                                          jVec,
                                          j };
    schedule.SetOrder(orderedIndices);

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = CacheRows * CacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    schedule.Cache<GeneralCachingStrategy>(input,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheParams);

    ArgumentType output_argType = ArgumentType::Output;
    std::string output_cacheName = "cacheOutput";
    auto output_extraCacheParams = std::make_tuple(output_argType,
                                                   output_cacheName,
                                                   maxCacheElts,
                                                   fillThreshold,
                                                   reduceFunction,
                                                   false);
    schedule.Cache<GeneralCachingStrategy>(output,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           output_extraCacheParams);

#if 0 // DEBUGGING
    auto loop = nest.GetUnderlyingLoopNest();
    DebugDump(loop);
#endif

    nest.Run();

    return VerifySame(output, input);
}

Scalar GeneralCachingStrategy_ValidateOutput_Test13()
{
    // BLASTCopy input caching with different BLASTCopy output caching
    loopnests::Index i("i"), j("j");

    const int Rows = 32;
    const int Columns = 32;
    const int InputCacheRows = 16;
    const int InputCacheCols = 16;
    const int InputStripeSize = 8;
    const int VecSize = 2;
    const int OutputCacheRows = 8;
    const int OutputCacheCols = InputStripeSize; // == InputStripeSize and in same dimension
    const int OutputStripeSize = 4;

    auto input = MakeIncrementingMatrix<int>(Rows, Columns, "input");
    auto output = MakeMatrix<int>(Rows, Columns, "output");

    // Define LoopNest
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, Rows)
                    .ForAll(j, 0, Columns)
                    .Do([=](Matrix input_, Matrix output_, Scalar i_, Scalar j_) {
                        output_(i_, j_) = input_(i_, j_);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iBlock = schedule.Split(i, InputCacheRows);
    auto iOutput = schedule.Split(i, OutputCacheRows);

    auto jBlock = schedule.Split(j, InputCacheCols);
    auto jOutput = schedule.Split(j, OutputCacheCols);
    auto jInputStripe = jOutput; // Split by the same amount in the same dimension
    auto jOutputStripe = schedule.Split(j, OutputStripeSize);
    auto jVec = schedule.Split(j, VecSize);

    std::vector<Index> orderedIndices = { jBlock,
                                          iBlock,
                                          jOutput,
                                          iOutput,
                                          jOutputStripe,
                                          i,
                                          jVec,
                                          j };
    schedule.SetOrder(orderedIndices);

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = InputCacheRows * InputCacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    schedule.Cache<GeneralCachingStrategy>(input,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheParams);

    ArgumentType output_argType = ArgumentType::Output;
    std::string output_cacheName = "cacheOutput";

    size_t output_maxCacheElts = OutputCacheRows * OutputCacheCols;
    size_t output_fillThreshold = output_maxCacheElts;
    std::function<void(Scalar, Scalar)> output_reduceFunction = CopyReduce;
    auto output_extraCacheParams = std::make_tuple(output_argType,
                                                   output_cacheName,
                                                   output_maxCacheElts,
                                                   output_fillThreshold,
                                                   output_reduceFunction,
                                                   false);
    schedule.Cache<GeneralCachingStrategy>(output,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           output_extraCacheParams);

#if 0 // DEBUGGING
    auto loop = nest.GetUnderlyingLoopNest();
    DebugDump(loop);
#endif

    nest.Run();

    return VerifySame(output, input);
}

Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput(int rows, int columns, int outputCacheRows, int outputCacheColumns)
{
    // Square output cache
    loopnests::Index i("i"), j("j");

    auto input = MakeIncrementingMatrix<int>(rows, columns, "input");
    auto output = MakeMatrix<int>(rows, columns, "output");

    // Define LoopNest
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, rows)
                    .ForAll(j, 0, columns)
                    .Do([=](Matrix input_, Matrix output_, Scalar i_, Scalar j_) {
                        output_(i_, j_) = input_(i_, j_);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iOutput = schedule.Split(i, outputCacheRows);
    auto jOutput = schedule.Split(j, outputCacheColumns);

    std::vector<Index> orderedIndices = { iOutput, jOutput, i, j };
    schedule.SetOrder(orderedIndices);

    ArgumentType argType = ArgumentType::Output;
    std::string cacheName = "cacheOutput";

    size_t maxCacheElts = outputCacheRows * outputCacheColumns;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    schedule.Cache<GeneralCachingStrategy>(output,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheParams);

#if 0 // DEBUGGING
    auto loop = nest.GetUnderlyingLoopNest();
    DebugDump(loop);
#endif

    nest.Run();

    return VerifySame(output, input);
}

Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test1()
{
    const int Rows = 8;
    const int Columns = 8;
    const int CacheRows = 2;
    const int CacheColumns = 3;
    return GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput(Rows, Columns, CacheRows, CacheColumns);
}

Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test2()
{
    const int Rows = 8;
    const int Columns = 8;
    const int CacheRows = 3;
    const int CacheColumns = 2;
    return GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput(Rows, Columns, CacheRows, CacheColumns);
}

Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test3()
{
    const int Rows = 8;
    const int Columns = 8;
    const int CacheRows = 3;
    const int CacheColumns = 3;
    return GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput(Rows, Columns, CacheRows, CacheColumns);
}

Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test4()
{
    const int Rows = 8;
    const int Columns = 8;
    const int CacheRows = 4;
    const int CacheColumns = 5;
    return GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput(Rows, Columns, CacheRows, CacheColumns);
}

Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test5()
{
    const int Rows = 8;
    const int Columns = 8;
    const int CacheRows = 5;
    const int CacheColumns = 4;
    return GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput(Rows, Columns, CacheRows, CacheColumns);
}

Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test6()
{
    const int Rows = 8;
    const int Columns = 8;
    const int CacheRows = 5;
    const int CacheColumns = 5;
    return GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput(Rows, Columns, CacheRows, CacheColumns);
}

Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test7()
{
    const int Rows = 8;
    const int Columns = 7;
    const int CacheRows = 2;
    const int CacheColumns = 2;
    return GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput(Rows, Columns, CacheRows, CacheColumns);
}

Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test8()
{
    const int Rows = 7;
    const int Columns = 8;
    const int CacheRows = 2;
    const int CacheColumns = 2;
    return GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput(Rows, Columns, CacheRows, CacheColumns);
}

Scalar GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput_Test9()
{
    const int Rows = 7;
    const int Columns = 7;
    const int CacheRows = 2;
    const int CacheColumns = 2;
    return GeneralCachingStrategy_BoundaryConditionOutput_ValidateOutput(Rows, Columns, CacheRows, CacheColumns);
}

// BLASTCOPY tests from above with GeneralCachingStrategy

Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_Test1()
{
    int N = 8;
    int cacheRows = N;
    int cacheCols = N;
    int stripeSize = 4;

    auto input = MakeIncrementingMatrix<int>(N, N, "input");
    auto output = MakeMatrix<int>(N, N, "output");
    auto expectedOutput = MakeIncrementingMatrix<int>(N, N, "expectedOutput");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);

    schedule.SetOrder({ iCache, jCache, jStripe, i, j });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    schedule.Cache<GeneralCachingStrategy>(input,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheParams);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    return VerifySame(output, expectedOutput);
}

// Test with smaller cache and stripe size than previous test
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_Test2()
{
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input, expectedOutput
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7]
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // [16, 17, 18, 19, 20, 21, 22, 23]
    // [24, 25, 26, 27, 28, 29, 30, 31]
    // [32, 33, 34, 35, 36, 37, 38, 39]
    // [40, 41, 42, 43, 44, 45, 46, 47]
    // [48, 49, 50, 51, 52, 53, 54, 55]
    // [56, 57, 58, 59, 60, 61, 62, 63]
    auto input = MakeIncrementingMatrix<int>(N, N, "input");
    auto output = MakeMatrix<int>(N, N, "output");
    auto expectedOutput = MakeIncrementingMatrix<int>(N, N, "expectedOutput");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);

    schedule.SetOrder({ iCache, jCache, jStripe, i, j });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    schedule.Cache<GeneralCachingStrategy>(input,
                                           { iTopLevel, jTopLevel },
                                           {},
                                           {},
                                           std::nullopt,
                                           extraCacheParams);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    return VerifySame(output, expectedOutput);
}

Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_Test1()
{
    int N = 8;
    int cacheRows = N;
    int cacheCols = N;
    int stripeSize = 4;
    int vecSize = stripeSize / 2;

    auto input = MakeIncrementingMatrix<int>(N, N, "input");
    auto output = MakeMatrix<int>(N, N, "output");

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7]
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // [16, 17, 18, 19, 20, 21, 22, 23]
    // [24, 25, 26, 27, 28, 29, 30, 31]
    // [32, 33, 34, 35, 36, 37, 38, 39]
    // [40, 41, 42, 43, 44, 45, 46, 47]
    // [48, 49, 50, 51, 52, 53, 54, 55]
    // [56, 57, 58, 59, 60, 61, 62, 63]
    // clang-format off
    Vector expectedCached =
        {
             0,  1,  2,  3,
             8,  9, 10, 11,
            16, 17, 18, 19,
            24, 25, 26, 27,
            32, 33, 34, 35,
            40, 41, 42, 43,
            48, 49, 50, 51,
            56, 57, 58, 59,

             4,  5,  6,  7,
            12, 13, 14, 15,
            20, 21, 22, 23,
            28, 29, 30, 31,
            36, 37, 38, 39,
            44, 45, 46, 47,
            52, 53, 54, 55,
            60, 61, 62, 63
        };
    // clang-format on

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);
    auto jVec = schedule.Split(j, vecSize);

    schedule.SetOrder({ jCache, iCache, jStripe, i, jVec, j });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    // Examine the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    rawCacheValue.SetLayout({ { (int)rawCacheValue.GetLayout().GetMemorySize() } });
    auto cacheVector = Vector(rawCacheValue);

    return VerifySame(cacheVector, expectedCached);
}

// Smaller stripe size than previous test
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_Test2()
{
    int N = 8;
    int cacheRows = N;
    int cacheCols = N;
    int stripeSize = 2;

    auto input = MakeIncrementingMatrix<int>(N, N, "input");
    auto output = MakeMatrix<int>(N, N, "output");

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7]
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // [16, 17, 18, 19, 20, 21, 22, 23]
    // [24, 25, 26, 27, 28, 29, 30, 31]
    // [32, 33, 34, 35, 36, 37, 38, 39]
    // [40, 41, 42, 43, 44, 45, 46, 47]
    // [48, 49, 50, 51, 52, 53, 54, 55]
    // [56, 57, 58, 59, 60, 61, 62, 63]
    // clang-format off
    Vector expectedCached =
        {
             0,  1,
             8,  9,
            16, 17,
            24, 25,
            32, 33,
            40, 41,
            48, 49,
            56, 57,

             2,  3,
            10, 11,
            18, 19,
            26, 27,
            34, 35,
            42, 43,
            50, 51,
            58, 59,

             4,  5,
            12, 13,
            20, 21,
            28, 29,
            36, 37,
            44, 45,
            52, 53,
            60, 61,

             6,  7,
            14, 15,
            22, 23,
            30, 31,
            38, 39,
            46, 47,
            54, 55,
            62, 63
        };
    // clang-format on

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);

    schedule.SetOrder({ iCache, jCache, jStripe, i, j });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);
#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    // Examine the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    rawCacheValue.SetLayout({ { (int)rawCacheValue.GetLayout().GetMemorySize() } });
    auto cacheVector = Vector(rawCacheValue);

    return VerifySame(cacheVector, expectedCached);
}

// Same stripe size as previous test, but don't cache entire matrix at once
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_Test3()
{
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    auto input = MakeIncrementingMatrix<int>(N, N, "input");
    auto output = MakeMatrix<int>(N, N, "output");

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7]
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // [16, 17, 18, 19, 20, 21, 22, 23]
    // [24, 25, 26, 27, 28, 29, 30, 31]
    // [32, 33, 34, 35, 36, 37, 38, 39]
    // [40, 41, 42, 43, 44, 45, 46, 47]
    // [48, 49, 50, 51, 52, 53, 54, 55]
    // [56, 57, 58, 59, 60, 61, 62, 63]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  1,
             8,  9,
            16, 17,
            24, 25,

             2,  3,
            10, 11,
            18, 19,
            26, 27,
        };
    Vector expectedCachedUpperRight =
        {
             4,  5,
            12, 13,
            20, 21,
            28, 29,

             6,  7,
            14, 15,
            22, 23,
            30, 31
        };
    Vector expectedCachedLowerLeft =
        {
            32, 33,
            40, 41,
            48, 49,
            56, 57,

            34, 35,
            42, 43,
            50, 51,
            58, 59,
        };
    Vector expectedCachedLowerRight =
        {
            36, 37,
            44, 45,
            52, 53,
            60, 61,

            38, 39,
            46, 47,
            54, 55,
            62, 63
        };
    // clang-format on

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);

    schedule.SetOrder({ iCache, jCache, jStripe, i, j });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

    // Get a handle to the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    int rawCacheSize = (int)rawCacheValue.GetLayout().NumElements();

    auto cachedUpperLeft = MakeVector<int>(rawCacheSize);
    auto cachedUpperRight = MakeVector<int>(rawCacheSize);
    auto cachedLowerLeft = MakeVector<int>(rawCacheSize);
    auto cachedLowerRight = MakeVector<int>(rawCacheSize);

    // Add a low level API kernel to access the underlying cache after it has been filled
    auto cacheSpyKernel = loopnests::Kernel("cache_spy_kernel")
                              .Inputs(rawCacheValue, cachedUpperLeft, cachedUpperRight, cachedLowerLeft, cachedLowerRight)
                              .Indices(iTopLevel, jTopLevel)
                              .Define([cacheRows, cacheCols](Value rawCacheValue, Vector cachedUpperLeft, Vector cachedUpperRight, Vector cachedLowerLeft, Vector cachedLowerRight, Scalar i, Scalar j) {
                                  auto cacheView = rawCacheValue;
                                  cacheView.SetLayout({ { (int)rawCacheValue.GetLayout().NumElements() } });
                                  auto vectorCacheView = Vector(cacheView);
                                  If(i == 0,
                                     [&]() {
                                         // TODO : remove nested if's
                                         If(j == 0,
                                            [&]() {
                                                cachedUpperLeft = vectorCacheView;
                                            })
                                             .ElseIf(j == cacheCols,
                                                     [&]() {
                                                         cachedUpperRight = vectorCacheView;
                                                     });
                                     })
                                      .ElseIf(i == cacheRows,
                                              [&]() {
                                                  If(j == 0, [&]() {
                                                      cachedLowerLeft = vectorCacheView;
                                                  }).ElseIf(j == cacheCols, [&]() {
                                                      cachedLowerRight = vectorCacheView;
                                                  });
                                              });
                              });
    auto cacheSpyPosition = loopnests::CodePositionConstraints{ loopnests::LoopFragmentType::body, { iCache, jCache }, {} };
    nest.GetUnderlyingLoopNest().AddKernel(cacheSpyKernel, cacheSpyPosition);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    auto ok = MakeScalar<int>("ok");
    ok = 1;
    auto printError = [&] {
        DebugPrint("Upper Left:");
        DebugPrintVector(cachedUpperLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Upper Right:");
        DebugPrintVector(cachedUpperRight);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperRight);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Lower Left:");
        DebugPrintVector(cachedLowerLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedLowerLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Lower Right:");
        DebugPrintVector(cachedLowerRight);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedLowerRight);
        DebugPrint("\n");
        DebugPrint("\n");
    };
    // TODO : replace nested if's
    If(VerifySame(cachedUpperLeft, expectedCachedUpperLeft) == 0, [&]() {
        If(VerifySame(cachedUpperRight, expectedCachedUpperRight) == 0, [&]() {
            If(VerifySame(cachedLowerLeft, expectedCachedLowerLeft) == 0, [&]() {
                If(VerifySame(cachedLowerRight, expectedCachedLowerRight) == 0, [&]() {
                    ok = 0;
                }).Else(printError);
            }).Else(printError);
        }).Else(printError);
    }).Else(printError);
    return ok;
}

Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(int M, int N, int cacheRows, int cacheCols, int stripeSize)
{
    int vecSize = stripeSize / 2;

    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");
    auto expectedOutput = MakeIncrementingMatrix<int>(M, N, "expectedOutput");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);
    auto jVec = schedule.Split(j, vecSize);

    schedule.SetOrder({ jCache, iCache, jStripe, i, jVec, j });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    return VerifySame(output, expectedOutput);
}

// input matrix rows evenly divides cache rows
// input matrix cols doesn't evenly divide cache cols
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test1()
{
    int M = 8;
    int N = 7; // N doesn't evenly divide the number of cache columns
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

// input matrix rows evenly divides cache rows
// input matrix cols doesn't evenly divide cache cols but does evenly divide stripeSize
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test2()
{
    int M = 8;
    int N = 6; // N doesn't evenly divide the number of cache columns, but does evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

// input matrix rows doesn't evenly divides cache rows
// input matrix cols doesn't evenly divide cache cols
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test3()
{
    int M = 6;
    int N = 7; // N doesn't evenly divide the number of cache columns
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

// input matrix rows doesn't evenly divides cache rows
// input matrix cols doesn't evenly divide cache cols but does evenly divide stripe size
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test4()
{
    int M = 6;
    int N = 6; // N doesn't evenly divide the number of cache columns, but does evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

// input matrix rows evenly divides cache rows
// input matrix cols < cache cols, doesn't evenly divide stripe size
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test5()
{
    int M = 8;
    int N = 3; // N < cache columns, doesn't evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

// input matrix rows evenly divides cache rows
// input matrix cols < cache cols, evenly divides stripe size
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test6()
{
    int M = 8;
    int N = 2; // N < cache columns, does evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

// input matrix rows < cache rows
// input matrix cols < cache cols, doesn't evenly divides stripe size
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test7()
{
    int M = 3;
    int N = 3; // N < cache columns, doesn't evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

// input matrix rows < cache rows
// input matrix cols < cache cols, does evenly divides stripe size
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test8()
{
    int M = 2;
    int N = 2; // N < cache columns, does evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

// input matrix rows < cache rows
// input matrix cols multiple of cache cols
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Test9()
{
    int M = 2;
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_BLASTCOPY_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize);
}

Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Runner(int M, int N, int cacheRows, int cacheCols, int stripeSize, Vector expectedCachedUpperLeft, Vector expectedCachedUpperRight, Vector expectedCachedLowerLeft, Vector expectedCachedLowerRight)
{
    int vecSize = stripeSize / 2;
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);
    auto jVec = schedule.Split(j, vecSize);

    schedule.SetOrder({ jCache, iCache, jStripe, i, jVec, j });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

    // Get a handle to the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    int rawCacheSize = (int)rawCacheValue.GetLayout().NumElements();

    auto cachedUpperLeft = MakeVector<int>(rawCacheSize);
    auto cachedUpperRight = MakeVector<int>(rawCacheSize);
    auto cachedLowerLeft = MakeVector<int>(rawCacheSize);
    auto cachedLowerRight = MakeVector<int>(rawCacheSize);

    // Add a low level API kernel to access the underlying cache after it has been filled
    auto cacheSpyKernel = loopnests::Kernel("cache_spy_kernel")
                              .Inputs(rawCacheValue, cachedUpperLeft, cachedUpperRight, cachedLowerLeft, cachedLowerRight)
                              .Indices(iTopLevel, jTopLevel)
                              .Define([cacheRows, cacheCols](Value rawCacheValue, Vector cachedUpperLeft, Vector cachedUpperRight, Vector cachedLowerLeft, Vector cachedLowerRight, Scalar i, Scalar j) {
                                  auto cacheView = rawCacheValue;
                                  cacheView.SetLayout({ { (int)rawCacheValue.GetLayout().NumElements() } });
                                  auto vectorCacheView = Vector(cacheView);
                                  If(i == 0,
                                     [&]() {
                                         // TODO : remove nested if's
                                         If(j == 0,
                                            [&]() {
                                                cachedUpperLeft = vectorCacheView;
                                            })
                                             .ElseIf(j == cacheCols,
                                                     [&]() {
                                                         cachedUpperRight = vectorCacheView;
                                                     });
                                     })
                                      .ElseIf(i == cacheRows,
                                              [&]() {
                                                  If(j == 0, [&]() {
                                                      cachedLowerLeft = vectorCacheView;
                                                  }).ElseIf(j == cacheCols, [&]() {
                                                      cachedLowerRight = vectorCacheView;
                                                  });
                                              });
                              });
    auto cacheSpyPosition = loopnests::CodePositionConstraints{ loopnests::LoopFragmentType::body, { iCache, jCache }, {} };
    nest.GetUnderlyingLoopNest().AddKernel(cacheSpyKernel, cacheSpyPosition);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    auto ok = MakeScalar<int>("ok");
    ok = 1;
    auto printError = [&] {
        DebugPrint("Upper Left:");
        DebugPrintVector(cachedUpperLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Upper Right:");
        DebugPrintVector(cachedUpperRight);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperRight);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Lower Left:");
        DebugPrintVector(cachedLowerLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedLowerLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Lower Right:");
        DebugPrintVector(cachedLowerRight);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedLowerRight);
        DebugPrint("\n");
        DebugPrint("\n");
    };
    // TODO : replace nested if's
    If(VerifySame(cachedUpperLeft, expectedCachedUpperLeft) == 0, [&]() {
        If(VerifySame(cachedUpperRight, expectedCachedUpperRight) == 0, [&]() {
            If(VerifySame(cachedLowerLeft, expectedCachedLowerLeft) == 0, [&]() {
                If(VerifySame(cachedLowerRight, expectedCachedLowerRight) == 0, [&]() {
                    ok = 0;
                }).Else(printError);
            }).Else(printError);
        }).Else(printError);
    }).Else(printError);
    return ok;
}

Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Runner_LeftCachesOnly(int M, int N, int cacheRows, int cacheCols, int stripeSize, Vector expectedCachedUpperLeft, Vector expectedCachedLowerLeft)
{
    int vecSize = stripeSize / 2;
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);
    auto jVec = schedule.Split(j, vecSize);

    schedule.SetOrder({ jCache, iCache, jStripe, i, jVec, j });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

    // Get a handle to the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    int rawCacheSize = (int)rawCacheValue.GetLayout().NumElements();

    // No right caches when N < cacheCols
    auto cachedUpperLeft = MakeVector<int>(rawCacheSize);
    auto cachedLowerLeft = MakeVector<int>(rawCacheSize);

    // Add a low level API kernel to access the underlying cache after it has been filled
    auto cacheSpyKernel = loopnests::Kernel("cache_spy_kernel")
                              .Inputs(rawCacheValue, cachedUpperLeft, cachedLowerLeft)
                              .Indices(iTopLevel, jTopLevel)
                              .Define([cacheRows](Value rawCacheValue, Vector cachedUpperLeft, Vector cachedLowerLeft, Scalar i, Scalar j) {
                                  auto cacheView = rawCacheValue;
                                  cacheView.SetLayout({ { (int)rawCacheValue.GetLayout().NumElements() } });
                                  auto vectorCacheView = Vector(cacheView);
                                  If(i == 0,
                                     [&]() {
                                         // TODO : remove nested if's
                                         If(j == 0,
                                            [&]() {
                                                cachedUpperLeft = vectorCacheView;
                                            });
                                     })
                                      .ElseIf(i == cacheRows,
                                              [&]() {
                                                  If(j == 0, [&]() {
                                                      cachedLowerLeft = vectorCacheView;
                                                  });
                                              });
                              });
    auto cacheSpyPosition = loopnests::CodePositionConstraints{ loopnests::LoopFragmentType::body, { iCache, jCache }, {} };
    nest.GetUnderlyingLoopNest().AddKernel(cacheSpyKernel, cacheSpyPosition);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    auto ok = MakeScalar<int>("ok");
    ok = 1;
    auto printError = [&] {
        DebugPrint("Upper Left:");
        DebugPrintVector(cachedUpperLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Lower Left:");
        DebugPrintVector(cachedLowerLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedLowerLeft);
        DebugPrint("\n");
        DebugPrint("\n");
    };
    // TODO : replace nested if's
    If(VerifySame(cachedUpperLeft, expectedCachedUpperLeft) == 0, [&]() {
        If(VerifySame(cachedLowerLeft, expectedCachedLowerLeft) == 0, [&]() {
            ok = 0;
        }).Else(printError);
    }).Else(printError);
    return ok;
}

Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Runner_UpperCachesOnly(int M, int N, int cacheRows, int cacheCols, int stripeSize, Vector expectedCachedUpperLeft, Vector expectedCachedUpperRight)
{
    int vecSize = stripeSize / 2;
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);
    auto jVec = schedule.Split(j, vecSize);

    schedule.SetOrder({ jCache, iCache, jStripe, i, jVec, j });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

    // Get a handle to the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    int rawCacheSize = (int)rawCacheValue.GetLayout().NumElements();

    auto cachedUpperLeft = MakeVector<int>(rawCacheSize);
    auto cachedUpperRight = MakeVector<int>(rawCacheSize);

    // Add a low level API kernel to access the underlying cache after it has been filled
    auto cacheSpyKernel = loopnests::Kernel("cache_spy_kernel")
                              .Inputs(rawCacheValue, cachedUpperLeft, cachedUpperRight)
                              .Indices(iTopLevel, jTopLevel)
                              .Define([cacheCols](Value rawCacheValue, Vector cachedUpperLeft, Vector cachedUpperRight, Scalar i, Scalar j) {
                                  auto cacheView = rawCacheValue;
                                  cacheView.SetLayout({ { (int)rawCacheValue.GetLayout().NumElements() } });
                                  auto vectorCacheView = Vector(cacheView);
                                  If(i == 0,
                                     [&]() {
                                         // TODO : remove nested if's
                                         If(j == 0,
                                            [&]() {
                                                cachedUpperLeft = vectorCacheView;
                                            })
                                             .ElseIf(j == cacheCols,
                                                     [&]() {
                                                         cachedUpperRight = vectorCacheView;
                                                     });
                                     });
                              });
    auto cacheSpyPosition = loopnests::CodePositionConstraints{ loopnests::LoopFragmentType::body, { iCache, jCache }, {} };
    nest.GetUnderlyingLoopNest().AddKernel(cacheSpyKernel, cacheSpyPosition);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    auto ok = MakeScalar<int>("ok");
    ok = 1;
    auto printError = [&] {
        DebugPrint("Upper Left:");
        DebugPrintVector(cachedUpperLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Upper Right:");
        DebugPrintVector(cachedUpperRight);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperRight);
        DebugPrint("\n");
        DebugPrint("\n");
    };
    // TODO : replace nested if's
    If(VerifySame(cachedUpperLeft, expectedCachedUpperLeft) == 0, [&]() {
        If(VerifySame(cachedUpperRight, expectedCachedUpperRight) == 0, [&]() {
            ok = 0;
        }).Else(printError);
    }).Else(printError);
    return ok;
}

Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Runner_UpperLeftCacheOnly(int M, int N, int cacheRows, int cacheCols, int stripeSize, Vector expectedCachedUpperLeft)
{
    int vecSize = stripeSize / 2;
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto jCache = schedule.Split(j, cacheCols);
    auto jStripe = schedule.Split(j, stripeSize);
    auto jVec = schedule.Split(j, vecSize);

    schedule.SetOrder({ jCache, iCache, jStripe, i, jVec, j });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = maxCacheElts;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

    // Get a handle to the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    int rawCacheSize = (int)rawCacheValue.GetLayout().NumElements();

    // No right caches when N < cacheCols
    auto cachedUpperLeft = MakeVector<int>(rawCacheSize);

    // Add a low level API kernel to access the underlying cache after it has been filled
    auto cacheSpyKernel = loopnests::Kernel("cache_spy_kernel")
                              .Inputs(rawCacheValue, cachedUpperLeft)
                              .Indices(iTopLevel, jTopLevel)
                              .Define([](Value rawCacheValue, Vector cachedUpperLeft, Scalar i, Scalar j) {
                                  auto cacheView = rawCacheValue;
                                  cacheView.SetLayout({ { (int)rawCacheValue.GetLayout().NumElements() } });
                                  auto vectorCacheView = Vector(cacheView);
                                  If(i == 0,
                                     [&]() {
                                         // TODO : remove nested if's
                                         If(j == 0,
                                            [&]() {
                                                cachedUpperLeft = vectorCacheView;
                                            });
                                     });
                              });
    auto cacheSpyPosition = loopnests::CodePositionConstraints{ loopnests::LoopFragmentType::body, { iCache, jCache }, {} };
    nest.GetUnderlyingLoopNest().AddKernel(cacheSpyKernel, cacheSpyPosition);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    return VerifySame(cachedUpperLeft, expectedCachedUpperLeft);
}

Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test1()
{
    int M = 8; // M does evenly divide cache rows
    int N = 7; // N doesn't evenly divide cache columns
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6],
    // [ 7,  8,  9, 10, 11, 12, 13],
    // [14, 15, 16, 17, 18, 19, 20],
    // [21, 22, 23, 24, 25, 26, 27],
    // [28, 29, 30, 31, 32, 33, 34],
    // [35, 36, 37, 38, 39, 40, 41],
    // [42, 43, 44, 45, 46, 47, 48],
    // [49, 50, 51, 52, 53, 54, 55]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  1,
             7,  8,
            14, 15,
            21, 22,

             2,  3,
             9, 10,
            16, 17,
            23, 24,
        };
    Vector expectedCachedUpperRight =
        {
             4,  5,
            11, 12,
            18, 19,
            25, 26,

             6,  0,
            13,  0,
            20,  0,
            27,  0
        };
    Vector expectedCachedLowerLeft =
        {
            28, 29,
            35, 36,
            42, 43,
            49, 50,

            30, 31,
            37, 38,
            44, 45,
            51, 52,
        };
    Vector expectedCachedLowerRight =
        {
            32, 33,
            39, 40,
            46, 47,
            53, 54,

            34,  0,
            41,  0,
            48,  0,
            55,  0
        };
    // clang-format on

    return GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight, expectedCachedLowerLeft, expectedCachedLowerRight);
}

Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test2()
{
    int M = 8; // M does evenly divide cache rows
    int N = 6; // N doesn't evenly divide cache columns, but does evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5],
    // [ 6,  7,  8,  9, 10, 11],
    // [12, 13, 14, 15, 16, 17],
    // [18, 19, 20, 21, 22, 23],
    // [24, 25, 26, 27, 28, 29],
    // [30, 31, 32, 33, 34, 35],
    // [36, 37, 38, 39, 40, 41],
    // [42, 43, 44, 45, 46, 47]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  1,
             6,  7,
            12, 13,
            18, 19,

             2,  3,
             8,  9,
            14, 15,
            20, 21,
        };
    Vector expectedCachedUpperRight =
        {
             4,  5,
            10, 11,
            16, 17,
            22, 23,

             0,  0,
             0,  0,
             0,  0,
             0,  0
        };
    Vector expectedCachedLowerLeft =
        {
            24, 25,
            30, 31,
            36, 37,
            42, 43,

            26, 27,
            32, 33,
            38, 39,
            44, 45,
        };
    Vector expectedCachedLowerRight =
        {
            28, 29,
            34, 35,
            40, 41,
            46, 47,

             0,  0,
             0,  0,
             0,  0,
             0,  0
        };
    // clang-format on

    return GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight, expectedCachedLowerLeft, expectedCachedLowerRight);
}

// input matrix rows doesn't evenly divides cache rows
// input matrix cols doesn't evenly divide cache cols
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test3()
{
    int M = 6;
    int N = 7; // N doesn't evenly divide the number of cache columns
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6],
    // [ 7,  8,  9, 10, 11, 12, 13],
    // [14, 15, 16, 17, 18, 19, 20],
    // [21, 22, 23, 24, 25, 26, 27],
    // [28, 29, 30, 31, 32, 33, 34],
    // [35, 36, 37, 38, 39, 40, 41],
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  1,
             7,  8,
            14, 15,
            21, 22,

             2,  3,
             9, 10,
            16, 17,
            23, 24,
        };
    Vector expectedCachedUpperRight =
        {
             4,  5,
            11, 12,
            18, 19,
            25, 26,

             6,  0,
            13,  0,
            20,  0,
            27,  0
        };
    
    // Check that it gets reviewed correctly to keep the cached data contiguous
    Vector expectedCachedLowerLeft =
        {
            28, 29,
            35, 36,
            30, 31,
            37, 38,

             0,  0,
             0,  0,            
             0,  0,
             0,  0,
        };
    Vector expectedCachedLowerRight =
        {
            32, 33,
            39, 40,
            34,  0,
            41,  0,

             0,  0,
             0,  0,
             0,  0,
             0,  0
        };
    // clang-format on

    return GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight, expectedCachedLowerLeft, expectedCachedLowerRight);
}

// input matrix rows doesn't evenly divides cache rows
// input matrix cols doesn't evenly divide cache cols but does evenly divide stripe size
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test4()
{
    int M = 6;
    int N = 6; // N doesn't evenly divide the number of cache columns, but does evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5],
    // [ 6,  7,  8,  9, 10, 11],
    // [12, 13, 14, 15, 16, 17],
    // [18, 19, 20, 21, 22, 23],
    // [24, 25, 26, 27, 28, 29],
    // [30, 31, 32, 33, 34, 35]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  1,
             6,  7,
            12, 13,
            18, 19,

             2,  3,
             8,  9,
            14, 15,
            20, 21,
        };
    Vector expectedCachedUpperRight =
        {
             4,  5,
            10, 11,
            16, 17,
            22, 23,

             0,  0,
             0,  0,
             0,  0,
             0,  0
        };
    Vector expectedCachedLowerLeft =
        {
            24, 25,
            30, 31,
            26, 27,
            32, 33,

             0,  0,
             0,  0,
             0,  0,
             0,  0,
        };
    Vector expectedCachedLowerRight =
        {
            28, 29,
            34, 35,
             0,  0,
             0,  0,

             0,  0,
             0,  0,
             0,  0,
             0,  0
        };
    // clang-format on
    return GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight, expectedCachedLowerLeft, expectedCachedLowerRight);
}

// input matrix rows evenly divides cache rows
// input matrix cols < cache cols, doesn't evenly divide stripe size
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test5()
{
    int M = 8;
    int N = 3; // N < cache columns, doesn't evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1,  2],
    // [ 3,  4,  5],
    // [ 6,  7,  8],
    // [ 9, 10, 11],
    // [12, 13, 14],
    // [15, 16, 17],
    // [18, 19, 20],
    // [21, 22, 23]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  1,
             3,  4,
             6,  7,
             9, 10,

             2, 0,
             5, 0,
             8, 0,
            11, 0,
        };
    Vector expectedCachedLowerLeft =
        {
            12, 13,
            15, 16,
            18, 19,
            21, 22,

            14,  0,
            17,  0,
            20,  0,
            23,  0,
        };
    // clang-format on

    return GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Runner_LeftCachesOnly(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft, expectedCachedLowerLeft);
}

// input matrix rows evenly divides cache rows
// input matrix cols < cache cols, evenly divides stripe size
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test6()
{
    int M = 8;
    int N = 2; // N < cache columns, does evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1],
    // [ 2,  3],
    // [ 4,  5],
    // [ 6,  7],
    // [ 8,  9],
    // [10, 11],
    // [12, 13],
    // [14, 15]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  1,
             2,  3,
             4,  5,
             6,  7,

             0,  0,
             0,  0,
             0,  0,
             0,  0,
        };
    Vector expectedCachedLowerLeft =
        {
             8,  9,
            10, 11,
            12, 13,
            14, 15,

             0,  0,
             0,  0,
             0,  0,
             0,  0,
        };
    // clang-format on
    return GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Runner_LeftCachesOnly(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft, expectedCachedLowerLeft);
}

// input matrix rows < cache rows
// input matrix cols < cache cols, doesn't evenly divides stripe size
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test7()
{
    int M = 3;
    int N = 3; // N < cache columns, doesn't evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input
    // A:
    // [0, 1, 2],
    // [3, 4, 5],
    // [6, 7, 8]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
            0, 1,
            3, 4,
            6, 7,
            2, 0,

            5, 0,
            8, 0,
            0, 0,
            0, 0
        };
    // clang-format on
    return GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Runner_UpperLeftCacheOnly(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft);
}

// input matrix rows < cache rows
// input matrix cols < cache cols, does evenly divides stripe size
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test8()
{
    int M = 2;
    int N = 2; // N < cache columns, does evenly divide stripe size
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0, 1],
    // [ 2, 3]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
            0, 1,
            2, 3,
            0, 0,
            0, 0,

            0, 0,
            0, 0,
            0, 0,
            0, 0,
        };
    // clang-format on
    return GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Runner_UpperLeftCacheOnly(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft);
}

// input matrix rows < cache rows
// input matrix cols multiple of cache cols
Scalar GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Test9()
{
    int M = 2;
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7],
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  1,
             8,  9,
             2,  3,
            10, 11,

            0, 0,
            0, 0,
            0, 0,
            0, 0,
        };
    Vector expectedCachedUpperRight =
        {
             4,  5,
            12, 13,
             6,  7,
            14, 15,

             0,  0,
             0,  0,
             0,  0,
             0,  0
        };
    // clang-format on
    return GeneralCachingStrategy_BLASTCOPY_ValidateMemory_BoundaryCondition_Runner_UpperCachesOnly(M, N, cacheRows, cacheCols, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight);
}

// General caching strategy Progressive BLASNCopy-style caching
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_Test1()
{
    int N = 8;
    int cacheRows = N;
    int cacheCols = N;
    int blockSize = 4;
    int stripeSize = 2;

    auto input = MakeIncrementingMatrix<int>(N, N, "input");
    auto output = MakeMatrix<int>(N, N, "output");
    auto expectedOutput = MakeIncrementingMatrix<int>(N, N, "expectedOutput");

    // input:
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7]
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // [16, 17, 18, 19, 20, 21, 22, 23]
    // [24, 25, 26, 27, 28, 29, 30, 31]
    // [32, 33, 34, 35, 36, 37, 38, 39]
    // [40, 41, 42, 43, 44, 45, 46, 47]
    // [48, 49, 50, 51, 52, 53, 54, 55]
    // [56, 57, 58, 59, 60, 61, 62, 63]

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto iBlock = schedule.Split(i, blockSize);
    auto iStripe = schedule.Split(i, stripeSize);
    auto jCache = schedule.Split(j, cacheCols);

    schedule.SetOrder({ iCache, jCache, iBlock, iStripe, j, i });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = blockSize * cacheCols;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    return VerifySame(output, expectedOutput);
}

// Test with smaller cache, block, and stripe size than previous test
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_Test2()
{
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 2;
    int stripeSize = 1;

    // input, expectedOutput
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7]
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // [16, 17, 18, 19, 20, 21, 22, 23]
    // [24, 25, 26, 27, 28, 29, 30, 31]
    // [32, 33, 34, 35, 36, 37, 38, 39]
    // [40, 41, 42, 43, 44, 45, 46, 47]
    // [48, 49, 50, 51, 52, 53, 54, 55]
    // [56, 57, 58, 59, 60, 61, 62, 63]
    auto input = MakeIncrementingMatrix<int>(N, N, "input");
    auto output = MakeMatrix<int>(N, N, "output");
    auto expectedOutput = MakeIncrementingMatrix<int>(N, N, "expectedOutput");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto iBlock = schedule.Split(i, blockSize);
    auto iStripe = schedule.Split(i, stripeSize);
    auto jCache = schedule.Split(j, cacheCols);

    schedule.SetOrder({ iCache, jCache, iBlock, iStripe, j, i });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = blockSize * cacheCols;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    return VerifySame(output, expectedOutput);
}

Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_Test1()
{
    int N = 8;
    int cacheRows = N;
    int cacheCols = N;
    int blockSize = N;
    int stripeSize = 4;

    auto input = MakeIncrementingMatrix<int>(N, N, "input");
    auto output = MakeMatrix<int>(N, N, "output");

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7]
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // [16, 17, 18, 19, 20, 21, 22, 23]
    // [24, 25, 26, 27, 28, 29, 30, 31]
    // [32, 33, 34, 35, 36, 37, 38, 39]
    // [40, 41, 42, 43, 44, 45, 46, 47]
    // [48, 49, 50, 51, 52, 53, 54, 55]
    // [56, 57, 58, 59, 60, 61, 62, 63]
    // clang-format off
    Vector expectedCached =
        {
             0,  8, 16, 24,
             1,  9, 17, 25,
             2, 10, 18, 26,
             3, 11, 19, 27,
             4, 12, 20, 28,
             5, 13, 21, 29,
             6, 14, 22, 30,
             7, 15, 23, 31,

            32, 40, 48, 56,
            33, 41, 49, 57,
            34, 42, 50, 58,
            35, 43, 51, 59,
            36, 44, 52, 60,
            37, 45, 53, 61,
            38, 46, 54, 62,
            39, 47, 55, 63
        };
    // clang-format on

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto iBlock = schedule.Split(i, blockSize);
    auto iStripe = schedule.Split(i, stripeSize);
    auto jCache = schedule.Split(j, cacheCols);

    schedule.SetOrder({ iCache, jCache, iBlock, iStripe, j, i });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = blockSize * cacheCols;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    // Examine the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    rawCacheValue.SetLayout({ { (int)rawCacheValue.GetLayout().GetMemorySize() } });
    auto cacheVector = Vector(rawCacheValue);

    return VerifySame(cacheVector, expectedCached);
}

// Smaller stripe size than previous test
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_Test2()
{
    int N = 8;
    int cacheRows = N;
    int cacheCols = N;
    int blockSize = 4;
    int stripeSize = 2;

    auto input = MakeIncrementingMatrix<int>(N, N, "input");
    auto output = MakeMatrix<int>(N, N, "output");

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7]
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // [16, 17, 18, 19, 20, 21, 22, 23]
    // [24, 25, 26, 27, 28, 29, 30, 31]
    // [32, 33, 34, 35, 36, 37, 38, 39]
    // [40, 41, 42, 43, 44, 45, 46, 47]
    // [48, 49, 50, 51, 52, 53, 54, 55]
    // [56, 57, 58, 59, 60, 61, 62, 63]
    // clang-format off
    Vector expectedCached =
        {
             0,  8,  1,  9,  2, 10,  3, 11,  4, 12,  5, 13,  6, 14,  7, 15,
            16, 24, 17, 25, 18, 26, 19, 27, 20, 28, 21, 29, 22, 30, 23, 31,
            
            32, 40, 33, 41, 34, 42, 35, 43, 36, 44, 37, 45, 38, 46, 39, 47,
            48, 56, 49, 57, 50, 58, 51, 59, 52, 60, 53, 61, 54, 62, 55, 63,
        };
    // clang-format on

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto iBlock = schedule.Split(i, blockSize);
    auto iStripe = schedule.Split(i, stripeSize);
    auto jCache = schedule.Split(j, cacheCols);

    schedule.SetOrder({ iCache, jCache, iBlock, iStripe, j, i });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = blockSize * cacheCols;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    // Examine the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    rawCacheValue.SetLayout({ { (int)rawCacheValue.GetLayout().GetMemorySize() } });
    auto cacheVector = Vector(rawCacheValue);

    return VerifySame(cacheVector, expectedCached);
}

// Same stripe size as previous test, but don't cache entire matrix at once
// Doesn't test the progressive nature of the cache over blocks
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_Test3()
{
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 2;
    int stripeSize = 2;

    auto input = MakeIncrementingMatrix<int>(N, N, "input");
    auto output = MakeMatrix<int>(N, N, "output");

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7]
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // [16, 17, 18, 19, 20, 21, 22, 23]
    // [24, 25, 26, 27, 28, 29, 30, 31]
    // [32, 33, 34, 35, 36, 37, 38, 39]
    // [40, 41, 42, 43, 44, 45, 46, 47]
    // [48, 49, 50, 51, 52, 53, 54, 55]
    // [56, 57, 58, 59, 60, 61, 62, 63]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  8,  1,  9,  2, 10,  3, 11,
            16, 24, 17, 25, 18, 26, 19, 27
        };
    Vector expectedCachedUpperRight =
        {
             4, 12,  5, 13,  6, 14,  7, 15,
            20, 28, 21, 29, 22, 30, 23, 31
        };
    Vector expectedCachedLowerLeft =
        {
            32, 40, 33, 41, 34, 42, 35, 43,
            48, 56, 49, 57, 50, 58, 51, 59
        };
    Vector expectedCachedLowerRight =
        {
            36, 44, 37, 45, 38, 46, 39, 47,
            52, 60, 53, 61, 54, 62, 55, 63
        };
    // clang-format on

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, N)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto iBlock = schedule.Split(i, blockSize);
    auto iStripe = schedule.Split(i, stripeSize);
    auto jCache = schedule.Split(j, cacheCols);

    schedule.SetOrder({ iCache, jCache, iBlock, iStripe, j, i });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = blockSize * cacheCols;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

    // Get a handle to the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    int rawCacheSize = (int)rawCacheValue.GetLayout().NumElements();

    auto cachedUpperLeft = MakeVector<int>(rawCacheSize);
    auto cachedUpperRight = MakeVector<int>(rawCacheSize);
    auto cachedLowerLeft = MakeVector<int>(rawCacheSize);
    auto cachedLowerRight = MakeVector<int>(rawCacheSize);

    // Add a low level API kernel to access the underlying cache after it has been filled
    auto cacheSpyKernel = loopnests::Kernel("cache_spy_kernel")
                              .Inputs(rawCacheValue, cachedUpperLeft, cachedUpperRight, cachedLowerLeft, cachedLowerRight)
                              .Indices(iTopLevel, jTopLevel)
                              .Define([cacheRows, cacheCols](Value rawCacheValue, Vector cachedUpperLeft, Vector cachedUpperRight, Vector cachedLowerLeft, Vector cachedLowerRight, Scalar i, Scalar j) {
                                  auto cacheView = rawCacheValue;
                                  cacheView.SetLayout({ { (int)rawCacheValue.GetLayout().NumElements() } });
                                  auto vectorCacheView = Vector(cacheView);
                                  If(i == 0,
                                     [&]() {
                                         // TODO : remove nested if's
                                         If(j == 0,
                                            [&]() {
                                                cachedUpperLeft = vectorCacheView;
                                            })
                                             .ElseIf(j == cacheCols,
                                                     [&]() {
                                                         cachedUpperRight = vectorCacheView;
                                                     });
                                     })
                                      .ElseIf(i == cacheRows,
                                              [&]() {
                                                  If(j == 0, [&]() {
                                                      cachedLowerLeft = vectorCacheView;
                                                  }).ElseIf(j == cacheCols, [&]() {
                                                      cachedLowerRight = vectorCacheView;
                                                  });
                                              });
                              });
    auto cacheSpyPosition = loopnests::CodePositionConstraints{ loopnests::LoopFragmentType::epilogue, { iCache, jCache }, {} };
    nest.GetUnderlyingLoopNest().AddKernel(cacheSpyKernel, cacheSpyPosition);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    auto ok = MakeScalar<int>("ok");
    ok = 1;
    auto printError = [&] {
        DebugPrint("Upper Left:");
        DebugPrintVector(cachedUpperLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Upper Right:");
        DebugPrintVector(cachedUpperRight);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperRight);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Lower Left:");
        DebugPrintVector(cachedLowerLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedLowerLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Lower Right:");
        DebugPrintVector(cachedLowerRight);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedLowerRight);
        DebugPrint("\n");
        DebugPrint("\n");
    };
    // TODO : replace nested if's
    If(VerifySame(cachedUpperLeft, expectedCachedUpperLeft) == 0, [&]() {
        If(VerifySame(cachedUpperRight, expectedCachedUpperRight) == 0, [&]() {
            If(VerifySame(cachedLowerLeft, expectedCachedLowerLeft) == 0, [&]() {
                If(VerifySame(cachedLowerRight, expectedCachedLowerRight) == 0, [&]() {
                    ok = 0;
                }).Else(printError);
            }).Else(printError);
        }).Else(printError);
    }).Else(printError);
    return ok;
}

Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(int M, int N, int cacheRows, int cacheCols, int blockSize, int stripeSize)
{
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");
    auto expectedOutput = MakeIncrementingMatrix<int>(M, N, "expectedOutput");

    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto iBlock = schedule.Split(i, blockSize);
    auto iStripe = schedule.Split(i, stripeSize);
    auto jCache = schedule.Split(j, cacheCols);

    schedule.SetOrder({ iCache, jCache, iBlock, iStripe, j, i });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = blockSize * cacheCols;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    return VerifySame(output, expectedOutput);
}

// input matrix rows doesn't evenly divide cache rows
// input matrix cols evenly divides cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test1()
{
    int M = 7; // M doesn't evenly divide the number of cache rows
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 2;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

// input matrix rows doesn't evenly divide cache rows, does evenly divide blocksize/stripesize
// input matrix cols evenly divides cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test2()
{
    int M = 6; // M doesn't evenly divide the number of cache rows
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 2;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

// input matrix rows doesn't evenly divide cache rows, does evenly divide blocksize/stripesize
// input matrix cols doesn't evenly divide cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test3()
{
    int M = 7; // M doesn't evenly divide the number of cache rows
    int N = 6;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 2;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

// input matrix rows doesn't evenly divide cache rows, does evenly divide blocksize/stripesize
// input matrix cols doesn't evenly divide cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test4()
{
    int M = 6; // M doesn't evenly divide the number of cache rows
    int N = 6;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 2;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

// input matrix rows < cache rows, doesn't evenly divide blocksize/stripesize
// input matrix cols evenly divides cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test5()
{
    int M = 3; // M < cache rows
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 2;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

// input matrix rows < cache rows, evenly divides blocksize/stripesize
// input matrix cols evenly divides cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test6()
{
    int M = 2; // M < cache rows, evenly divides stripesize
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 2;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

// input matrix rows < cache rows, doesn't evenly divide blocksize/stripesize
// input matrix cols < cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test7()
{
    int M = 3; // M < cache rows, doens't evenly divide stripesize
    int N = 3;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 2;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

// input matrix rows < cache rows, evenly divides blocksize/stripesize
// input matrix cols < cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test8()
{
    int M = 2; // M < cache rows, evenly divides stripesize
    int N = 2;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 2;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

// input matrix rows multiple of cache rows
// input matrix cols < cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_SmallBlocks_Test9()
{
    int M = 8;
    int N = 2; // N < cache cols
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 2;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

// input matrix rows doesn't evenly divide cache rows
// input matrix cols evenly divides cache cols
// stripeSize == blockSize / 2, blockSize == cacheRows
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test1()
{
    int M = 7; // M doesn't evenly divide the number of cache rows
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

// input matrix rows doesn't evenly divide cache rows, does evenly divide blocksize/stripesize
// input matrix cols evenly divides cache cols
// stripeSize == blockSize / 2, blockSize == cacheRows
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test2()
{
    int M = 6; // M doesn't evenly divide the number of cache rows
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

// input matrix rows doesn't evenly divide cache rows, does evenly divide blocksize/stripesize
// input matrix cols doesn't evenly divide cache cols
// stripeSize == blockSize / 2, blockSize == cacheRows
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test3()
{
    int M = 7; // M doesn't evenly divide the number of cache rows
    int N = 6;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

// input matrix rows doesn't evenly divide cache rows, does evenly divide blocksize/stripesize
// input matrix cols doesn't evenly divide cache cols
// stripeSize == blockSize / 2, blockSize == cacheRows
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test4()
{
    int M = 6; // M doesn't evenly divide the number of cache rows
    int N = 6;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

// input matrix rows < cache rows, doesn't evenly divide blocksize/stripesize
// input matrix cols evenly divides cache cols
// stripeSize == blockSize / 2, blockSize == cacheRows
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test5()
{
    int M = 3; // M < cache rows
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

// input matrix rows < cache rows, evenly divides blocksize/stripesize
// input matrix cols evenly divides cache cols
// stripeSize == blockSize / 2, blockSize == cacheRows
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test6()
{
    int M = 2; // M < cache rows, evenly divides stripesize
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

// input matrix rows < cache rows, doesn't evenly divide blocksize/stripesize
// input matrix cols < cache cols
// stripeSize == blockSize / 2, blockSize == cacheRows
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test7()
{
    int M = 3; // M < cache rows, doens't evenly divide stripesize
    int N = 3;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

// input matrix rows < cache rows, evenly divides blocksize/stripesize
// input matrix cols < cache cols
// stripeSize == blockSize / 2, blockSize == cacheRows
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test8()
{
    int M = 2; // M < cache rows, evenly divides stripesize
    int N = 2;
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

// input matrix rows multiple of cache rows
// input matrix cols < cache cols
// stripeSize == blockSize / 2, blockSize == cacheRows
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_LargeBlocks_Test9()
{
    int M = 8;
    int N = 2; // N < cache cols
    int cacheRows = 4;
    int cacheCols = 4;
    int blockSize = 4;
    int stripeSize = 2;

    return GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateOutput_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, blockSize, stripeSize);
}

Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner(int M, int N, int cacheRows, int cacheCols, int blockSize, int stripeSize, Vector expectedCachedUpperLeft, Vector expectedCachedUpperRight, Vector expectedCachedLowerLeft, Vector expectedCachedLowerRight)
{
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");
    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto iBlock = schedule.Split(i, blockSize);
    auto iStripe = schedule.Split(i, stripeSize);
    auto jCache = schedule.Split(j, cacheCols);

    schedule.SetOrder({ iCache, jCache, iBlock, iStripe, j, i });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = blockSize * cacheCols;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

    // Get a handle to the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    int rawCacheSize = (int)rawCacheValue.GetLayout().NumElements();

    auto cachedUpperLeft = MakeVector<int>(rawCacheSize);
    auto cachedUpperRight = MakeVector<int>(rawCacheSize);
    auto cachedLowerLeft = MakeVector<int>(rawCacheSize);
    auto cachedLowerRight = MakeVector<int>(rawCacheSize);

    // Add a low level API kernel to access the underlying cache after it has been filled
    auto cacheSpyKernel = loopnests::Kernel("cache_spy_kernel")
                              .Inputs(rawCacheValue, cachedUpperLeft, cachedUpperRight, cachedLowerLeft, cachedLowerRight)
                              .Indices(iTopLevel, jTopLevel)
                              .Define([cacheRows, cacheCols](Value rawCacheValue, Vector cachedUpperLeft, Vector cachedUpperRight, Vector cachedLowerLeft, Vector cachedLowerRight, Scalar i, Scalar j) {
                                  auto cacheView = rawCacheValue;
                                  cacheView.SetLayout({ { (int)rawCacheValue.GetLayout().NumElements() } });
                                  auto vectorCacheView = Vector(cacheView);
                                  If(i == 0,
                                     [&]() {
                                         // TODO : remove nested if's
                                         If(j == 0,
                                            [&]() {
                                                cachedUpperLeft = vectorCacheView;
                                            })
                                             .ElseIf(j == cacheCols,
                                                     [&]() {
                                                         cachedUpperRight = vectorCacheView;
                                                     });
                                     })
                                      .ElseIf(i == cacheRows,
                                              [&]() {
                                                  If(j == 0, [&]() {
                                                      cachedLowerLeft = vectorCacheView;
                                                  }).ElseIf(j == cacheCols, [&]() {
                                                      cachedLowerRight = vectorCacheView;
                                                  });
                                              });
                              });
    auto cacheSpyPosition = loopnests::CodePositionConstraints{ loopnests::LoopFragmentType::epilogue, { iCache, jCache }, {} };
    nest.GetUnderlyingLoopNest().AddKernel(cacheSpyKernel, cacheSpyPosition);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    auto ok = MakeScalar<int>("ok");
    ok = 1;
    auto printError = [&] {
        DebugPrint("Upper Left:");
        DebugPrintVector(cachedUpperLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Upper Right:");
        DebugPrintVector(cachedUpperRight);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperRight);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Lower Left:");
        DebugPrintVector(cachedLowerLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedLowerLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Lower Right:");
        DebugPrintVector(cachedLowerRight);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedLowerRight);
        DebugPrint("\n");
        DebugPrint("\n");
    };
    // TODO : replace nested if's
    If(VerifySame(cachedUpperLeft, expectedCachedUpperLeft) == 0, [&]() {
        If(VerifySame(cachedUpperRight, expectedCachedUpperRight) == 0, [&]() {
            If(VerifySame(cachedLowerLeft, expectedCachedLowerLeft) == 0, [&]() {
                If(VerifySame(cachedLowerRight, expectedCachedLowerRight) == 0, [&]() {
                    ok = 0;
                }).Else(printError);
            }).Else(printError);
        }).Else(printError);
    }).Else(printError);
    return ok;
}

Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner_LeftCachesOnly(int M, int N, int cacheRows, int cacheCols, int blockSize, int stripeSize, Vector expectedCachedUpperLeft, Vector expectedCachedLowerLeft)
{
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");
    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto iBlock = schedule.Split(i, blockSize);
    auto iStripe = schedule.Split(i, stripeSize);
    auto jCache = schedule.Split(j, cacheCols);

    schedule.SetOrder({ iCache, jCache, iBlock, iStripe, j, i });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = blockSize * cacheCols;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

    // Get a handle to the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    int rawCacheSize = (int)rawCacheValue.GetLayout().NumElements();

    auto cachedUpperLeft = MakeVector<int>(rawCacheSize);
    auto cachedLowerLeft = MakeVector<int>(rawCacheSize);

    // Add a low level API kernel to access the underlying cache after it has been filled
    auto cacheSpyKernel = loopnests::Kernel("cache_spy_kernel")
                              .Inputs(rawCacheValue, cachedUpperLeft, cachedLowerLeft)
                              .Indices(iTopLevel, jTopLevel)
                              .Define([cacheRows](Value rawCacheValue, Vector cachedUpperLeft, Vector cachedLowerLeft, Scalar i, Scalar j) {
                                  auto cacheView = rawCacheValue;
                                  cacheView.SetLayout({ { (int)rawCacheValue.GetLayout().NumElements() } });
                                  auto vectorCacheView = Vector(cacheView);
                                  If(i == 0,
                                     [&]() {
                                         // TODO : remove nested if's
                                         If(j == 0,
                                            [&]() {
                                                cachedUpperLeft = vectorCacheView;
                                            });
                                     })
                                      .ElseIf(i == cacheRows,
                                              [&]() {
                                                  If(j == 0, [&]() {
                                                      cachedLowerLeft = vectorCacheView;
                                                  });
                                              });
                              });
    auto cacheSpyPosition = loopnests::CodePositionConstraints{ loopnests::LoopFragmentType::epilogue, { iCache, jCache }, {} };
    nest.GetUnderlyingLoopNest().AddKernel(cacheSpyKernel, cacheSpyPosition);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    auto ok = MakeScalar<int>("ok");
    ok = 1;
    auto printError = [&] {
        DebugPrint("Upper Left:");
        DebugPrintVector(cachedUpperLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Lower Left:");
        DebugPrintVector(cachedLowerLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedLowerLeft);
        DebugPrint("\n");
        DebugPrint("\n");
    };
    // TODO : replace nested if's
    If(VerifySame(cachedUpperLeft, expectedCachedUpperLeft) == 0, [&]() {
        If(VerifySame(cachedLowerLeft, expectedCachedLowerLeft) == 0, [&]() {
            ok = 0;
        }).Else(printError);
    }).Else(printError);
    return ok;
}

Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner_UpperCachesOnly(int M, int N, int cacheRows, int cacheCols, int blockSize, int stripeSize, Vector expectedCachedUpperLeft, Vector expectedCachedUpperRight)
{
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");
    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto iBlock = schedule.Split(i, blockSize);
    auto iStripe = schedule.Split(i, stripeSize);
    auto jCache = schedule.Split(j, cacheCols);

    schedule.SetOrder({ iCache, jCache, iBlock, iStripe, j, i });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = blockSize * cacheCols;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

    // Get a handle to the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    int rawCacheSize = (int)rawCacheValue.GetLayout().NumElements();

    auto cachedUpperLeft = MakeVector<int>(rawCacheSize);
    auto cachedUpperRight = MakeVector<int>(rawCacheSize);

    // Add a low level API kernel to access the underlying cache after it has been filled
    auto cacheSpyKernel = loopnests::Kernel("cache_spy_kernel")
                              .Inputs(rawCacheValue, cachedUpperLeft, cachedUpperRight)
                              .Indices(iTopLevel, jTopLevel)
                              .Define([cacheCols](Value rawCacheValue, Vector cachedUpperLeft, Vector cachedUpperRight, Scalar i, Scalar j) {
                                  auto cacheView = rawCacheValue;
                                  cacheView.SetLayout({ { (int)rawCacheValue.GetLayout().NumElements() } });
                                  auto vectorCacheView = Vector(cacheView);
                                  If(i == 0,
                                     [&]() {
                                         // TODO : remove nested if's
                                         If(j == 0,
                                            [&]() {
                                                cachedUpperLeft = vectorCacheView;
                                            })
                                             .ElseIf(j == cacheCols,
                                                     [&]() {
                                                         cachedUpperRight = vectorCacheView;
                                                     });
                                     });
                              });
    auto cacheSpyPosition = loopnests::CodePositionConstraints{ loopnests::LoopFragmentType::epilogue, { iCache, jCache }, {} };
    nest.GetUnderlyingLoopNest().AddKernel(cacheSpyKernel, cacheSpyPosition);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    auto ok = MakeScalar<int>("ok");
    ok = 1;
    auto printError = [&] {
        DebugPrint("Upper Left:");
        DebugPrintVector(cachedUpperLeft);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperLeft);
        DebugPrint("\n");
        DebugPrint("\n");
        DebugPrint("Upper Right:");
        DebugPrintVector(cachedUpperRight);
        DebugPrint("\n");
        DebugPrintVector(expectedCachedUpperRight);
        DebugPrint("\n");
        DebugPrint("\n");
    };
    // TODO : replace nested if's
    If(VerifySame(cachedUpperLeft, expectedCachedUpperLeft) == 0, [&]() {
        If(VerifySame(cachedUpperRight, expectedCachedUpperRight) == 0, [&]() {
            ok = 0;
        }).Else(printError);
    }).Else(printError);
    return ok;
}

Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner_UpperLeftCacheOnly(int M, int N, int cacheRows, int cacheCols, int blockSize, int stripeSize, Vector expectedCachedUpperLeft)
{
    auto input = MakeIncrementingMatrix<int>(M, N, "input");
    auto output = MakeMatrix<int>(M, N, "output");
    Index i("i"), j("j");
    auto nest = Using({ input }, ArgumentType::Input)
                    .Using({ output }, ArgumentType::Output)
                    .ForAll(i, 0, M)
                    .ForAll(j, 0, N)
                    .Do([](Matrix input, Matrix output, Scalar i, Scalar j) {
                        output(i, j) = input(i, j);
                    });

    auto& schedule = nest.GetSchedule();

    auto iTopLevel = i;
    auto jTopLevel = j;

    auto iCache = schedule.Split(i, cacheRows);
    auto iBlock = schedule.Split(i, blockSize);
    auto iStripe = schedule.Split(i, stripeSize);
    auto jCache = schedule.Split(j, cacheCols);

    schedule.SetOrder({ iCache, jCache, iBlock, iStripe, j, i });

    ArgumentType argType = ArgumentType::Input;
    std::string cacheName = "cacheInput";
    size_t maxCacheElts = cacheRows * cacheCols;
    size_t fillThreshold = blockSize * cacheCols;
    std::function<void(Scalar, Scalar)> reduceFunction = CopyReduce;
    auto extraCacheParams = std::make_tuple(argType,
                                            cacheName,
                                            maxCacheElts,
                                            fillThreshold,
                                            reduceFunction,
                                            false);
    GeneralCachingStrategy cachingProvider{};
    schedule.Cache(cachingProvider,
                   input,
                   { iTopLevel, jTopLevel },
                   {},
                   {},
                   std::nullopt,
                   extraCacheParams);

    // Get a handle to the underlying cached memory
    auto rawCacheValue = cachingProvider._rawCache;
    int rawCacheSize = (int)rawCacheValue.GetLayout().NumElements();

    auto cachedUpperLeft = MakeVector<int>(rawCacheSize);

    // Add a low level API kernel to access the underlying cache after it has been filled
    auto cacheSpyKernel = loopnests::Kernel("cache_spy_kernel")
                              .Inputs(rawCacheValue, cachedUpperLeft)
                              .Indices(iTopLevel, jTopLevel)
                              .Define([](Value rawCacheValue, Vector cachedUpperLeft, Scalar i, Scalar j) {
                                  auto cacheView = rawCacheValue;
                                  cacheView.SetLayout({ { (int)rawCacheValue.GetLayout().NumElements() } });
                                  auto vectorCacheView = Vector(cacheView);
                                  If(i == 0,
                                     [&]() {
                                         // TODO : remove nested if's
                                         If(j == 0,
                                            [&]() {
                                                cachedUpperLeft = vectorCacheView;
                                            });
                                     });
                              });
    auto cacheSpyPosition = loopnests::CodePositionConstraints{ loopnests::LoopFragmentType::epilogue, { iCache, jCache }, {} };
    nest.GetUnderlyingLoopNest().AddKernel(cacheSpyKernel, cacheSpyPosition);

#if 0 // DEBUGGING
    DebugDump(nest.GetUnderlyingLoopNest());
#endif
    nest.Run();

    return VerifySame(cachedUpperLeft, expectedCachedUpperLeft);
}

// input matrix rows doesn't evenly divide cache rows
// input matrix cols evenly divides cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test1()
{
    int M = 7; // M doesn't evenly divide the number of cache rows
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int smallBlockSize = 2;
    int largeBlockSize = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7]
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // [16, 17, 18, 19, 20, 21, 22, 23]
    // [24, 25, 26, 27, 28, 29, 30, 31]
    // [32, 33, 34, 35, 36, 37, 38, 39]
    // [40, 41, 42, 43, 44, 45, 46, 47]
    // [48, 49, 50, 51, 52, 53, 54, 55]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  8,  1,  9,  2, 10,  3, 11,
            16, 24, 17, 25, 18, 26, 19, 27
        };
    Vector expectedCachedUpperRight =
        {
             4, 12,  5, 13,  6, 14,  7, 15,
            20, 28, 21, 29, 22, 30, 23, 31
        };
    Vector expectedCachedLowerLeft =
        {
            32, 40, 33, 41, 34, 42, 35, 43,
            48,  0, 49,  0, 50,  0, 51,  0
        };
    Vector expectedCachedLowerRight =
        {
            36, 44, 37, 45, 38, 46, 39, 47,
            52,  0, 53,  0, 54,  0, 55,  0
        };
    // clang-format on

    auto smallBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, smallBlockSize, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight, expectedCachedLowerLeft, expectedCachedLowerRight);
    auto largeBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, largeBlockSize, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight, expectedCachedLowerLeft, expectedCachedLowerRight);
    return smallBlockResult + largeBlockResult;
}

// input matrix rows doesn't evenly divide cache rows, does evenly divide blocksize/stripesize
// input matrix cols evenly divides cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test2()
{
    int M = 6; // M doesn't evenly divide the number of cache rows
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int smallBlockSize = 2;
    int largeBlockSize = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7]
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // [16, 17, 18, 19, 20, 21, 22, 23]
    // [24, 25, 26, 27, 28, 29, 30, 31]
    // [32, 33, 34, 35, 36, 37, 38, 39]
    // [40, 41, 42, 43, 44, 45, 46, 47]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  8,  1,  9,  2, 10,  3, 11,
            16, 24, 17, 25, 18, 26, 19, 27
        };
    Vector expectedCachedUpperRight =
        {
             4, 12,  5, 13,  6, 14,  7, 15,
            20, 28, 21, 29, 22, 30, 23, 31
        };
    Vector expectedCachedLowerLeft =
        {
            32, 40, 33, 41, 34, 42, 35, 43,
             0,  0,  0,  0,  0,  0,  0,  0
        };
    Vector expectedCachedLowerRight =
        {
            36, 44, 37, 45, 38, 46, 39, 47,
             0,  0,  0,  0,  0,  0,  0,  0
        };
    // clang-format on

    auto smallBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, smallBlockSize, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight, expectedCachedLowerLeft, expectedCachedLowerRight);
    auto largeBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, largeBlockSize, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight, expectedCachedLowerLeft, expectedCachedLowerRight);
    return smallBlockResult + largeBlockResult;
}

// input matrix rows doesn't evenly divide cache rows, does evenly divide blocksize/stripesize
// input matrix cols doesn't evenly divide cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test3()
{
    int M = 7; // M doesn't evenly divide the number of cache rows
    int N = 6;
    int cacheRows = 4;
    int cacheCols = 4;
    int smallBlockSize = 2;
    int largeBlockSize = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5]
    // [ 6,  7,  8,  9, 10, 11]
    // [12, 13, 14, 15, 16, 17]
    // [18, 19, 20, 21, 22, 23]
    // [24, 25, 26, 27, 28, 29]
    // [30, 31, 32, 33, 34, 35]
    // [36, 37, 38, 39, 40, 41]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  6,  1,  7,  2,  8,  3,  9,
            12, 18, 13, 19, 14, 20, 15, 21
        };
    Vector expectedCachedUpperRight =
        {
             4, 10,  5, 11, 16, 22, 17, 23,
             0,  0,  0,  0,  0,  0,  0,  0
        };
    Vector expectedCachedLowerLeft =
        {
            24, 30, 25, 31, 26, 32, 27, 33,
            36,  0, 37,  0, 38,  0, 39,  0
        };
    Vector expectedCachedLowerRight =
        {
            28, 34, 29, 35, 40,  0, 41,  0,
             0,  0,  0,  0,  0,  0,  0,  0
        };
    // clang-format on

    auto smallBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, smallBlockSize, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight, expectedCachedLowerLeft, expectedCachedLowerRight);
    auto largeBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, largeBlockSize, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight, expectedCachedLowerLeft, expectedCachedLowerRight);
    return smallBlockResult + largeBlockResult;
}

// input matrix rows doesn't evenly divide cache rows, does evenly divide blocksize/stripesize
// input matrix cols doesn't evenly divide cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test4()
{
    int M = 6; // M doesn't evenly divide the number of cache rows
    int N = 6;
    int cacheRows = 4;
    int cacheCols = 4;
    int smallBlockSize = 2;
    int largeBlockSize = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5],
    // [ 6,  7,  8,  9, 10, 11],
    // [12, 13, 14, 15, 16, 17],
    // [18, 19, 20, 21, 22, 23],
    // [24, 25, 26, 27, 28, 29],
    // [30, 31, 32, 33, 34, 35]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  6,  1,  7,  2,  8,  3,  9,
            12, 18, 13, 19, 14, 20, 15, 21
        };
    Vector expectedCachedUpperRight =
        {
             4, 10,  5, 11, 16, 22, 17, 23,
             0,  0,  0,  0,  0,  0,  0,  0
        };
    Vector expectedCachedLowerLeft =
        {
            24, 30, 25, 31, 26, 32, 27, 33,
             0,  0,  0,  0,  0,  0,  0,  0
        };
    Vector expectedCachedLowerRight =
        {
            28, 34, 29, 35,  0,  0,  0,  0,
             0,  0,  0,  0,  0,  0,  0,  0
        };
    // clang-format on

    auto smallBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, smallBlockSize, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight, expectedCachedLowerLeft, expectedCachedLowerRight);
    auto largeBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner(M, N, cacheRows, cacheCols, largeBlockSize, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight, expectedCachedLowerLeft, expectedCachedLowerRight);
    return smallBlockResult + largeBlockResult;
}

// input matrix rows < cache rows, doesn't evenly divide blocksize/stripesize
// input matrix cols evenly divides cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test5()
{
    int M = 3; // M < cache rows
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int smallBlockSize = 2;
    int largeBlockSize = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7],
    // [ 8,  9, 10, 11, 12, 13, 14, 15],
    // [16, 17, 18, 19, 20, 21, 22, 23]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  8,  1,  9,  2, 10,  3, 11,
            16,  0, 17,  0, 18,  0, 19,  0
        };
    Vector expectedCachedUpperRight =
        {
             4, 12,  5, 13,  6, 14,  7, 15,
            20,  0, 21,  0, 22,  0, 23,  0
        };
    // clang-format on

    auto smallBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner_UpperCachesOnly(M, N, cacheRows, cacheCols, smallBlockSize, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight);
    auto largeBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner_UpperCachesOnly(M, N, cacheRows, cacheCols, largeBlockSize, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight);
    return smallBlockResult + largeBlockResult;
}

// input matrix rows < cache rows, evenly divides blocksize/stripesize
// input matrix cols evenly divides cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test6()
{
    int M = 2; // M < cache rows, evenly divides stripesize
    int N = 8;
    int cacheRows = 4;
    int cacheCols = 4;
    int smallBlockSize = 2;
    int largeBlockSize = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1,  2,  3,  4,  5,  6,  7],
    // [ 8,  9, 10, 11, 12, 13, 14, 15]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0,  8,  1,  9,  2, 10,  3, 11,
             0,  0,  0,  0,  0,  0,  0,  0
        };
    Vector expectedCachedUpperRight =
        {
             4, 12,  5, 13,  6, 14,  7, 15,
             0,  0,  0,  0,  0,  0,  0,  0
        };
    // clang-format on

    auto smallBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner_UpperCachesOnly(M, N, cacheRows, cacheCols, smallBlockSize, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight);
    auto largeBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner_UpperCachesOnly(M, N, cacheRows, cacheCols, largeBlockSize, stripeSize, expectedCachedUpperLeft, expectedCachedUpperRight);
    return smallBlockResult + largeBlockResult;
}

// input matrix rows < cache rows, doesn't evenly divide blocksize/stripesize
// input matrix cols < cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test7()
{
    int M = 3; // M < cache rows, doens't evenly divide stripesize
    int N = 3;
    int cacheRows = 4;
    int cacheCols = 4;
    int smallBlockSize = 2;
    int largeBlockSize = 4;
    int stripeSize = 2;

    // input
    // A:
    // [0, 1, 2],
    // [3, 4, 5],
    // [6, 7, 8]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0, 3, 1, 4, 2, 5, 6, 0,
             7, 0, 8, 0, 0, 0, 0, 0
        };
    // clang-format on
    auto smallBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner_UpperLeftCacheOnly(M, N, cacheRows, cacheCols, smallBlockSize, stripeSize, expectedCachedUpperLeft);
    auto largeBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner_UpperLeftCacheOnly(M, N, cacheRows, cacheCols, largeBlockSize, stripeSize, expectedCachedUpperLeft);
    return smallBlockResult + largeBlockResult;
}

// input matrix rows < cache rows, evenly divides blocksize/stripesize
// input matrix cols < cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test8()
{
    int M = 2; // M < cache rows, evenly divides stripesize
    int N = 2;
    int cacheRows = 4;
    int cacheCols = 4;
    int smallBlockSize = 2;
    int largeBlockSize = 4;
    int stripeSize = 2;

    // input
    // A:
    // [0, 1]
    // [2, 3]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
             0, 2, 1, 3, 0, 0, 0, 0,
             0, 0, 0, 0, 0, 0, 0, 0
        };
    // clang-format on

    auto smallBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner_UpperLeftCacheOnly(M, N, cacheRows, cacheCols, smallBlockSize, stripeSize, expectedCachedUpperLeft);
    auto largeBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner_UpperLeftCacheOnly(M, N, cacheRows, cacheCols, largeBlockSize, stripeSize, expectedCachedUpperLeft);
    return smallBlockResult + largeBlockResult;
}

// input matrix rows multiple of cache rows
// input matrix cols < cache cols
// blockSize == stripeSize == cacheRows / 2
Scalar GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Test9()
{
    int M = 8;
    int N = 2; // N < cache cols
    int cacheRows = 4;
    int cacheCols = 4;
    int smallBlockSize = 2;
    int largeBlockSize = 4;
    int stripeSize = 2;

    // input
    // A:
    // [ 0,  1],
    // [ 2,  3],
    // [ 4,  5],
    // [ 6,  7],
    // [ 8,  9],
    // [10, 11],
    // [12, 13],
    // [14, 15]
    // clang-format off
    Vector expectedCachedUpperLeft =
        {
            0, 2, 1, 3, 4, 6, 5, 7,
            0, 0, 0, 0, 0, 0, 0, 0
        };
    Vector expectedCachedLowerLeft =
        {
            8, 10,  9, 11, 12, 14, 13, 15,
            0,  0,  0,  0,  0,  0,  0,  0
        };
    // clang-format on

    auto smallBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner_LeftCachesOnly(M, N, cacheRows, cacheCols, smallBlockSize, stripeSize, expectedCachedUpperLeft, expectedCachedLowerLeft);
    auto largeBlockResult = GeneralCachingStrategy_ProgressiveBLASNCopy_ValidateMemory_BoundaryCondition_Runner_LeftCachesOnly(M, N, cacheRows, cacheCols, largeBlockSize, stripeSize, expectedCachedUpperLeft, expectedCachedLowerLeft);
    return smallBlockResult + largeBlockResult;
}

} // namespace ell
