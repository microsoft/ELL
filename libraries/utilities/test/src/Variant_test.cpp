
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Variant_test.cpp (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Variant_test.h"

// utilities
#include "Variant.h"

// testing
#include "testing.h"

void TestVariant()
{
    utilities::Variant v = utilities::MakeVariant<int>(5);

    // testing::ProcessTest("Variant check type", v.IsType<int>());
    // testing::ProcessTest("Variant check type", !v.IsType<float>());
    // testing::ProcessTest("Variant check type", !v.IsType<bool>());

    // testing::ProcessTest("Variant check value", v.GetValue<int>() == 5);


    // v = (int)3;
    // testing::ProcessTest("Variant check type", v.IsType<int>());
    
    // v = (float)3.25;
    // testing::ProcessTest("Variant check type", v.IsType<float>());
    // testing::ProcessTest("Variant check value", v.GetValue<float>() == 3.25);
}
