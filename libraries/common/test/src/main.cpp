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
#include "Files.h"

// stl
#include <iostream>

/// Runs all tests
///
int main(int argc, char* argv[])
{
    using namespace ell;

    try
    {
        std::string basePath = ell::utilities::GetDirectoryPath(argv[0]);
        std::string examplePath = ell::utilities::JoinPaths(basePath, { "..", "..", "..", "examples" });
        if (argc > 1) 
        {
            examplePath = argv[1];
        }
        std::cout << "using model path: " << examplePath << std::endl;
        TestLoadSampleModels();
        TestLoadTreeModels();
        TestLoadSavedModels(examplePath);

        TestSaveModels();

        TestLoadMapWithDefaultArgs(examplePath);
        TestLoadMapWithPorts(examplePath);

        TestLoadDataset(examplePath);
        TestLoadMappedDataset(examplePath);
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
