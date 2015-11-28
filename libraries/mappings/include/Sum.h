// Sum.h
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
    class Sum : public Layer
    {
    public:

        /// Ctor
        ///
        Sum();

        /// Ctor
        ///
        Sum(double bias, const vector<Coordinate>& coordinates);

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
        double _bias;
        vector<Coordinate> _coordinates;
        static const int _currentVersion = 1;
    };
}
