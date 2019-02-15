////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (passes_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Optimization pass tests
//

#include "ModelOptimizerTest.h"
#include "TransformationTest.h"

#include <testing/include/testing.h>

#include <utilities/include/Exception.h>

#include <iostream>

using namespace ell;

int main()
{
    try
    {
        TestFuseLinearOpsPass();

        TestOptimizeReorderDataNodes1();
        TestOptimizeReorderDataNodes2();
        TestOptimizeReorderDataNodes3();
        TestOptimizeReorderDataNodes4();

        TestSetConvolutionMethodPass();

        // Test Transformations
        TestTransformations();
    }
    catch (const utilities::Exception& exception)
    {
        std::cerr << "ERROR, got ELL exception. Message: " << exception.GetMessage() << std::endl;
        return 1;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "ERROR, got unhandled exception. Message: " << exception.what() << std::endl;
        return 1;
    }

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
