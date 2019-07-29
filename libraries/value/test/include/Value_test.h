////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Value_test.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <value/include/EmitterContext.h>

#include <functional>
#include <memory>

namespace ell
{
value::Scalar Basic_test();
value::Scalar DebugPrint_test();
value::Scalar Value_test1();
value::Scalar Scalar_test1();
value::Scalar Vector_test1();
value::Scalar Vector_test2();
value::Scalar Vector_test3();
value::Scalar Matrix_test1();
value::Scalar Matrix_test2();
value::Scalar Matrix_test3();
value::Scalar Reshape_test();
value::Scalar GEMV_test();
value::Scalar Tensor_test1();
value::Scalar Tensor_test2();
value::Scalar Tensor_test3();
value::Scalar Tensor_slice_test1();
value::Scalar Casting_test1();
value::Scalar If_test1();
value::Scalar Sum_test();
value::Scalar Dot_test();
value::Scalar Intrinsics_test1();
value::Scalar Intrinsics_test2();
value::Scalar For_test1();
value::Scalar For_test2();
value::Scalar ForRangeCasting_test1();
value::Scalar ForRangeCasting_test2();
value::Scalar Parallelized_test1();
value::Scalar Parallelized_test2();
value::Scalar Prefetch_test1();
value::Scalar ScalarRefTest();
value::Scalar ScalarRefRefTest();
value::Scalar ScalarRefRefRefTest();
value::Scalar MatrixReferenceTest();
value::Scalar RefScalarRefTest();
value::Scalar RefScalarRefCtorsTest();
value::Scalar RefScalarRefRefTest();
value::Scalar RefScalarRefRefRefTest();
value::Scalar RefMatrixReferenceTest();

void DebugPrint(value::Vector message); // expecting null terminated ValueType::Char8
void DebugPrintVector(value::Vector data);
void DebugPrintScalar(value::Scalar value);

} // namespace ell
