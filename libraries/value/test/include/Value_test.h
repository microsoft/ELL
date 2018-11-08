////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Value_test.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterContext.h"

// stl
#include <memory>
#include <vector>

namespace ell
{

void Value_test1();
void Scalar_test1();
void Vector_test1();
void Matrix_test1();
void Matrix_test2();
void Tensor_test1();
void Tensor_test2();

std::vector<std::unique_ptr<value::EmitterContext>> GetContexts();

} // namespace ell
