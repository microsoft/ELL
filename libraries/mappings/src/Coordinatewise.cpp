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
            int index = _index_values[i].GetIndex();
            double value = _index_values[i].GetValue();
            output[i] = _func(input[index], value);
        }
    }

    int Coordinatewise::GetMinInputDim() const
    {
        int min_input_dim = 0;
        for(size_t i = 0; i<_index_values.size(); ++i)
        {
            if(_index_values[i].GetIndex() >= min_input_dim)
            {
                min_input_dim = _index_values[i].GetIndex() + 1;
            }
        }
        return min_input_dim;
    }

    int Coordinatewise::GetOutputDim() const
    {
        return (int)_index_values.size();
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


