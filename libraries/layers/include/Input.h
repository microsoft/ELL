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
        ///
        /// <param name="size"> The dimension of the input. </param>
        Input(uint64 size = 0);

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
        virtual void Compute(uint64 layerIndex, std::vector<std::vector<double>>& outputs) const override;

        /// <summary> Returns an iterator that enumerates the input coordinates for a specified element. </summary>
        ///
        /// <param name="index"> Zero-based element index. </param>
        ///
        /// <returns> The input coordinates. </returns>
        virtual Layer::InputCoordinateIterator GetInputCoordinates(uint64 index) const override;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static const char*  GetTypeName();

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual const char* GetRuntimeTypeName() const override;

        virtual void Read(utilities::XMLDeserializer& deserializer) override;
        virtual void Write(utilities::XMLSerializer& serializer) const override;

        // TODO: remove JSON serializaiton

        /// <summary> Serializes the Layer in json format. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        virtual void Serialize(utilities::JsonSerializer & serializer) const override;

        /// <summary> Deserializes the Layer in json format. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        /// <param name="version"> The version. </param>
        virtual void Deserialize(utilities::JsonSerializer & serializer, int version) override;

    private:
        uint64 _size;
        static const int _currentVersion = 1;
    };
}
