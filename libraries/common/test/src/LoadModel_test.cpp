
//
// Loading model tests
//

#include "LoadModel_test.h"

// common
#include "LoadTestModels.h"
#include "Files.h"

// model
#include "Model.h"

// testing
#include "testing.h"

// stl
#include <iostream>

namespace ell
{
void TestLoadSampleModels()
{
    auto model1 = common::LoadTestModel("[1]");
    auto model2 = common::LoadTestModel("[2]");
    auto model3 = common::LoadTestModel("[3]");

    std::cout << "Model 1 size: " << model1.Size() << std::endl;
    std::cout << "Model 2 size: " << model2.Size() << std::endl;
    std::cout << "Model 3 size: " << model3.Size() << std::endl;
}

void TestLoadTreeModels()
{
    auto model0 = common::LoadTestModel("[tree_0]");
    auto model1 = common::LoadTestModel("[tree_1]");
    auto model2 = common::LoadTestModel("[tree_2]");
    auto model3 = common::LoadTestModel("[tree_3]");

    std::cout << "Tree 0 size: " << model0.Size() << std::endl;
    std::cout << "Tree 1 size: " << model1.Size() << std::endl;
    std::cout << "Tree 2 size: " << model2.Size() << std::endl;
    std::cout << "Tree 3 size: " << model3.Size() << std::endl;
}

void TestLoadSavedModels(const std::string& examplePath)
{
    std::cout << "Testing loading of saved model" << std::endl;
    auto model1 = common::LoadModel(utilities::JoinPaths(examplePath, { "models", "model_1.model" }));
    std::cout << "Model 1 size: " << model1.Size() << std::endl;
}

void TestSaveModels()
{
    std::string ext = "model";
    auto model1 = common::LoadTestModel("[1]");
    auto model2 = common::LoadTestModel("[2]");
    auto model3 = common::LoadTestModel("[3]");
    auto tree0 = common::LoadTestModel("[tree_0]");
    auto tree1 = common::LoadTestModel("[tree_1]");
    auto tree2 = common::LoadTestModel("[tree_2]");
    auto tree3 = common::LoadTestModel("[tree_3]");

    common::SaveModel(model1, "model_1." + ext);
    common::SaveModel(model2, "model_2." + ext);
    common::SaveModel(model3, "model_3." + ext);
    common::SaveModel(tree0, "tree_0." + ext);
    common::SaveModel(tree1, "tree_1." + ext);
    common::SaveModel(tree2, "tree_2." + ext);
    common::SaveModel(tree3, "tree_3." + ext);

    auto newModel1 = common::LoadModel("model_1." + ext);
    auto newModel2 = common::LoadModel("model_2." + ext);
    auto newModel3 = common::LoadModel("model_3." + ext);
    auto newTree0 = common::LoadModel("tree_0." + ext);
    auto newTree1 = common::LoadModel("tree_1." + ext);
    auto newTree2 = common::LoadModel("tree_2." + ext);
    auto newTree3 = common::LoadModel("tree_3." + ext);
}
}
