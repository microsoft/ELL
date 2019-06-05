////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TestTransformData.cpp (finetune_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TestTransformData.h"
#include "TransformData.h"

#include <model/include/InputNode.h>
#include <model/include/Model.h>
#include <model/include/OutputNode.h>

#include <nodes/include/UnaryOperationNode.h>

#include <testing/include/testing.h>

#include <utilities/include/MemoryLayout.h>

#include <iostream>
#include <numeric>
#include <vector>

using namespace ell;
using namespace ell::model;
using namespace ell::testing;

namespace
{
bool IsEqual(const UnlabeledDataContainer& a, const UnlabeledDataContainer& b, float epsilon)
{
    if (a.Size() != b.Size())
    {
        return false;
    }

    for (size_t i = 0; i < a.Size(); ++i)
    {
        if (!testing::IsEqual(a[i].ToArray(), b[i].ToArray(), epsilon))
        {
            return false;
        }
    }
    return true;
}

UnlabeledDataContainer GetTestDataset(int numRows, int numColumns)
{
    UnlabeledDataContainer result;
    std::vector<float> row(numColumns);
    for (int i = 0; i < numRows; ++i)
    {
        std::iota(row.begin(), row.end(), static_cast<float>(i));
        result.Add({ row });
    }

    return result;
}

UnlabeledDataContainer GetConstantDataset(int numRows, int numColumns, float value)
{
    UnlabeledDataContainer result;
    std::vector<float> row(numColumns, value);
    for (int i = 0; i < numRows; ++i)
    {
        result.Add({ row });
    }

    return result;
}

Model GetTestModel(int featureSize)
{
    //  in -> sqrt -> square
    Model model;
    auto in = model.AddNode<InputNode<float>>(featureSize);
    auto n1 = model.AddNode<nodes::UnaryOperationNode<float>>(in->output, nodes::UnaryOperationType::sqrt);
    auto n2 = model.AddNode<nodes::UnaryOperationNode<float>>(n1->output, nodes::UnaryOperationType::square);
    auto n3 = model.AddNode<nodes::UnaryOperationNode<float>>(n2->output, nodes::UnaryOperationType::exp);
    auto n4 = model.AddNode<nodes::UnaryOperationNode<float>>(n3->output, nodes::UnaryOperationType::log);
    model.AddNode<OutputNode<float>>(n4->output);
    return model;
}

const OutputPort<float>& GetModelOutput(const Model& model)
{
    auto nodes = model.GetNodesByType<OutputNode<float>>();
    return nodes[0]->output;
}

std::vector<const OutputPortBase*> GetModelOutputPorts(const Model& model)
{
    std::vector<const OutputPortBase*> result;
    auto it = model.GetNodeIterator();
    while (it.IsValid())
    {
        auto node = it.Get();
        for (auto p : node->GetOutputPorts())
        {
            result.push_back(p);
        }
        it.Next();
    }
    return result;
}
} // namespace

// prototypes
void TestRemovePaddingNoPadding();
void TestRemovePaddingWithPadding();

// implementation
void TestTransformData()
{
    FailOnException(TestRemovePadding);
    FailOnException(TestTransformDataWithModel);
    FailOnException(TestTransformDataWithSubmodel);
    FailOnException(TestTransformDataWithCachedSubmodel);
}

void TestRemovePadding()
{
    TestRemovePaddingNoPadding();
    TestRemovePaddingWithPadding();
}

void TestRemovePaddingNoPadding()
{
    utilities::MemoryLayout layout({ 2, 2, 2 });
    std::vector<float> data(layout.GetMemorySize());
    std::iota(data.begin(), data.end(), 0);
    // Now data is a 2x2x2 thing containing:
    // 0,1    2,3
    // 4,5    6,7
    auto unpadded = RemovePadding(data, layout);
    ProcessTest("RemovePadding of unpadded memory", IsEqual(unpadded, data));
}

void TestRemovePaddingWithPadding()
{
    utilities::MemoryLayout layout(utilities::MemoryShape{ 2, 2, 2 }, utilities::MemoryShape{ 1, 1, 0 });
    std::vector<float> data(layout.GetMemorySize());
    std::iota(data.begin(), data.end(), 0);
    // Now data is a 4x4x2 thing containing:
    //  0,1      2,3     4,5     6,7
    //  8,9    10,11   12,13   14,15
    // 16,17   18,19   20,21   22,23
    // 24,25   26,27   28,29   30,31
    //
    // where the "active" part is just:
    //  10,11   12,13
    //  18,19   20,21

    auto unpadded = RemovePadding(data, layout);
    std::vector<float> expected = { 10, 11, 12, 13, 18, 19, 20, 21 };
    ProcessTest("RemovePadding of padded memory", IsEqual(unpadded, expected));
}

void TestTransformDataWithModel()
{
    auto dataset = GetTestDataset(3, 4);
    auto model = GetTestModel(4);
    const auto& modelOutput = GetModelOutput(model);
    auto outputPorts = GetModelOutputPorts(model);

    auto transformedData = TransformDataWithModel(dataset, modelOutput);
    for (size_t i = 0; i < dataset.Size(); ++i)
    {
        ProcessTest("Testing transformed model", IsEqual(dataset[i].ToArray(), transformedData[i].ToArray(), 0.0001f));
    }

    auto transformedData1 = TransformDataWithModel(dataset, static_cast<const OutputPort<float>&>(*outputPorts[1]));
    for (size_t i = 0; i < dataset.Size(); ++i)
    {
        auto row = dataset[i].ToArray();
        std::transform(row.begin(), row.end(), row.begin(), [](auto x) { return std::sqrt(x); });
        ProcessTest("Testing transformed model", IsEqual(row, transformedData1[i].ToArray(), 0.0001f));
    }

    auto transformedData2 = TransformDataWithModel(dataset, static_cast<const OutputPort<float>&>(*outputPorts[2]));
    for (size_t i = 0; i < dataset.Size(); ++i)
    {
        ProcessTest("Testing transformed model", IsEqual(dataset[i].ToArray(), transformedData2[i].ToArray(), 0.0001f));
    }
}

void TestTransformDataWithSubmodel()
{
    auto dataset = GetTestDataset(3, 4);
    auto model = GetTestModel(4);
    const auto& modelOutput = GetModelOutput(model);
    auto outputPorts = GetModelOutputPorts(model);

    Submodel submodel{ { &modelOutput } };
    auto transformedData = TransformDataWithSubmodel(dataset, submodel);
    for (size_t i = 0; i < dataset.Size(); ++i)
    {
        ProcessTest("Testing transformed submodel", IsEqual(dataset[i].ToArray(), transformedData[i].ToArray(), 0.0001f));
    }

    Submodel submodel1{ { outputPorts[1] } };
    auto transformedData1 = TransformDataWithSubmodel(dataset, submodel1);
    for (size_t i = 0; i < dataset.Size(); ++i)
    {
        auto row = dataset[i].ToArray();
        std::transform(row.begin(), row.end(), row.begin(), [](auto x) { return std::sqrt(x); });
        ProcessTest("Testing transformed submodel", IsEqual(row, transformedData1[i].ToArray(), 0.0001f));
    }

    Submodel submodel2{ { outputPorts[2] } };
    auto transformedData2 = TransformDataWithSubmodel(dataset, submodel2);
    for (size_t i = 0; i < dataset.Size(); ++i)
    {
        ProcessTest("Testing transformed submodel", IsEqual(dataset[i].ToArray(), transformedData2[i].ToArray(), 0.0001f));
    }
}

void TestTransformDataWithCachedSubmodel()
{
    ModelOutputDataCache cache;
    int numRows = 3;
    int numColumns = 4;
    auto dataset = GetTestDataset(numRows, numColumns);
    auto model = GetTestModel(numColumns);
    const auto& modelOutput = GetModelOutput(model);
    auto outputPorts = GetModelOutputPorts(model);

    UnlabeledDataContainer sqrtDataset;
    for (int i = 0; i < numRows; ++i)
    {
        auto row = dataset[i].ToArray();
        std::transform(row.begin(), row.end(), row.begin(), [](auto x) { return std::sqrt(x); });
        sqrtDataset.Add({ row });
    }

    Submodel submodel{ { &modelOutput } };
    auto transformedData = TransformDataWithSubmodel(dataset, submodel, cache);
    ProcessTest("Testing transformed cached submodel", IsEqual(dataset, transformedData, 0.0001f));

    Submodel submodel1{ { outputPorts[1] } };
    auto transformedData1 = TransformDataWithSubmodel(dataset, submodel1, cache);
    ProcessTest("Testing transformed cached submodel", IsEqual(sqrtDataset, transformedData1, 0.0001f));

    Submodel submodel2{ { outputPorts[2] } };
    auto transformedData2 = TransformDataWithSubmodel(dataset, submodel2, cache);
    ProcessTest("Testing transformed cached submodel", IsEqual(dataset, transformedData2, 0.0001f));

    auto transformedData1_take2 = TransformDataWithSubmodel(dataset, submodel1, cache);
    ProcessTest("Testing transformed cached submodel", IsEqual(sqrtDataset, transformedData1_take2, 0.0001f));

    auto bogusDataset = GetConstantDataset(3, 4, 12345);
    cache.SetCachedData(submodel1.GetOutputs()[0], bogusDataset);
    auto transformedData1_take3 = TransformDataWithSubmodel(dataset, submodel1, cache);
    ProcessTest("Testing transformed cached submodel with broken cache", !IsEqual(sqrtDataset, transformedData1_take3, 0.0001f));
}
