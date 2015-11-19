// Sum.h
#pragma once

#include "Mapping.h"
#include <vector>

using std::vector;

namespace mappings
{
    /// Sums a consecutive sequence of inputs
    ///
    class Sum : public Mapping
    {
    public:
        /// Constructs a Sum Mapping 
        ///
        Sum(uint64 firstInputIndex = 0, uint64 inputIndexCount = 0);

        /// Sets the first input index to sum
        ///
        void SetFirstInputIndices(uint64 index);

        /// Sets the first input index to sum
        ///
        uint64 GetFirstInputIndices() const;

        /// Sets the first input index to sum
        ///
        void SetInputIndicesCount(uint64 count);

        /// Sets the first input index to sum
        ///
        uint64 GetInputIndicesCount() const;

        /// applys the Mapping (reads inputs from the input vector and writes output to the output vector
        ///
        virtual void Apply(const double* input, double* output) const;

        /// \returns The input dimension of the Mapping. Namely, the Apply function assumes that the input array is at least this long
        ///
        virtual uint64 GetMinInputDim() const;

        /// \returns The output dimension of the Mapping. Namely, the Apply function assumes that the output array is at least this long
        ///    
        virtual uint64 GetOutputDim() const;

        /// Serializes the Mapping in json format
        ///
        virtual void Serialize(JsonSerializer& serializer) const;

        /// Deserializes the Mapping in json format
        ///
        virtual void Deserialize(JsonSerializer& serializer, int version = _currentVersion);

    protected:
        uint64 _firstInputIndex;
        uint64 _inputIndexCount;
        static const int _currentVersion = 1;
    };
}

