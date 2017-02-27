////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkPredictor.cpp (predictors)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NeuralNetworkPredictor.h"

namespace ell
{
namespace predictors
{
    NeuralNetworkPredictor::NeuralNetworkPredictor(Layers&& layers) :
        _layers(std::move(layers))
    {
        
    }

    size_t NeuralNetworkPredictor::NumInputs() const
    {
        if (_layers.size() > 0)
        {
            return _layers.front()->NumInputs();
        }
        return 0;
    }

    size_t NeuralNetworkPredictor::NumOutputs() const
    {
        if (_layers.size() > 0)
        {
            return _layers.back()->NumOutputs();
        }
        return 0;
    }

    const std::vector<double>& NeuralNetworkPredictor::Predict(const DataVectorType& dataVector) const
    {
        neural::ILayer::LayerVector* output = nullptr;

        // Forward feed inputs through the layers
        for (size_t i = 0; i < _layers.size(); i++)
        {
            if (i == 0)
            {
                // Convert the input type to a math::Vector of doubles.
                // Note that since the input could be trailing zeros, in which  we need to
                // tell it what size to use. The size is always the number of inputs the layer expects.
                std::vector<double> convert = dataVector.ToArray(_layers[i]->NumInputs());
                neural::ILayer::LayerVector input(std::move(convert));

                output = &(_layers[i]->FeedForward(input));
            }
            else
            {
                output = &(_layers[i]->FeedForward(_layers[i - 1]->GetOutput()));
            }
        }

        if (output != nullptr)
        {
            _output = (*output).ToArray();
        }
        else
        {
            _output.clear();
        }

        return _output;
    }

    void NeuralNetworkPredictor::WriteToArchive(utilities::Archiver& archiver) const
    {
        //archiver["layers"] << _layers;
    }

    void NeuralNetworkPredictor::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        //archiver["layers"] >> _layers;
    }

}
}
