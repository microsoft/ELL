////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (common_test)
//  Authors:  Ofer Dekel, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoadDataset_test.h"
#include "LoadMap_test.h"
#include "LoadModel_test.h"

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
    using namespace ell;

    try
    {
        TestLoadSampleModels();
        TestLoadTreeModels();
        TestLoadSavedModels();

        TestSaveModels();

        TestLoadMapWithDefaultArgs();
        TestLoadMapWithPorts();

        TestLoadDataset();
        TestLoadMappedDataset();
    }
    catch (const utilities::Exception& exception)
    {
        std::cerr << "ERROR, got ELL exception. Message: " << exception.GetMessage() << std::endl;
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

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
