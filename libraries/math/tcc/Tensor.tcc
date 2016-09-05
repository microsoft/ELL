////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Tensor.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Exception.h"

namespace math
{
    template<typename ElementPointerType>
    TensorReferenceBase<ElementPointerType>::TensorReferenceBase(ElementPointerType pData) : _pData(pData)
    {
        // TODO check inputs
    }

    template<typename ElementType>
    void TensorBase<ElementType>::Reset() 
    { 
        std::fill(_data.begin(), _data.end(), 0); 
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
        ElementType* end = _pData + _size;
        while (current < end)
        {
            *current = value;
            current += _stride;
        }
    }

    template<typename ElementType, bool ColumnOrientation>
    void TensorReference<ElementType, 1, ColumnOrientation>::Reset() 
    { 
        Fill(0);
    }

    template<typename ElementType, bool ColumnOrientation>
    void TensorReference<ElementType, 1, ColumnOrientation>::Generate(std::function<ElementType()> generator)
    {
        ElementType* current = _pData;
        ElementType* end = _pData + _size;
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
    TensorReference<ElementType, 1, ColumnOrientation>::TensorReference(ElementType* pData, size_t size, size_t stride) : TensorReferenceBase<ElementType*>(pData), TensorDimensions<1>(size), _stride(stride)
    {
        // TODO check pData != 0
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
    TensorConstReference<ElementType, 1, ColumnOrientation>::TensorConstReference(const ElementType* pData, size_t size, size_t stride) : TensorReferenceBase<const ElementType*>(pData), TensorDimensions<1>(size), _stride(stride)
    {
        // TODO check pData != 0
    }

    template<typename ElementType, bool ColumnOrientation>
    Tensor<ElementType, 1, ColumnOrientation>::Tensor(size_t size) : TensorBase<ElementType>(size), TensorDimensions<1>(size)
    {}

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

    template<typename ElementType, bool LeftOrientation, bool RightOrientation>
    ElementType TensorOperations::Dot(const Tensor<ElementType, 1, LeftOrientation>& left, const Tensor<ElementType, 1, RightOrientation>& right)
    {
        // TODO check inputs for equal size

        ElementType result = 0;
        const ElementType* pLeft = left.GetDataPointer();
        const ElementType* pLeftEnd = pLeft + left.Size();
        const ElementType* pRight = right.GetDataPointer();

        while (pLeft < pLeftEnd)
        {
            result += (*pLeft) * (*pRight);
            ++pLeft;
            ++pRight;
        }

        return result;
    }

    template<typename ElementType, bool LeftOrientation, bool RightOrientation>
    ElementType TensorOperations::Dot(const TensorConstReference<ElementType, 1, LeftOrientation>& left, const TensorConstReference<ElementType, 1, RightOrientation>& right)
    {
        // TODO check inputs for equal size

        ElementType result = 0;
        const ElementType* pLeft = left.GetDataPointer();
        const ElementType* pLeftEnd = pLeft + left.Size();
        const ElementType* pRight = right.GetDataPointer();
        size_t leftStride = left._stride;
        size_t rightStride = left._stride;

        while (pLeft < pLeftEnd)
        {
            result += (*pLeft) * (*pRight);
            pLeft += leftStride;
            pRight += rightStride;
        }

        return result;
    }

    template<typename ElementType>
    void TensorOperations::Product(const Tensor<ElementType, 1, false>& left, const Tensor<ElementType, 1, true>& right, ElementType & result)
    {
        result = return Dot(left, right);
    }

    template<typename ElementType>
    void TensorOperations::Product(const TensorConstReference<ElementType, 1, false>& left, const TensorConstReference<ElementType, 1, true>& right, ElementType & result)
    {
        result = Dot(left, right);
    }
}