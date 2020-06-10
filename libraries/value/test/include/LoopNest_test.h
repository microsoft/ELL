////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoopNest_test.h (value)
//  Authors:  Kern Handa, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <value/include/Scalar.h>

namespace ell
{
value::Scalar SplitIterationDomain_test1();

// Loop nest tests
value::Scalar LoopNest_test1();
value::Scalar LoopNest_test2();
value::Scalar LoopNest_test3();
value::Scalar LoopNest_test4();
value::Scalar LoopNest_test5();
value::Scalar LoopNest_test6();

value::Scalar LoopNestNonzeroStart_test();
value::Scalar LoopNestBoundary_test1();
value::Scalar LoopNestBoundary_test2();
value::Scalar LoopNestBoundary_test3();
value::Scalar LoopNestBoundary_test4();
value::Scalar LoopNestBoundary_test5();
value::Scalar LoopNestReorder_test1();
value::Scalar LoopNestReorder_test2();
value::Scalar TwoKernel_test();

value::Scalar LoopNestLastPredicate_test1();
value::Scalar LoopNestLastPredicate_test2();
value::Scalar LoopNestLastPredicate_test3();
value::Scalar LoopNestLastPredicate_test4();
value::Scalar LoopNestBoundaryPredicate_test1();

value::Scalar MissingIndex_test();
value::Scalar RequiredIndex_test();
value::Scalar SimpleImperfectNest_test();
value::Scalar ImperfectNest_test_ijk();
value::Scalar ImperfectNest_test_ikj();
value::Scalar ImperfectNest_test_kij();
value::Scalar ImperfectNest_test_ijkijk();
value::Scalar ImperfectNest_test_kijijk();
value::Scalar ImperfectNest_test_ijkkij();
value::Scalar SplitIndex_test1_old();
value::Scalar SplitIndex_test1();
value::Scalar SplitIndex_test2();
value::Scalar SplitIndex_test3();
value::Scalar EpilogueIndex_test();
value::Scalar RenameKernelArg_test();

value::Scalar NonInnermostKernel_test1();
value::Scalar NonInnermostKernel_test2();
value::Scalar NonInnermostKernel_test3();
value::Scalar NonInnermostKernel_test4();
value::Scalar CachedMatrix_test1();
value::Scalar CachedMatrix_test1_new();
value::Scalar CachedMatrix_test2();
value::Scalar CachedMatrix_test3();
value::Scalar CachedMatrix_test4();
value::Scalar CachedMatrix_test5();

value::Scalar LoopNest_Parallelized_test1();
value::Scalar LoopNest_Parallelized_test2();

value::Scalar LoopNest_Unrolled_test1();

value::Scalar LoopNest_DebugDump_test1();
value::Scalar LoopNest_DebugDump_test2();

value::Scalar SimpleMatMult_test();
value::Scalar GotoBLASGemm_LowLevelAPI();
value::Scalar GotoBLASGemmWithRefDeref();
value::Scalar YG12LowLevel_TestBoundary();

value::Scalar KernelPredicate_test();
value::Scalar MatMul3_test1();
value::Scalar MatMul3_test2();
value::Scalar LoopNestFuse_test1();
value::Scalar LoopNestFuse_test2();
value::Scalar LoopNestFuse_test3();
value::Scalar ConvertedConstraint_test1();
value::Scalar ConvertedConstraint_test2();
} // namespace ell
