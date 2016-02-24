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
    class Coordinatewise : public Layer
    {
    public:

        using DoubleOperation = std::function<double(double, double)>;

        /// Ctor
        ///
        Coordinatewise(Type type);

        /// Ctor
        ///
        Coordinatewise(double value, Coordinate coordinate, Type type);

        /// Ctor
        ///
        Coordinatewise(const std::vector<double>& values, const CoordinateList& coordinates, Type type);

        /// Default virtual destructor
        ///
        virtual ~Coordinatewise() = default;

        /// \returns The size of the layer's output
        ///
        virtual uint64 Size() const override;

        /// Computes the layer output
        ///
        virtual void Compute(uint64 layerIndex, std::vector<types::DoubleArray>& outputs) const override;

        /// \Returns An Iterator to the inputs that the specified output depends on
        ///
        virtual utilities::VectorIterator<Coordinate> GetInputCoordinates(uint64 index) const override;

        /// Serializes the Layer in json format
        ///
        virtual void Serialize(utilities::JsonSerializer& serializer) const override;

        /// Deserializes the Layer in json format
        ///
        virtual void Deserialize(utilities::JsonSerializer& serializer, int version = _currentVersion) override;

    protected:
        std::vector<double> _values;
        CoordinateList _inputCoordinates;
        DoubleOperation _operation;
        static const int _currentVersion = 1;
    };
}
