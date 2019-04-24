////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TransformData.cpp (utilities)
//  Authors:  Byron Changuion, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TransformData.h"
#include "DataUtils.h"
#include "ModelUtils.h"

#include <model/include/Map.h>

#include <utilities/include/MemoryLayout.h>

#include <memory>
#include <vector>

using namespace ell;
using namespace model;

namespace
{
template <typename T1, typename T2>
std::vector<T1> CastVector(const std::vector<T2>& v)
{
    auto size = v.size();
    std::vector<T1> result(size);
    for (size_t i = 0; i < size; ++i)
    {
        result[i] = static_cast<T1>(v[i]);
    }
    return result;
}

template <typename DataVector, typename LabelType>
void AddExample(UnlabeledDataContainer& dataset, const DataVector& example, const LabelType& label)
{
    dataset.Add(CastVector<float>(example));
}

template <typename DataVector, typename LabelType>
void AddExample(BinaryLabelDataContainer& dataset, const DataVector& data, const LabelType& label)
{
    dataset.push_back({ CastVector<float>(data), label });
}

template <typename DataVector, typename LabelType>
void AddExample(MultiClassDataContainer& dataset, const DataVector& data, const LabelType& label)
{
    dataset.Add({ CastVector<float>(data), label });
}

template <typename DataVector, typename LabelType>
void AddExample(VectorLabelDataContainer& dataset, const DataVector& data, const LabelType& label)
{
    dataset.push_back({ CastVector<float>(data), label });
}

template <typename ExampleType>
auto GetInput(const ExampleType& example)
{
    return example.input;
}

template <typename ExampleType>
auto GetOutput(const ExampleType& example)
{
    return example.output;
}

template <>
auto GetInput(const UnlabeledExample& example)
{
    return example;
}

template <>
auto GetOutput(const UnlabeledExample& example)
{
    return 0;
}

template <typename DataVectorType, typename Visitor>
void Visit(const DataVectorType& data, const utilities::MemoryLayout& layout, int currentDimension, int currentOffset, Visitor visitor)
{
    auto offset = layout.GetOffset(currentDimension);
    auto length = layout.GetActiveSize(currentDimension);
    bool isInnerLoop = currentDimension == layout.NumDimensions() - 1;
    for (int i = 0; i < length; ++i)
    {
        auto index = (i + offset) * layout.GetCumulativeIncrement(currentDimension);
        if (isInnerLoop)
        {
            visitor(data[index + currentOffset]);
        }
        else
        {
            Visit(data, layout, currentDimension + 1, currentOffset + index, visitor);
        }
    }
}

template <typename DataVectorType, typename Visitor>
void Visit(const DataVectorType& data, const utilities::MemoryLayout& layout, Visitor visitor)
{
    Visit(data, layout, 0, 0, visitor);
}
} // namespace

// prototypes
template <typename DatasetType>
double GetModelAccuracyImpl(Model& model, const OutputPortBase& output, const DatasetType& testDataset);
template <typename ElementType, typename DatasetType>
double GetModelAccuracyImpl(Model& model, const OutputPort<ElementType>& output, const DatasetType& testDataset);

// implementation
template <typename DataVectorType>
DataVectorType RemovePadding(const DataVectorType& data, const utilities::MemoryLayout& layout)
{
    DataVectorType result(layout.NumElements());
    int index = 0;
    Visit(data, layout, [&result, &index](auto value) {
        result[index] = value;
        ++index;
    });

    return result;
}

template <typename ElementType>
std::unique_ptr<Map> GetMapForModel(Model& model, const OutputPort<ElementType>& output, bool compile = false);

template <typename DataContainerType, typename ElementType>
DataContainerType TransformDataInputsWithModelImpl(const DataContainerType& dataset, Model& model, const OutputPort<ElementType>& output)
{
    auto map = GetMapForModel(model, output);

    auto inputSize = map->GetInputSize(0);
    DataContainerType result;
    for (size_t i = 0; i < dataset.Size(); ++i)
    {
        auto example = dataset[i];
        const auto& exampleData = GetInput(example);
        std::vector<ElementType> input = CastVector<ElementType>(exampleData.ToArray());
        input.resize(inputSize);
        auto pred = map->template Compute<ElementType>(input);
        //        pred = RemovePadding(pred, output.GetMemoryLayout());
        AddExample(result, pred, GetOutput(example));
    }
    return result;
}

template <typename ElementType>
std::unique_ptr<Map> GetMapForModel(Model& model, const OutputPort<ElementType>& output, bool compile)
{
    TransformContext context;
    ModelTransformer transformer;
    auto newModel = transformer.CopyModel(model);
    const auto& newOutput = transformer.GetCorrespondingOutputs(output);
    const auto& newNewOutput = RemoveSourceAndSinkNodes(newModel, newOutput);

    Submodel submodel({ &newNewOutput });
    auto prunedSubmodel = transformer.CopySubmodel(submodel, context);
    const auto& prunedOutput = *prunedSubmodel.GetOutputs()[0];

    auto inputNode = GetInputNode<ElementType>(prunedSubmodel.GetModel(), prunedOutput);
    if (!inputNode)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::badData, "Couldn't find input nodes in model");
    }

    Map map(prunedSubmodel.GetModel(), { { "input", inputNode } }, { { "output", prunedOutput } });
    if (!compile)
    {
        return std::make_unique<Map>(map);
    }

    MapCompilerOptions settings;
    settings.compilerSettings.targetDevice.deviceName = "host";
    settings.compilerSettings.allowVectorInstructions = true;
    settings.compilerSettings.optimize = true;
    model::ModelOptimizerOptions optimizerOptions;
    model::IRMapCompiler compiler(settings, optimizerOptions);

    return std::make_unique<IRCompiledMap>(compiler.Compile(map));
}

template <typename ElementType>
UnlabeledDataContainer TransformDataWithModel(const UnlabeledDataContainer& dataset, Model& model, const OutputPort<ElementType>& output)
{
    return TransformDataInputsWithModelImpl(dataset, model, output);
}

template <typename ElementType>
BinaryLabelDataContainer TransformDataInputsWithModel(const BinaryLabelDataContainer& dataset, Model& model, const OutputPort<ElementType>& output)
{
    auto inputNode = GetInputNode<ElementType>(model, output);
    if (!inputNode)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::badData, "Couldn't find input nodes in model");
    }

    auto originalModelOutputs = TransformDataInputsWithModelImpl(dataset, model, output);
    return originalModelOutputs;
}

template <typename ElementType>
MultiClassDataContainer TransformDataInputsWithModel(const MultiClassDataContainer& dataset, Model& model, const OutputPort<ElementType>& output)
{
    auto inputNode = GetInputNode<ElementType>(model, output);
    if (!inputNode)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::badData, "Couldn't find input nodes in model");
    }

    auto originalModelOutputs = TransformDataInputsWithModelImpl(dataset, model, output);
    return originalModelOutputs;
}

template <typename ElementType>
VectorLabelDataContainer TransformDataInputsWithModel(const VectorLabelDataContainer& dataset, Model& model, const OutputPort<ElementType>& output)
{
    auto inputNode = GetInputNode<ElementType>(model, output);
    if (!inputNode)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::badData, "Couldn't find input nodes in model");
    }

    auto originalModelOutputs = TransformDataInputsWithModelImpl(dataset, model, output);
    return originalModelOutputs;
}

double GetModelAccuracy(Model& model, const OutputPortBase& output, const BinaryLabelDataContainer& testDataset)
{
    return GetModelAccuracyImpl(model, output, testDataset);
}

double GetModelAccuracy(Model& model, const OutputPortBase& output, const MultiClassDataContainer& testDataset)
{
    return GetModelAccuracyImpl(model, output, testDataset);
}

template <typename DatasetType>
double GetModelAccuracyImpl(Model& model, const OutputPortBase& output, const DatasetType& testDataset)
{
    switch (output.GetType())
    {
    case Port::PortType::smallReal:
    {
        return GetModelAccuracyImpl<float>(model, static_cast<const OutputPort<float>&>(output), testDataset);
        break;
    }
    case Port::PortType::real:
    {
        return GetModelAccuracyImpl<double>(model, static_cast<const OutputPort<double>&>(output), testDataset);
        break;
    }
    default:
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Unexpected output type for model. Should be double or float.");
        break;
    };
}

template <typename ElementType, typename DatasetType>
double GetModelAccuracyImpl(Model& model, const OutputPort<ElementType>& output, const DatasetType& testDataset)
{
    auto predictions = GetDatasetInputs(TransformDataInputsWithModel(testDataset, model, output));
    return GetModelAccuracy(testDataset, predictions);
}

// Explicit instantiation definitions
template UnlabeledDataContainer TransformDataWithModel(const UnlabeledDataContainer& dataset, Model& model, const OutputPort<float>& output);
template BinaryLabelDataContainer TransformDataInputsWithModel(const BinaryLabelDataContainer& dataset, Model& model, const OutputPort<float>& output);
template MultiClassDataContainer TransformDataInputsWithModel(const MultiClassDataContainer& dataset, Model& model, const OutputPort<float>& output);
template VectorLabelDataContainer TransformDataInputsWithModel(const VectorLabelDataContainer& dataset, Model& model, const OutputPort<float>& output);

template UnlabeledDataContainer TransformDataWithModel(const UnlabeledDataContainer& dataset, Model& model, const OutputPort<double>& output);
template BinaryLabelDataContainer TransformDataInputsWithModel(const BinaryLabelDataContainer& dataset, Model& model, const OutputPort<double>& output);
template MultiClassDataContainer TransformDataInputsWithModel(const MultiClassDataContainer& dataset, Model& model, const OutputPort<double>& output);
template VectorLabelDataContainer TransformDataInputsWithModel(const VectorLabelDataContainer& dataset, Model& model, const OutputPort<double>& output);

template std::vector<float> RemovePadding(const std::vector<float>& data, const utilities::MemoryLayout& layout);
template std::vector<double> RemovePadding(const std::vector<double>& data, const utilities::MemoryLayout& layout);
