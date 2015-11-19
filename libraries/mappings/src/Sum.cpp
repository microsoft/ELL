// Sum.cpp

#include "Sum.h"

#include <stdexcept>
using std::runtime_error;

#include <string>
using std::to_string;

namespace mappings
{

    Sum::Sum(uint64 first_input_indices, uint64 input_index_count) : _first_input_indices(first_input_indices), _input_indices_count(input_index_count) // TODO indices ?
    {
        _type = types::sum;
    }

    void Sum::SetFirstInputIndices(uint64 index)
    {
        _first_input_indices = index;
    }

    uint64 Sum::GetFirstInputIndices() const
    {
        return _first_input_indices;
    }

    void Sum::SetInputIndicesCount(uint64 count)
    {
        _input_indices_count = count;
    }

    uint64 Sum::GetInputIndicesCount() const
    {
        return _input_indices_count;
    }
    
    void Sum::Apply(const double* input, double* output) const
    {
        double value = 0.0;
        
        for (uint64 i = _first_input_indices; i < _first_input_indices + _input_indices_count; ++i)
        {
            value += input[i];
        }
        output[0] = value;
    }

    uint64 Sum::GetMinInputDim() const
    {
        return _first_input_indices + _input_indices_count;
    }

    uint64 Sum::GetOutputDim() const
    {
        return 1;
    }

    void Sum::Serialize(JsonSerializer& serializer) const
    {
        // version 1
        Mapping::SerializeHeader(serializer, 1);
        serializer.Write("first_input_indices", _first_input_indices);
        serializer.Write("input_index_count", _input_indices_count);
    }

    void Sum::Deserialize(JsonSerializer& serializer, int version)
    {
        if (version == 1)
        {
            serializer.Read("first_input_indices", _first_input_indices);
            serializer.Read("input_index_count", _input_indices_count);
        }
        else
        {
          throw runtime_error("unsupported version: " + to_string(version));
        }
    }
}
