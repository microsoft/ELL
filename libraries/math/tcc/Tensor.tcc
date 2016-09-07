////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Tensor.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BlasWrapper.h"
//#define USE_BLAS

// utilities
#include "Exception.h"

// stl
#include <memory> // for std::move
#include <algorithm> // for std::generate

namespace math
{
    //
    // ConstVectorReference
    //  

    template<typename ElementType, TensorOrientation Orientation>
    ElementType ConstVectorReference<ElementType, Orientation>::operator[] (size_t index) const
    {
        // TODO check index<size
        // 
        return _pData[index * _stride];
    }

    template<typename ElementType, TensorOrientation Orientation>
    ElementType ConstVectorReference<ElementType, Orientation>::Norm2() const
    {
#ifdef USE_BLAS
        return Blas::Nrm2(_size, _pData, _stride);
#else
        return std::sqrt(Aggregate([](ElementType x){ return x*x; }));
#endif
    }

    template<typename ElementType, TensorOrientation Orientation>
    ElementType ConstVectorReference<ElementType, Orientation>::Norm1() const
    {
        return Aggregate([](ElementType x) { return std::abs(x); });
    }

    template<typename ElementType, TensorOrientation Orientation>
    ElementType ConstVectorReference<ElementType, Orientation>::Norm0() const
    {
        return Aggregate([](ElementType x) { return x!=0 ? 1 : 0; });
    }

    template<typename ElementType, TensorOrientation Orientation>
    ElementType ConstVectorReference<ElementType, Orientation>::Min() const
    {
        ElementType result = *_pData;
        const ElementType* ptr = _pData+1;
        const ElementType* end = _pData + _size * _stride;
        while(ptr < end)
        {
            if((*ptr) < result)
            {
                result = *ptr;
            }
            ptr += _stride;
        }
        return result;
    }

    template<typename ElementType, TensorOrientation Orientation>
    ElementType ConstVectorReference<ElementType, Orientation>::Max() const
    {
        ElementType result = *_pData;
        const ElementType* ptr = _pData+1;
        const ElementType* end = _pData + _size * _stride;
        while(ptr < end)
        {
            if((*ptr) > result)
            {
                result = *ptr;
            }
            ptr += _stride;
        }
        return result;
    }

    template<typename ElementType, TensorOrientation Orientation>
    ConstVectorReference<ElementType, Orientation> ConstVectorReference<ElementType, Orientation>::GetReference() const
    {
        return ConstVectorReference<ElementType, Orientation>(_pData, _size, _stride);
    }

    template<typename ElementType, TensorOrientation Orientation>
    ConstVectorReference<ElementType, Orientation> ConstVectorReference<ElementType, Orientation>::GetSubVector(size_t offset, size_t size, size_t strideMultiplier) const
    {
        return ConstVectorReference<ElementType, Orientation>(_pData + offset * _stride, size, strideMultiplier * _stride);
    }

    template<typename ElementType, TensorOrientation Orientation>
    ConstVectorReference<ElementType, FlipOrientation<Orientation>::value> ConstVectorReference<ElementType, Orientation>::Transpose() const
    {
        return ConstVectorReference<ElementType, FlipOrientation<Orientation>::value>(_pData, _size, _stride);
    }

    template<typename ElementType, TensorOrientation Orientation>
    bool ConstVectorReference<ElementType, Orientation>::operator==(const ConstVectorReference<ElementType, Orientation>& other) const
    {
        if (_size != other._size)
        {
            return false;
        }

        const ElementType* pThis = _pData;
        const ElementType* pThisEnd = _pData + _size;
        const ElementType* pOther = other._pData;

        while (pThis < pThisEnd)
        {
            if ((*pThis) != (*pOther))
            {
                return false;
            }
            ++pThis;
            ++pOther;
        }
        return true;
    }

    template<typename ElementType, TensorOrientation Orientation>
    ConstVectorReference<ElementType, Orientation>::ConstVectorReference(ElementType * pData, size_t size, size_t stride) : _pData(pData), _size(size), _stride(stride)
    {
        // TODO check that pData != 0
    }

    template<typename ElementType, TensorOrientation Orientation>
    template<typename MapperType>
    ElementType ConstVectorReference<ElementType, Orientation>::Aggregate(MapperType mapper) const
    {
        ElementType result = 0;
        const ElementType* ptr = _pData;
        const ElementType* end = _pData + _size * _stride;
        while(ptr < end)
        {
            result += mapper(*ptr);
            ptr += _stride;
        }
        return result;
    }

    //
    // VectorReference
    //

    template<typename ElementType, TensorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::Reset()
    {
        Fill(0);
    }

    template<typename ElementType, TensorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::Fill(ElementType value)
    {
        ElementType* current = _pData;
        ElementType* end = _pData + _size * _stride;
        while(current < end)
        {
            *current = value;
            current += _stride;
        }
    }

    template<typename ElementType, TensorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::Generate(std::function<ElementType()> generator)
    {
        ElementType* current = _pData;
        ElementType* end = _pData + _size * _stride;
        while(current < end)
        {
            *current = generator();
            current += _stride;
        }
    }

    template<typename ElementType, TensorOrientation Orientation>
    ElementType& VectorReference<ElementType, Orientation>::operator[] (size_t index)
    {
        // TODO check index<size
        // 
        return _pData[index * _stride];
    }

    template<typename ElementType, TensorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::operator+=(ElementType scalar)
    {
        ForEach([scalar](ElementType x) { return x+scalar; });
    }

    template<typename ElementType, TensorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::operator-=(ElementType scalar)
    {
        ForEach([scalar](ElementType x) { return x-scalar; });
    }

    template<typename ElementType, TensorOrientation Orientation>
    void VectorReference<ElementType, Orientation>::operator*=(ElementType scalar)
    {
        ForEach([scalar](ElementType x) { return x*scalar; });
    }

    template<typename ElementType, TensorOrientation Orientation>
    VectorReference<ElementType, Orientation> VectorReference<ElementType, Orientation>::GetReference()
    {
        return VectorReference<ElementType, Orientation>(_pData, _size, _stride);
    }

    template<typename ElementType, TensorOrientation Orientation>
    VectorReference<ElementType, Orientation> VectorReference<ElementType, Orientation>::GetSubVector(size_t offset, size_t size, size_t strideMultiplier)
    {
        return VectorReference<ElementType, Orientation>(_pData + offset * _stride, size, strideMultiplier * _stride);
    }

    template<typename ElementType, TensorOrientation Orientation>
    VectorReference<ElementType, FlipOrientation<Orientation>::value> VectorReference<ElementType, Orientation>::Transpose()
    {
        return VectorReference<ElementType, FlipOrientation<Orientation>::value>(_pData, _size, _stride);
    }

    template<typename ElementType, TensorOrientation Orientation>
    template<typename MapperType>
    void VectorReference<ElementType, Orientation>::ForEach(MapperType mapper)
    {
        ElementType* ptr = _pData;
        const ElementType* end = _pData + _size * _stride;
        while(ptr < end)
        {
            *ptr = mapper(*ptr);
            ptr += _stride;
        }
    }

    //
    // Vector
    //

    template<typename ElementType, TensorOrientation Orientation>
    Vector<ElementType, Orientation>::Vector(size_t size) : _data(size), VectorReference<ElementType, Orientation>(nullptr, size, 1)
    {
        _pData = _data.data();
    }

    template<typename ElementType, TensorOrientation Orientation>
    Vector<ElementType, Orientation>::Vector(std::initializer_list<ElementType> list) : _data(list.begin(), list.end()), VectorReference<ElementType, Orientation>(nullptr, list.size(), 1)
    {
        _pData = _data.data(); // TODO do this in a privat emember called setDataPointer
    }

    template<typename ElementType, TensorOrientation Orientation>
    void Vector<ElementType, Orientation>::Reset()
    {
        std::fill(_data.begin(), _data.end(), static_cast<ElementType>(0));
    }

    template<typename ElementType, TensorOrientation Orientation>
    void Vector<ElementType, Orientation>::Fill(ElementType value)
    {
        std::fill(_data.begin(), _data.end(), value);
    }

    template <typename ElementType, TensorOrientation Orientation>
    ElementType& Vector<ElementType, Orientation>::operator[] (size_t index)
    {
        // TODO check index<size
        // 
        return _pData[index];
    }

    template <typename ElementType, TensorOrientation Orientation>
    ElementType Vector<ElementType, Orientation>::operator[] (size_t index) const
    {
        // TODO check index<size
        // 
        return _pData[index];
    }
}