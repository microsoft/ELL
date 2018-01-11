////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RingBuffer.tcc (utilities)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace utilities
{
    //
    // RingBuffer class
    //
    template <typename T>
    RingBuffer<T>::RingBuffer(size_t size)
        : _buffer(size), _currentPos(0)
    {
    }

    template <typename T>
    size_t RingBuffer<T>::Size() const
    {
        return _buffer.size();
    }

    template <typename T>
    size_t RingBuffer<T>::GetBufferIndex(int entryIndex) const
    {
        int size = static_cast<int>(Size());
        return (size + _currentPos - entryIndex) % (int)size; // Note: it's important the RHS argument to % is a signed int
    }

    template <typename T>
    const T& RingBuffer<T>::operator[](int index) const
    {
        return _buffer[GetBufferIndex(index)];
    }

    template <typename T>
    T& RingBuffer<T>::operator[](int index)
    {
        return _buffer[GetBufferIndex(index)];
    }

    template <typename T>
    void RingBuffer<T>::Append(const T& val)
    {
        int size = static_cast<int>(Size());
        _currentPos = (_currentPos + 1) % size;
        _buffer[_currentPos] = val;
    }

    template <typename T>
    void RingBuffer<T>::Fill(const T& val)
    {
        std::fill(_buffer.begin(), _buffer.end(), val);
    }
}
}
