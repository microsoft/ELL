////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (features_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Features_test.h"

// testing
#include "testing.h"


/// Runs all tests
///
int main()
{
   TestMagnitudeFeature();
    
    TestMeanFeature();

    TestIncrementalMeanFeature();

    TestVarianceFeature();

    if(testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}




