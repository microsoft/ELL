// Zero.h 

#pragma once

#include "Layer.h"

#include "types.h"

namespace mappings
{
    class Zero : public Layer
    {
    public:

        /// Ctor
        ///
        Zero(uint64 size = 0);

        /// Default copy ctor
        ///
        Zero(const Zero&) = default;

        /// Computes the output of the layer
        ///
        virtual void Compute(const vector<unique_ptr<Layer>>& previousLayers) override;

        /// Serializes the Layer in json format
        ///
        virtual void Serialize(JsonSerializer & serializer) const override;

        /// Deserializes the Layer in json format
        ///
        virtual void Deserialize(JsonSerializer & serializer, int version) override;

    };
}