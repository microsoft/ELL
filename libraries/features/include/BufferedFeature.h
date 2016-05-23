////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BufferedFeature.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Feature.h"

#include <string>
#include <vector>
#include <memory>
#include <ostream>
#include <unordered_map>
#include <functional>
#include <cstddef>

namespace features
{
    /// <summary> Base class for features that operate on a buffer of input </summary>
    class BufferedFeature : public Feature
    {
    public:
        size_t GetWindowSize() const { return _windowSize; }
        virtual void Reset() override;
        virtual size_t GetWarmupTime() const override;

    protected:
        BufferedFeature(const std::vector<Feature*>& inputs, size_t windowSize);
        
        // BufferedFeature-specific routines
        size_t NumSamples() const;
        double GetSample(int column, size_t offset) const;
        std::vector<std::vector<double>>& GetAllSamples();
        const std::vector<std::vector<double>>& GetAllSamples() const;
        std::vector<double>& GetAllSamples(int column) { return _samples[column]; }
        const std::vector<double>& GetAllSamples(int column) const { return _samples[column]; }
        void UpdateRowSamples(const std::vector<double>& row) const;

        virtual void AddToDescription(std::vector<std::string>& description) const override;

        size_t _windowSize = 0;

    private:
        mutable int _currentIndex = 0;
        mutable std::vector<std::vector<double>> _samples;
        void AllocateSampleBuffer(size_t numColumns) const;
    };
}
