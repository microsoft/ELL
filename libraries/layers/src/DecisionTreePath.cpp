////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     DecisionTreePath.cpp (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DecisionTreePath.h"

// stl
#include <stdexcept>
#include <string>
#include <cassert>

namespace layers
{
    uint64_t DecisionTreePath::Size() const
    {
        return 2*_inputSplitRulesCoordinates.Size();
    }

    void DecisionTreePath::Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const
    {
        // TODO
    }

    CoordinateIterator DecisionTreePath::GetInputCoordinateIterator(uint64_t index) const
    {
        return _inputSplitRulesCoordinates.GetIterator(index, 1);
    }

    uint64_t DecisionTreePath::GetRequiredLayerSize(uint64_t layerIndex) const
    {
        return _inputSplitRulesCoordinates.GetRequiredLayerSize(layerIndex);
    }

    std::string DecisionTreePath::GetTypeName()
    {
        return "DecisionTreePath";
    }

    std::string DecisionTreePath::GetRuntimeTypeName() const
    {
        return GetTypeName();
    }

    void DecisionTreePath::Read(utilities::XMLDeserializer& deserializer)
    {
        int version = 0;
        std::string operationName;
        deserializer.Deserialize("version", version);
        if (version == 1)
        {
            // TODO
        }
        else
        {
            throw std::runtime_error("unsupported version: " + std::to_string(version));
        }
    }

    void DecisionTreePath::Write(utilities::XMLSerializer& serializer) const
    {
        serializer.Serialize("version", _currentVersion);
        // TODO
    }
}


