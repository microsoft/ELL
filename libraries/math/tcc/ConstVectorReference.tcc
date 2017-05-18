////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ConstVectorReference.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Exception.h"

// utilities
#include "Debug.h"

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
    template<typename ElementType>
    ElementType UnorientedConstVectorReference<ElementType>::Norm0() const 
    { 
        return Operations::Norm0(*this); 
    }

    template<typename ElementType>
    ElementType UnorientedConstVectorReference<ElementType>::Norm1() const 
    { 
        return Operations::Norm1(*this); 
    }

    template<typename ElementType>
    ElementType UnorientedConstVectorReference<ElementType>::Norm2() const 
    { 
        return Operations::Norm2(*this); 
    }

    template <typename ElementType>
    std::vector<ElementType> UnorientedConstVectorReference<ElementType>::ToArray() const
    {
        std::vector<ElementType> result(_size);

        ElementType* data = _pData;
        for (size_t i = 0; i < _size; ++i, data += _increment)
            result[i] = *data;

        return result;
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

    template <typename ElementType, VectorOrientation orientation>
    ConstVectorReference<ElementType, orientation> ConstVectorReference<ElementType, orientation>::GetReference() const
    {
        return ConstVectorReference<ElementType, orientation>(_pData, _size, _increment);
    }

    template <typename ElementType, VectorOrientation orientation>
    ConstVectorReference<ElementType, orientation> ConstVectorReference<ElementType, orientation>::GetSubVector(size_t offset, size_t size) const
    {
        DEBUG_THROW(offset + size > _size, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "subvector offset + subvector size exceeds vector size."));

        return ConstVectorReference<ElementType, orientation>(_pData + offset * _increment, size, _increment);
    }

    template <typename ElementType, VectorOrientation orientation>
    bool ConstVectorReference<ElementType, orientation>::IsEqual(ConstVectorReference<ElementType, orientation> other, ElementType tolerance) const
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
            auto diff = (*pThis) - (*pOther);

            if(diff > tolerance || -diff > tolerance)
            {
                return false;
            }
            pThis += _increment;
            pOther += other._increment;
        }
        return true;
    }

    template <typename ElementType, VectorOrientation orientation>
    bool ConstVectorReference<ElementType, orientation>::operator==(const ConstVectorReference<ElementType, orientation>& other) const
    {
        return IsEqual(other);
    }

    template <typename ElementType, VectorOrientation orientation>
    bool ConstVectorReference<ElementType, orientation>::operator!=(const ConstVectorReference<ElementType, orientation>& other) const
    {
        return !(*this == other);
    }
}
}