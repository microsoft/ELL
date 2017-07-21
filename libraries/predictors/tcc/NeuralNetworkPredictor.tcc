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
    constexpr int c_currentNeuralNetworkPredictorArchiveVersion = 1;

    template <typename ElementType>
    NeuralNetworkPredictor<ElementType>::NeuralNetworkPredictor(InputLayerReference&& inputLayer, Layers&& layers) :
        _inputLayer(std::move(inputLayer)),
        _layers(std::move(layers)),
        _output(_layers.back()->GetOutput().Size())
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
        archiver["inputLayer"] << _inputLayer.get();

        std::vector<const neural::Layer<ElementType>*> layerElements;
        for (size_t i = 0; i < _layers.size(); i++)
        {
            layerElements.emplace_back(_layers[i].get());
        }
        archiver["layers"] << layerElements;
        archiver["output"] << _output;
    }

    template <typename ElementType>
    void NeuralNetworkPredictor<ElementType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        neural::LayerSerializationContext<ElementType> layerContext(archiver.GetContext());
        RegisterNeuralNetworkPredictorTypes(layerContext);
        archiver.PushContext(layerContext);

        std::unique_ptr<neural::InputLayer<ElementType>> inputLayer;
        archiver["inputLayer"] >> inputLayer;
        _inputLayer = std::move(inputLayer);

        std::vector<const neural::Layer<ElementType>*> layerElements;
        archiver["layers"] >> layerElements;
        _layers.resize(layerElements.size());
        for (size_t i = 0; i < layerElements.size(); i++)
        {
            _layers[i].reset((neural::Layer<ElementType>*)layerElements[i]);
        }
        archiver["output"] >> _output;

        archiver.PopContext();
    }

    template <typename ElementType>
    void NeuralNetworkPredictor<ElementType>::RegisterNeuralNetworkPredictorTypes(utilities::SerializationContext& context)
    {
        using namespace ell::predictors::neural;
        context.GetTypeFactory().AddType<neural::InputLayer<ElementType>, neural::InputLayer<ElementType>>(); 
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ActivationLayer<ElementType, LeakyReLUActivation>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ActivationLayer<ElementType, ReLUActivation>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ActivationLayer<ElementType, SigmoidActivation>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::BatchNormalizationLayer<ElementType>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::BiasLayer<ElementType>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::BinaryConvolutionalLayer<ElementType>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ConvolutionalLayer<ElementType>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::FullyConnectedLayer<ElementType>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::PoolingLayer<ElementType, MaxPoolingFunction>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::PoolingLayer<ElementType, MeanPoolingFunction>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ScalingLayer<ElementType>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::SoftmaxLayer<ElementType>>();
        context.GetTypeFactory().AddType<NeuralNetworkPredictor<ElementType>, NeuralNetworkPredictor<ElementType>>();
    }

    template <typename ElementType>
    utilities::ArchiveVersion NeuralNetworkPredictor<ElementType>::GetCurrentArchiveVersion()
    {
        return { c_currentNeuralNetworkPredictorArchiveVersion };        
    }

    template <typename ElementType>
    utilities::ArchiveVersion NeuralNetworkPredictor<ElementType>::GetArchiveVersion() const
    {
        return GetCurrentArchiveVersion();
    }


}
}
