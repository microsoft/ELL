////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LoadModelGraph.cpp (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoadModelGraph.h"

// model
#include "ModelGraph.h"
#include "InputNode.h"
// #include "OutputNode.h"
#include "ExtremalValueNode.h"

// nodes
#include "MovingAverageNode.h"
#include "MovingVarianceNode.h"
#include "LinearPredictorNode.h"
#include "L2NormNode.h"
#include "DelayNode.h"
#include "BinaryOperationNode.h"
#include "DotProductNode.h"

// predictors
#include "LinearPredictor.h"

// utilities
#include "Files.h"

namespace common
{
    model::Model GetModel1()
    {
        // For now, just create a model and return it
        const int dimension = 3;
        model::Model model;
        auto inputNode = model.AddNode<model::InputNode<double>>(dimension);
        auto mean8 = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, 8);
        auto var8 = model.AddNode<nodes::MovingVarianceNode<double>>(inputNode->output, 8);
        auto mean16 = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, 16);
        auto var16 = model.AddNode<nodes::MovingVarianceNode<double>>(inputNode->output, 16);

        // classifier
        auto inputs = model::Concat(model::MakeOutputPortElements(mean8->output), model::MakeOutputPortElements(var8->output), model::MakeOutputPortElements(mean16->output), model::MakeOutputPortElements(var16->output));
        predictors::LinearPredictor predictor(inputs.Size());
        // Set some values into the predictor's vector
        for (int index = 0; index < inputs.Size(); ++index)
        {
            predictor.GetWeights()[index] = (double)(index % 5);
        }
        auto classifierNode = model.AddNode<nodes::LinearPredictorNode>(inputs, predictor);
//        auto outputNode = model.AddNode<model::OutputNode<double>>(classifierNode->output);
        return model;
    }

    model::Model GetModel2()
    {
        const int dimension = 3;
        model::Model model;
        auto inputNode = model.AddNode<model::InputNode<double>>(dimension);

        // one "leg"        
        auto mean1 = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, 8);
        auto mag1 = model.AddNode<nodes::L2NormNode<double>>(mean1->output);

        // other "leg"
        auto mag2 = model.AddNode<nodes::L2NormNode<double>>(inputNode->output);
        auto mean2 = model.AddNode<nodes::MovingAverageNode<double>>(mag2->output, 8);

        // combine them
        auto diff = model.AddNode<nodes::BinaryOperationNode<double>>(mag1->output, mean2->output, nodes::BinaryOperationNode<double>::OperationType::subtract);

//        auto output = model.AddNode<model::OutputNode<double>>(maxVal->val);
        return model;
    }

    model::Model GetModel3()
    {
        const int dimension = 3;
        model::Model model;
        auto inputNode = model.AddNode<model::InputNode<double>>(dimension);
        auto lowpass = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, 16);
        auto highpass = model.AddNode<nodes::BinaryOperationNode<double>>(inputNode->output, lowpass->output, nodes::BinaryOperationNode<double>::OperationType::subtract);

        auto delay1 = model.AddNode<nodes::DelayNode<double>>(highpass->output, 4);
        auto delay2 = model.AddNode<nodes::DelayNode<double>>(highpass->output, 8);

        auto dot1 = model.AddNode<nodes::DotProductNode<double>>(highpass->output, delay1->output);
        auto dot2 = model.AddNode<nodes::DotProductNode<double>>(highpass->output, delay2->output);

        auto dotDifference = model.AddNode<nodes::BinaryOperationNode<double>>(dot1->output, dot2->output, nodes::BinaryOperationNode<double>::OperationType::subtract);

//        auto output = model.AddNode<model::OutputNode<double>>(classifierNode->output);
        return model;
    }

    model::Model LoadModelGraph(const std::string& filename)
    {
        if (filename == "2")
        {
            return GetModel2();
        }
        else if (filename == "3")
        {
            return GetModel3();
        }
        else
        {
            return GetModel1();
        }
    }
}
