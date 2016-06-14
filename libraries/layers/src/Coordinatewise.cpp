////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Coordinatewise.cpp (layers)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Coordinatewise.h"

// utilities
#include "Exception.h"

// stl
#include <stdexcept>
#include <string>
#include <cassert>

namespace
{
    std::string addOperationName = "Add";
    std::string multiplyOperationName = "Multiply";
}

namespace layers
{
    const int Coordinatewise::_currentVersion;

    Coordinatewise::Coordinatewise(OperationType operationType) : _operationType(operationType)
    {}

    Coordinatewise::Coordinatewise(double value, Coordinate coordinate, OperationType operationType) : _values(1), _operationType(operationType)
    {
        _values[0] = value;
        _inputCoordinates.AddCoordinate(coordinate);
    }

    Coordinatewise::Coordinatewise(std::vector<double> values, CoordinateList coordinates, OperationType operationType) :
        _values(std::move(std::move(values))), _inputCoordinates(std::move(coordinates)), _operationType(operationType)
    {}

    const std::string Coordinatewise::GetOperationName(OperationType type)
    {
        switch (type)
        {
        case OperationType::add:
            return addOperationName;
        case OperationType::multiply:
            return multiplyOperationName;
        default:
            throw utilities::Exception(utilities::ExceptionErrorCodes::invalidArgument, "unrecognized operation type");
        }
    }

    Coordinatewise::OperationType Coordinatewise::GetOperationType(const std::string& name)
    {
        if (name == addOperationName)
        {
            return OperationType::add;
        }
        else if (name == multiplyOperationName)
        {
            return OperationType::multiply;
        }
        else
        {
            throw utilities::Exception(utilities::ExceptionErrorCodes::invalidArgument, "unrecognized operation type");
        }
    }

    std::function<double(double, double)> Coordinatewise::GetOperation(OperationType type)
    {
        switch (type)
        {
        case OperationType::add:
            return std::plus<double>();
        case OperationType::multiply:
            return std::multiplies<double>();
        default:
            throw utilities::Exception(utilities::ExceptionErrorCodes::invalidArgument, "unrecognized operation type");
        }
    }

    void Coordinatewise::Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const
    {
        auto operation = GetOperation(_operationType);

        for (uint64_t k = 0; k < _values.size(); ++k)
        {
            Coordinate coordinate = _inputCoordinates[k];
            double input = inputs[coordinate.GetLayerIndex()][coordinate.GetElementIndex()];
            outputs[k] = operation(_values[k], input);
        }
    }

    CoordinateIterator Coordinatewise::GetInputCoordinateIterator(uint64_t index) const
    {
        return _inputCoordinates.GetIterator(index, 1);
    }

    uint64_t Coordinatewise::GetRequiredLayerSize(uint64_t layerIndex) const
    {
        return _inputCoordinates.GetRequiredLayerSize(layerIndex);
    }

    void Coordinatewise::Read(utilities::XMLDeserializer& deserializer)
    {
        int version = 0;
        std::string operationName;
        deserializer.Deserialize("version", version);
        if (version == 1)
        {
            deserializer.Deserialize("operationType", operationName);
            _operationType = GetOperationType(operationName);
            deserializer.Deserialize("values", _values);
            deserializer.Deserialize("coordinates", _inputCoordinates);
        }
        else
        {
            throw utilities::Exception(utilities::ExceptionErrorCodes::badStringFormat, "unsupported version: " + std::to_string(version));
        }

        // check validity of deserialized vectors
        assert(_values.size() == _inputCoordinates.Size());
    }

    void Coordinatewise::Write(utilities::XMLSerializer& serializer) const
    {
        // sanity check
        assert(_values.size() == _inputCoordinates.Size());

        serializer.Serialize("version", _currentVersion);
        serializer.Serialize("operationType", GetOperationName(_operationType));
        serializer.Serialize("values", _values);
        serializer.Serialize("coordinates", _inputCoordinates);
    }
}
