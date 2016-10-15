////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (common_test)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoadModel_test.h"
#include "LoadMap_test.h"

// testing
#include "testing.h"

// utilities
#include "Exception.h"

// stl
#include <iostream>

/// Runs all tests
///
int main()
{
    using namespace emll;

    try
    {
        TestLoadSampleModels();
        TestLoadTreeModels();

        TestSaveModels("xml");
        TestSaveModels("json");

        TestLoadMapWithDefaultArgs();
        TestLoadMapWithPorts();
    }
    catch (const utilities::Exception& exception)
    {
        std::cerr << "ERROR, got EMLL exception. Message: " << exception.GetMessage() << std::endl;
        throw;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "ERROR, got std exception. Message: " << exception.what() << std::endl;
        throw;
    }
    catch (...)
    {
        std::cerr << "ERROR, got unknown exception." << std::endl;
        throw;
    }
}
