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
        Sum(int first_input_indices = 0, int input_index_count = 0);

        /// Sets the first input index to sum
        ///
        void SetFirstInputIndices(int index);

        /// Sets the first input index to sum
        ///
        int GetFirstInputIndices() const;

        /// Sets the first input index to sum
        ///
        void SetInputIndicesCount(int count);

        /// Sets the first input index to sum
        ///
        int GetInputIndicesCount() const;

        /// applys the Mapping (reads inputs from the input vector and writes output to the output vector
        ///
        virtual void Apply(const double* input, double* output) const;

        /// \returns The input dimension of the Mapping. Namely, the Apply function assumes that the input array is at least this long
        ///
        virtual int GetMinInputDim() const;

        /// \returns The output dimension of the Mapping. Namely, the Apply function assumes that the output array is at least this long
        ///    
        virtual int GetOutputDim() const;

        /// Serializes the Mapping in json format
        ///
        virtual void Serialize(JsonSerializer& serializer) const;

        /// Deserializes the Mapping in json format
        ///
        virtual void Deserialize(JsonSerializer& serializer, int version = _current_version);

    protected:
        int _first_input_indices;
        int _input_indices_count;
        static const int _current_version = 1;
    };
}

