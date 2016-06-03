////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BufferedFeature.cpp (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Feature.h"
#include "BufferedFeature.h"

#include <vector>
#include <memory>
#include <stdexcept>
#include <cassert>

namespace features
{
    //
    // BufferedFeature
    //

    BufferedFeature::BufferedFeature(const std::vector<Feature*>& inputs, size_t windowSize) : Feature({inputs}), _windowSize(windowSize), _currentIndex(0)
    {
        // TODO: resize _samples buffer to be windowSize here?
    }

    BufferedFeature::BufferedFeature(const std::string& id, const std::vector<Feature*>& inputs, size_t windowSize) : Feature(id, {inputs}), _windowSize(windowSize), _currentIndex(0)
    {
        // TODO: resize _samples buffer to be windowSize here?
    }

    void BufferedFeature::Reset()
    {
        auto oldSize = _samples.size() > 0 ? _samples[0].size() : 0;
        for (auto& w : _samples)
        {
            w.resize(oldSize);
            w.clear();
        }
    }

    void BufferedFeature::AddToDescription(std::vector<std::string>& description) const
    {
        using std::to_string;
        description.push_back(to_string(_windowSize));
    }

    void BufferedFeature::AllocateSampleBuffer(size_t numColumns) const
    {
        _samples.resize(numColumns);
        for (int index = 0; index < numColumns; ++index)
        {
            _samples[index].resize(_windowSize);
        }
    }

    void BufferedFeature::UpdateRowSamples(const std::vector<double>& row) const
    {
        if (_windowSize == 0)
        {
            return;
        }

        // allocate samples if necessary
        auto numColumns = row.size();
        if (_samples.size() < numColumns || (_samples.size() >0 && _samples[0].size() < _windowSize))
        {
            AllocateSampleBuffer(numColumns);
        }

        assert(NumSamples() == _windowSize);
        _currentIndex = (_currentIndex + 1) % _windowSize;
        for (unsigned int col_index = 0; col_index < numColumns; col_index++)
        {
            _samples[col_index][_currentIndex] = row[col_index];
        }
    }

    size_t BufferedFeature::NumSamples() const
    {
        if (_samples.size() == 0)
        {
            return 0;
        }

        return _samples[0].size();
    }

    double BufferedFeature::GetSample(int column, size_t offset) const
    {
        // implements a simple ring buffer
        assert(NumSamples() == _windowSize);
        assert(offset <= _windowSize);
        return _samples[column][(_currentIndex - offset + _windowSize) % _windowSize];
    }

    std::vector<double> BufferedFeature::GetDelayedSamples(size_t offset) const
    {
        auto numChannels = _samples.size();
        std::vector<double> result;
        for(size_t channelIndex = 0; channelIndex < numChannels; ++channelIndex)
        {
            result.push_back(GetSample(channelIndex, offset));
        }        
        return result;
    }

    std::vector<std::vector<double>>& BufferedFeature::GetAllSamples()
    {
        return _samples;
    }

    const std::vector<std::vector<double>>& BufferedFeature::GetAllSamples() const
    {
        return _samples;
    }
    
    std::vector<double>& BufferedFeature::GetAllSamples(int column) 
    { 
        return _samples[column]; 
    }
    
    const std::vector<double>& BufferedFeature::GetAllSamples(int column) const
    { 
        return _samples[column]; 
    }

    size_t BufferedFeature::GetWarmupTime() const
    {
        return Feature::GetWarmupTime() + GetWindowSize();
    }
}
