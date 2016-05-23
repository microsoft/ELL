////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ShiftRegisterLayer.cpp (layers)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ShiftRegisterLayer.h"
#include "CoordinateList.h"

// stl
#include <stdexcept>
#include <string>
#include <cassert>
#include <algorithm>

namespace layers
{
    //
    // ShiftRegister implementation
    //
    
    ShiftRegister::ShiftRegister(uint64_t inputDimension, uint64_t size) : _inputDimension(inputDimension), _size(size), _data(inputDimension*size)
    {
    }

    void ShiftRegister::AddSample(std::vector<double> value)
    {
        assert(value.size() == _inputDimension);
        assert(_data.size() == _inputDimension * _size);
        if(_size == 0) return;

        // erase _inputDimension things from the back
        _data.resize(_inputDimension*(_size-1));
        _data.insert(_data.begin(), value.begin(), value.end());
                
        // // TODO: use a ring buffer instead of this simple but slower version
        // if(_data.size() >= _inputDimension*_size)
        // {
        //     _data.erase(_data.begin(), _data.begin() + _inputDimension);
        // }

        // for(auto x: value) _data.push_back(x);
    }
    
    //
    // ShiftRegisterLayer implementation
    //
    const int ShiftRegisterLayer::_currentVersion;

    ShiftRegisterLayer::ShiftRegisterLayer(const CoordinateList& input, uint64_t windowSize) : _inputCoordinates(input), _windowSize(windowSize), _shiftRegister(input.Size(), windowSize)
    {
    }

    uint64_t ShiftRegisterLayer::GetInputDimension() const
    {
        return _inputCoordinates.Size();
    }

    uint64_t ShiftRegisterLayer::GetOutputDimension() const
    {
        return _inputCoordinates.Size() * _windowSize;
    }

    void ShiftRegisterLayer::Compute(const std::vector<std::vector<double>>& inputs, std::vector<double>& outputs) const
    {
        auto numEntries = _inputCoordinates.Size();
        std::vector<double> input;
        
        // TODO: make a helper function for this 'get input' procedure
        for (uint64_t index = 0; index < numEntries; ++index)
        {
            Coordinate coordinate = _inputCoordinates[index];
            input.push_back(inputs[coordinate.GetLayerIndex()][coordinate.GetElementIndex()]);
        }
        _shiftRegister.AddSample(input);

        // Note: if vector of outputs doesn't change between invocations of Compute, we can just use it as the shift register backing store!
        std::copy(_shiftRegister.begin(), _shiftRegister.end(), outputs.begin());
    }

    CoordinateIterator ShiftRegisterLayer::GetInputCoordinateIterator(uint64_t index) const
    {
        return _inputCoordinates.GetIterator(index, 1);
    }

    Coordinate ShiftRegisterLayer::GetDelayedOutputCoordinate(const CoordinateList& outputCoordinates, uint64_t channel, uint64_t delay)
    {        
        auto numEntries = _inputCoordinates.Size();
        return outputCoordinates[numEntries*delay + channel];
    }
        
    CoordinateList ShiftRegisterLayer::GetDelayedOutputCoordinates(const CoordinateList& outputCoordinates, uint64_t delay)
    {
        CoordinateList result;
        auto numEntries = _inputCoordinates.Size();
        auto offset = numEntries * delay;
        for(uint64_t index = 0; index < numEntries; ++index)
        {
            result.AddCoordinate(outputCoordinates[offset + index]);
        }
        return result;
    }
    
    CoordinateList ShiftRegisterLayer::GetChannelOutputCoordinates(const CoordinateList& outputCoordinates, uint64_t channel)
    {
        CoordinateList result;
        auto windowSize = GetWindowSize();
        auto dimension = _inputCoordinates.Size();
        for(uint64_t index = 0; index < windowSize; ++index)
        {
            result.AddCoordinate(outputCoordinates[index*dimension + channel]);
        }
        return result;        
    }

    uint64_t ShiftRegisterLayer::GetRequiredLayerSize(uint64_t layerIndex) const
    {
        return _inputCoordinates.GetRequiredLayerSize(layerIndex);
    }

    std::string ShiftRegisterLayer::GetTypeName()
    {
        return "ShiftRegisterLayer";
    }

    std::string ShiftRegisterLayer::GetRuntimeTypeName() const
    {
        return GetTypeName();
    }

    void ShiftRegisterLayer::Read(utilities::XMLDeserializer& deserializer)
    {
        int version = 0;
        deserializer.Deserialize("version", version);
        if (version == 1)
        {
            deserializer.Deserialize("coordinates", _inputCoordinates);
            deserializer.Deserialize("windowSize", _windowSize);
        }
        else
        {
            throw std::runtime_error("unsupported version: " + std::to_string(version));
        }
    }

    void ShiftRegisterLayer::Write(utilities::XMLSerializer& serializer) const
    {
        serializer.Serialize("version", _currentVersion);
        serializer.Serialize("coordinates", _inputCoordinates);
        serializer.Serialize("windowSize", _windowSize);
    }
}


