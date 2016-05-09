////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BinaryOpLayer.cpp (layers)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BinaryOpLayer.h"

// stl
#include <stdexcept>
#include <string>
#include <cassert>
#include <algorithm>

namespace
{
    std::string addOperationName = "Add";
    std::string multiplyOperationName = "Multiply";
}

namespace layers
{
    const int BinaryOpLayer::_currentVersion;

    BinaryOpLayer::BinaryOpLayer(const CoordinateList& input1, const CoordinateList& input2, OperationType operationType) : _operationType(operationType)
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

    uint64_t BinaryOpLayer::GetInputDimension() const
    {
        return _inputCoordinates.size();
    }

    uint64_t BinaryOpLayer::GetOutputDimension() const
    {
        return _inputCoordinates.size();
    }

    const std::string BinaryOpLayer::GetOperationName(OperationType type)
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

    BinaryOpLayer::OperationType BinaryOpLayer::GetOperationType(const std::string& name)
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

    std::function<double(double, double)> BinaryOpLayer::GetOperation(OperationType type)
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

    BinaryOpLayer::OperationType BinaryOpLayer::GetOperationType() const
    {
        return _operationType;
    }

    void BinaryOpLayer::Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const
    {
        std::cout << "BinaryOpLayer::Compute" << std::endl;
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

    CoordinateIterator BinaryOpLayer::GetInputCoordinateIterator(uint64_t index) const
    {
        return _inputCoordinates[index].GetIterator();
    }

    uint64_t BinaryOpLayer::GetRequiredLayerSize(uint64_t layerIndex) const
    {
        auto maxSize = _inputCoordinates[0].GetRequiredLayerSize(layerIndex);
        auto size = _inputCoordinates.size();
        for(uint64_t index = 1; index < size; ++index)
        {
            maxSize = std::max(maxSize, _inputCoordinates[index].GetRequiredLayerSize(layerIndex));
        }
        return maxSize;
    }

    std::string BinaryOpLayer::GetTypeName()
    {
        return "BinaryOpLayer";
    }

    std::string BinaryOpLayer::GetRuntimeTypeName() const
    {
        return GetTypeName();
    }

    void BinaryOpLayer::Read(utilities::XMLDeserializer& deserializer)
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

    void BinaryOpLayer::Write(utilities::XMLSerializer& serializer) const
    {
        serializer.Serialize("version", _currentVersion);
        serializer.Serialize("operationType", GetOperationName(_operationType));
        serializer.Serialize("coordinates", _inputCoordinates);
    }
}


