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

    Row::Row() : _in_dim(0), _out_dim(0)
    {
        _type = Mapping::row;
    }

    void Row::Apply(const double* input, double* output) const
    {
        size_t size = _row_elements.size();

        for (size_t i = 0; i < size; ++i)
        {
            _row_elements[i]->Apply(input, output);
            output += _row_elements[i]->GetOutputDim();
        }
    }

    int Row::GetMinInputDim() const
    {
        return _in_dim;
    }

    int Row::GetOutputDim() const
    {
        return _out_dim;
    }

    void Row::PushBack(shared_ptr<Mapping> m)
    {
        // Don't nest columns in rows. The reason is that the output dimension of a column could decrease after it is nested, and this could cause inconsistencies 
        assert(m->GetType() != Mapping::types::column);

        if (_in_dim < m->GetMinInputDim())
        {
            _in_dim = m->GetMinInputDim();
        }
        _out_dim += m->GetOutputDim();
        _row_elements.push_back(m);
    }

    const shared_ptr<Mapping> Row::operator[] (int index) const
    {
        return _row_elements[index];
    }

    void Row::Serialize(JsonSerializer& serializer) const
    {
        // version 1
        Mapping::SerializeHeader(serializer, 1);
        serializer.Write("mappings", _row_elements);
        serializer.Write("in_dim", _in_dim);
        serializer.Write("out_dim", _out_dim);
    }

    void Row::Deserialize(JsonSerializer& serializer, int version)
    {
        if (version == 1)
        {
            serializer.Read("mappings", _row_elements);
            serializer.Read("in_dim", _in_dim);
            serializer.Read("out_dim", _out_dim);
        }
        else
        {
          throw runtime_error("unsupported version: " + to_string(version));
        }
    }
}
