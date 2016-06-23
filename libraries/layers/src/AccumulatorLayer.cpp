////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AccumulatorLayer.cpp (layers)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AccumulatorLayer.h"

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
    const int AccumulatorLayer::_currentVersion;

    AccumulatorLayer::AccumulatorLayer(OperationType operationType) : _operationType(operationType)
    {}

    AccumulatorLayer::AccumulatorLayer(CoordinateList coordinates, OperationType operationType) : _inputCoordinates(std::move(coordinates)), _operationType(operationType)
    {
        auto inputSize = _inputCoordinates.Size();
        _currentValues.resize(inputSize);
        _initialValues.resize(inputSize);
    }

    AccumulatorLayer::AccumulatorLayer(std::vector<double> initialValues, CoordinateList coordinates, OperationType operationType) :
        _initialValues(initialValues), _currentValues(initialValues), _inputCoordinates(std::move(coordinates)), _operationType(operationType)
    {}

    const std::string AccumulatorLayer::GetOperationName(OperationType type)
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

    AccumulatorLayer::OperationType AccumulatorLayer::GetOperationType(const std::string& name)
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

    std::function<double(double, double)> AccumulatorLayer::GetOperation(OperationType type)
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

    void AccumulatorLayer::Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const
    {
        auto operation = GetOperation(_operationType);

        for (uint64_t k = 0; k < _currentValues.size(); ++k)
        {
            Coordinate coordinate = _inputCoordinates[k];
            double input = inputs[coordinate.GetLayerIndex()][coordinate.GetElementIndex()];
            _currentValues[k] = operation(_currentValues[k], input);
            outputs[k] = _currentValues[k];
        }
    }

    CoordinateIterator AccumulatorLayer::GetInputCoordinateIterator(uint64_t index) const
    {
        return _inputCoordinates.GetIterator(index, 1);
    }

    uint64_t AccumulatorLayer::GetRequiredLayerSize(uint64_t layerIndex) const
    {
        return _inputCoordinates.GetRequiredLayerSize(layerIndex);
    }

    void AccumulatorLayer::Read(utilities::XMLDeserializer& deserializer)
    {
        int version = 0;
        std::string operationName;
        deserializer.Deserialize("version", version);
        if (version == 1)
        {
            deserializer.Deserialize("operationType", operationName);
            _operationType = GetOperationType(operationName);
            deserializer.Deserialize("initialValues", _initialValues);
            deserializer.Deserialize("coordinates", _inputCoordinates);
        }
        else
        {
            throw utilities::Exception(utilities::ExceptionErrorCodes::badStringFormat, "unsupported version: " + std::to_string(version));
        }

        // check validity of deserialized vectors
        assert(_initialValues.size() == _inputCoordinates.Size());
    }

    void AccumulatorLayer::Write(utilities::XMLSerializer& serializer) const
    {
        // sanity check
        assert(_initialValues.size() == _inputCoordinates.Size());

        serializer.Serialize("version", _currentVersion);
        serializer.Serialize("operationType", GetOperationName(_operationType));
        serializer.Serialize("initialValues", _initialValues);
        serializer.Serialize("coordinates", _inputCoordinates);
    }
}
