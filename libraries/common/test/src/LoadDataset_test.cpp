
//
// Loading map tests
//

#include "LoadMap_test.h"

// common
#include "DataLoadArguments.h"
#include "DataLoaders.h"
#include "LoadModel.h"
#include "MapLoadArguments.h"

// testing
#include "testing.h"

// utilities
#include "Files.h"

// stl
#include <iostream>

namespace ell
{

void TestLoadDataset(const std::string& examplePath)
{
    auto stream = utilities::OpenIfstream(utilities::JoinPaths(examplePath, { "data", "testData.txt" }));
    auto dataset = common::GetDataset(stream);
}

void TestLoadMappedDataset(const std::string& examplePath)
{
    common::MapLoadArguments args;
    args.inputModelFilename = utilities::JoinPaths(examplePath, { "models", "model_1.model" });
    args.modelInputsString = "";
    args.modelOutputsString = "1026.output";

    auto map = common::LoadMap(args);
    auto stream = utilities::OpenIfstream(utilities::JoinPaths(examplePath, { "data", "testData.txt" }));
    auto dataset = common::GetDataset(stream);
    dataset = common::TransformDataset(dataset, map);
}
}
