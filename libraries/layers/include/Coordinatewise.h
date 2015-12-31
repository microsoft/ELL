// Coordinatewise.h
#pragma once

#include "Layer.h"
#include "Coordinate.h"

#include "types.h"
#include "RealArray.h"

#include <vector>
using std::vector;

#include <functional>
using std::function;

namespace layers
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
        Coordinatewise(double value, Coordinate coordinate, const DoubleOperation& operation, Type type);

        /// Ctor
        ///
        Coordinatewise(const vector<double>& values, const vector<Coordinate>& coordinates, const DoubleOperation& operation, Type type);

        /// \returns The size of the layer's output
        ///
        virtual uint64 Size() const override;

        /// Computes the layer output
        ///
        virtual void Compute(uint64 rowIndex, vector<DoubleArray>& outputs) const override;

        /// \Returns An Iterator to the inputs that the specified output depends on
        ///
        virtual VectorIterator<Coordinate> GetInputCoordinates(uint64 index) const override;

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
