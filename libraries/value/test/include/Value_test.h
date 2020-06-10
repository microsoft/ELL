////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Value_test.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <value/include/Scalar.h>

namespace ell
{
value::Scalar Basic_test();
value::Scalar DebugPrint_test();
value::Scalar Value_test1();
value::Scalar Array_test1();
value::Scalar Casting_test1();
value::Scalar If_test1();
value::Scalar Sum_test();
value::Scalar Dot_test();
value::Scalar Intrinsics_test1();
value::Scalar Intrinsics_test2();
value::Scalar For_test1();
value::Scalar For_test2();
value::Scalar ForInsideIf_test();
value::Scalar While_test();
value::Scalar WhileInsideIf_test();
value::Scalar ForRangeCasting_test1();
value::Scalar ForRangeCasting_test2();
value::Scalar Parallelized_test1();
value::Scalar Parallelized_test2();
value::Scalar Parallelized_test3();
value::Scalar Prefetch_test1();

value::Scalar Prefetch_parallelized_test1();
value::Scalar Fma_test1();
value::Scalar Fma_test2();
value::Scalar Fma_test3();
value::Scalar UniqueName_test1();
value::Scalar Parallelized_ComputeContext_test1();

value::Scalar MemCopy_test1();
value::Scalar MemSet_test1();

value::Scalar NamedLoops_test1();

value::Scalar ThreadLocalAllocation_test1();

value::Scalar FunctionPointer_test1();

} // namespace ell
