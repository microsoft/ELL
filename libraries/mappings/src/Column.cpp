// Column.cpp

#include "Column.h"
#include "deserializer.h"

#include <stdexcept>
using std::runtime_error;

#include <cassert>

#include <string>
using std::swap;
using std::to_string;

namespace mappings
{

    Column::Column() : _max_internal_dim(0)
    {
        _type = Mapping::column;
    }

    void Column::Apply(const double* input, double* output) const
    {
        size_t size = _column_elements.size();

        if (size == 1)
        {
            _column_elements[0]->Apply(input, output);
        }
        else if (size == 2)
        {
            vector<double> tmp(_max_internal_dim);
            _column_elements[0]->Apply(input, &tmp[0]);
            _column_elements[1]->Apply(&tmp[0], output);
        }
        else if (size > 2)
        {
            vector<double> tmp_in(_max_internal_dim);
            vector<double> tmp_out(_max_internal_dim);

            _column_elements[0]->Apply(input, &tmp_in[0]);

            for (size_t i = 1; i < size - 2; ++i)
            {
                _column_elements[i]->Apply(&tmp_in[0], &tmp_out[0]);
                swap(tmp_in, tmp_out);
            }

            _column_elements[size - 1]->Apply(&tmp_in[0], output);
        }
    }

    uint64 Column::GetMinInputDim() const
    {
        if (_column_elements.size() == 0)
        {
            return 0;
        }
        return _column_elements[0]->GetMinInputDim();
    }

    uint64 Column::GetOutputDim() const
    {
        if (_column_elements.size() == 0)
        {
            return 0;
        }
        return _column_elements[_column_elements.size() - 1]->GetOutputDim();
    }

    void Column::PushBack(shared_ptr<Mapping> m)
    {
        // don't nest columns
        assert(m->GetType() != Mapping::types::column);

        if (_column_elements.size() > 0)
        {
            auto dim = _column_elements[_column_elements.size() - 1]->GetOutputDim();
            
            if (dim > _max_internal_dim)
            {
                _max_internal_dim = dim;
            }

            // check that dimensions are compatible
            assert(dim >= m->GetMinInputDim());
        }
        _column_elements.push_back(m);
    }

    shared_ptr<Mapping> Column::operator[] (int index)
    {
        return _column_elements[index];
    }

    const shared_ptr<Mapping> Column::operator[] (int index) const
    {
        return _column_elements[index];
    }

    void Column::Serialize(JsonSerializer& serializer) const
    {
        // version 1
        Mapping::SerializeHeader(serializer, 1);
        serializer.Write("mappings", _column_elements);
        serializer.Write("max_dim", _max_internal_dim);
    }

    void Column::Deserialize(JsonSerializer& serializer, int version)
    {
        if (version == 1)
        {
            serializer.Read("mappings", _column_elements);
            serializer.Read("max_dim", _max_internal_dim);
        }
        else
        {
          throw runtime_error("unsupported version: " + to_string(version));
        }
    }

    void Column::KeepLayers(int num)
    {
        int height = (int)_column_elements.size();

        if(num <= 0)
        {
            num += height;
        }

        if (num >= height)
        {
            return;
        }

        _column_elements.resize(num);
    }

}
