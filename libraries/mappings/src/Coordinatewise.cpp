// Coordinatewise.cpp

#include "Coordinatewise.h"

#include <stdexcept>
using std::runtime_error;

#include <string>
using std::to_string;

namespace mappings
{
    Coordinatewise::Coordinatewise(function<double(double, double)> func) : _func(func)
    {}

    Coordinatewise::IndexValueIterator Coordinatewise::begin() const
    {
        return _index_values.cbegin();
    }

    Coordinatewise::IndexValueIterator Coordinatewise::end() const
    {
        return _index_values.cend();
    }

    void Coordinatewise::Apply(const double* input, double* output) const
    {
        for(size_t i = 0; i<_index_values.size(); ++i)
        {
            uint64 index = _index_values[i].index;
            double value = _index_values[i].value;
            output[i] = _func(input[index], value);
        }
    }

    uint64 Coordinatewise::GetMinInputDim() const
    {
        uint64 min_input_dim = 0;
        for(size_t i = 0; i<_index_values.size(); ++i)
        {
            if(_index_values[i].index >= min_input_dim)
            {
                min_input_dim = _index_values[i].index + 1;
            }
        }
        return min_input_dim;
    }

    uint64 Coordinatewise::GetOutputDim() const
    {
        return (uint64)_index_values.size();
    }

    void Coordinatewise::Serialize(JsonSerializer& serializer) const
    {
        // version 1
        Mapping::SerializeHeader(serializer, 1);
        serializer.Write("index_values", _index_values);
    }

    void Coordinatewise::Deserialize(JsonSerializer& serializer, int version)
    {
        if(version == 1)
        {
            serializer.Read("index_values", _index_values);
        }
        else
        {
            throw runtime_error("unsupported version: " + to_string(version));
        }
    }
}


