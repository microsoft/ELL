////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     test.cpp (layers_test)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// layers
#include "Map.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <string>


void TestMap()
{
    float val1 = 0;
    float val2 = 0;
    testing::ProcessTest("Comparing ", testing::IsEqual(val1, val2, 1.0e-8));
}

/// Runs all tests
///
int main()
{
    TestMap();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}




