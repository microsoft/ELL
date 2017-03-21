////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Tensor.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Operations.h"

// stl
#include <algorithm> // for std::generate

namespace ell
{
namespace math
{
    //
    // TensorBase
    // 

    template<typename ElementType>
    TensorBase<ElementType>::TensorBase(size_t numRows, size_t numColumns, size_t numChannels, TensorLayout layout, ElementType* pData)
        : _numRows(numRows), _numColumns(numColumns), _numChannels(numChannels), _layout(layout), _pData(pData)
    {}

    //
    // TensorLayoutBase
    //

    template<typename ElementType>
    TensorLayoutBase<ElementType, Dimension::column, Dimension::row, Dimension::channel>::TensorLayoutBase(size_t numRows, size_t numColumns, size_t numChannels)
        : TensorBase(numRows, numColumns, numChannels, { numColumns, numRows, numChannels, numColumns, numColumns*numRows })
    {}

    template<typename ElementType>
    TensorLayoutBase<ElementType, Dimension::channel, Dimension::column, Dimension::row>::TensorLayoutBase(size_t numRows, size_t numColumns, size_t numChannels)
        : TensorBase(numRows, numColumns, numChannels, { numChannels, numColumns, numRows, numChannels, numChannels*numColumns })
    {}

    //
    // ConstTensorReference
    // 

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ElementType ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::operator()(size_t row, size_t column, size_t channel) const
    {
        DEBUG_THROW(row >= _numRows || column >= _numColumns || channel >= _numChannels, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

        return _pData[row * _rowIncrement + column * _columnIncrement + channel * _channelIncrement];
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstMatrixReference<ElementType, MatrixLayout::rowMajor> ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::FlattenFirstDimension() const
    {
        return ConstMatrixReference<ElementType, MatrixLayout::rowMajor>(_layout.size2, _layout.size0 * _layout.size1, _layout.increment2, _pData);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template<Dimension rowDimension, Dimension columnDimension>
    auto ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::GetSlice(size_t index) const
    {
        return ConstTensorSlicer<rowDimension, columnDimension>::GetSlice(_layout, index, _pData);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstTensorReference<ElementType, dimension0, dimension1, dimension2> ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::GetSubTensor(size_t firstRow, size_t firstColumn, size_t firstChannel, size_t numRows, size_t numColumns, size_t numChannels) const
    {
        DEBUG_THROW(firstRow+numRows >= _numRows || firstColumn+numColumns >= _numColumns || firstChannel+numChannels >= _numChannels, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "subtensor exceeds tensor dimensions."));

        size_t offset = firstRow * _rowIncrement + firstColumn * _columnIncrement + firstChannel * _channelIncrement;
        return ConstTensorReference(numRows,
            numColumns,
            numChannels,
            _layout,
            _pData + offset);
    }

    //
    // TensorReference
    // 

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    TensorReference<ElementType, dimension0, dimension1, dimension2>::TensorReference(size_t numRows, size_t numColumns, size_t numChannels) 
        : ConstTensorRef(numRows, numColumns, numChannels)
    {}

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ElementType& TensorReference<ElementType, dimension0, dimension1, dimension2>::operator()(size_t row, size_t column, size_t channel)
    {
        DEBUG_THROW(row >= _numRows || column >= _numColumns || channel >= _numChannels, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

        return _pData[row * _rowIncrement + column * _columnIncrement + channel * _channelIncrement];
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    MatrixReference<ElementType, MatrixLayout::rowMajor> TensorReference<ElementType, dimension0, dimension1, dimension2>::FlattenFirstDimension()
    {
        return MatrixReference<ElementType, MatrixLayout::rowMajor>(_layout.size2, _layout.size0 * _layout.size1, _layout.increment2, _pData);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template<Dimension rowDimension, Dimension columnDimension>
    auto TensorReference<ElementType, dimension0, dimension1, dimension2>::GetSlice(size_t index)
    {
        return TensorSlicer<rowDimension, columnDimension>::GetSlice(_layout, index, _pData);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    TensorReference<ElementType, dimension0, dimension1, dimension2> TensorReference<ElementType, dimension0, dimension1, dimension2>::GetSubTensor(size_t firstRow, size_t firstColumn, size_t firstChannel, size_t numRows, size_t numColumns, size_t numChannels)
    {
        DEBUG_THROW(firstRow + numRows >= _numRows || firstColumn + numColumns >= _numColumns || firstChannel + numChannels >= _numChannels, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "subtensor exceeds tensor dimensions."));

        size_t offset = firstRow * _rowIncrement + firstColumn * _columnIncrement + firstChannel * _channelIncrement;
        return TensorReference(numRows,
            numColumns,
            numChannels,
            _layout,
            _pData + offset);
    }

    //
    // Tensor
    // 

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(size_t numRows, size_t numColumns, size_t numChannels) 
        : TensorRef(numRows, numColumns, numChannels), _data(numRows*numColumns*numChannels)
    {
        _pData = _data.data();
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(const Tensor<ElementType, dimension0, dimension1, dimension2>& other) 
        : TensorRef(other), _data(other._data)
    {
        _pData = _data.data();
    }
    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template<Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2> other)
        : TensorRef(other.NumRows(), other.NumColumns(), other.NumChannels()), _data(other.NumRows() * other.NumColumns() * other.NumChannels())
    {
        _pData = _data.data();
        for (size_t i = 0; i < _numRows; ++i)
        {
            for (size_t j = 0; j < _numColumns; ++j)
            {
                for (size_t k = 0; k < _numChannels; ++k)
                {
                    (*this)(i, j, k) = other(i, j, k);
                }
            }
        }
    }
}
}