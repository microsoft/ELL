// Input.h 

#pragma once

#include "Layer.h"

#include "types.h"

namespace layers
{
    class Input : public Layer
    {
    public:

        /// Ctor
        ///
        Input(uint64 size = 0);

        /// Default copy ctor
        ///
        Input(const Input&) = default;

        /// Computes the output of the layer
        ///
        virtual void Compute(const vector<unique_ptr<Layer>>& previousLayers) override;

        /// \Returns An Iterator to the inputs that the specified output depends on
        ///
        virtual VectorIterator<Coordinate> GetInputCoordinates(uint64 index) const override;

        /// Serializes the Layer in json format
        ///
        virtual void Serialize(JsonSerializer & serializer) const override;

        /// Deserializes the Layer in json format
        ///
        virtual void Deserialize(JsonSerializer & serializer, int version) override;

    };
}