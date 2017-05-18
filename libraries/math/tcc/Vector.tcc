////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Vector.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Operations.h"

// utilities
#include "Debug.h"
#include "Exception.h"

namespace ell
{
namespace math
{
    //
    // Vector
    //

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(size_t size)
        : VectorReference<ElementType, orientation>(nullptr, size, 1), _data(size)
    {
        _pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(std::vector<ElementType> data)
        : VectorReference<ElementType, orientation>(nullptr, data.size(), 1), _data(std::move(data))
    {
        _pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(std::initializer_list<ElementType> list)
        : VectorReference<ElementType, orientation>(nullptr, list.size(), 1), _data(list.begin(), list.end())
    {
        _pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(Vector<ElementType, orientation>&& other)
        : VectorReference<ElementType, orientation>(nullptr, other._size, other._increment), _data(std::move(other._data))
    {
        _pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>::Vector(const Vector<ElementType, orientation>& other)
        : VectorReference<ElementType, orientation>(nullptr, other._size, other._increment), _data(other._data)
    {
        _pData = _data.data();
    }

    template <typename ElementType, VectorOrientation orientation>
    void Vector<ElementType, orientation>::Resize(size_t size)
    {
        _data.resize(size);
        _pData = _data.data();
        _size = size;
    }

    template <typename ElementType, VectorOrientation orientation>
    Vector<ElementType, orientation>& Vector<ElementType, orientation>::operator=(Vector<ElementType, orientation> other)
    {
        Swap(other);
        return *this;
    }

    template <typename ElementType, VectorOrientation orientation>
    void Vector<ElementType, orientation>::Swap(Vector<ElementType, orientation>& other)
    {
        UnorientedConstVectorReference<ElementType>::Swap(other);
        std::swap(_data, other._data);
    }
}
}