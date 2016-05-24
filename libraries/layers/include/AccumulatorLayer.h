////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AccumulatorLayer.h (layers)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Layer.h"
#include "CoordinateList.h"

// stl
#include <cstdint>
#include <vector>
#include <functional>
#include <string>

namespace layers
{
    /// <summary> A layer that computes the running sum or product of its input. </summary>
    class AccumulatorLayer : public Layer
    {
    public:

        enum class OperationType {add, multiply};

        /// <summary> Constructs an instance of a AccumulatorLayer layer. </summary>
        ///
        /// <param name="operationType"> The type of AccumulatorLayer layer to construct. </param>
        AccumulatorLayer(OperationType operationType = OperationType::add);

        /// <summary> Constructs an instance of a AccumulatorLayer layer. </summary>
        ///
        /// <param name="coordinates"> The input coordinate for each element. </param>
        /// <param name="operationType"> The type of AccumulatorLayer layer to construct. </param>
        AccumulatorLayer(CoordinateList coordinates, OperationType operationType = OperationType::add);

        /// <summary> Constructs an instance of a AccumulatorLayer layer. </summary>
        ///
        /// <param name="initialValue"> The initial value for each element. </param>
        /// <param name="coordinates"> The input coordinate for each element. </param>
        /// <param name="operationType"> The type of AccumulatorLayer layer to construct. </param>
        AccumulatorLayer(std::vector<double> initialValues, CoordinateList coordinates, OperationType operationType = OperationType::add);

        /// <summary> Returns the input dimension of the layer. </summary>
        ///
        /// <returns> The input dimension. </returns>
        virtual uint64_t GetInputDimension() const override { return _inputCoordinates.Size(); }

        /// <summary> Returns the output dimension of the layer. </summary>
        ///
        /// <returns> The output dimension. </returns>
        virtual uint64_t GetOutputDimension() const override { return _inputCoordinates.Size(); }

        /// <summary> Gets the operation name from the operation type. </summary>
        ///
        /// <param name="type"> The operation type. </param>
        ///
        /// <returns> The operation name. </returns>
        static const std::string GetOperationName(OperationType type);

        /// <summary> Gets the operation type from its name. </summary>
        ///
        /// <param name="name"> The operation name. </param>
        ///
        /// <returns> The operation type. </returns>
        static OperationType GetOperationType(const std::string& name);

        /// <summary> Gets the operation from its type. </summary>
        ///
        /// <param name="type"> The operation type. </param>
        ///
        /// <returns> The operation. </returns>
        static std::function<double(double, double)> GetOperation(OperationType type);

        /// <summary> Gets the operation type of this AccumulatorLayer layer. </summary>
        ///
        /// <returns> The operation type. </returns>
        OperationType GetOperationType() const { return _operationType; }

        /// <summary> Computes the layer output. </summary>
        ///
        /// <param name="inputs"> The set of vectors holding input values to use </param>
        /// <param name="outputs"> [out] The vector where the output values are written. </param>
        virtual void Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const override;

        /// <summary> Returns an iterator that enumerates the input coordinates for a specified element. </summary>
        ///
        /// <param name="index"> Zero-based element index. </param>
        ///
        /// <returns> The input coordinates. </returns>
        virtual CoordinateIterator GetInputCoordinateIterator(uint64_t index) const override;

        /// <summary> Returns the minimal required size of an input layer, which is the maximum input element from that layer plus 1. </summary>
        ///
        /// <param name="layerindex"> The layer index. </param>
        ///
        /// <returns> The required layer size. </returns>
        virtual uint64_t GetRequiredLayerSize(uint64_t layerIndex) const override;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "AccumulatorLayer"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Reads the layer from an XMLDeserializer. </summary>
        ///
        /// <param name="deserializer"> [in,out] The deserializer. </param>
        virtual void Read(utilities::XMLDeserializer& deserializer) override;

        /// <summary> Writes the layer to an XMLSerializer. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        virtual void Write(utilities::XMLSerializer& serializer) const override;

    protected:
        std::vector<double> _initialValues;
        mutable std::vector<double> _currentValues;
        CoordinateList _inputCoordinates;
        OperationType _operationType;
        static const int _currentVersion = 1;
    };
}
