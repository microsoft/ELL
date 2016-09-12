
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Variant_test.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Variant_test.h"

// utilities
#include "TypeName.h"
#include "Variant.h"

// testing
#include "testing.h"

// stl
#include <vector>

namespace emll
{
void TestScalarVariant()
{
    utilities::Variant v = utilities::MakeVariant<int>(5);

    testing::ProcessTest("Variant check type", v.IsType<int>());
    testing::ProcessTest("Variant check type", !v.IsType<float>());
    testing::ProcessTest("Variant check type", !v.IsType<bool>());

    testing::ProcessTest("Variant check value", v.GetValue<int>() == 5);

    v = (int)3;
    testing::ProcessTest("Variant check type", v.IsType<int>());

    v = (float)3.25;
    testing::ProcessTest("Variant check type", v.IsType<float>());
    testing::ProcessTest("Variant check value", v.GetValue<float>() == 3.25);
}

void TestVectorVariant()
{
    auto intVecName = utilities::TypeName<std::vector<int>>::GetName();
    utilities::Variant v = utilities::MakeVariant<std::vector<int>>(std::vector<int>{ 3 });

    testing::ProcessTest("Variant check type", v.IsType<std::vector<int>>());
    testing::ProcessTest("Variant check type", !v.IsType<float>());
    testing::ProcessTest("Variant check type", !v.IsType<bool>());

    testing::ProcessTest("Variant check value", v.GetValue<std::vector<int>>()[0] == 3);
}
}
