////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Scalar_test.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <value/include/Scalar.h>

namespace ell
{
value::Scalar Scalar_test1();
value::Scalar Scalar_test2();
value::Scalar ScalarRefTest();
value::Scalar ScalarRefRefTest();
value::Scalar ScalarRefRefRefTest();
value::Scalar RefScalarRefTest();
value::Scalar RefScalarRefCtorsTest();
value::Scalar RefScalarRefRefTest();
value::Scalar RefScalarRefRefRefTest();
value::Scalar SequenceLogicalAndTest();
value::Scalar SequenceLogicalAndTestWithCopy();
} // namespace ell
