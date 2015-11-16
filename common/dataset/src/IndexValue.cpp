// IndexValue.cpp

#include "IndexValue.h"

namespace dataset
{
    IndexValue::IndexValue(uint index, double value) : _index(index), _value(value)
    {}

    uint IndexValue::GetIndex() const
    {
        return _index;
    }

    double IndexValue::GetValue() const
    {
        return _value;
    }

    void IndexValue::Print(ostream & os) const
    {
        os << _index << ':' << _value;
    }

    ostream& operator<<(ostream& os, const IndexValue& indexValue)
    {
        indexValue.Print(os);
        return os;
    }
}