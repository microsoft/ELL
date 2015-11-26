// Coordinatewise2.h
#pragma once

#include "Layer.h"
#include "IndexPairList.h"

#include "types.h"
#include "RealArray.h"
using common::DoubleArray;

#include <vector>
using std::vector;

#include <functional>
using std::function;

namespace mappings
{
    class Coordinatewise2 : public Layer
    {
    public:

        using DoubleOperation = function<double(double, double)>;

        /// Ctor
        ///
        Coordinatewise2(const DoubleOperation& operation);

        /// Ctor
        ///
        Coordinatewise2(const vector<double>& values, const IndexPairList& coordinates, const DoubleOperation& operation);

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
        IndexPairList _coordinates;
        DoubleOperation _operation;
        static const int _currentVersion = 1;
    };
}
