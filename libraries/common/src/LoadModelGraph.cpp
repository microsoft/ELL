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
#include "OutputNode.h"

// nodes
#include "MovingAverageNode.h"
#include "MovingVarianceNode.h"
#include "LinearPredictorNode.h"

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
        auto inputs = model::Concat(model::MakeOutputPortElementList(mean8->output), model::MakeOutputPortElementList(var8->output), model::MakeOutputPortElementList(mean16->output), model::MakeOutputPortElementList(var16->output));
        predictors::LinearPredictor predictor(inputs.Size());
        // Set some values into the predictor's vector
        for (int index = 0; index < inputs.Size(); ++index)
        {
            predictor.GetVector()[index] = (double)(index % 5);
        }
        auto classifierNode = model.AddNode<nodes::LinearPredictorNode>(inputs, predictor);

        return model;
    }

    model::Model GetModel2()
    {
        const int dimension = 3;
        model::Model model;
        auto inputNode = model.AddNode<model::InputNode<double>>(dimension);
        auto mean8 = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, 8);
        auto var8 = model.AddNode<nodes::MovingVarianceNode<double>>(inputNode->output, 8);
        auto mean16 = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, 16);
        auto var16 = model.AddNode<nodes::MovingVarianceNode<double>>(inputNode->output, 16);

        // classifier
        auto inputs = model::Concat(model::MakeOutputPortElementList(mean8->output), model::MakeOutputPortElementList(var8->output), model::MakeOutputPortElementList(mean16->output), model::MakeOutputPortElementList(var16->output));
        predictors::LinearPredictor predictor(inputs.Size());
        // Set some values into the predictor's vector
        for (int index = 0; index < inputs.Size(); ++index)
        {
            predictor.GetVector()[index] = (double)(index % 5);
        }
        auto classifierNode = model.AddNode<nodes::LinearPredictorNode>(inputs, predictor);

//        auto output = model.AddNode<model::OutputNode<double>>(classifierNode->output);
        return model;
    }

    model::Model LoadModelGraph(const std::string& filename)
    {
        if (filename == "2")
        {
            return GetModel2();
        }
        else
        {
            return GetModel1();
        }
    }
}
