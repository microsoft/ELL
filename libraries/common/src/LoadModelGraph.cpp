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
#include "ExtremalValueNode.h"

// nodes
#include "MovingAverageNode.h"
#include "MovingVarianceNode.h"
#include "LinearPredictorNode.h"
#include "L2NormNode.h"
#include "DelayNode.h"
#include "DotProductNode.h"
#include "UnaryOperationNode.h"
#include "BinaryOperationNode.h"
#include "BinaryPredicateNode.h"
#include "ElementSelectorNode.h"
#include "ForestNode.h"

// predictors
#include "LinearPredictor.h"
#include "ForestPredictor.h"
#include "SingleElementThresholdPredictor.h"

// utilities
#include "Files.h"
#include "Serializer.h"
#include "JsonSerializer.h"
#include "XMLSerializer.h"

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
        // auto outputNode = model.AddNode<model::OutputNode<double>>(classifierNode->prediction);
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

    predictors::SimpleForestPredictor CreateForest(int numSplits)
    {
        // define some abbreviations
        using SplitAction = predictors::SimpleForestPredictor::SplitAction;
        using SplitRule = predictors::SingleElementThresholdPredictor;
        using EdgePredictorVector = std::vector<predictors::ConstantPredictor>;
        using NodeId = predictors::SimpleForestPredictor::SplittableNodeId;

        // build a forest
        predictors::SimpleForestPredictor forest;
        SplitRule dummyRule{ 0, 0.0 };
        EdgePredictorVector dummyEdgePredictor{ -1.0, 1.0 };
        auto root = forest.Split(SplitAction{ forest.GetNewRootId(), dummyRule, dummyEdgePredictor });
        std::vector<size_t> interiorNodeVector;
        interiorNodeVector.push_back(root);

        for (int index = 0; index < numSplits; ++index)
        {
            auto node = interiorNodeVector.back();
            interiorNodeVector.pop_back();
            interiorNodeVector.push_back(forest.Split(SplitAction{ forest.GetChildId(node, 0), dummyRule, dummyEdgePredictor }));
            interiorNodeVector.push_back(forest.Split(SplitAction{ forest.GetChildId(node, 1), dummyRule, dummyEdgePredictor }));
        }
        return forest;
    }

    model::Model GetTreeModel(int numSplits)
    {
        auto forest = CreateForest(numSplits);
        model::Model model;
        auto inputNode = model.AddNode<model::InputNode<double>>(3);
        auto simpleForestNode = model.AddNode<nodes::SimpleForestNode>(inputNode->output, forest);
        auto outputNode = model.AddNode<model::OutputNode<double>>(simpleForestNode->output);
        return model;
    }

    model::Model GetRefinedTreeModel(int numSplits)
    {
        auto model = GetTreeModel(numSplits);
        model::TransformContext context;
        model::ModelTransformer transformer;
        auto refinedModel = transformer.RefineModel(model, context);
        return refinedModel;
    }

    void RegisterNodeTypes(utilities::SerializationContext& context)
    {
        context.GetTypeFactory().AddType<model::Node, utilities::UniqueId>();
        context.GetTypeFactory().AddType<model::Node, model::InputNode<double>>();
        context.GetTypeFactory().AddType<model::Node, model::OutputNode<double>>();

        context.GetTypeFactory().AddType<model::Node, nodes::AccumulatorNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BinaryOperationNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BinaryPredicateNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConstantNode<bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConstantNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DelayNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DotProductNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ElementSelectorNode<double, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MovingAverageNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MovingVarianceNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LinearPredictorNode>();
        context.GetTypeFactory().AddType<model::Node, nodes::L2NormNode<double>>();
        //        context.GetTypeFactory().AddType<model::Node, nodes::SimpleForestNode>();
        context.GetTypeFactory().AddType<model::Node, nodes::SingleElementThresholdNode>();
        context.GetTypeFactory().AddType<model::Node, nodes::SumNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::UnaryOperationNode<double>>();
    }

    template <typename DeserializerType>
    model::Model DeserializeModel(std::istream& stream)
    {
        DeserializerType deserializer(stream);
        utilities::SerializationContext context;
        RegisterNodeTypes(context);
        model::Model model;
        deserializer.Deserialize(model, context);
        return model;
    }

    template <typename SerializerType>
    void SerializeModel(const model::Model& model, std::ostream& stream)
    {
        SerializerType serializer(stream);
        serializer.Serialize(model);
    }

    model::Model LoadModelGraph(const std::string& filename)
    {
        std::string treePrefix = "[tree_";
        if (filename == "[1]")
        {
            return GetModel1();
        }
        if (filename == "[2]")
        {
            return GetModel2();
        }
        else if (filename == "[3]")
        {
            return GetModel3();
        }
        else if (filename.find(treePrefix) == 0)
        {
            auto pos = filename.find(treePrefix);
            auto suffix = filename.substr(pos + treePrefix.size());
            auto numSplits = std::stoi(suffix);
            return GetRefinedTreeModel(numSplits);
        }
        else
        {
            auto ext = utilities::GetFileExtension(filename);
            if(ext == "xml")
            {
                auto filestream = utilities::OpenIfstream(filename);
                return DeserializeModel<utilities::SimpleXmlDeserializer>(filestream);
            }
            else if(ext == "json")
            {
                auto filestream = utilities::OpenIfstream(filename);
                return DeserializeModel<utilities::JsonDeserializer>(filestream);
            }
            else
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Error: Unknown file type \"" + ext + "\"");   
            }
        }
    }

    void SaveModelGraph(const model::Model& model, const std::string& filename)
    {
        auto ext = utilities::GetFileExtension(filename);
        if(ext == "xml")
        {
            auto filestream = utilities::OpenOfstream(filename);
            SerializeModel<utilities::SimpleXmlSerializer>(model, filestream);
        }
        else if(ext == "json")
        {
            auto filestream = utilities::OpenOfstream(filename);
            SerializeModel<utilities::JsonSerializer>(model, filestream);
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Error: Unknown file type \"" + ext + "\"");   
        }
    }
}
