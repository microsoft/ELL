////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoopNestAPI_test.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
namespace value
{
    class Scalar;
}
value::Scalar LoopNest_api_test1();
value::Scalar LoopNest_api_test2();
value::Scalar LoopNest_api_test3();
value::Scalar LoopNest_api_test4();
value::Scalar LoopNest_api_test5();
value::Scalar LoopNest_api_Parallelized_test1();
value::Scalar LoopNest_api_Parallelized_test2();
value::Scalar LoopNest_api_Unrolled_test1();
value::Scalar LoopNest_api_SetOrder_test1();
value::Scalar LoopNest_api_CachedMatrix_test1();
value::Scalar LoopNest_api_SlidingCachedMatrix_test();
value::Scalar SimpleGemm_HighLevelAPI();
value::Scalar SimpleGemm_HighLevelAPI_NoCachingHelper();
value::Scalar MLAS_GEMM_GeneralCachingStrategy();
value::Scalar OneSplitBoundaryTest();
value::Scalar TwoSplitBoundaryTest();
value::Scalar SplitLargerThanSizeBoundaryTest();
value::Scalar TwoSplitsLargerThanSizeBoundaryTest();
value::Scalar LoopNest_api_tunable_parameters_test1();

} // namespace ell
