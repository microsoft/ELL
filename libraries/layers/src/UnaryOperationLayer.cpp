////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     UnaryOperationLayer.cpp (layers)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "UnaryOperationLayer.h"

// utilities
#include "Exception.h"

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
    const int UnaryOperationLayer::_currentVersion;
    std::string sqrtOperationName = "Sqrt";
    
    UnaryOperationLayer::UnaryOperationLayer(const CoordinateList& inputCoordinates, OperationType operationType) : _operationType(operationType)
    {
        _inputCoordinates = inputCoordinates;
    }

    uint64_t UnaryOperationLayer::GetInputDimension() const
    {
        return _inputCoordinates.Size();
    }

    uint64_t UnaryOperationLayer::GetOutputDimension() const
    {
        return _inputCoordinates.Size();
    }

    const std::string UnaryOperationLayer::GetOperationName(OperationType type)
    {
        switch (type)
        {
        case OperationType::sqrt:
            return sqrtOperationName;
        }

        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "unrecognized operation type");
    }

    UnaryOperationLayer::OperationType UnaryOperationLayer::GetOperationType(const std::string& name)
    {
        if (name == sqrtOperationName)
        {
            return OperationType::sqrt;
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "unrecognized operation type");
        }
    }

    std::function<double(double)> UnaryOperationLayer::GetOperation(OperationType type)
    {
        switch (type)
        {
        case OperationType::sqrt:
            return sqrtOperator<double>();
        }

        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "unrecognized operation type");
    }


    UnaryOperationLayer::OperationType UnaryOperationLayer::GetOperationType() const
    {
        return _operationType;
    }

    void UnaryOperationLayer::Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const
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

    CoordinateIterator UnaryOperationLayer::GetInputCoordinateIterator(uint64_t index) const
    {
        return _inputCoordinates.GetIterator(index, 1);
    }

    uint64_t UnaryOperationLayer::GetRequiredLayerSize(uint64_t layerIndex) const
    {
        return _inputCoordinates.GetRequiredLayerSize(layerIndex);
    }

    std::string UnaryOperationLayer::GetTypeName()
    {
        return "UnaryOperationLayer";
    }

    std::string UnaryOperationLayer::GetRuntimeTypeName() const
    {
        return GetTypeName();
    }

    void UnaryOperationLayer::Read(utilities::XMLDeserializer& deserializer)
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
            throw utilities::InputException(utilities::InputExceptionErrors::badStringFormat, "unsupported version: " + version);
        }
    }

    void UnaryOperationLayer::Write(utilities::XMLSerializer& serializer) const
    {
        serializer.Serialize("version", _currentVersion);
        serializer.Serialize("operationType", GetOperationName(_operationType));
        serializer.Serialize("coordinates", _inputCoordinates);
    }
}


