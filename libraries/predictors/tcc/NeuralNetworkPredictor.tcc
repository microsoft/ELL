////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkPredictor.tcc (predictors)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "NeuralNetworkPredictor.h"

//stl
#include <iostream>

namespace ell
{
namespace predictors
{
    template <typename ElementType>
    NeuralNetworkPredictor<ElementType>::NeuralNetworkPredictor(InputLayerReference&& inputLayer, Layers&& layers) :
        _inputLayer(std::move(inputLayer)),
        _layers(std::move(layers)),
        _output(_layers.back()->GetOutput().NumElements())
    {
    }

    template <typename ElementType>
    typename NeuralNetworkPredictor<ElementType>::Shape NeuralNetworkPredictor<ElementType>::GetInputShape() const
    {
        if (_inputLayer != nullptr)
        {
            return _inputLayer->GetInputShape();
        }
        return {};
    }

    template <typename ElementType>
    typename NeuralNetworkPredictor<ElementType>::Shape NeuralNetworkPredictor<ElementType>::GetOutputShape() const
    {
        if (_layers.size() > 0)
        {
            return _layers.back()->GetOutputShape();
        }
        return {};
    }

    template <typename ElementType>
    const std::vector<ElementType>& NeuralNetworkPredictor<ElementType>::Predict(const DataVectorType& dataVector) const
    {
        if (_inputLayer != nullptr)
        {
            _inputLayer->SetInput(dataVector);
            _inputLayer->Compute();
        }

        // Forward feed inputs through the layers
        for (size_t i = 0; i < _layers.size(); i++)
        {
            _layers[i]->Compute();
            // Uncomment the following line to print layer info
            //_layers[i]->Print(std::cout);
        }

        if (_layers.size() > 0)
        {
            auto output = _layers.back()->GetOutput();
            size_t vectorIndex = 0;

            //_output.resize(output.NumElements());
            for (size_t i = 0; i < output.NumRows(); i++)
            {
                for (size_t j = 0; j < output.NumColumns(); j++)
                {
                    for (size_t k = 0; k < output.NumChannels(); k++)
                    {
                        _output[vectorIndex++] = output(i, j, k);
                    }
                }
            }
        }
        else
        {
            _output.assign(_output.size(), 0);
        }

        return _output;
    }

    template <typename ElementType>
    void NeuralNetworkPredictor<ElementType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        //archiver["layers"] << _layers;
    }

    template <typename ElementType>
    void NeuralNetworkPredictor<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        //archiver["layers"] >> _layers;
    }

}
}
