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
#include "RealArray.h"

namespace layers
{
    /// <summary> Implements an input layer. </summary>
    class Input : public Layer
    {
    public:

        /// <summary> Constructs an instance of Input. </summary>
        ///
        /// <param name="size"> The size. </param>
        Input(uint64 size = 0);

        /// <summary> Copy constructor. </summary>
        ///
        /// <param name="parameter1"> The first parameter. </param>
        Input(const Input&) = default;

        /// <summary> Default virtual destructor. </summary>
        virtual ~Input() = default;

        /// <summary> \returns The size of the layer's output. </summary>
        ///
        /// <returns> An uint64. </returns>
        virtual uint64 Size() const override;

        /// <summary> Computes the output of the layer. </summary>
        ///
        /// <param name="rowIndex"> Zero-based index of the row. </param>
        /// <param name="outputs"> [in,out] The outputs. </param>
        virtual void Compute(uint64 rowIndex, vector<types::DoubleArray>& outputs) const override;

        /// <summary> Returns an Iterator to the inputs that the specified output depends on. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        ///
        /// <returns> The input coordinates. </returns>
        virtual utilities::VectorIterator<Coordinate> GetInputCoordinates(uint64 index) const override;

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
    };
}
