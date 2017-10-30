////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Nodes_test.tcc (nodes_test)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// testing
#include "testing.h"
#include "LinearPredictor.h"
#include "LinearPredictorNode.h"

using namespace ell;
using namespace ell::nodes;

template <typename ElementType>
void TestLinearPredictorNodeCompute()
{
    const int dim = 5;
    math::ColumnVector<ElementType> weights({ 1, 2, 3, 4, 5 });
    ElementType bias = 1.5f;

    predictors::LinearPredictor<ElementType> predictor(weights, bias);

    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(dim);
    auto predictorNode = model.AddNode<nodes::LinearPredictorNode<ElementType>>(inputNode->output, predictor);
    auto outputNode = model.AddNode<model::OutputNode<ElementType>>(predictorNode->output);

    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", outputNode->output } });

    std::vector<ElementType> input{ 1.0, 2.0, 1.0, -1.0, 0.5 };
    auto result = map.Compute<ElementType>(input);

    testing::ProcessTest("TestLinearPredictorNodeCompute", testing::IsEqual(result[0], static_cast<ElementType>(8.0)));
}

template <typename ElementType>
void TestLinearPredictorNodeRefine()
{
    // make a linear predictor
    size_t dim = 3;
    predictors::LinearPredictor<ElementType> predictor(dim);
    predictor.GetBias() = 2.0;
    predictor.GetWeights() = math::ColumnVector<ElementType>{ 3.0, 4.0, 5.0 };

    // make a model
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(3);
    auto linearPredictorNode = model.AddNode<nodes::LinearPredictorNode<ElementType>>(inputNode->output, predictor);

    // refine the model
    model::TransformContext context;
    model::ModelTransformer transformer;
    auto newModel = transformer.RefineModel(model, context);

    // check for equality
    auto newInputNode = transformer.GetCorrespondingInputNode(inputNode);
    auto newOutputElements = transformer.GetCorrespondingOutputs(model::PortElements<ElementType>{ linearPredictorNode->output });
    inputNode->SetInput({ 1.0, 1.0, 1.0 });
    newInputNode->SetInput({ 1.0, 1.0, 1.0 });
    auto modelOutputValue = model.ComputeOutput(linearPredictorNode->output)[0];
    auto newOutputValue = newModel.ComputeOutput(newOutputElements)[0];

    testing::ProcessTest("Testing LinearPredictorNode refine", testing::IsEqual(modelOutputValue, newOutputValue));
}

