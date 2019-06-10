////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Value_test.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <value/include/EmitterContext.h>

#include <memory>
#include <functional>

namespace ell
{
value::Scalar Basic_test();
value::Scalar DebugPrint_test();
value::Scalar Value_test1();
value::Scalar Scalar_test1();
value::Scalar Vector_test1();
value::Scalar Vector_test2();
value::Scalar Matrix_test1();
value::Scalar Matrix_test2();
value::Scalar Matrix_test3();
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

void DebugPrint(std::string message);
void DebugPrint(value::Vector message); // expecting null terminated ValueType::Char8
void DebugPrintVector(value::Vector data);

} // namespace ell
