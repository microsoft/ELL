// Row.cpp

#include "Row.h"
#include "deserializer.h"

#include <stdexcept>
using std::runtime_error;

#include <cassert>

#include <string>
using std::to_string;

namespace mappings
{

    Row::Row() : _inDim(0), _outDim(0)
    {
        _type = Mapping::row;
    }

    void Row::Apply(const double* input, double* output) const
    {
        size_t size = _rowElements.size();

        for (size_t i = 0; i < size; ++i)
        {
            _rowElements[i]->Apply(input, output);
            output += _rowElements[i]->GetOutputDim();
        }
    }

    uint64 Row::GetMinInputDim() const
    {
        return _inDim;
    }

    uint64 Row::GetOutputDim() const
    {
        return _outDim;
    }

    void Row::PushBack(shared_ptr<Mapping> m)
    {
        // Don't nest columns in rows. The reason is that the output dimension of a column could decrease after it is nested, and this could cause inconsistencies 
        assert(m->GetType() != Mapping::types::column);

        if (_inDim < m->GetMinInputDim())
        {
            _inDim = m->GetMinInputDim();
        }
        _outDim += m->GetOutputDim();
        _rowElements.push_back(m);
    }

    const shared_ptr<Mapping> Row::operator[] (int index) const
    {
        return _rowElements[index];
    }

    void Row::Serialize(JsonSerializer& serializer) const
    {
        // version 1
        Mapping::SerializeHeader(serializer, 1);
        serializer.Write("mappings", _rowElements);
        serializer.Write("inDim", _inDim);
        serializer.Write("outDim", _outDim);
    }

    void Row::Deserialize(JsonSerializer& serializer, int version)
    {
        if (version == 1)
        {
            serializer.Read("mappings", _rowElements);
            serializer.Read("inDim", _inDim);
            serializer.Read("outDim", _outDim);
        }
        else
        {
          throw runtime_error("unsupported version: " + to_string(version));
        }
    }
}
