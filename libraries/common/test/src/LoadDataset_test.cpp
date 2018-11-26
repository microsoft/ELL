
//
// Loading map tests
//

#include "LoadMap_test.h"

#include <common/include/DataLoadArguments.h>
#include <common/include/DataLoaders.h>
#include <common/include/LoadModel.h>
#include <common/include/MapLoadArguments.h>

#include <testing/include/testing.h>

#include <utilities/include/Files.h>

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
} // namespace ell
