////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TestUtil.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <value/include/Matrix.h>
#include <value/include/Scalar.h>
#include <value/include/Tensor.h>
#include <value/include/Vector.h>

#include <string>

namespace ell
{
value::Scalar EqualEpsilon(value::Scalar x, value::Scalar y, double epsilon);
value::Scalar NotEqualEpsilon(value::Scalar x, value::Scalar y, double epsilon);

value::Scalar Verify(value::Vector actual, value::Vector expected, double epsilon = 1e-7);
value::Scalar VerifyDifferent(value::Vector actual, value::Vector expected, double epsilon = 1e-7);
value::Scalar Verify(value::Matrix actual, value::Matrix expected, double epsilon = 1e-7);
value::Scalar Verify(value::Tensor actual, value::Tensor expected, double epsilon = 1e-7);

void PrintMatrix(std::string indent, value::Matrix e);
void DebugPrint(std::string message);
void DebugPrint(value::Vector message); // expecting null terminated ValueType::Char8
void DebugPrintVector(value::Vector data);
void DebugPrintScalar(value::Scalar value);
} // namespace ell
