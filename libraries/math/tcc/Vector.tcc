////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Vector.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BlasWrapper.h"

// utilities
#include "Exception.h"

// stl
#include <algorithm> // for std::generate

namespace math
{
    //
    // ConstVectorReference
    //  

    template<typename ElementType, VectorOrientation Orientation>
    ElementType ConstVectorReference<ElementType, Orientation>::operator[] (size_t index) const
    {
        // TODO check index<size
        // 
        return _pData[index * _increment ];
    }

    template<typename ElementType, VectorOrientation Orientation>
    ElementType ConstVectorReference<ElementType, Orientation>::Norm2() const
    {
#ifdef USE_BLAS
        return Blas::Nrm2(static_cast<int>(_size), _pData, static_cast<int>(_increment));
#else
        return std::sqrt(Aggregate([](ElementType x){ return x*x; }));
#endif
    }

    template<typename ElementType, VectorOrientation Orientation>
    ElementType ConstVectorReference<ElementType, Orientation>::Norm1() const
    {
#ifdef USE_BLAS
        return Blas::Asum(static_cast<int>(_size), _pData, static_cast<int>(_increment));
#else
        return Aggregate([](ElementType x) { return std::abs(x); });
#endif
    }

    template<typename ElementType, VectorOrientation Orientation>
    ElementType ConstVectorReference<ElementType, Orientation>::Norm0() const
    {
        return Aggregate([](ElementType x) { return x!=0 ? 1 : 0; });
    }

    template<typename ElementType, VectorOrientation Orientation>
    ElementType ConstVectorReference<ElementType, Orientation>::Min() const
    {
        ElementType result = *_pData;
        const ElementType* ptr = _pData+1;
        const ElementType* end = _pData + _size * _increment ;
        while(ptr < end)
        {
            if((*ptr) < result)
            {
                result = *ptr;
            }
            ptr += _increment ;
        }
        return result;
    }

    template<typename ElementType, VectorOrientation Orientation>
    ElementType ConstVectorReference<ElementType, Orientation>::Max() const
    {
        ElementType result = *_pData;
        const ElementType* ptr = _pData+1;
        const ElementType* end = _pData + _size * _increment ;
        while(ptr < end)
        {
            if((*ptr) > result)
            {
                result = *ptr;
            }
            ptr += _increment ;
        }
        return result;
    }

    template<typename ElementType, VectorOrientation Orientation>
    ConstVectorReference<ElementType, Orientation> ConstVectorReference<ElementType, Orientation>::GetReference() const
    {
        return ConstVectorReference<ElementType, Orientation>(_pData, _size, _increment );
    }

    template<typename ElementType, VectorOrientation Orientation>
    ConstVectorReference<ElementType, Orientation> ConstVectorReference<ElementType, Orientation>::GetSubVector(size_t offset, size_t size) const
    {
        return ConstVectorReference<ElementType, Orientation>(_pData + offset * _increment , size, _increment );
    }

    template<typename ElementType, VectorOrientation Orientation>
    auto ConstVectorReference<ElementType, Orientation>::Transpose() const -> ConstVectorReference<ElementType, VectorBase<Orientation>::transposeOrientation>
    {
        return ConstVectorReference<ElementType, VectorBase<Orientation>::transposeOrientation>(_pData, _size, _increment );
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
    
    template<typename ElementType, VectorOrientation Orientation>
    ConstVectorReference<ElementType, Orientation>::ConstVectorReference(ElementType * pData, size_t size, size_t increment) : _pData(pData), _size(size), _increment(increment)
    {
        // TODO check that pData != 0
    }

    template<typename ElementType, VectorOrientation Orientation>
    template<typename MapperType>
    ElementType ConstVectorReference<ElementType, Orientation>::Aggregate(MapperType mapper) const
    {
        ElementType result = 0;
        const ElementType* ptr = _pData;
        const ElementType* end = _pData + _size * _increment ;
        while(ptr < end)
        {
            result += mapper(*ptr);
            ptr += _increment ;
        }
        return result;
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
        ElementType* current = _pData;
        ElementType* end = _pData + _size * _increment ;
        while(current < end)
        {
            *current = value;
            current += _increment ;
        }
    }

    template<typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::Generate(std::function<ElementType()> generator)
    {
        ElementType* current = _pData;
        ElementType* end = _pData + _size * _increment ;
        while(current < end)
        {
            *current = generator();
            current += _increment ;
        }
    }

    template<typename ElementType, VectorOrientation Orientation>
    ElementType& VectorReference<ElementType, Orientation>::operator[] (size_t index)
    {
        // TODO check index<size
        // 
        return _pData[index * _increment ];
    }

    template<typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::operator+=(ElementType scalar)
    {
        ForEach([scalar](ElementType x) { return x+scalar; });
    }

    template<typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::operator-=(ElementType scalar)
    {
        ForEach([scalar](ElementType x) { return x-scalar; });
    }

    template<typename ElementType, VectorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::operator*=(ElementType scalar)
    {
#ifdef USE_BLAS
        Blas::Scal(static_cast<int>(_size), scalar, _pData, static_cast<int>(_increment));
#else
        ForEach([scalar](ElementType x) { return x*scalar; });
#endif
    }

    template<typename ElementType, VectorOrientation Orientation>
    VectorReference<ElementType, Orientation> VectorReference<ElementType, Orientation>::GetReference()
    {
        return VectorReference<ElementType, Orientation>(_pData, _size, _increment );
    }

    template<typename ElementType, VectorOrientation Orientation>
    VectorReference<ElementType, Orientation> VectorReference<ElementType, Orientation>::GetSubVector(size_t offset, size_t size)
    {
        return VectorReference<ElementType, Orientation>(_pData + offset * _increment , size, _increment );
    }

    template<typename ElementType, VectorOrientation Orientation>
    auto VectorReference<ElementType, Orientation>::Transpose() -> VectorReference<ElementType, VectorBase<Orientation>::transposeOrientation>
    {
        return VectorReference<ElementType, VectorBase<Orientation>::transposeOrientation>(_pData, _size, _increment );
    }

    template<typename ElementType, VectorOrientation Orientation>
    template<typename MapperType>
    void VectorReference<ElementType, Orientation>::ForEach(MapperType mapper)
    {
        ElementType* ptr = _pData;
        const ElementType* end = _pData + _size * _increment ;
        while(ptr < end)
        {
            *ptr = mapper(*ptr);
            ptr += _increment ;
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

    template<typename ElementType, VectorOrientation Orientation>
    void Vector<ElementType, Orientation>::Reset()
    {
        std::fill(_data.begin(), _data.end(), static_cast<ElementType>(0));
    }

    template<typename ElementType, VectorOrientation Orientation>
    void Vector<ElementType, Orientation>::Fill(ElementType value)
    {
        std::fill(_data.begin(), _data.end(), value);
    }

    template<typename ElementType, VectorOrientation Orientation>
    void Vector<ElementType, Orientation>::Generate(std::function<ElementType()> generator)
    {
        std::generate(_data.begin(), _data.end(), generator);
    }

    template <typename ElementType, VectorOrientation Orientation>
    ElementType& Vector<ElementType, Orientation>::operator[] (size_t index)
    {
        // TODO check index<size
        // 
        return _pData[index];
    }

    template <typename ElementType, VectorOrientation Orientation>
    ElementType Vector<ElementType, Orientation>::operator[] (size_t index) const
    {
        // TODO check index<size
        // 
        return _pData[index];
    }
}