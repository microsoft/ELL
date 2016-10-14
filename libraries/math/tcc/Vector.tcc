////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Vector.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BlasWrapper.h"
#include "Operations.h"

// utilities
#include "Exception.h"

// stl
#include <algorithm> // for std::generate

namespace emll
{
namespace math
{
    //
    // UnorientedConstVectorReference
    //  

    template<typename ElementType>
    ElementType UnorientedConstVectorReference<ElementType>::operator[] (size_t index) const
    {
        if (index >= _size)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds vector size.");
        }
        return _pData[index * _increment ];
    }

    template<typename ElementType>
    std::vector<double> UnorientedConstVectorReference<ElementType>::ToArray() const
    {
        return std::vector<double>(_pData, _pData+_size);
    }

    template<typename ElementType>
    UnorientedConstVectorReference<ElementType>::UnorientedConstVectorReference(ElementType * pData, size_t size, size_t increment) : _pData(pData), _size(size), _increment(increment)
    {}

    template<typename ElementType>
    template<typename MapperType>
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

    //
    // ConstVectorReference
    //  

    template<typename ElementType, VectorOrientation Orientation>
    ConstVectorReference<ElementType, Orientation> ConstVectorReference<ElementType, Orientation>::GetReference() const
    {
        return ConstVectorReference<ElementType, Orientation>(_pData, _size, _increment );
    }

    template<typename ElementType, VectorOrientation Orientation>
    ConstVectorReference<ElementType, Orientation> ConstVectorReference<ElementType, Orientation>::GetSubVector(size_t offset, size_t size) const
    {
        if (offset + size > _size)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "subvector offset + subvector size exceeds vector size.");
        }
        return ConstVectorReference<ElementType, Orientation>(_pData + offset * _increment , size, _increment );
    }

    template<typename ElementType, VectorOrientation Orientation>
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

    template<typename ElementType, VectorOrientation Orientation>
    bool ConstVectorReference<ElementType, Orientation>::operator!=(const ConstVectorReference<ElementType, Orientation>& other) const
    {
        return !(*this == other);
    }
    
    //
    // VectorReference
    //

    template<typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::Reset()
    {
        Fill(0);
    }

    template<typename ElementType, VectorOrientation Orientation>
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

    template<typename ElementType, VectorOrientation Orientation>
    template<typename GeneratorType>
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

    template<typename ElementType, VectorOrientation Orientation>
    ElementType& VectorReference<ElementType, Orientation>::operator[] (size_t index)
    {
        if (index >= _size)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds vector size.");
        }
        return _pData[index * _increment ];
    }

    template<typename ElementType, VectorOrientation Orientation>
    VectorReference<ElementType, Orientation> VectorReference<ElementType, Orientation>::GetReference()
    {
        return VectorReference<ElementType, Orientation>(_pData, _size, _increment );
    }

    template<typename ElementType, VectorOrientation Orientation>
    VectorReference<ElementType, Orientation> VectorReference<ElementType, Orientation>::GetSubVector(size_t offset, size_t size)
    {
        if (offset + size > _size)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "subvector offset + subvector size exceeds vector size.");
        }
        return VectorReference<ElementType, Orientation>(_pData + offset * _increment , size, _increment );
    }

    template<typename ElementType, VectorOrientation Orientation>
    template<typename MapperType>
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

    //
    // Vector
    //

    template<typename ElementType, VectorOrientation Orientation>
    Vector<ElementType, Orientation>::Vector(size_t size) : _data(size), VectorReference<ElementType, Orientation>(nullptr, size, 1)
    {
        _pData = _data.data();
    }

    template<typename ElementType, VectorOrientation Orientation>
    Vector<ElementType, Orientation>::Vector(std::initializer_list<ElementType> list) : _data(list.begin(), list.end()), VectorReference<ElementType, Orientation>(nullptr, list.size(), 1)
    {
        _pData = _data.data(); 
    }
}
}