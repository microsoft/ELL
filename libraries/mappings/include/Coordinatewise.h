// Coordinatewise.h
#pragma once

#include "Layer.h"
#include "Coordinate.h"

#include "types.h"
#include "RealArray.h"
using common::DoubleArray;

#include <vector>
using std::vector;

#include <functional>
using std::function;

namespace mappings
{
    class Coordinatewise : public Layer
    {
    public:

        using DoubleOperation = function<double(double, double)>;

        /// Ctor
        ///
        Coordinatewise(const DoubleOperation& operation, Type type);

        /// Ctor
        ///
        Coordinatewise(const vector<double>& values, const vector<Coordinate>& coordinates, const DoubleOperation& operation, Type type);

        /// Computes the layer output
        ///
        virtual void Compute(const vector<unique_ptr<Layer>>& previousLayers);

        /// Serializes the Layer in json format
        ///
        virtual void Serialize(JsonSerializer& serializer) const;

        /// Deserializes the Layer in json format
        ///
        virtual void Deserialize(JsonSerializer& serializer, int version = _currentVersion);

    protected:
        vector<double> _values;
        vector<Coordinate> _coordinates;
        DoubleOperation _operation;
        static const int _currentVersion = 1;
    };
}
