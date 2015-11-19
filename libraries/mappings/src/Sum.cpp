// Sum.cpp

#include "Sum.h"

#include <stdexcept>
using std::runtime_error;

#include <string>
using std::to_string;

namespace mappings
{

    Sum::Sum(uint64 firstInputIndex, uint64 inputIndexCount) : _firstInputIndex(firstInputIndex), _inputIndexCount(inputIndexCount) // TODO indices ?
    {
        _type = types::sum;
    }

    void Sum::SetFirstInputIndices(uint64 index)
    {
        _firstInputIndex = index;
    }

    uint64 Sum::GetFirstInputIndices() const
    {
        return _firstInputIndex;
    }

    void Sum::SetInputIndicesCount(uint64 count)
    {
        _inputIndexCount = count;
    }

    uint64 Sum::GetInputIndicesCount() const
    {
        return _inputIndexCount;
    }
    
    void Sum::Apply(const double* input, double* output) const
    {
        double value = 0.0;
        
        for (uint64 i = _firstInputIndex; i < _firstInputIndex + _inputIndexCount; ++i)
        {
            value += input[i];
        }
        output[0] = value;
    }

    uint64 Sum::GetMinInputDim() const
    {
        return _firstInputIndex + _inputIndexCount;
    }

    uint64 Sum::GetOutputDim() const
    {
        return 1;
    }

    void Sum::Serialize(JsonSerializer& serializer) const
    {
        // version 1
        Mapping::SerializeHeader(serializer, 1);
        serializer.Write("firstInputIndex", _firstInputIndex);
        serializer.Write("inputIndexCount", _inputIndexCount);
    }

    void Sum::Deserialize(JsonSerializer& serializer, int version)
    {
        if (version == 1)
        {
            serializer.Read("firstInputIndex", _firstInputIndex);
            serializer.Read("inputIndexCount", _inputIndexCount);
        }
        else
        {
          throw runtime_error("unsupported version: " + to_string(version));
        }
    }
}
