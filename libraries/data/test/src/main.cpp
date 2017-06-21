////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (dataset_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataVector_test.h"
#include "Dataset_test.h"
#include "Example_test.h"
#include "Parser_test.h"

// testing
#include "testing.h"

using namespace ell;

/// Runs all tests
///
int main()
{
    IDataVectorTests();
    DataVectorCopyAsTests();
    AutoDataVectorTest();
    TransformedDataVectorTest();
    IteratorTests();
    ExampleCopyAsTests();
    DatasetCastingTests();
    DataVectorParseTest();
    AutoDataVectorParseTest();
    SingleFileParseTest();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
