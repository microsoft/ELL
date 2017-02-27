////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (predictors_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForestPredictor.h"
#include "NeuralNetworkPredictor.h"
#include "ReLUActivation.h"
#include "MaxPoolingFunction.h"

// testing
#include "testing.h"

using namespace ell;

void ForestPredictorTest()
{
    // define some abbreviations
    using SplitAction = predictors::SimpleForestPredictor::SplitAction;
    using SplitRule = predictors::SingleElementThresholdPredictor;
    using EdgePredictorVector = std::vector<predictors::ConstantPredictor>;

    // add a tree
    predictors::SimpleForestPredictor forest;
    auto tree0Root = forest.Split(SplitAction{ forest.GetNewRootId(), SplitRule{ 0, 0.3 }, EdgePredictorVector{ -1.0, 1.0 } });
    forest.Split(SplitAction{ forest.GetChildId(0, 0), SplitRule{ 1, 0.6 }, EdgePredictorVector{ -2.0, 2.0 } });
    forest.Split(SplitAction{ forest.GetChildId(0, 1), SplitRule{ 2, 0.9 }, EdgePredictorVector{ -4.0, 4.0 } });

    // add another tree
    auto tree1Root = forest.Split(SplitAction{ forest.GetNewRootId(), SplitRule{ 0, 0.2 }, EdgePredictorVector{ -3.0, 3.0 } });

    // test NumTrees
    testing::ProcessTest("Testing ForestPredictor, NumTrees()", forest.NumTrees() == 2);

    // test NumInteriorNodes
    testing::ProcessTest("Testing ForestPredictor, NumInteriorNodes()", forest.NumInteriorNodes() == 4);
    testing::ProcessTest("Testing ForestPredictor, NumInteriorNodes(tree0)", forest.NumInteriorNodes(tree0Root) == 3);
    testing::ProcessTest("Testing ForestPredictor, NumInteriorNodes(tree1)", forest.NumInteriorNodes(tree1Root) == 1);

    // test NumEdges
    testing::ProcessTest("Testing ForestPredictor, NumEdges()", forest.NumEdges() == 8);
    testing::ProcessTest("Testing ForestPredictor, NumEdges(tree0)", forest.NumEdges(tree0Root) == 6);
    testing::ProcessTest("Testing ForestPredictor, NumEdges(tree1)", forest.NumEdges(tree1Root) == 2);

    // test Compute
    using ExampleType = predictors::SimpleForestPredictor::DataVectorType;
    double output = forest.Predict(ExampleType{ 0.2, 0.5, 0.0 }, tree0Root);
    testing::ProcessTest("Testing ForestPredictor, Compute(tree0)", testing::IsEqual(output, -3.0, 1.0e-8));

    output = forest.Predict(ExampleType{ 0.18, 0.7, 0.0 }, tree0Root);
    testing::ProcessTest("Testing ForestPredictor, Compute(tree0)", testing::IsEqual(output, 1.0, 1.0e-8));

    output = forest.Predict(ExampleType{ 0.5, 0.7, 0.7 }, tree0Root);
    testing::ProcessTest("Testing ForestPredictor, Compute(tree0)", testing::IsEqual(output, -3.0, 1.0e-8));

    output = forest.Predict(ExampleType{ 0.5, 0.7, 1.0 }, tree0Root);
    testing::ProcessTest("Testing ForestPredictor, Compute(tree0)", testing::IsEqual(output, 5.0, 1.0e-8));

    output = forest.Predict(ExampleType{ 0.18, 0.5, 0.0 });
    testing::ProcessTest("Testing ForestPredictor, Compute()", testing::IsEqual(output, -6.0, 1.0e-8));

    output = forest.Predict(ExampleType{ 0.25, 0.7, 0.0 });
    testing::ProcessTest("Testing ForestPredictor, Compute()", testing::IsEqual(output, 4.0, 1.0e-8));

    // test path generation
    auto edgeIndicator = forest.GetEdgeIndicatorVector(ExampleType{ 0.25, 0.7, 0.0 });
    testing::ProcessTest("Testing ForestPredictor, SetEdgeIndicatorVector()", testing::IsEqual(edgeIndicator, std::vector<bool>{ 1, 0, 0, 1, 0, 0, 0, 1 }));
}

void LayersTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;

    // Verify Pooling layer. One with padding, one without
    neural::PoolingParameters poolingParams = { 4, 4, 2, 2, 2, 0 }; // { widht, height, depth, poolingSize, stride, padding }
    neural::PoolingLayer<MaxPoolingFunction> poolingLayer1(poolingParams);

    ILayer::LayerVector input1{ 
        2, 3, 6, 0,
        6, 7, 9, 7,
        9, 9, 4, 7,
        1, 4, 5, 3,
        8, 7, 8, 1,
        2, 2, 1, 5,
        2, 9, 3, 2,
        8, 3, 2, 4 };
    ILayer::LayerVector P1{ 
        7, 9,
        9, 7,
        8, 8,
        9, 4 };
    poolingLayer1.FeedForward(input1);
    testing::ProcessTest("Testing NeuralNetworkPredictor, Pooling Layer no padding", poolingLayer1.GetOutput() == P1);

    poolingParams.width = 2;
    poolingParams.height = 2;
    poolingParams.depth = 2;
    poolingParams.padding = 1;
    poolingParams.poolingSize = 3;
    poolingParams.stride = 1;
    neural::PoolingLayer<MaxPoolingFunction> poolingLayer2(poolingParams);
    ILayer::LayerVector input2{ 
        2, 3,
        6, 7,
        9, 9,
        1, 4 };
    ILayer::LayerVector P2{ 
        7, 7,
        7, 7,
        9, 9,
        9, 9 };
    poolingLayer2.FeedForward(input2);
    testing::ProcessTest("Testing NeuralNetworkPredictor, Pooling Layer with padding and overlapping stride", poolingLayer2.GetOutput() == P2);

    // Verify convolutional layer
    neural::ConvolutionalParameters convolutionalParams = { 2, 1, 2, 3, 1, 1, 2 }; // { widht, height, depth, receptiveFieldSize, stride, padding, numFilters }
    std::vector<double> weights{ 
        1, 3, 2, 3, 1, 1, 2, 3, 1,
        2, 4, 1, 3, 1, 2, 1, 4, 2,
        1, 2, 1, 2, 3, 2, 1, 2, 1,
        0, 3, 2, 3, 1, 2, 1, 0, 2 };
    std::vector<double> bias{0.1, 0.2};
    ILayer::LayerVector input3{ 
        2, 1,
        3, 2 };
    neural::ConvolutionalLayer convolutionalLayer(convolutionalParams, weights, bias);
    ILayer::LayerVector C1{
        10.1, 18.1,
        15.2, 18.2 };
    convolutionalLayer.FeedForward(input3);
    testing::ProcessTest("Testing NeuralNetworkPredictor, Convolutional Layer", convolutionalLayer.GetOutput() == C1);
}

void NeuralNetworkPredictorTest()
{
    using namespace ell::predictors;
    using namespace ell::predictors::neural;

    // Verify individual layers not tested below
    LayersTest();

    // Build an XOR net from previously trained values.
    NeuralNetworkPredictor::Layers layers;
    layers.push_back(std::unique_ptr<ILayer>(new InputLayer(2)));
    layers.push_back(std::unique_ptr<ILayer>(new FullyConnectedLayer(3, 2, std::vector<double>{-0.97461396, 1.40845299, -0.14135513, -0.54136097, 0.99313086, -0.99083692})));
    layers.push_back(std::unique_ptr<ILayer>(new BiasLayer(3, std::vector<double>{-0.43837756, -0.90868396, -0.0323102})));
    layers.push_back(std::unique_ptr<ILayer>(new ActivationLayer<ReLUActivation>(3)));
    layers.push_back(std::unique_ptr<ILayer>(new FullyConnectedLayer(1, 3, std::vector<double>{1.03084767, -0.10772263, 1.04077697})));
    layers.push_back(std::unique_ptr<ILayer>(new BiasLayer(1, std::vector<double>{1.40129846e-45})));

    NeuralNetworkPredictor nerualnetwork(std::move(layers));
    std::vector<double> output;

    // Check  the result for the 4 permutations of input. This validates that:
    // - the weights loaded correctly.
    // - the operations in each layer are working correctly
    // - the feed forward logic is working correctly

    output = nerualnetwork.Predict(NeuralNetworkPredictor::DataVectorType{0, 0});
    testing::ProcessTest("Testing NeuralNetworkPredictor, Predict of XOR net for 0 0 ", testing::IsEqual(output[0], 0.0, 1.0e-5));

    output = nerualnetwork.Predict(NeuralNetworkPredictor::DataVectorType{ 0, 1 });
    testing::ProcessTest("Testing NeuralNetworkPredictor, Predict of XOR net for 0 1 ", testing::IsEqual(output[0], 1.0, 1.0e-5));

    output = nerualnetwork.Predict(NeuralNetworkPredictor::DataVectorType{ 1, 0 });
    testing::ProcessTest("Testing NeuralNetworkPredictor, Predict of XOR net for 1 0 ", testing::IsEqual(output[0], 1.0, 1.0e-5));

    output = nerualnetwork.Predict(NeuralNetworkPredictor::DataVectorType{ 1, 1 });
    testing::ProcessTest("Testing NeuralNetworkPredictor, Predict of XOR net for 1 1 ", testing::IsEqual(output[0], 0.0, 1.0e-5));
}

/// Runs all tests
///
int main()
{
    ForestPredictorTest();
    NeuralNetworkPredictorTest();

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
