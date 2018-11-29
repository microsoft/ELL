////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IIRFilterValue_test.h (emittable_functions)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <value/include/EmitterContext.h>

#include <memory>
#include <vector>

namespace ell
{
void TestIIRFilter();

void TestIIRFilter(std::vector<double> signal, std::vector<double> b, std::vector<double> a, std::vector<double> expected);
} // namespace ell
