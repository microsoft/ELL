////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ShiftRegisterLayer.h (layers)
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
    class ShiftRegister
    {
    public:
        ShiftRegister(uint64_t inputDimension, uint64_t size);
        void AddSample(std::vector<double> value);
        std::vector<double>::const_iterator begin() { return _data.begin(); }
        std::vector<double>::const_iterator end() { return _data.end(); }

    private:
        uint64_t _inputDimension;
        uint64_t _size;
        std::vector<double> _data;
    };
    
    /// <summary> A class that represents a layer that holds a finite history of its input. </summary>
    class ShiftRegisterLayer : public Layer
    {
    public:

        /// <summary> Constructs an instance of a BinaryOperationLayer layer. </summary>
        ShiftRegisterLayer() = default;

        /// <summary> Constructs an instance of a BinaryOperationLayer layer. </summary>
        ///
        /// <param name="values"> The value for each element. </param>
        /// <param name="input1"> The input coordinate for the left side of the operation each element. </param>
        /// <param name="input2"> The input coordinate for the right side of the operation each element. </param>
        /// <param name="operationType"> The type of BinaryOperationLayer layer to construct. </param>
        ShiftRegisterLayer(const CoordinateList& input, uint64_t windowSize);

        /// <summary> Default virtual destructor. </summary>
        virtual ~ShiftRegisterLayer() = default;

        /// <summary> Returns the input dimension of the layer. </summary>
        ///
        /// <returns> The input dimension. </returns>
        virtual uint64_t GetInputDimension() const override;

        /// <summary> Returns the output dimension of the layer. </summary>
        ///
        /// <returns> The output dimension. </returns>
        virtual uint64_t GetOutputDimension() const override;

        /// <summary> Returns the window size. </summary>
        uint64_t GetWindowSize() const { return _windowSize; }
        
        /// <summary> Computes the layer output. </summary>
        ///
        /// <param name="inputs"> The set of vectors holding input values to use </param>
        /// <param name="outputs"> [out] The vector where the output values are written. </param>
        virtual void Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const override;

        /// <summary> Returns an iterator that enumerates the input coordinates for a specified output element. </summary>
        ///
        /// <param name="index"> Zero-based element index. </param>
        ///
        /// <returns> The input coordinates. </returns>
        virtual CoordinateIterator GetInputCoordinateIterator(uint64_t index) const override;

        /// <summary> Returns the coordinate for a delayed sample for a given input channel, with a given delay </summary>
        ///
        /// <param name="channel"> The channel (index) of the input. </param>
        /// <param name="delay"> The number of samples of delay to use. </param>
        ///
        /// <returns> The coordinate of the delayed sample. </returns>
        Coordinate GetDelayedOutputCoordinate(const CoordinateList& outputCoordinates, uint64_t channel, uint64_t delay);
        
        /// <summary> Returns the coordinates for the input at a given delay </summary>
        ///
        /// <param name="delay"> The number of samples of delay to use. </param>
        ///
        /// <returns> The coordinates of the delayed input. </returns>
        CoordinateList GetDelayedOutputCoordinates(const CoordinateList& outputCoordinates, uint64_t delay);

        /// <summary> Returns the coordinates for all delayed samples of the specified channel. </summary>
        ///
        /// <param name="channel"> The channel to return the samples from. </param>
        ///
        /// <returns> The delayed coordinates of the input channel. </returns>
        CoordinateList GetChannelOutputCoordinates(const CoordinateList& outputCoordinates, uint64_t channel);
        
        /// <summary> Returns the minimal required size of an input layer, which is the maximum input element from that layer plus 1. </summary>
        ///
        /// <param name="layerindex"> The layer index. </param>
        ///
        /// <returns> The required layer size. </returns>
        virtual uint64_t GetRequiredLayerSize(uint64_t layerIndex) const override;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName();

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override;

        /// <summary> Reads the layer from an XMLDeserializer. </summary>
        ///
        /// <param name="deserializer"> [in,out] The deserializer. </param>
        virtual void Read(utilities::XMLDeserializer& deserializer) override;

        /// <summary> Writes the layer to an XMLSerializer. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        virtual void Write(utilities::XMLSerializer& serializer) const override;

    protected:
        CoordinateList _inputCoordinates;
        uint64_t _windowSize;
        mutable ShiftRegister _shiftRegister;
        
        static const int _currentVersion = 1;
    };
}
