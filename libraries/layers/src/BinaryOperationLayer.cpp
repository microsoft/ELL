////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryOperationLayer.cpp (layers)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BinaryOperationLayer.h"

// utilities
#include "Exception.h"

// stl
#include <stdexcept>
#include <string>
#include <cassert>
#include <algorithm>

namespace
{
    std::string addOperationName = "Add";
    std::string subtractOperationName = "Subtract";
    std::string multiplyOperationName = "Multiply";
    std::string divideOperationName = "Divide";
}

namespace layers
{
    const int BinaryOperationLayer::_currentVersion;

    BinaryOperationLayer::BinaryOperationLayer(const CoordinateList& input1, const CoordinateList& input2, OperationType operationType) : _operationType(operationType)
    {
        assert(input1.Size() == input2.Size());
        for(uint64_t index = 0; index < input1.Size(); ++index)
        {
            CoordinateList entryCoordinates;
            entryCoordinates.AddCoordinate(input1[index]);
            entryCoordinates.AddCoordinate(input2[index]);            
            _inputCoordinates.push_back(entryCoordinates);
        }
    }

    uint64_t BinaryOperationLayer::GetInputDimension() const
    {
        return _inputCoordinates.size();
    }

    uint64_t BinaryOperationLayer::GetOutputDimension() const
    {
        return _inputCoordinates.size();
    }

    const std::string BinaryOperationLayer::GetOperationName(OperationType type)
    {
        switch (type)
        {
        case OperationType::add:
            return addOperationName;
        case OperationType::subtract:
            return subtractOperationName;
        case OperationType::multiply:
            return multiplyOperationName;
        case OperationType::divide:
            return divideOperationName;
        }

        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "unrecognized operation type");
    }

    BinaryOperationLayer::OperationType BinaryOperationLayer::GetOperationType(const std::string& name)
    {
        if (name == addOperationName)
        {
            return OperationType::add;
        }
        else if (name == subtractOperationName)
        {
            return OperationType::subtract;
        }
        else if (name == multiplyOperationName)
        {
            return OperationType::multiply;
        }
        else if (name == divideOperationName)
        {
            return OperationType::divide;
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "unrecognized operation type");
        }
    }

    std::function<double(double, double)> BinaryOperationLayer::GetOperation(OperationType type)
    {
        switch (type)
        {
        case OperationType::add:
            return std::plus<double>();
        case OperationType::subtract:
            return std::minus<double>();
        case OperationType::multiply:
            return std::multiplies<double>();
        case OperationType::divide:
            return std::divides<double>();
        }

        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "unrecognized operation type");
    }

    BinaryOperationLayer::OperationType BinaryOperationLayer::GetOperationType() const
    {
        return _operationType;
    }

    void BinaryOperationLayer::Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const
    {
        auto operation = GetOperation(_operationType);
        auto numEntries = _inputCoordinates.size();
        for (uint64_t index = 0; index < numEntries; ++index)
        {
            Coordinate coordinate1 = _inputCoordinates[index][0];
            Coordinate coordinate2 = _inputCoordinates[index][1];
            double input1 = inputs[coordinate1.GetLayerIndex()][coordinate1.GetElementIndex()];
            double input2 = inputs[coordinate2.GetLayerIndex()][coordinate2.GetElementIndex()];
            outputs[index] = operation(input1, input2);
        }
    }

    CoordinateIterator BinaryOperationLayer::GetInputCoordinateIterator(uint64_t index) const
    {
        return _inputCoordinates[index].GetIterator();
    }

    uint64_t BinaryOperationLayer::GetRequiredLayerSize(uint64_t layerIndex) const
    {
        auto maxSize = _inputCoordinates[0].GetRequiredLayerSize(layerIndex);
        auto size = _inputCoordinates.size();
        for(uint64_t index = 1; index < size; ++index)
        {
            maxSize = std::max(maxSize, _inputCoordinates[index].GetRequiredLayerSize(layerIndex));
        }
        return maxSize;
    }

    void BinaryOperationLayer::Read(utilities::XMLDeserializer& deserializer)
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

    void BinaryOperationLayer::Write(utilities::XMLSerializer& serializer) const
    {
        serializer.Serialize("version", _currentVersion);
        serializer.Serialize("operationType", GetOperationName(_operationType));
        serializer.Serialize("coordinates", _inputCoordinates);
    }
}


