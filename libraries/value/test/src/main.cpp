////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Value_test.h"

#include <utilities/include/Exception.h>

#include <testing/include/testing.h>

#include <iostream>

int main()
{
    using namespace ell;
    using namespace utilities;
    try
    {
        for (const auto& context : GetContexts())
        {
            value::ContextGuard guard(*context);

            // Value tests
            If_test1();
            Value_test1();
            Scalar_test1();
            Vector_test1();
            Vector_test2();
            Matrix_test1();
            Matrix_test2();
            Matrix_test3();
            Tensor_test1();
            Tensor_test2();
            Tensor_test3();
            Tensor_slice_test1();
            Casting_test1();
            Accumulate_test();
            Dot_test();
            Intrinsics_test1();
            Intrinsics_test2();
        }
    }
    catch (const Exception& exception)
    {
        std::cerr << "ERROR, got ELL exception. Message: " << exception.GetMessage() << std::endl;
        throw;
    }

    if (testing::DidTestFail())
    {
        return 1;
    }
    return 0;
}
