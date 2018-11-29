////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkPredictor.h (predictors)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IPredictor.h"

#include <math/include/Matrix.h>
#include <math/include/Vector.h>

#include <data/include/Dataset.h>

// neural network
#include <predictors/neural/include/ActivationLayer.h>
#include <predictors/neural/include/BatchNormalizationLayer.h>
#include <predictors/neural/include/BiasLayer.h>
#include <predictors/neural/include/BinaryConvolutionalLayer.h>
#include <predictors/neural/include/ConvolutionalLayer.h>
#include <predictors/neural/include/FullyConnectedLayer.h>
#include <predictors/neural/include/HardSigmoidActivation.h>
#include <predictors/neural/include/InputLayer.h>
#include <predictors/neural/include/Layer.h>
#include <predictors/neural/include/LeakyReLUActivation.h>
#include <predictors/neural/include/MaxPoolingFunction.h>
#include <predictors/neural/include/MeanPoolingFunction.h>
#include <predictors/neural/include/ParametricReLUActivation.h>
#include <predictors/neural/include/PoolingLayer.h>
#include <predictors/neural/include/ReLUActivation.h>
#include <predictors/neural/include/RegionDetectionLayer.h>
#include <predictors/neural/include/ScalingLayer.h>
#include <predictors/neural/include/SigmoidActivation.h>
#include <predictors/neural/include/SoftmaxLayer.h>
#include <predictors/neural/include/TanhActivation.h>

#include <utilities/include/IArchivable.h>

#include <cstddef>
#include <memory>

namespace ell
{
namespace predictors
{
    /// <summary> A neural network predictor. </summary>
    template <typename ElementType>
    class NeuralNetworkPredictor : public IPredictor<std::vector<ElementType>>
        , public utilities::IArchivable
    {
    public:
        /// <summary> Type of the input vector expected by this predictor type. </summary>
        using DataVectorType = typename neural::Layer<ElementType>::DataVectorType;

        /// <summary> Dimensions of the input and output tensors. </summary>
        using Shape = typename neural::Layer<ElementType>::Shape;

        /// <summary> A unique_ptr to the input layer for this predictor. </summary>
        using InputLayerReference = std::shared_ptr<neural::InputLayer<ElementType>>;

        /// <summary> A vector of layers. </summary>
        using Layers = std::vector<std::shared_ptr<neural::Layer<ElementType>>>;

        NeuralNetworkPredictor() = default;
        NeuralNetworkPredictor(const NeuralNetworkPredictor&) = default;

        /// <summary> Constructs an instance of NerualNetworkPredictor. </summary>
        ///
        /// <param name="inputLayer"> The input layer for this network. </param>
        /// <param name="layers"> The substantive layers comprising this network. The first layer
        /// in this vector receives its input from the input layer. </param>
        NeuralNetworkPredictor(InputLayerReference&& inputLayer, Layers&& layers);

        /// <summary> Returns the input layer. </summary>
        ///
        /// <returns> The underlying input layer. </returns>
        const neural::InputLayer<ElementType>& GetInputLayer() const { return *_inputLayer; }

        /// <summary> Returns the number of underlying layers. </summary>
        ///
        /// <returns> The underlying vector of layers. </returns>
        size_t NumLayers() const { return _layers.size(); }

        /// <summary> Returns the underlying layers. </summary>
        ///
        /// <returns> The underlying vector of layers. </returns>
        const Layers& GetLayers() const { return _layers; }

        /// <summary> Sets the underlying layers. </summary>
        ///
        /// <returns> The underlying vector of layers. </returns>
        void SetLayers(Layers&& layers) { _layers = std::move(layers); } // STYLE discrepancy

        /// <summary> Removes layers from the end of the neural network.  </summary>
        ///
        /// <param name="numberToRemove"> The number of layers to remove from the end of the neural network. </param>
        void RemoveLastLayers(size_t numberToRemove = 1);

        /// <summary> Gets the dimension of the input layer. </summary>
        ///
        /// <returns> The dimension. </returns>
        Shape GetInputShape() const;

        /// <summary> Gets the dimension of the output layer. </summary>
        ///
        /// <returns> The dimension. </returns>
        Shape GetOutputShape() const;

        /// <summary> Returns the output of the network for a given input. </summary>
        ///
        /// <param name="input"> The data vector. </param>
        ///
        /// <returns> The prediction. </returns>
        const std::vector<ElementType>& Predict(const DataVectorType& dataVector) const;

        /// <summary> Returns the output of the network for a given input. </summary>
        ///
        /// <param name="input"> The input data. </param>
        ///
        /// <returns> The prediction. </returns>
        const std::vector<ElementType>& Predict(const std::vector<ElementType>& input) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ElementType>("NeuralNetworkPredictor"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Register known types for neural network predictors to a serialization context </summary>
        ///
        /// <param name="context"> The `SerializationContext` </param>
        static void RegisterNeuralNetworkPredictorTypes(utilities::SerializationContext& context);

        /// <summary> Gets the current archive format version. </summary>
        ///
        /// <returns> The current archive format version. </summary>
        static utilities::ArchiveVersion GetCurrentArchiveVersion();

        /// <summary> Reset the state of the predictor </summary>
        void Reset();

    protected:
        utilities::ArchiveVersion GetArchiveVersion() const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        void Compute() const;
        InputLayerReference _inputLayer;
        Layers _layers;
        mutable std::vector<ElementType> _output;
    };
} // namespace predictors
} // namespace ell

#pragma region implementation

#include <iostream>

namespace ell
{
namespace predictors
{
    constexpr utilities::ArchiveVersion c_currentNeuralNetworkPredictorArchiveVersion = { utilities::ArchiveVersionNumbers::v1 };

    template <typename ElementType>
    NeuralNetworkPredictor<ElementType>::NeuralNetworkPredictor(InputLayerReference&& inputLayer, Layers&& layers) :
        _inputLayer(std::move(inputLayer)),
        _layers(std::move(layers)),
        _output(_layers.back()->GetOutput().Size())
    {
    }

    template <typename ElementType>
    void NeuralNetworkPredictor<ElementType>::RemoveLastLayers(size_t numberToRemove)
    {
        if (_layers.size() > numberToRemove)
        {
            _layers.resize(_layers.size() - numberToRemove);
            _output.resize(_layers.back()->GetOutput().Size());
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "RemoveLastLayers numberToRemove exceeds number of layers.");
        }
    }

    template <typename ElementType>
    typename NeuralNetworkPredictor<ElementType>::Shape NeuralNetworkPredictor<ElementType>::GetInputShape() const
    {
        if (_inputLayer != nullptr)
        {
            return _inputLayer->GetInputShape();
        }
        return { 0, 0, 0 };
    }

    template <typename ElementType>
    typename NeuralNetworkPredictor<ElementType>::Shape NeuralNetworkPredictor<ElementType>::GetOutputShape() const
    {
        if (_layers.size() > 0)
        {
            return _layers.back()->GetOutputShape();
        }
        return { 0, 0, 0 };
    }

    template <typename ElementType>
    const std::vector<ElementType>& NeuralNetworkPredictor<ElementType>::Predict(const DataVectorType& dataVector) const
    {
        if (_inputLayer != nullptr)
        {
            _inputLayer->SetInput(dataVector);
            _inputLayer->Compute();
        }
        Compute();
        return _output;
    }

    template <typename ElementType>
    const std::vector<ElementType>& NeuralNetworkPredictor<ElementType>::Predict(const std::vector<ElementType>& input) const
    {
        if (_inputLayer != nullptr)
        {
            _inputLayer->SetInput(input);
            _inputLayer->Compute();
        }
        Compute();
        return _output;
    }

    template <typename ElementType>
    void NeuralNetworkPredictor<ElementType>::Compute() const
    {
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
    }

    template <typename ElementType>
    void NeuralNetworkPredictor<ElementType>::Reset()
    {
        for (size_t i = 0; i < _layers.size(); i++)
        {
            _layers[i]->Reset();
        }
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
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ActivationLayer<ElementType>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::BatchNormalizationLayer<ElementType>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::BiasLayer<ElementType>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::BinaryConvolutionalLayer<ElementType>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ConvolutionalLayer<ElementType>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::FullyConnectedLayer<ElementType>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::PoolingLayer<ElementType, MaxPoolingFunction>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::PoolingLayer<ElementType, MeanPoolingFunction>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::RegionDetectionLayer<ElementType>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ScalingLayer<ElementType>>();
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::SoftmaxLayer<ElementType>>();
        context.GetTypeFactory().AddType<NeuralNetworkPredictor<ElementType>, NeuralNetworkPredictor<ElementType>>();

        // Map the old type names to the new ones for compatibility reasons.
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ActivationLayer<float>>("ActivationLayer<float,SigmoidActivation>");
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ActivationLayer<float>>("ActivationLayer<float,HardSigmoidActivation<float>>");
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ActivationLayer<float>>("ActivationLayer<float,ReLUActivation>");
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ActivationLayer<float>>("ActivationLayer<float,LeakyReLUActivation>");
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ActivationLayer<float>>("ActivationLayer<float,TanhActivation>");
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ActivationLayer<float>>("ActivationLayer<float,ParametricReLUActivation>");

        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ActivationLayer<double>>("ActivationLayer<double,SigmoidActivation>");
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ActivationLayer<double>>("ActivationLayer<double,HardSigmoidActivation<double>>");
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ActivationLayer<double>>("ActivationLayer<double,ReLUActivation>");
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ActivationLayer<double>>("ActivationLayer<double,LeakyReLUActivation>");
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ActivationLayer<double>>("ActivationLayer<double,TanhActivation>");
        context.GetTypeFactory().AddType<neural::Layer<ElementType>, neural::ActivationLayer<double>>("ActivationLayer<double,ParametricReLUActivation>");
    }

    template <typename ElementType>
    utilities::ArchiveVersion NeuralNetworkPredictor<ElementType>::GetCurrentArchiveVersion()
    {
        return c_currentNeuralNetworkPredictorArchiveVersion;
    }

    template <typename ElementType>
    utilities::ArchiveVersion NeuralNetworkPredictor<ElementType>::GetArchiveVersion() const
    {
        return GetCurrentArchiveVersion();
    }
} // namespace predictors
} // namespace ell

#pragma endregion implementation
