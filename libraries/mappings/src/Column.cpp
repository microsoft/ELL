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

    Column::Column() : _maxInternalDim(0)
    {
        _type = Mapping::column;
    }

    void Column::Apply(const double* input, double* output) const
    {
        size_t size = _columnElements.size();

        if (size == 1)
        {
            _columnElements[0]->Apply(input, output);
        }
        else if (size == 2)
        {
            vector<double> tmp(_maxInternalDim);
            _columnElements[0]->Apply(input, &tmp[0]);
            _columnElements[1]->Apply(&tmp[0], output);
        }
        else if (size > 2)
        {
            vector<double> tmpIn(_maxInternalDim);
            vector<double> tmpOut(_maxInternalDim);

            _columnElements[0]->Apply(input, &tmpIn[0]);

            for (size_t i = 1; i < size - 2; ++i)
            {
                _columnElements[i]->Apply(&tmpIn[0], &tmpOut[0]);
                swap(tmpIn, tmpOut);
            }

            _columnElements[size - 1]->Apply(&tmpIn[0], output);
        }
    }

    uint64 Column::GetMinInputDim() const
    {
        if (_columnElements.size() == 0)
        {
            return 0;
        }
        return _columnElements[0]->GetMinInputDim();
    }

    uint64 Column::GetOutputDim() const
    {
        if (_columnElements.size() == 0)
        {
            return 0;
        }
        return _columnElements[_columnElements.size() - 1]->GetOutputDim();
    }

    void Column::PushBack(shared_ptr<Mapping> m)
    {
        // don't nest columns
        assert(m->GetType() != Mapping::types::column);

        if (_columnElements.size() > 0)
        {
            auto dim = _columnElements[_columnElements.size() - 1]->GetOutputDim();
            
            if (dim > _maxInternalDim)
            {
                _maxInternalDim = dim;
            }

            // check that dimensions are compatible
            assert(dim >= m->GetMinInputDim());
        }
        _columnElements.push_back(m);
    }

    shared_ptr<Mapping> Column::operator[] (int index)
    {
        return _columnElements[index];
    }

    const shared_ptr<Mapping> Column::operator[] (int index) const
    {
        return _columnElements[index];
    }

    void Column::Serialize(JsonSerializer& serializer) const
    {
        // version 1
        Mapping::SerializeHeader(serializer, 1);
        serializer.Write("mappings", _columnElements);
        serializer.Write("maxDim", _maxInternalDim);
    }

    void Column::Deserialize(JsonSerializer& serializer, int version)
    {
        if (version == 1)
        {
            serializer.Read("mappings", _columnElements);
            serializer.Read("maxDim", _maxInternalDim);
        }
        else
        {
          throw runtime_error("unsupported version: " + to_string(version));
        }
    }

    void Column::KeepLayers(int num)
    {
        int height = (int)_columnElements.size();

        if(num <= 0)
        {
            num += height;
        }

        if (num >= height)
        {
            return;
        }

        _columnElements.resize(num);
    }

}
