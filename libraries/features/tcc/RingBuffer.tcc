////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     RingBuffer.tcc (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <cassert>

namespace features
{
    //
    // RingBuffer class
    //
    template <typename T>
    RingBuffer<T>::RingBuffer(size_t size) : _currentPos(0), _size(size)
    {
        _buffer.reserve(size);
    }

    template <typename T>
    size_t RingBuffer<T>::Size() const
    {
        return _buffer.size();
    }

    template <typename T>
    T RingBuffer<T>::operator[](int index) const
    {
        if (_size != 0)
        {
            if (_buffer.size() != _size) // buffer not full yet
            {
                if (_buffer.size() > index)
                {
                    return _buffer[_buffer.size() - index - 1];
                }
                else
                {
                    assert(false);
                    return 0;
                }
            }
            else
            {
                return _buffer[(_currentPos - index) % (int)_size]; // Note: it's important the RHS argument to % is a signed int
            }
        }
        else
        {
            assert(false);
            return 0;
        }
    }

    template <typename T>
    void RingBuffer<T>::Append(const T& val)
    {
        if (_size != 0)
        {
            if (_buffer.size() == _size) // full
            {
                _currentPos = (_currentPos + 1) % _size;
                _buffer[_currentPos] = val;
            }
            else
            {
                _buffer.push_back(val);
                _currentPos = _buffer.size() - 1;
            }
        }
    }

    //
    // RingBufferArray version, which uses a std::array as the backing store
    //
    template <typename T, int N>
    RingBufferArray<T,N>::RingBufferArray()
    {
        _arr.fill(T());
    }

    template <typename T, int N>
    size_t RingBufferArray<T,N>::Size() const
    {
        return _arr.size();
    }

    template <typename T, int N>
    T RingBufferArray<T,N>::operator[](int index) const
    {
        // allow negative indices
        index = (index + _currentPos) % (int)_arr.size(); // yikes! need to cast size to signed value, otherwise % behaves differently
        if (index < 0) index += _arr.size();
        return _arr[index];
    }

    template <typename T, int N>
    void RingBufferArray<T,N>::Append(const T& val)
    {
        _currentPos = (_currentPos + 1) % _arr.size();
        _arr[_currentPos] = val;
    }


    // specialization for zero-length buffer (which hopefully never happens) 
    // Idea: we could interpret this to be a dynamically-sized buffer, and thereby unify the 
    // interface
    template <typename T>
    class RingBufferArray<T,0>
    {
    public:
        RingBufferArray();
        T operator[](int index) const;
        void Append(const T& val);
        size_t Size() const;
    };
        
    template <typename T>
    RingBufferArray<T,0>::RingBufferArray()
    {
    }

    template <typename T>
    size_t RingBufferArray<T,0>::Size() const
    {
        return 0;
    }

    template <typename T>
    T RingBufferArray<T,0>::operator[](int) const
    {
    //    static_assert(false, ":(");
        return T();
    }

    template <typename T>
    void RingBufferArray<T,0>::Append(const T&)
    {
    //    static_assert(false, ":(");
    }

}