// Sum.h
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
    class Sum : public Layer
    {
    public:

        /// Ctor - constructs an empty sum
        ///
        Sum();

        /// Ctor - constructs a single sum
        ///
        Sum(const CoordinateList& coordinates);

        /// Ctor - constructs a multi-dimensional sum
        ///
        Sum(const  vector<CoordinateList>& coordinates);

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
        virtual void Serialize(JsonSerializer& serializer) const override;

        /// Deserializes the Layer in json format
        ///
        virtual void Deserialize(JsonSerializer& serializer, int version = _currentVersion) override;

    protected:
        vector<CoordinateList> _coordinates;
        static const int _currentVersion = 1;
    };
}
