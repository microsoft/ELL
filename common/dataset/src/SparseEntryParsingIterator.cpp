// SparseEntryParsingIterator.cpp

#include "SparseEntryParsingIterator.h"

#include <memory>
using std::move;

#include <stdexcept>
using std::runtime_error;

namespace dataset
{
    SparseEntryParsingIterator::SparseEntryParsingIterator(stringstream&& sstream) : _sstream(move(sstream)), _currentIndexValue(0,0)
    {
        Next();   
    }

    bool SparseEntryParsingIterator::IsValid() const
    {
        return !_sstream.fail();
    }

    void SparseEntryParsingIterator::Next()
    {
        uint index;
        _sstream >> index;

        char ch;
        _sstream >> ch;

        if (ch != ':')
        {
            _sstream.seekg(0, _sstream.end);
        }

        double value;
        _sstream >> value;

        _currentIndexValue = IndexValue(index, value);
    }

    IndexValue SparseEntryParsingIterator::GetValue() const
    {
        return _currentIndexValue;
    }
}