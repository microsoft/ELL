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
    template<typename ElementPointerType>
    TensorReferenceBase<ElementPointerType>::TensorReferenceBase(ElementPointerType pData, size_t stride) : _pData(pData), _stride(stride)
    {
        // TODO check inputs
    }

    template<typename ElementType>
    TensorBase<ElementType>::TensorBase(std::vector<ElementType> data) : _data(std::move(data))
    {}

    template<typename ElementType>
    template<class StlIteratorType>
    TensorBase<ElementType>::TensorBase(StlIteratorType begin, StlIteratorType end) : _data(begin, end)
    {}

    template<typename ElementType>
    void TensorBase<ElementType>::Reset() 
    { 
        std::fill(_data.begin(), _data.end(), static_cast<ElementType>(0)); 
    }

    template<typename ElementType>
    void TensorBase<ElementType>::Fill(ElementType value) 
    { 
        std::fill(_data.begin(), _data.end(), value); 
    }

    template<typename ElementType>
    TensorBase<ElementType>::TensorBase(size_t size) : _data(size) 
    {}

    template<typename ElementType>
    void TensorBase<ElementType>::Generate(std::function<ElementType()> generator) 
    { 
        std::generate(_data.begin(), _data.end(), generator); 
    }

    TensorDimensions<1>::TensorDimensions(size_t size) : _size(size)
    {
        // TODO check size>0
    }

    TensorDimensions<2>::TensorDimensions(size_t numRows, size_t numColumns) : _numRows(numRows), _numColumns(numColumns)
    {
        // TODO check inputs
    }

    TensorDimensions<3>::TensorDimensions(size_t size1, size_t size2, size_t size3) : _size1(size1), _size2(size2), _size3(size3)
    {
        // TODO check inputs
    }

    template<typename ElementType, bool ColumnOrientation>
    void TensorReference<ElementType, 1, ColumnOrientation>::Fill(ElementType value)
    {
        ElementType* current = _pData;
        ElementType* end = _pData + _size * _stride;
        while (current < end)
        {
            *current = value;
            current += _stride;
        }
    }

    template<typename ElementType, bool ColumnOrientation>
    ElementType& TensorReference<ElementType, 1, ColumnOrientation>::operator() (size_t index) 
    {
        // TODO check index
        // 
        return _pData[index*_stride]; 
    }

    template<typename ElementType, bool ColumnOrientation>
    ElementType TensorReference<ElementType, 1, ColumnOrientation>::operator() (size_t index) const
    {
        // TODO check index
        // 
        return _pData[index*_stride];
    }

    template<typename ElementType, bool ColumnOrientation>
    ElementType& TensorReference<ElementType, 1, ColumnOrientation>::operator[] (size_t index)
    {
        return operator()(index);
    }

    template<typename ElementType, bool ColumnOrientation>
    ElementType TensorReference<ElementType, 1, ColumnOrientation>::operator[] (size_t index) const
    { 
        return operator()(index);
    }

    template<typename ElementType, bool ColumnOrientation>
    void TensorReference<ElementType, 1, ColumnOrientation>::Reset() 
    { 
        Fill(static_cast<ElementType>(0));
    }

    template<typename ElementType, bool ColumnOrientation>
    void TensorReference<ElementType, 1, ColumnOrientation>::Generate(std::function<ElementType()> generator)
    {
        ElementType* current = _pData;
        ElementType* end = _pData + _size * _stride;
        while (current < end)
        {
            *current = generator();
            current += _stride;
        }
    }

    template<typename ElementType, bool ColumnOrientation>
    TensorReference<ElementType, 1, ColumnOrientation> TensorReference<ElementType, 1, ColumnOrientation>::GetSubVector(size_t offset, size_t size, size_t strideMultiplier)
    {
        // TODO check inputs

        return TensorReference<ElementType, 1, ColumnOrientation>(_pData + offset * _stride, size, strideMultiplier * _stride);
    }

    template<typename ElementType, bool ColumnOrientation>
    TensorReference<ElementType, 1, !ColumnOrientation> TensorReference<ElementType, 1, ColumnOrientation>::Transpose() 
    { 
        return TensorReference<ElementType, 1, !ColumnOrientation>(_pData, _size, _stride);
    }

    template<typename ElementType, bool ColumnOrientation>
    TensorConstReference<ElementType, 1, ColumnOrientation> TensorReference<ElementType, 1, ColumnOrientation>::GetConstReference()
    {
        return TensorConstReference<ElementType, 1, ColumnOrientation>(_pData, _size, _stride);
    }

    template<typename ElementType, bool ColumnOrientation>
    TensorReference<ElementType, 1, ColumnOrientation>::TensorReference(ElementType* pData, size_t size, size_t stride) : TensorReferenceBase<ElementType*>(pData, stride), TensorDimensions<1>(size)
    {
        // TODO check pData != 0
    }

    template<typename ElementType, bool ColumnOrientation>
    ElementType TensorConstReference<ElementType, 1, ColumnOrientation>::operator() (size_t index) const
    {
        // check input
        return _data[index*_stride];
    }

    template<typename ElementType, bool ColumnOrientation>
    ElementType TensorConstReference<ElementType, 1, ColumnOrientation>::operator[] (size_t index) const
    {
        return operator()(index);
    }

    template<typename ElementType, bool ColumnOrientation>
    TensorConstReference<ElementType, 1, ColumnOrientation> TensorConstReference<ElementType, 1, ColumnOrientation>::GetSubVector(size_t offset, size_t size, size_t strideMultiplier)
    {
        // TODO check inputs

        return TensorConstReference<ElementType, 1, ColumnOrientation>(_pData + offset * _stride, size, strideMultiplier * _stride);
    }

    template<typename ElementType, bool ColumnOrientation>
    TensorConstReference<ElementType, 1, !ColumnOrientation> TensorConstReference<ElementType, 1, ColumnOrientation>::Transpose()
    {
        return TensorConstReference<ElementType, 1, !ColumnOrientation>(_pData, _size, _stride);
    }

    template<typename ElementType, bool ColumnOrientation>
    TensorConstReference<ElementType, 1, ColumnOrientation>::TensorConstReference(const ElementType* pData, size_t size, size_t stride) : TensorReferenceBase<const ElementType*>(pData, stride), TensorDimensions<1>(size)
    {
        // TODO check pData != 0
    }

    template<typename ElementType, bool ColumnOrientation>
    Tensor<ElementType, 1, ColumnOrientation>::Tensor(size_t size) : TensorBase<ElementType>(size), TensorDimensions<1>(size)
    {}

    template<typename ElementType, bool ColumnOrientation>
    Tensor<ElementType, 1, ColumnOrientation>::Tensor(std::vector<ElementType> data) : TensorBase<ElementType>(std::move(data)), TensorDimensions<1>(data.size()) 
    {}
    
    template<typename ElementType, bool ColumnOrientation>
    Tensor<ElementType, 1, ColumnOrientation>::Tensor(std::initializer_list<ElementType> list) : TensorBase<ElementType>(list.begin(), list.end()), TensorDimensions<1>(list.size()) 
    {}

    template<typename ElementType, bool ColumnOrientation>
    ElementType& Tensor<ElementType, 1, ColumnOrientation>::operator() (size_t index)
    {
        // TODO check input
        return _data[index];
    }

    template<typename ElementType, bool ColumnOrientation>
    ElementType Tensor<ElementType, 1, ColumnOrientation>::operator() (size_t index) const
    {
        // TODO check input 
        return _data[index];
    }

    template<typename ElementType, bool ColumnOrientation>
    ElementType& Tensor<ElementType, 1, ColumnOrientation>::operator[] (size_t index)
    {
        return operator()(index);
    }

    template<typename ElementType, bool ColumnOrientation>
    ElementType Tensor<ElementType, 1, ColumnOrientation>::operator[] (size_t index) const
    {
        return operator()(index); 
    }

    template<typename ElementType, bool ColumnOrientation>
    TensorReference<ElementType, 1, ColumnOrientation> Tensor<ElementType, 1, ColumnOrientation>::GetReference()
    {
        return TensorReference<ElementType, 1, ColumnOrientation>(_data.data(), _data.size(), 1);
    }

    template<typename ElementType, bool ColumnOrientation>
    TensorConstReference<ElementType, 1, ColumnOrientation> Tensor<ElementType, 1, ColumnOrientation>::GetConstReference() const
    {
        return TensorConstReference<ElementType, 1, ColumnOrientation>(_data.data(), _data.size(), 1);
    }

    template<typename ElementType, bool ColumnOrientation>
    bool Tensor<ElementType, 1, ColumnOrientation>::operator==(const Tensor<ElementType, 1, ColumnOrientation>& other) const
    {
        if (Size() != other.Size())
        {
            return false;
        }

        const ElementType* pThis = GetDataPointer();
        const ElementType* pThisEnd = pThis + Size();
        const ElementType* pOther = other.GetDataPointer();

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

    template<typename ElementType, bool Orientation1, bool Orientation2>
    ElementType TensorOperations::Dot(const Tensor<ElementType, 1, Orientation1>& vector1, const Tensor<ElementType, 1, Orientation2>& vector2)
    {
        size_t size1 = vector1.Size();
        size_t size2 = vector2.Size();

        // TODO check inputs for equal size

        const ElementType* ptr1 = vector1.GetDataPointer();
        const ElementType* ptr2 = vector2.GetDataPointer();

#ifdef USE_BLAS
        return Blas::Dot(size1, ptr1, 1, ptr2, 1);
#else
        ElementType result = 0;
        const ElementType* end1 = ptr1 + size1;

        while (ptr1 < end1)
        {
            result += (*ptr1) * (*ptr2);
            ++ptr1;
            ++ptr2;
        }
        return result;
#endif
    }

    template<typename ElementType, bool Orientation1, bool Orientation2>
    ElementType TensorOperations::Dot(const TensorConstReference<ElementType, 1, Orientation1>& vector1, const TensorConstReference<ElementType, 1, Orientation2>& vector2)
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
    void TensorOperations::Product(const Tensor<ElementType, 1, false>& left, const Tensor<ElementType, 1, true>& right, ElementType & result)
    {
        result = Dot(left, right);
    }

    template<typename ElementType>
    void TensorOperations::Product(const TensorConstReference<ElementType, 1, false>& left, const TensorConstReference<ElementType, 1, true>& right, ElementType & result)
    {
        result = Dot(left, right);
    }
}