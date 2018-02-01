////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoadTestModels.cpp (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoadModel.h"

// model
#include "InputNode.h"
#include "Model.h"
#include "OutputNode.h"

// nodes
#include "BinaryOperationNode.h"
#include "DelayNode.h"
#include "DotProductNode.h"
#include "ForestPredictorNode.h"
#include "L2NormSquaredNode.h"
#include "LinearPredictorNode.h"
#include "MovingAverageNode.h"
#include "MovingVarianceNode.h"

// predictors
#include "ForestPredictor.h"
#include "LinearPredictor.h"
#include "SingleElementThresholdPredictor.h"

// utilities
#include "Archiver.h"
#include "Files.h"
#include "JsonArchiver.h"

// stl
#include <cstdint>
#include <istream>
#include <vector>

namespace ell
{
namespace common
{
    model::Model GetModel1()
    {
        // For now, just create a model and return it
        const size_t dimension = 3;
        model::Model model;
        auto inputNode = model.AddNode<model::InputNode<double>>(dimension);
        auto mean8 = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, 8);
        auto var8 = model.AddNode<nodes::MovingVarianceNode<double>>(inputNode->output, 8);
        auto mean16 = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, 16);
        auto var16 = model.AddNode<nodes::MovingVarianceNode<double>>(inputNode->output, 16);

        // classifier
        auto inputs = model::Concat(model::MakePortElements(mean8->output), model::MakePortElements(var8->output), model::MakePortElements(mean16->output), model::MakePortElements(var16->output));
        predictors::LinearPredictor<double> predictor(inputs.Size());
        // Set some values into the predictor's vector
        for (size_t index = 0; index < inputs.Size(); ++index)
        {
            predictor.GetWeights()[index] = (double)(index % 5);
        }
        model.AddNode<nodes::LinearPredictorNode<double>>(inputs, predictor);
        return model;
    }

    model::Model GetModel2()
    {
        const size_t dimension = 3;
        model::Model model;
        auto inputNode = model.AddNode<model::InputNode<double>>(dimension);

        // one "leg"
        auto mean1 = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, 8);
        auto mag1 = model.AddNode<nodes::L2NormSquaredNode<double>>(mean1->output);

        // other "leg"
        auto mag2 = model.AddNode<nodes::L2NormSquaredNode<double>>(inputNode->output);
        auto mean2 = model.AddNode<nodes::MovingAverageNode<double>>(mag2->output, 8);

        // combine them
        model.AddNode<nodes::BinaryOperationNode<double>>(mag1->output, mean2->output, emitters::BinaryOperationType::subtract);
        return model;
    }

    model::Model GetModel3()
    {
        const size_t dimension = 3;
        model::Model model;
        auto inputNode = model.AddNode<model::InputNode<double>>(dimension);
        auto lowpass = model.AddNode<nodes::MovingAverageNode<double>>(inputNode->output, 16);
        auto highpass = model.AddNode<nodes::BinaryOperationNode<double>>(inputNode->output, lowpass->output, emitters::BinaryOperationType::subtract);

        auto delay1 = model.AddNode<nodes::DelayNode<double>>(highpass->output, 4);
        auto delay2 = model.AddNode<nodes::DelayNode<double>>(highpass->output, 8);

        auto dot1 = model.AddNode<nodes::DotProductNode<double>>(highpass->output, delay1->output);
        auto dot2 = model.AddNode<nodes::DotProductNode<double>>(highpass->output, delay2->output);

        model.AddNode<nodes::BinaryOperationNode<double>>(dot1->output, dot2->output, emitters::BinaryOperationType::subtract);
        return model;
    }

    predictors::SimpleForestPredictor CreateForest(size_t numSplits)
    {
        // define some abbreviations
        using SplitAction = predictors::SimpleForestPredictor::SplitAction;
        using SplitRule = predictors::SingleElementThresholdPredictor;
        using EdgePredictorVector = std::vector<predictors::ConstantPredictor>;

        // build a forest
        predictors::SimpleForestPredictor forest;
        SplitRule dummyRule{ 0, 0.0 };
        EdgePredictorVector dummyEdgePredictor{ -1.0, 1.0 };
        auto root = forest.Split(SplitAction{ forest.GetNewRootId(), dummyRule, dummyEdgePredictor });
        std::vector<size_t> interiorNodeVector;
        interiorNodeVector.push_back(root);

        for (size_t index = 0; index < numSplits; ++index)
        {
            auto node = interiorNodeVector.back();
            interiorNodeVector.pop_back();
            interiorNodeVector.push_back(forest.Split(SplitAction{ forest.GetChildId(node, 0), dummyRule, dummyEdgePredictor }));
            interiorNodeVector.push_back(forest.Split(SplitAction{ forest.GetChildId(node, 1), dummyRule, dummyEdgePredictor }));
        }
        return forest;
    }

    model::Model GetTreeModel(size_t numSplits)
    {
        auto forest = CreateForest(numSplits);
        model::Model model;
        auto inputNode = model.AddNode<model::InputNode<double>>(3);
        model.AddNode<nodes::SimpleForestPredictorNode>(inputNode->output, forest);
        return model;
    }

    model::Model GetRefinedTreeModel(size_t numSplits)
    {
        auto model = GetTreeModel(numSplits);
        model::TransformContext context;
        model::ModelTransformer transformer;
        auto refinedModel = transformer.RefineModel(model, context);
        return refinedModel;
    }

    template <typename UnarchiverType>
    model::Model LoadArchivedModel(std::istream& stream)
    {
        utilities::SerializationContext context;
        RegisterNodeTypes(context);
        UnarchiverType unarchiver(stream, context);
        model::Model model;
        unarchiver.Unarchive(model);
        return model;
    }

    model::Model LoadTestModel(const std::string& name)
    {
        const std::string treePrefix = "[tree_";
        if (name == "")
        {
            return model::Model{};
        }
        if (name == "[1]")
        {
            return GetModel1();
        }
        if (name == "[2]")
        {
            return GetModel2();
        }
        else if (name == "[3]")
        {
            return GetModel3();
        }
        else if (name.find(treePrefix) == 0)
        {
            auto pos = name.find(treePrefix);
            auto suffix = name.substr(pos + treePrefix.size());
            auto numSplits = std::stoi(suffix);
            return GetRefinedTreeModel(numSplits);
        }
        else
        {
            throw utilities::SystemException(utilities::SystemExceptionErrors::fileNotFound);
        }
    }
}
}
