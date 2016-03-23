////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Input.h (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Layer.h"

// types
#include "types.h"

// stl
#include <vector>

namespace layers
{
    /// <summary> Implements an input layer. </summary>
    class Input : public Layer
    {
    public:

        /// <summary> Constructs an Input layer. </summary>
        Input();

        /// <summary> Copy constructor. </summary>
        Input(const Input&) = default;

        /// <summary> Default virtual destructor. </summary>
        virtual ~Input() = default;

        /// <summary> Returns the number of elements in the layer. </summary>
        ///
        /// <returns> The number of elements in the layer. </returns>
        virtual uint64 Size() const override;

        /// <summary> Computes the layer output. </summary>
        ///
        /// <param name="layerIndex"> The layer's own zero-based index in the map. </param>
        /// <param name="outputs"> [in,out] The vector where the output values are written. </param>
        virtual void Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const override;

        /// <summary> Returns an iterator that enumerates the input coordinates for a specified element. </summary>
        ///
        /// <param name="index"> Zero-based element index. </param>
        ///
        /// <returns> The input coordinates. </returns>
        virtual Layer::InputCoordinateIterator GetInputCoordinates(uint64 index) const override;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string  GetTypeName();

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override;

        /// <summary> Reads the map from an XMLDeserializer. </summary>
        ///
        /// <param name="deserializer"> [in,out] The deserializer. </param>
        virtual void Read(utilities::XMLDeserializer& deserializer) override;

        /// <summary> Writes the map to an XMLSerializer. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        virtual void Write(utilities::XMLSerializer& serializer) const override;

        /// <summary> Increases the size of the layer if necessary </summary>
        ///
        /// <param name="minimumSize"> The new minimum size of the layer </param>
        void IncreaseSize(uint64 minimumSize);

    private:
        uint64 _size;
        static const int _currentVersion = 1;
    };
}
