////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Coordinatewise.h (layers)
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
    /// <summary> A class that represents a layer that performs a coordinatewise operation. </summary>
    class Coordinatewise : public Layer
    {
    public:

        using DoubleOperation = std::function<double(double, double)>;

        /// <summary> Constructs an instance of a Coordinatewise layer. </summary>
        ///
        /// <param name="type"> The type of Coordinatewise layer to construct. </param>
        Coordinatewise(Type type);

        /// <summary> Constructs an single-element instance of a Coordinatewise layer. </summary>
        ///
        /// <param name="value"> The element's value. </param>
        /// <param name="coordinate"> The element's input coordinate. </param>
        /// <param name="type"> The type of Coordinatewise layer to construct. </param>
        Coordinatewise(double value, Coordinate coordinate, Type type);

        /// <summary> Constructs an instance of a Coordinatewise layer. </summary>
        ///
        /// <param name="values"> The value for each element. </param>
        /// <param name="coordinates"> The input coordinate for each element. </param>
        /// <param name="type"> The type of Coordinatewise layer to construct. </param>
        Coordinatewise(const std::vector<double>& values, const CoordinateList& coordinates, Type type);

        /// <summary> Default virtual destructor. </summary>
        virtual ~Coordinatewise() = default;

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
        std::vector<double> _values;
        CoordinateList _inputCoordinates;
        DoubleOperation _operation;
        static const int _currentVersion = 1;
    };
}
