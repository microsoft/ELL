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
        static const char* GetTypeName();

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual const char* GetRuntimeTypeName() const override;

        virtual void Read(utilities::XMLDeserializer& deserializer) override;
        virtual void Write(utilities::XMLSerializer& serializer) const override;

    protected:
        std::vector<CoordinateList> _inputCoordinates;
        static const int _currentVersion = 1;
    };
}
