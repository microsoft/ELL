////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ConstantLayer.cpp (layers)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ConstantLayer.h"

// utilities
#include "Exception.h"

// stl
#include <string>
#include <stdexcept>
#include <cassert>
#include <algorithm>

namespace layers
{
    const int ConstantLayer::_currentVersion;

    ConstantLayer::ConstantLayer(std::vector<double> values) :
        _values(std::move(values))
    {}

    void ConstantLayer::Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const
    {
        std::copy(_values.begin(), _values.end(), outputs.begin());
    }

    CoordinateIterator ConstantLayer::GetInputCoordinateIterator(uint64_t index) const
    {
        // _inputCoordinates is empty
        return _inputCoordinates.GetIterator();
    }

    uint64_t ConstantLayer::GetRequiredLayerSize(uint64_t layerIndex) const
    {
        return 0;
    }

    void ConstantLayer::Read(utilities::XMLDeserializer& deserializer)
    {
        int version = 0;
        deserializer.Deserialize("version", version);
        if (version == 1)
        {
            deserializer.Deserialize("values", _values);
            deserializer.Deserialize("coordinates", _inputCoordinates);
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "unsupported version: " + std::to_string(version));
        }
    }

    void ConstantLayer::Write(utilities::XMLSerializer& serializer) const
    {
        serializer.Serialize("version", _currentVersion);
        serializer.Serialize("values", _values);
        serializer.Serialize("coordinates", _inputCoordinates);
    }
}
