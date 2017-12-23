////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TrainerInterface.cpp (interfaces)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AppendNodeToModel.h"
#include "AutoDataVector.h"
#include "Exception.h"
#include "Dataset.h"
#include "DatasetInterfaceImpl.h"
#include "Map.h"
#include "LoadModel.h"
#include "MakeTrainer.h"
#include "MapLoadArguments.h"
#include "ModelInterface.h"
#include "ProtoNNTrainer.h"
#include "ProtoNNPredictorNode.h"
#include "TrainerInterface.h"

// stl
#include <vector>

using namespace ell;

namespace ELL_API
{

class ProtoNNPredictor::ProtoNNPredictorImpl
{
public:
    std::shared_ptr<trainers::ProtoNNTrainer> _trainer;
    trainers::ProtoNNTrainerParameters _parameters;
};

ProtoNNPredictor::ProtoNNPredictor()
    : _impl(std::make_shared<ProtoNNPredictorImpl>())
{
}

std::vector<double> ProtoNNPredictor::Predict(const AutoDataVector& data) const
{
    auto result = _impl->_trainer->GetPredictor().Predict(*data._impl->_vector);
    return result.ToArray();
}

std::vector<double> ProtoNNPredictor::Predict(const std::vector<double>& data) const
{
    auto result = _impl->_trainer->GetPredictor().Predict(data);
    return result.ToArray();
}

Map ProtoNNPredictor::GetMap() const
{
    auto predictor = _impl->_trainer->GetPredictor();
    auto numFeatures = predictor.GetDimension();

    Map map;

    std::shared_ptr<ell::model::Map> innerMap = map.GetInnerMap();
    model::Model& model = innerMap->GetModel();

    // add the input node.
    auto inputNode = model.AddNode<model::InputNode<double>>(numFeatures);

    // add the predictor node, taking input from the input node
    model::PortElements<double> inputElements(inputNode->output);
    auto predictorNode = model.AddNode<nodes::ProtoNNPredictorNode>(inputElements, predictor);

    // add an output node taking input from the predictor node.
    auto outputNode = model.AddNode<model::OutputNode<double>>(predictorNode->output);
    model::PortElements<double> outputElements(outputNode->output);

    // name the inputs and outputs to the map.
    innerMap->AddInput("input", inputNode);
    innerMap->AddOutput("output", outputElements);
    
    return map;
}


class ProtoNNTrainer::ProtoNNTrainerImpl
{
public:
    AutoSupervisedDataset _dataset;
    trainers::ProtoNNTrainerParameters _params;
    std::shared_ptr<trainers::ProtoNNTrainer> _trainer;
};

ProtoNNTrainer::ProtoNNTrainer(const ProtoNNTrainerParameters& parameters) :
    _impl(std::make_shared<ProtoNNTrainerImpl>())
{
    // copy to real parameter struct
    _impl->_params = {
        parameters.numFeatures,
        parameters.numLabels,
        parameters.projectedDimension,
        parameters.numPrototypesPerLabel,
        parameters.sparsityW,
        parameters.sparsityZ,
        parameters.sparsityB,
        parameters.gamma,
        static_cast<trainers::ProtoNNLossFunction>(parameters.lossFunction),
        parameters.numIterations,
        parameters.numInnerIterations,
        parameters.verbose
    };

    if (parameters.numLabels == 0)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument,
            "parameters.numLabels must not be zero");
    }

}

void ProtoNNTrainer::SetDataset(const AutoSupervisedDataset& dataset)
{
    this->_impl->_dataset = dataset;
    if (_impl->_trainer != nullptr)
    {
        if (_impl->_params.verbose) std::cout << "Dataset changed, so resetting trainer" << std::endl;
        _impl->_trainer = nullptr; // have to retrain then!
    }
}

void ProtoNNTrainer::Update()
{
    auto& parameters = this->_impl->_params;
    auto& innerDataset = this->_impl->_dataset._impl->_dataset;
    size_t numExamples = innerDataset.NumExamples();
    size_t numFeatures = _impl->_params.numFeatures;

    if (numFeatures == 0)
    {
         numFeatures = innerDataset.NumFeatures();
         _impl->_params.numFeatures = numFeatures;
    }
    if (_impl->_trainer == nullptr) 
    {
        // lazily create protonn trainer (can't create it until we see the dataset
        // because of the -dd auto feature).
        _impl->_trainer = std::make_shared<trainers::ProtoNNTrainer>(_impl->_params);
        _impl->_trainer->SetDataset(innerDataset.GetAnyDataset(0, numExamples));
    }
    
    // Train the predictor
    if (parameters.verbose) std::cout << "Training ..." << std::endl;

    _impl->_trainer->Update();
}

ProtoNNPredictor ProtoNNTrainer::GetPredictor() const
{
    ProtoNNPredictor result;
    result._impl->_trainer = _impl->_trainer;
    result._impl->_parameters = this->_impl->_params;
    return result;
}

}
