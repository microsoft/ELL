////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkPredictor.h (predictors)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IPredictor.h"

// math
#include "Vector.h"
#include "Matrix.h"

// datasets
#include "Dataset.h"

// neural network
#include "ILayer.h"
#include "ActivationLayer.h"
#include "BatchNormalizationLayer.h"
#include "BiasLayer.h"
#include "ConvolutionalLayer.h"
#include "FullyConnectedLayer.h"
#include "InputLayer.h"
#include "PoolingLayer.h"
#include "ScaledInputLayer.h"

// utilities
#include "IArchivable.h"

// stl
#include <cstddef>
#include <memory>


namespace ell
{
namespace predictors
{
    /// <summary> A neural network predictor. </summary>
    class NeuralNetworkPredictor : public IPredictor<std::vector<double>>, public utilities::IArchivable
    {
    public:
        /// <summary> Type of the input vector expected by this predictor type. </summary>
        using DataVectorType = data::FloatDataVector;

        /// <summary> A vector of layers. </summary>
        using Layers = std::vector<std::unique_ptr<neural::ILayer>>;

        NeuralNetworkPredictor() = default;

        /// <summary> Constructs an instance of NerualNetworkPredictor. </summary>
        ///
        /// <param name="layers"> The layers making up this network. </param>
        NeuralNetworkPredictor(Layers&& layers);

        /// <summary> Returns the underlying layers. </summary>
        ///
        /// <returns> The underlying vector of layers. </returns>
        const Layers& GetLayers() { return _layers; }

        /// <summary> Sets the underlying layers. </summary>
        ///
        /// <returns> The underlying vector of layers. </returns>
        void SetLayers(Layers&& layers) { _layers = std::move(layers); }

        /// <summary> Gets the dimension of the input layer. </summary>
        ///
        /// <returns> The dimension. </returns>
        size_t NumInputs() const;

        /// <summary> Gets the dimension of the output layer. </summary>
        ///
        /// <returns> The dimension. </returns>
        size_t NumOutputs() const;

        /// <summary> Returns the output of the network for a given input. </summary>
        ///
        /// <param name="input"> The data vector. </param>
        ///
        /// <returns> The prediction. </returns>
        const std::vector<double>& Predict(const DataVectorType& dataVector) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "NeuralPredictor"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        Layers _layers;
        mutable std::vector<double> _output;
    };

}
}
