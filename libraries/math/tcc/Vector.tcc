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

// stl
#include <algorithm> // for std::generate

namespace ell
{
namespace math
{
    //
    // UnorientedConstVectorReference
    //

    template <typename ElementType>
    ElementType UnorientedConstVectorReference<ElementType>::operator[](size_t index) const
    {
        DEBUG_THROW(index >= _size, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds vector size."));

        return _pData[index * _increment];
    }

    template <typename ElementType>
    std::vector<ElementType> UnorientedConstVectorReference<ElementType>::ToArray() const
    {
        return std::vector<ElementType>(_pData, _pData + _size);
    }

    template <typename ElementType>
    UnorientedConstVectorReference<ElementType>::UnorientedConstVectorReference(ElementType* pData, size_t size, size_t increment)
        : _pData(pData), _size(size), _increment(increment)
    {
    }

    template <typename ElementType>
    void UnorientedConstVectorReference<ElementType>::Swap(UnorientedConstVectorReference<ElementType>& other)
    {
        std::swap(_pData, other._pData);
        std::swap(_size, other._size);
        std::swap(_increment, other._increment);
    }

    template <typename ElementType>
    template <typename MapperType>
    ElementType UnorientedConstVectorReference<ElementType>::Aggregate(MapperType mapper) const
    {
        ElementType result = 0;
        const ElementType* current = _pData;
        const ElementType* end = _pData + _size * _increment;
        while (current < end)
        {
            result += mapper(*current);
            current += _increment;
        }
        return result;
    }

    template <typename ElementType>
    void UnorientedConstVectorReference<ElementType>::Print(std::ostream& ostream) const
    {
        if (_size > 0)
        {
            ostream << *_pData;
        }

        for (size_t i = 1; i < _size; ++i)
        {
            ostream << '\t' << _pData[i * _increment];
        }
    }

    template <typename ElementType>
    std::ostream& operator<<(std::ostream& ostream, UnorientedConstVectorReference<ElementType> vector)
    {
        vector.Print(ostream);
        return ostream;
    }

    //
    // ConstVectorReference
    //

    template <typename ElementType, VectorOrientation Orientation>
    ConstVectorReference<ElementType, Orientation> ConstVectorReference<ElementType, Orientation>::GetReference() const
    {
        return ConstVectorReference<ElementType, Orientation>(_pData, _size, _increment);
    }

    template <typename ElementType, VectorOrientation Orientation>
    ConstVectorReference<ElementType, Orientation> ConstVectorReference<ElementType, Orientation>::GetSubVector(size_t offset, size_t size) const
    {
        DEBUG_THROW(offset + size > _size, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "subvector offset + subvector size exceeds vector size."));

        return ConstVectorReference<ElementType, Orientation>(_pData + offset * _increment, size, _increment);
    }

    template <typename ElementType, VectorOrientation Orientation>
    bool ConstVectorReference<ElementType, Orientation>::operator==(const ConstVectorReference<ElementType, Orientation>& other) const
    {
        if (_size != other._size)
        {
            return false;
        }

        const ElementType* pThis = _pData;
        const ElementType* pThisEnd = _pData + _size * _increment;
        const ElementType* pOther = other._pData;

        while (pThis < pThisEnd)
        {
            if ((*pThis) != (*pOther))
            {
                return false;
            }
            pThis += _increment;
            pOther += other._increment;
        }
        return true;
    }

    template <typename ElementType, VectorOrientation Orientation>
    bool ConstVectorReference<ElementType, Orientation>::operator!=(const ConstVectorReference<ElementType, Orientation>& other) const
    {
        return !(*this == other);
    }

    //
    // ScaledConstVectorReference
    //

    template <typename ElementType, VectorOrientation Orientation>
    ScaledConstVectorReference<ElementType, Orientation>::ScaledConstVectorReference(double scalar, ConstVectorReference<ElementType, Orientation> vector)
        : _scalar(scalar), _vector(vector)
    {
    }

    template <typename ElementType, VectorOrientation Orientation>
    ScaledConstVectorReference<ElementType, Orientation> operator*(double scalar, ConstVectorReference<ElementType, Orientation> vector)
    {
        return ScaledConstVectorReference<ElementType, Orientation>(scalar, vector);
    }

    //
    // VectorReference
    //

    template <typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::Set(ConstVectorReference<ElementType, Orientation> other)
    {
        Operations::Copy(other, *this);
    }

    template <typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::Set(ScaledConstVectorReference<ElementType, Orientation> other)
    {
        Reset();
        Operations::Add(other.GetScalar(), other.GetVector(), *this);
    }

    template <typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::Reset()
    {
        Fill(0);
    }

    template <typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::Fill(ElementType value)
    {
        ElementType* data = _pData;
        ElementType* end = _pData + _size * _increment;

        if (_increment == 1)
        {
            std::fill(data, end, value);
        }
        else
        {
            while (data < end)
            {
                *data = value;
                data += _increment;
            }
        }
    }

    template <typename ElementType, VectorOrientation Orientation>
    template <typename GeneratorType>
    void VectorReference<ElementType, Orientation>::Generate(GeneratorType generator)
    {
        ElementType* data = _pData;
        ElementType* end = _pData + _size * _increment;

        if (_increment == 1)
        {
            std::generate(data, end, generator);
        }
        else
        {
            while (data < end)
            {
                *data = generator();
                data += _increment;
            }
        }
    }

    template <typename ElementType, VectorOrientation Orientation>
    ElementType& VectorReference<ElementType, Orientation>::operator[](size_t index)
    {
        DEBUG_THROW(index >= _size, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds vector size."));

        return _pData[index * _increment];
    }

    template <typename ElementType, VectorOrientation Orientation>
    VectorReference<ElementType, Orientation> VectorReference<ElementType, Orientation>::GetReference()
    {
        return VectorReference<ElementType, Orientation>(_pData, _size, _increment);
    }

    template <typename ElementType, VectorOrientation Orientation>
    VectorReference<ElementType, Orientation> VectorReference<ElementType, Orientation>::GetSubVector(size_t offset, size_t size)
    {
        DEBUG_THROW(offset + size > _size, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "subvector offset + subvector size exceeds vector size."));

        return VectorReference<ElementType, Orientation>(_pData + offset * _increment, size, _increment);
    }

    template <typename ElementType, VectorOrientation Orientation>
    template <typename MapperType>
    void VectorReference<ElementType, Orientation>::Transform(MapperType mapper)
    {
        ElementType* current = _pData;
        const ElementType* end = _pData + _size * _increment;
        while (current < end)
        {
            *current = mapper(*current);
            current += _increment;
        }
    }

    template <typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::operator+=(ConstVectorReference<ElementType, Orientation> other)
    {
        Operations::Add(1.0, other, *this);
    }

    template <typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::operator+=(ScaledConstVectorReference<ElementType, Orientation> other)
    {
        Operations::Add(other.GetScalar(), other.GetVector(), *this);
    }

    template <typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::operator-=(ConstVectorReference<ElementType, Orientation> other)
    {
        Operations::Add(-1.0, other, *this);
    }

    template <typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::operator+=(ElementType value)
    {
        Operations::Add(value, *this);
    }

    template <typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::operator-=(ElementType value)
    {
        Operations::Add(-value, *this);
    }

    template <typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::operator*=(ElementType value)
    {
        Operations::Multiply(value, *this);
    }

    template <typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::operator/=(ElementType value)
    {
        DEBUG_THROW(value == 0, utilities::NumericException(utilities::NumericExceptionErrors::divideByZero, "divide by zero"));

        Operations::Multiply(1.0 / value, *this);
    }

    template <typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::CoordinatewiseSquare()
    {
        Transform([](ElementType x) { return x * x; });
    }

    template <typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::CoordinatewiseSquareRoot()
    {
        Transform([](ElementType x) { return static_cast<ElementType>(std::sqrt(x)); });
    }

    //
    // Vector
    //

    template <typename ElementType, VectorOrientation Orientation>
    Vector<ElementType, Orientation>::Vector(size_t size)
        : VectorReference<ElementType, Orientation>(nullptr, size, 1), _data(size)
    {
        _pData = _data.data();
    }

    template <typename ElementType, VectorOrientation Orientation>
    Vector<ElementType, Orientation>::Vector(std::vector<ElementType> data)
        : VectorReference<ElementType, Orientation>(nullptr, data.size(), 1), _data(std::move(data))
    {
        _pData = _data.data();
    }

    template <typename ElementType, VectorOrientation Orientation>
    Vector<ElementType, Orientation>::Vector(std::initializer_list<ElementType> list)
        : VectorReference<ElementType, Orientation>(nullptr, list.size(), 1), _data(list.begin(), list.end())
    {
        _pData = _data.data();
    }

    template <typename ElementType, VectorOrientation Orientation>
    Vector<ElementType, Orientation>::Vector(Vector<ElementType, Orientation>&& other)
        : VectorReference<ElementType, Orientation>(nullptr, other._size, other._increment), _data(std::move(other._data))
    {
        _pData = _data.data();
    }

    template <typename ElementType, VectorOrientation Orientation>
    Vector<ElementType, Orientation>::Vector(const Vector<ElementType, Orientation>& other)
        : VectorReference<ElementType, Orientation>(nullptr, other._size, other._increment), _data(other._data)
    {
        _pData = _data.data();
    }

    template <typename ElementType, VectorOrientation Orientation>
    void Vector<ElementType, Orientation>::Resize(size_t size)
    {
        _data.resize(size);
        _pData = _data.data();
        _size = size;
    }

    template <typename ElementType, VectorOrientation Orientation>
    Vector<ElementType, Orientation>& Vector<ElementType, Orientation>::operator=(Vector<ElementType, Orientation> other)
    {
        Swap(other);
        return *this;
    }

    template <typename ElementType, VectorOrientation Orientation>
    void Vector<ElementType, Orientation>::Swap(Vector<ElementType, Orientation>& other)
    {
        UnorientedConstVectorReference<ElementType>::Swap(other);
        std::swap(_data, other._data);
    }
}
}