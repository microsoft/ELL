// Sum.h
#pragma once

#include "Layer.h"
#include "Coordinate.h"

#include "types.h"

#include <vector>
using std::vector;

#include <functional>
using std::function;

namespace layers
{
    class Sum : public Layer
    {
    public:

        /// Ctor
        ///
        Sum();

        /// Ctor
        ///
        Sum(const vector<Coordinate>& coordinates);

        /// Computes the layer output
        ///
        virtual void Compute(const vector<unique_ptr<Layer>>& previousLayers);

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
        vector<Coordinate> _coordinates;
        static const int _currentVersion = 1;
    };
}
