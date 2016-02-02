// Sum.h
#pragma once

#include "Layer.h"
#include "Coordinate.h"

#include "types.h"
#include "RealArray.h"

#include <vector>

#include <functional>

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
        Sum(const  std::vector<CoordinateList>& coordinates);

        /// default virtual destructor
        ///
        virtual ~Sum() = default;

        /// \returns The size of the layer's output
        ///
        virtual uint64 Size() const override;

        /// Computes the layer output
        ///
        virtual void Compute(uint64 rowIndex, std::vector<types::DoubleArray>& outputs) const override;

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
        std::vector<CoordinateList> _coordinates;
        static const int _currentVersion = 1;
    };
}
