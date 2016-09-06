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
#include "..\include\Tensor.h"

namespace math
{
    //
    // TensorReferenceBase
    //

    template<typename ElementType>
    math::TensorReferenceBase<ElementType>::TensorReferenceBase(ElementType* pData) : _pData(pData)
    {
        // TODO assert pData!= 0
    }

    //
    // VectorReferenceBase
    //  

    template<typename ElementType>
    void VectorReferenceBase<ElementType>::Reset()
    {
        Fill(0);
    }

    template<typename ElementType>
    void VectorReferenceBase<ElementType>::Fill(ElementType value)
    {
        ElementType* current = _pData;
        ElementType* end = _pData + _size * _stride;
        while(current < end)
        {
            *current = value;
            current += _stride;
        }
    }

    template<typename ElementType>
    void VectorReferenceBase<ElementType>::Generate(std::function<ElementType()> generator)
    {
        ElementType* current = _pData;
        ElementType* end = _pData + _size * _stride;
        while(current < end)
        {
            *current = generator();
            current += _stride;
        }
    }

    template <typename ElementType>
    ElementType& VectorReferenceBase<ElementType>::operator[] (size_t index)
    {
        // TODO check index<size
        // 
        return _pData[index * _stride];
    }

    template <typename ElementType>
    ElementType VectorReferenceBase<ElementType>::operator[] (size_t index) const
    {
        // TODO check index<size
        // 
        return _pData[index * _stride];
    }

    template<typename ElementType>
    VectorReferenceBase<ElementType>::VectorReferenceBase(ElementType * pData, size_t size, size_t stride) : TensorReferenceBase<ElementType>(pData), _size(size), _stride(stride)
    {}

    template<typename ElementType>
    void VectorReferenceBase<ElementType>::operator+=(ElementType scalar)
    {
        ForEach([scalar](ElementType x) { return x+scalar; });
    }

    template<typename ElementType>
    void VectorReferenceBase<ElementType>::operator-=(ElementType scalar)
    {
        ForEach([scalar](ElementType x) { return x-scalar; });
    }

    template<typename ElementType>
    void VectorReferenceBase<ElementType>::operator*=(ElementType scalar)
    {
        ForEach([scalar](ElementType x) { return x*scalar; });
    }

    template<typename ElementType>
    inline ElementType VectorReferenceBase<ElementType>::Norm2() const
    {
#ifdef USE_BLAS
        return Blas::Nrm2(_size, _pData, _stride);
#else
        return Aggregate([](ElementType x){ return x*x; });
#endif
    }

    template<typename ElementType>
    inline ElementType VectorReferenceBase<ElementType>::Norm1() const
    {
        return Aggregate([](ElementType x) { return std::abs(x); });
    }

    template<typename ElementType>
    inline ElementType VectorReferenceBase<ElementType>::Norm0() const
    {
        return Aggregate([](ElementType x) { return x!=0 ? 1 : 0; });
    }

    template<typename ElementType>
    inline ElementType VectorReferenceBase<ElementType>::Min() const
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

    template<typename ElementType>
    inline ElementType VectorReferenceBase<ElementType>::Max() const
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

    template<typename ElementType>
    template<typename MapperType>
    ElementType VectorReferenceBase<ElementType>::Aggregate(MapperType mapper) const
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

    template<typename ElementType>
    template<typename MapperType>
    void VectorReferenceBase<ElementType>::ForEach(MapperType mapper)
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
    // VectorReference
    //

    template<typename ElementType, TensorOrientation Orientation>
    inline VectorReference<ElementType, Orientation> VectorReference<ElementType, Orientation>::GetReference()
    {
        return VectorReference<ElementType, Orientation>(_pData, _size, _stride);
    }

    template<typename ElementType, TensorOrientation Orientation>
    inline const VectorReference<ElementType, Orientation> VectorReference<ElementType, Orientation>::GetReference() const
    {
        return VectorReference<ElementType, Orientation>(_pData, _size, _stride);
    }

    template<typename ElementType, TensorOrientation Orientation>
    inline VectorReference<ElementType, Orientation> VectorReference<ElementType, Orientation>::GetSubVector(size_t offset, size_t size, size_t strideMultiplier)
    {
        return VectorReference<ElementType, Orientation>(_pData + offset * _stride, size, strideMultiplier * _stride);
    }

    template<typename ElementType, TensorOrientation Orientation>
    inline const VectorReference<ElementType, Orientation> VectorReference<ElementType, Orientation>::GetSubVector(size_t offset, size_t size, size_t strideMultiplier) const
    {
        return VectorReference<ElementType, Orientation>(_pData + offset * _stride, size, strideMultiplier * _stride);
    }

    template<typename ElementType, TensorOrientation Orientation>
    inline VectorReference<ElementType, FlipOrientation<Orientation>::value> VectorReference<ElementType, Orientation>::Transpose()
    {
        return VectorReference<ElementType, FlipOrientation<Orientation>::value>(_pData, _size, _stride);
    }

    template<typename ElementType, TensorOrientation Orientation>
    inline const VectorReference<ElementType, FlipOrientation<Orientation>::value> VectorReference<ElementType, Orientation>::Transpose() const
    {
        return VectorReference<ElementType, FlipOrientation<Orientation>::value>(_pData, _size, _stride);
    }

    template<typename ElementType, TensorOrientation Orientation>
    bool VectorReference<ElementType, Orientation>::operator==(const VectorReference<ElementType, Orientation>& other) const
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

    //
    // Vector
    //

    template<typename ElementType, TensorOrientation Orientation>
    inline Vector<ElementType, Orientation>::Vector(size_t size) : _data(size), VectorReference<ElementType, Orientation>(nullptr, size, 1)
    {
        _pData = _data.data();
    }

    template<typename ElementType, TensorOrientation Orientation>
    inline Vector<ElementType, Orientation>::Vector(std::initializer_list<ElementType> list) : _data(list.begin(), list.end()), VectorReference<ElementType, Orientation>(nullptr, list.size(), 1)
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

    //
    // TensorOperations
    //
   
    template<typename ElementType>
    ElementType TensorOperations::Dot(const VectorReferenceBase<ElementType>& vector1, const VectorReferenceBase<ElementType>& vector2)
    {
        size_t size1 = vector1.Size();
        size_t size2 = vector2.Size();
        
        // TODO check inputs for equal size

        const ElementType* ptr1 = vector1.GetDataPointer();
        size_t stride1 = vector1.GetStride();
        const ElementType* ptr2 = vector2.GetDataPointer();
        size_t stride2 = vector2.GetStride();

#ifdef USE_BLAS
        return Blas::Dot(size1, ptr1, stride1, ptr2, stride2);
#else
        ElementType result = 0;
        const ElementType* end1 = ptr1 + size1;

        while (ptr1 < end1)
        {
            result += (*ptr1) * (*ptr2);
            ptr1 += stride1;
            ptr2 += stride2;
        }
        return result;
#endif
    }

    template<typename ElementType>
    void TensorOperations::Product(const VectorReference<ElementType, TensorOrientation::rowMajor>& left, const VectorReference<ElementType, TensorOrientation::columnMajor>& right, ElementType& result)
    {
        result = Dot(left, right);
    }
}