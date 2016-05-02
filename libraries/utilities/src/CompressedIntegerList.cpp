////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CompressedIntegerList.cpp (utilities)
//  Authors:  Chuck Jacobs
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompressedIntegerList.h"

// stl
#include <cassert>
#include <stdexcept>
#include <cstring>

namespace utilities
{

    bool CompressedIntegerList::Iterator::IsValid() const
    {
        return _iter < _end;
    }

    void CompressedIntegerList::Iterator::Next()
    {
        uint64_t delta;
        _iter += _iter_increment;
        uint8_t first_val = *_iter;

        // chop off top 2 bits --- they encode the # of bytes needed for this delta
        // 00 = 1 byte, 01 = 2 bytes, 10 = 4 bytes, 11 = 8 bytes
        int total_bytes = 1 << ((first_val >> 6) & 0x03);
        assert(total_bytes <= 8);
        if (total_bytes == 1)
        {
            // first_val &= 0x3f; // this is commented out because we don't need to strip off the top 2 bits of first_val in this case, because they're always zero
            delta = first_val;
        }
        else
        {
            // read in the Next bytes, shift them over to fit the 6 bits of first_val we're using, and Add first_val
            delta = 0;
            std::memcpy(&delta, _iter + 1, total_bytes - 1);
            delta = (delta << 6) | (first_val & 0x3f);
        }

        _iter_increment = total_bytes;
        _value += delta;
    }

    uint64_t CompressedIntegerList::Iterator::Get() const
    {
        return _value;
    }

    CompressedIntegerList::Iterator::Iterator(const uint8_t *iter, const uint8_t *end) : _iter(iter), _end(end), _value(0), _iter_increment(0)
    {
        Next();
    }

    CompressedIntegerList::CompressedIntegerList() : _last(UINT64_MAX), _size(0)
    {}

    uint64_t CompressedIntegerList::Size() const
    {
        return _size;
    }

    void CompressedIntegerList::Reserve(uint64_t size)
    {
        _data.reserve(size*2); // guess that, on average, every entry will occupy 2 bytes
    }

    uint64_t CompressedIntegerList::Max() const
    {
        if(_size == 0)
        {
            throw std::runtime_error("Can't get max of empty list");
        }

        return _last;
    }

    /// adds an integer at the end of the list
    void CompressedIntegerList::Append(uint64_t value)
    {
        assert(value != UINT64_MAX);    // special value reserved for initialization

        // calculate the delta from the previous number pushed
        uint64_t delta = 0;

        // allow the first Append to have a value of zero, but subsequently require an increasing value
        if (_last < UINT64_MAX)
        {
            assert(value > _last);
        }
        else
        {
            _last = 0;
        }

        // compute the delta
        delta = value - _last;
        _last = value;

        // figure out how many bits we need to represent this value
        int log2bytes = 0;
        if ((delta & 0xffffffffffffffc0) == 0)
        {
            log2bytes = 0; // just need 1 byte
        }
        else if ((delta & 0xffffffffffffc000) == 0)
        {
            log2bytes = 1; // two bytes
        }
        else if ((delta & 0xffffffffc0000000) == 0)
        {
            log2bytes = 2; // four bytes
        }
        else
        {
            log2bytes = 3; // 8 bytes
        }

        int total_bytes = 1 << log2bytes;
        _data.resize(_data.size() + total_bytes); // make room for new data
        uint8_t *buf = _data.data() + _data.size() - total_bytes; // get pointer to correct place in array
        unsigned int mask = log2bytes << 6; // mask == log2(# bytes) shifted to be high-order 2 bits of a byte
        // splice the data length encoding in as the top 2 bits of the first byte
        // So, move all high-order bits of the delta over by 2 to make room, Add the mask, and Add the residual low-order bits of the delta
        uint64_t write_val = ((delta << 2) & 0xffffffffffffff00) | mask | (delta & 0x3f);
        std::memcpy(buf, &write_val, total_bytes);

        ++_size;
    }

    void CompressedIntegerList::Reset()
    {
        _data.resize(0);
        _last = UINT64_MAX;
        _size = 0;
    }

    CompressedIntegerList::Iterator CompressedIntegerList::GetIterator() const
    {
        return Iterator(_data.data(), _data.data() + _data.size());
    }
}
