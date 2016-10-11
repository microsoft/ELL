////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (dataset_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataVector_test.h"
#include "Example_test.h"
#include "Dataset_test.h"

// testing
#include "testing.h"

using namespace emll;

/// Runs all tests
///
int main()
{
    IDataVectorTests();
    ToDataVectorTests();
    AutoDataVectorTest();
    ToExampleTests();
    DatasetCastingTests();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
