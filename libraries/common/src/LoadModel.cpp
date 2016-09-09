////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LoadModel.cpp (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LoadModel.h"
#include "IsNodeCompilable.h"

// model
#include "InputNode.h"
#include "Model.h"
#include "OutputNode.h"

// nodes
#include "BinaryOperationNode.h"
#include "BinaryPredicateNode.h"
#include "DelayNode.h"
#include "DotProductNode.h"
#include "ExtremalValueNode.h"
#include "ForestPredictorNode.h"
#include "L2NormNode.h"
#include "LinearPredictorNode.h"
#include "MovingAverageNode.h"
#include "MovingVarianceNode.h"
#include "MultiplexerNode.h"
#include "UnaryOperationNode.h"

// predictors
#include "ForestPredictor.h"
#include "LinearPredictor.h"
#include "SingleElementThresholdPredictor.h"

// utilities
#include "Archiver.h"
#include "Files.h"
#include "JsonArchiver.h"
#include "XMLArchiver.h"

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
        auto inputs = model::Concat(model::MakePortElements(mean8->output), model::MakePortElements(var8->output), model::MakePortElements(mean16->output), model::MakePortElements(var16->output));
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
        auto simpleForestPredictorNode = model.AddNode<nodes::SimpleForestPredictorNode>(inputNode->output, forest);
        auto outputNode = model.AddNode<model::OutputNode<double>>(simpleForestPredictorNode->output);
        return model;
    }

    model::Model GetRefinedTreeModel(int numSplits)
    {
        auto model = GetTreeModel(numSplits);
        model::TransformContext context{ common::IsNodeCompilable() };
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
        context.GetTypeFactory().AddType<model::Node, nodes::BinaryPredicateNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::BinaryPredicateNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConstantNode<bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConstantNode<int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::ConstantNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DelayNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DotProductNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MultiplexerNode<double, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MultiplexerNode<bool, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MovingAverageNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::MovingVarianceNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::DemultiplexerNode<bool, bool>>();
        context.GetTypeFactory().AddType<model::Node, nodes::LinearPredictorNode>();
        context.GetTypeFactory().AddType<model::Node, nodes::L2NormNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::SimpleForestPredictorNode>();
        context.GetTypeFactory().AddType<model::Node, nodes::SingleElementThresholdNode>();
        context.GetTypeFactory().AddType<model::Node, nodes::SumNode<double>>();
        context.GetTypeFactory().AddType<model::Node, nodes::TypeCastNode<bool, int>>();
        context.GetTypeFactory().AddType<model::Node, nodes::UnaryOperationNode<double>>();
    }

    template <typename UnarchiverType>
    model::Model LoadArchivedModel(std::istream& stream)
    {
        try
        {
            utilities::SerializationContext context;
            RegisterNodeTypes(context);
            UnarchiverType unarchiver(stream, context);
            model::Model model;
            unarchiver.Unarchive(model);
            return model;
        }
        catch (const std::exception&)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Error: couldn't read file.");
        }
    }

    template <typename ArchiverType>
    void SaveArchivedModel(const model::Model& model, std::ostream& stream)
    {
        ArchiverType archiver(stream);
        archiver.Archive(model);
    }

    bool IsKnownExtension(const std::string& ext)
    {
        return ext == "xml" || ext == "json";
    }

    model::Model LoadModel(const std::string& filename)
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
            auto ext = utilities::GetFileExtension(filename, true);
            if (IsKnownExtension(ext))
            {
                if (!utilities::IsFileReadable(filename))
                {
                    throw utilities::SystemException(utilities::SystemExceptionErrors::fileNotFound);
                }

                auto filestream = utilities::OpenIfstream(filename);
                if (ext == "xml")
                {
                    return LoadArchivedModel<utilities::XmlUnarchiver>(filestream);
                }
                else if (ext == "json")
                {
                    return LoadArchivedModel<utilities::JsonUnarchiver>(filestream);
                }
            }

            model::Model emptyModel;
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Error: Unknown file type \"" + ext + "\"");
        }
    }

    void SaveModel(const model::Model& model, const std::string& filename)
    {
        auto ext = utilities::GetFileExtension(filename);
        if (ext == "xml")
        {
            auto filestream = utilities::OpenOfstream(filename);
            SaveArchivedModel<utilities::XmlArchiver>(model, filestream);
        }
        else if (ext == "json")
        {
            auto filestream = utilities::OpenOfstream(filename);
            SaveArchivedModel<utilities::JsonArchiver>(model, filestream);
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Error: Unknown file type \"" + ext + "\"");
        }
    }
}
