// Column.cpp

#include "Column.h"
#include "deserializer.h"
#include <stdexcept>
#include <cassert>
#include <string>

using std::swap;
using std::runtime_error;
using std::to_string;

namespace mappings
{

    Column::Column() : _max_internal_dim(0)
    {
        _type = Mapping::Column;
    }

    void Column::apply(const double* input, double* output) const
    {
        size_t size = _column_elements.size();

        if (size == 1)
        {
            _column_elements[0]->apply(input, output);
        }
        else if (size == 2)
        {
            vector<double> tmp(_max_internal_dim);
            _column_elements[0]->apply(input, &tmp[0]);
            _column_elements[1]->apply(&tmp[0], output);
        }
        else if (size > 2)
        {
            vector<double> tmp_in(_max_internal_dim);
            vector<double> tmp_out(_max_internal_dim);

            _column_elements[0]->apply(input, &tmp_in[0]);

            for (size_t i = 1; i < size - 2; ++i)
            {
                _column_elements[i]->apply(&tmp_in[0], &tmp_out[0]);
                swap(tmp_in, tmp_out);
            }

            _column_elements[size - 1]->apply(&tmp_in[0], output);
        }
    }

    int Column::GetMinInputDim() const
    {
        if (_column_elements.size() == 0)
        {
            return 0;
        }
        return _column_elements[0]->GetMinInputDim();
    }

    int Column::GetOutputDim() const
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
        assert(m->GetType() != Mapping::types::Column);

        if (_column_elements.size() > 0)
        {
            int dim = _column_elements[_column_elements.size() - 1]->GetOutputDim();
            
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

    void Column::Serialize(JsonSerializer& js) const
    {
        // version 1
        Mapping::SerializeHeader(js, 1);
        js.write("mappings", _column_elements);
        js.write("max_dim", _max_internal_dim);
    }

    void Column::Deserialize(JsonSerializer& js, int version)
    {
        if (version == 1)
        {
            js.read("mappings", _column_elements);
            js.read("max_dim", _max_internal_dim);
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
