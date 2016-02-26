////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Sum.h (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Layer.h"
#include "Coordinate.h"

// types
#include "types.h"
#include "RealArray.h"

// stl
#include <vector>
#include <functional>

namespace layers
{
    /// <summary> Implements a layer of sums. </summary>
    class Sum : public Layer
    {
    public:

        /// <summary> Default constructor, creates an empty sum </summary>
        Sum();

        /// <summary> Constructs a single sum. </summary>
        ///
        /// <param name="coordinates"> The coordinates. </param>
        Sum(const CoordinateList& coordinates);

        /// <summary> Constructs a multi-dimensional sum. </summary>
        ///
        /// <param name="coordinates"> The coordinates. </param>
        Sum(const  std::vector<CoordinateList>& coordinates);

        /// <summary> Destructor. </summary>
        virtual ~Sum() = default;

        /// <summary> Returns the size of the layer's output. </summary>
        ///
        /// <returns> An uint64. </returns>
        virtual uint64 Size() const override;

        /// <summary> Computes the layer output. </summary>
        ///
        /// <param name="layerIndex"> Zero-based index of the layer. </param>
        /// <param name="outputs"> [in,out] The outputs. </param>
        virtual void Compute(uint64 layerIndex, std::vector<types::DoubleArray>& outputs) const override;

        /// <summary> Gets an Iterator to the inputs that the specified output depends on. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        ///
        /// <returns> The input coordinates. </returns>
        virtual Layer::Iterator GetInputCoordinates(uint64 index) const override;

        /// <summary> Serializes the Layer in json format. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        virtual void Serialize(utilities::JsonSerializer& serializer) const override;

        /// <summary> Deserializes the Layer in json format. </summary>
        ///
        /// <param name="serializer"> [in,out] The serializer. </param>
        /// <param name="version"> The version. </param>
        virtual void Deserialize(utilities::JsonSerializer& serializer, int version = _currentVersion) override;

    protected:
        std::vector<CoordinateList> _inputCoordinates;
        static const int _currentVersion = 1;
    };
}
