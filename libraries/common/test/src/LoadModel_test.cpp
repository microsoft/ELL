
//
// Loading model tests
//

#include "LoadModel_test.h"

#include "LoadTestModels.h"

#include <model/include/Model.h>

#include <utilities/include/Files.h>

#include <testing/include/testing.h>

#include <iostream>

namespace ell
{
namespace
{
    const int expectedModel1Size = 7;
    const int expectedModel2Size = 6;
    const int expectedModel3Size = 8;

    const int expectedTreeModel0Size = 23;
    const int expectedTreeModel1Size = 64;
    const int expectedTreeModel2Size = 104;
    const int expectedTreeModel3Size = 144;
} // namespace

void TestLoadSampleModels()
{
    auto model1 = common::LoadTestModel("[1]");
    auto model2 = common::LoadTestModel("[2]");
    auto model3 = common::LoadTestModel("[3]");

    testing::ProcessTest("Testing model 1 size", model1.Size() == expectedModel1Size);
    testing::ProcessTest("Testing model 2 size", model2.Size() == expectedModel2Size);
    testing::ProcessTest("Testing model 3 size", model3.Size() == expectedModel3Size);
}

void TestLoadTreeModels()
{
    auto model0 = common::LoadTestModel("[tree_0]");
    auto model1 = common::LoadTestModel("[tree_1]");
    auto model2 = common::LoadTestModel("[tree_2]");
    auto model3 = common::LoadTestModel("[tree_3]");

    testing::ProcessTest("Testing tree model 0 size", model0.Size() == expectedTreeModel0Size);
    testing::ProcessTest("Testing tree model 1 size", model1.Size() == expectedTreeModel1Size);
    testing::ProcessTest("Testing tree model 2 size", model2.Size() == expectedTreeModel2Size);
    testing::ProcessTest("Testing tree model 3 size", model3.Size() == expectedTreeModel3Size);
}

void TestLoadSavedModels(const std::string& examplePath)
{
    std::cout << "Testing loading of saved model" << std::endl;
    auto model1 = common::LoadModel(utilities::JoinPaths(examplePath, { "models", "model_1.model" }));
    std::cout << "Model 1 size: " << model1.Size() << std::endl;
    testing::ProcessTest("Testing saved model 1 size", model1.Size() == expectedModel1Size);
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

    testing::ProcessTest("Testing model 1 size", newModel1.Size() == expectedModel1Size);
    testing::ProcessTest("Testing model 2 size", newModel2.Size() == expectedModel2Size);
    testing::ProcessTest("Testing model 3 size", newModel3.Size() == expectedModel3Size);
    testing::ProcessTest("Testing tree model 0 size", newTree0.Size() == expectedTreeModel0Size);
    testing::ProcessTest("Testing tree model 1 size", newTree1.Size() == expectedTreeModel1Size);
    testing::ProcessTest("Testing tree model 2 size", newTree2.Size() == expectedTreeModel2Size);
    testing::ProcessTest("Testing tree model 3 size", newTree3.Size() == expectedTreeModel3Size);
}
} // namespace ell
