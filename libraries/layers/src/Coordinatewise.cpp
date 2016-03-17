////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     Coordinatewise.cpp (layers)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Coordinatewise.h"

// stl
#include <stdexcept>
#include <string>

namespace layers
{
    const int Coordinatewise::_currentVersion;
    std::string addOperationName = "Add";
    std::string multiplyOperationName = "Multiply";

    Coordinatewise::Coordinatewise(double value, Coordinate coordinate, OperationType operationType) : _operationType(operationType)
    {
        _values.push_back(value);
        _inputCoordinates.push_back(coordinate);
    }

    Coordinatewise::Coordinatewise(const std::vector<double>& values, const CoordinateList& coordinates, OperationType operationType) : _operationType(operationType)
    {
        _values = values;
        _inputCoordinates = coordinates;
    }

    const std::string Coordinatewise::GetOperationName(OperationType type)
    {
        switch (type)
        {
        case OperationType::add:
            return addOperationName;
        case OperationType::multiply:
            return multiplyOperationName;
        }

        throw std::runtime_error("unrecognized operation type");
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
            throw std::runtime_error("unrecognized operation type");
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

        }

        throw std::runtime_error("unrecognized operation type");
    }


    Coordinatewise::OperationType Coordinatewise::GetOperationType() const
    {
        return _operationType;
    }

    uint64_t Coordinatewise::Size() const
    {
        return _inputCoordinates.size();
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

    Layer::InputCoordinateIterator Coordinatewise::GetInputCoordinates(uint64_t index) const
    {
        return Layer::InputCoordinateIterator(_inputCoordinates.cbegin() + index, _inputCoordinates.cbegin() + index + 1);
    }

    std::string Coordinatewise::GetTypeName()
    {
        return "Coordinatewise";
    }

    std::string Coordinatewise::GetRuntimeTypeName() const
    {
        return GetTypeName();
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
            throw std::runtime_error("unsupported version: " + std::to_string(version));
        }
    }

    void Coordinatewise::Write(utilities::XMLSerializer& serializer) const
    {
        serializer.Serialize("version", _currentVersion);
        serializer.Serialize("operationType", GetOperationName(_operationType));
        serializer.Serialize("values", _values);
        serializer.Serialize("coordinates", _inputCoordinates);
    }
}


