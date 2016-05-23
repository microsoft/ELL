////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UnaryOpLayer.cpp (layers)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "UnaryOpLayer.h"

// stl
#include <stdexcept>
#include <string>
#include <cassert>
#include <algorithm>
#include <cmath>

namespace
{
    template <typename T>
    class sqrtOperator
    {
    public:
        T operator()( const T& val) const
        {
            return std::sqrt(val);
        }
    };
}

namespace layers
{
    const int UnaryOpLayer::_currentVersion;
    std::string sqrtOperationName = "Sqrt";
    
    UnaryOpLayer::UnaryOpLayer(const CoordinateList& inputCoordinates, OperationType operationType) : _operationType(operationType)
    {
        _inputCoordinates = inputCoordinates;
    }

    uint64_t UnaryOpLayer::GetInputDimension() const
    {
        return _inputCoordinates.Size();
    }

    uint64_t UnaryOpLayer::GetOutputDimension() const
    {
        return _inputCoordinates.Size();
    }

    const std::string UnaryOpLayer::GetOperationName(OperationType type)
    {
        switch (type)
        {
        case OperationType::sqrt:
            return sqrtOperationName;
        }

        throw std::runtime_error("unrecognized operation type");
    }

    UnaryOpLayer::OperationType UnaryOpLayer::GetOperationType(const std::string& name)
    {
        if (name == sqrtOperationName)
        {
            return OperationType::sqrt;
        }
        else
        {
            throw std::runtime_error("unrecognized operation type");
        }
    }

    std::function<double(double)> UnaryOpLayer::GetOperation(OperationType type)
    {
        switch (type)
        {
        case OperationType::sqrt:
            return sqrtOperator<double>();
        }

        throw std::runtime_error("unrecognized operation type");
    }


    UnaryOpLayer::OperationType UnaryOpLayer::GetOperationType() const
    {
        return _operationType;
    }

    void UnaryOpLayer::Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const
    {
        auto operation = GetOperation(_operationType);
        auto numEntries = _inputCoordinates.Size();
        for (uint64_t index = 0; index < numEntries; ++index)
        {
            Coordinate coordinate = _inputCoordinates[index];
            double input = inputs[coordinate.GetLayerIndex()][coordinate.GetElementIndex()];
            outputs[index] = operation(input);
        }
    }

    CoordinateIterator UnaryOpLayer::GetInputCoordinateIterator(uint64_t index) const
    {
        return _inputCoordinates.GetIterator(index, 1);
    }

    uint64_t UnaryOpLayer::GetRequiredLayerSize(uint64_t layerIndex) const
    {
        return _inputCoordinates.GetRequiredLayerSize(layerIndex);
    }

    std::string UnaryOpLayer::GetTypeName()
    {
        return "UnaryOpLayer";
    }

    std::string UnaryOpLayer::GetRuntimeTypeName() const
    {
        return GetTypeName();
    }

    void UnaryOpLayer::Read(utilities::XMLDeserializer& deserializer)
    {
        int version = 0;
        std::string operationName;
        deserializer.Deserialize("version", version);
        if (version == 1)
        {
            deserializer.Deserialize("operationType", operationName);
            _operationType = GetOperationType(operationName);
            deserializer.Deserialize("coordinates", _inputCoordinates);
        }
        else
        {
            throw std::runtime_error("unsupported version: " + std::to_string(version));
        }
    }

    void UnaryOpLayer::Write(utilities::XMLSerializer& serializer) const
    {
        serializer.Serialize("version", _currentVersion);
        serializer.Serialize("operationType", GetOperationName(_operationType));
        serializer.Serialize("coordinates", _inputCoordinates);
    }
}


