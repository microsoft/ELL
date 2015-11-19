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
        return _indexValues.cbegin();
    }

    Coordinatewise::IndexValueIterator Coordinatewise::end() const
    {
        return _indexValues.cend();
    }

    void Coordinatewise::Apply(const double* input, double* output) const
    {
        for(size_t i = 0; i<_indexValues.size(); ++i)
        {
            uint64 index = _indexValues[i].index;
            double value = _indexValues[i].value;
            output[i] = _func(input[index], value);
        }
    }

    uint64 Coordinatewise::GetMinInputDim() const
    {
        uint64 min_input_dim = 0;
        for(size_t i = 0; i<_indexValues.size(); ++i)
        {
            if(_indexValues[i].index >= min_input_dim)
            {
                min_input_dim = _indexValues[i].index + 1;
            }
        }
        return min_input_dim;
    }

    uint64 Coordinatewise::GetOutputDim() const
    {
        return (uint64)_indexValues.size();
    }

    void Coordinatewise::Serialize(JsonSerializer& serializer) const
    {
        // version 1
        Mapping::SerializeHeader(serializer, 1);
        serializer.Write("indexValues", _indexValues);
    }

    void Coordinatewise::Deserialize(JsonSerializer& serializer, int version)
    {
        if(version == 1)
        {
            serializer.Read("indexValues", _indexValues);
        }
        else
        {
            throw runtime_error("unsupported version: " + to_string(version));
        }
    }
}


