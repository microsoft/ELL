// Input.h 

#pragma once

#include "Layer.h"

#include "types.h"
#include "RealArray.h"

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

        /// Default virtual destructor
        ///
        virtual ~Input() = default;

        /// \returns The size of the layer's output
        ///
        virtual uint64 Size() const override;

        /// Computes the output of the layer
        ///
        virtual void Compute(uint64 rowIndex, vector<types::DoubleArray>& outputs) const override;

        /// \Returns An Iterator to the inputs that the specified output depends on
        ///
        virtual utilities::VectorIterator<Coordinate> GetInputCoordinates(uint64 index) const override;

        /// Serializes the Layer in json format
        ///
        virtual void Serialize(utilities::JsonSerializer & serializer) const override;

        /// Deserializes the Layer in json format
        ///
        virtual void Deserialize(utilities::JsonSerializer & serializer, int version) override;

    private:
        uint64 _size;
    };
}
