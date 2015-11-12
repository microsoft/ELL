// Sum.cpp

#include "Sum.h"

#include <stdexcept>
using std::runtime_error;

#include <string>
using std::to_string;

namespace mappings
{

    Sum::Sum(int first_input_indices, int input_index_count) : _first_input_indices(first_input_indices), _input_indices_count(input_index_count)
    {
        _type = types::sum;
    }

    void Sum::SetFirstInputIndices(int index)
    {
        _first_input_indices = index;
    }

    int Sum::GetFirstInputIndices() const
    {
        return _first_input_indices;
    }

    void Sum::SetInputIndicesCount(int count)
    {
        _input_indices_count = count;
    }

    int Sum::GetInputIndicesCount() const
    {
        return _input_indices_count;
    }
    
    void Sum::Apply(const double* input, double* output) const
    {
        double value = 0.0;
        
        for (int i = _first_input_indices; i < _first_input_indices + _input_indices_count; ++i)
        {
            value += input[i];
        }
        output[0] = value;
    }

    int Sum::GetMinInputDim() const
    {
        return _first_input_indices + _input_indices_count;
    }

    int Sum::GetOutputDim() const
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
