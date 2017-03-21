////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Matrix.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Exception.h"
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
    // RectangularMatrixBase
    //

    template <typename ElementType>
    RectangularMatrixBase<ElementType>::RectangularMatrixBase(size_t numRows, size_t numColumns, size_t increment, ElementType* pData)
        : _numRows(numRows), _numColumns(numColumns), _increment(increment), _pData(pData)
    {
    }

    template <typename ElementType>
    void RectangularMatrixBase<ElementType>::Swap(RectangularMatrixBase<ElementType>& other)
    {
        std::swap(_pData, other._pData);
        std::swap(_numRows, other._numRows);
        std::swap(_numColumns, other._numColumns);
        std::swap(_increment, other._increment);
    }

    template <typename ElementType>
    template <VectorOrientation Orientation>
    VectorReference<ElementType, Orientation> RectangularMatrixBase<ElementType>::ConstructVectorReference(ElementType* pData, size_t size, size_t increment)
    {
        return VectorReference<ElementType, Orientation>(pData, size, increment);
    }

    template <typename ElementType>
    template <VectorOrientation Orientation>
    ConstVectorReference<ElementType, Orientation> RectangularMatrixBase<ElementType>::ConstructConstVectorReference(ElementType* pData, size_t size, size_t increment) const
    {
        return ConstVectorReference<ElementType, Orientation>(pData, size, increment);
    }

    //
    // MatrixBase
    //

    template <typename ElementType>
    MatrixBase<ElementType, MatrixLayout::rowMajor>::MatrixBase(size_t numRows, size_t numColumns, ElementType* pData)
        : RectangularMatrixBase<ElementType>(numRows, numColumns, numColumns, pData)
    {
    }

    template <typename ElementType>
    MatrixBase<ElementType, MatrixLayout::columnMajor>::MatrixBase(size_t numRows, size_t numColumns, ElementType* pData)
        : RectangularMatrixBase<ElementType>(numRows, numColumns, numRows, pData)
    {
    }

    //
    // ConstMatrixReference
    //

    template <typename ElementType, MatrixLayout Layout>
    ElementType ConstMatrixReference<ElementType, Layout>::operator()(size_t rowIndex, size_t columnIndex) const
    {
        DEBUG_THROW(rowIndex >= _numRows || columnIndex >= _numColumns, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "(rowIndex, columnIndex) exceeds matrix dimensions."));

        return _pData[rowIndex * _rowIncrement + columnIndex * _columnIncrement];
    }

    template <typename ElementType, MatrixLayout Layout>
    auto ConstMatrixReference<ElementType, Layout>::Transpose() const
    {
        return ConstMatrixReference<ElementType, _transposeLayout>(_pData, _numColumns, _numRows, _increment);
    }

    template <typename ElementType, MatrixLayout Layout>
    ConstMatrixReference<ElementType, Layout> ConstMatrixReference<ElementType, Layout>::GetSubMatrix(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns) const
    {
        DEBUG_THROW(firstRow + numRows > _numRows || firstColumn + numColumns > _numColumns, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "block exceeds matrix dimensions."));

        return ConstMatrixReference<ElementType, Layout>(_pData + firstRow * _rowIncrement + firstColumn * _columnIncrement, numRows, numColumns, _increment);
    }

    template <typename ElementType, MatrixLayout Layout>
    ConstVectorReference<ElementType, VectorOrientation::column> ConstMatrixReference<ElementType, Layout>::GetColumn(size_t index) const
    {
        DEBUG_THROW(index >= _numColumns, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "column index exceeds matrix dimensions."));

        return RectangularMatrixBase<ElementType>::template ConstructConstVectorReference<VectorOrientation::column>(_pData + index * _columnIncrement, _numRows, _rowIncrement);
    }

    template <typename ElementType, MatrixLayout Layout>
    ConstVectorReference<ElementType, VectorOrientation::row> ConstMatrixReference<ElementType, Layout>::GetRow(size_t index) const
    {
        DEBUG_THROW(index >= _numRows, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "row index exceeds matrix dimensions."));

        return RectangularMatrixBase<ElementType>::template ConstructConstVectorReference<VectorOrientation::row>(_pData + index * _rowIncrement, _numColumns, _columnIncrement);
    }

    template <typename ElementType, MatrixLayout Layout>
    ConstVectorReference<ElementType, VectorOrientation::column> ConstMatrixReference<ElementType, Layout>::GetDiagonal() const
    {
        auto size = std::min(NumColumns(), NumRows());
        return RectangularMatrixBase<ElementType>::template ConstructConstVectorReference<VectorOrientation::column>(_pData, size, _increment + 1);
    }

    template <typename ElementType, MatrixLayout Layout>
    auto ConstMatrixReference<ElementType, Layout>::GetMajorVector(size_t index) const
    {
        return RectangularMatrixBase<ElementType>::template ConstructConstVectorReference<MatrixBase<ElementType, Layout>::_intervalOrientation>(GetMajorVectorBegin(index), _intervalSize, 1);
    }

    template <typename ElementType, MatrixLayout Layout>
    bool ConstMatrixReference<ElementType, Layout>::operator==(const ConstMatrixReference<ElementType, Layout>& other) const
    {
        if (NumRows() != other.NumRows() || NumColumns() != other.NumColumns())
        {
            return false;
        }

        for (size_t i = 0; i < NumIntervals(); ++i)
        {
            if (GetMajorVector(i) != other.GetMajorVector(i))
            {
                return false;
            }
        }
        return true;
    }

    template <typename ElementType, MatrixLayout Layout>
    bool ConstMatrixReference<ElementType, Layout>::operator==(const ConstMatrixReference<ElementType, _transposeLayout>& other) const
    {
        if (NumRows() != other.NumRows() || NumColumns() != other.NumColumns())
        {
            return false;
        }

        for (size_t i = 0; i < NumRows(); ++i)
        {
            if (GetRow(i) != other.GetRow(i))
            {
                return false;
            }
        }
        return true;
    }
    
    template <typename ElementType, MatrixLayout Layout>
    template <MatrixLayout OtherLayout>
    bool ConstMatrixReference<ElementType, Layout>::operator!=(const ConstMatrixReference<ElementType, OtherLayout>& other)
    {
        return !(*this == other);
    }

    template <typename ElementType, MatrixLayout Layout>
    ElementType* ConstMatrixReference<ElementType, Layout>::GetMajorVectorBegin(size_t index) const
    {
        return _pData + index * _increment;
    }

    //
    // MatrixReference
    //

    template <typename ElementType, MatrixLayout Layout>
    MatrixReference<ElementType, Layout>::MatrixReference(size_t numRows, size_t numColumns, ElementType* pData) :
        ConstMatrixReference<ElementType, Layout>(numRows, numColumns, pData)
    {
    }

    template <typename ElementType, MatrixLayout Layout>
    void MatrixReference<ElementType, Layout>::Swap(MatrixReference<ElementType, Layout>& other)
    {
        RectangularMatrixBase<ElementType>::Swap(other);
    }

    template <typename ElementType, MatrixLayout Layout>
    ElementType& MatrixReference<ElementType, Layout>::operator()(size_t rowIndex, size_t columnIndex)
    {
        DEBUG_THROW(rowIndex >= _numRows || columnIndex >= _numColumns, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "(rowIndex, columnIndex) exceeds matrix dimensions."));

        return _pData[rowIndex * _rowIncrement + columnIndex * _columnIncrement];
    }

    template <typename ElementType, MatrixLayout Layout>
    void MatrixReference<ElementType, Layout>::Fill(ElementType value)
    {
        for (size_t i = 0; i < _numIntervals; ++i)
        {
            auto begin = GetMajorVectorBegin(i);
            std::fill(begin, begin + _intervalSize, value);
        }
    }

    template <typename ElementType, MatrixLayout Layout>
    template <typename GeneratorType>
    void MatrixReference<ElementType, Layout>::Generate(GeneratorType generator)
    {
        for (size_t i = 0; i < _numIntervals; ++i)
        {
            auto begin = GetMajorVectorBegin(i);
            std::generate(begin, begin + _intervalSize, generator);
        }
    }

    template <typename ElementType, MatrixLayout Layout>
    MatrixReference<ElementType, Layout> MatrixReference<ElementType, Layout>::GetReference()
    {
        return MatrixReference<ElementType, Layout>(_numRows, _numColumns, _increment, _pData);
    }

    template <typename ElementType, MatrixLayout Layout>
    ConstMatrixReference<ElementType, Layout> MatrixReference<ElementType, Layout>::GetConstReference() const
    {
        return constMatrixReference<ElementType, Layout>(_numRows, _numColumns, _increment, _pData);
    }

    template <typename ElementType, MatrixLayout Layout>
    auto MatrixReference<ElementType, Layout>::Transpose() const
    {
        return MatrixReference<ElementType, _transposeLayout>(_numColumns, _numRows, _increment, _pData);
    }

    template <typename ElementType, MatrixLayout Layout>
    MatrixReference<ElementType, Layout> MatrixReference<ElementType, Layout>::GetSubMatrix(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns)
    {
        DEBUG_THROW(firstRow + numRows > _numRows || firstColumn + numColumns > _numColumns, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "block exceeds matrix dimensions."));

        return MatrixReference<ElementType, Layout>(numRows, numColumns, _increment, _pData + firstRow * _rowIncrement + firstColumn * _columnIncrement);
    }

    template <typename ElementType, MatrixLayout Layout>
    VectorReference<ElementType, VectorOrientation::column> MatrixReference<ElementType, Layout>::GetColumn(size_t index)
    {
        DEBUG_THROW(index >= _numColumns, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "column index exceeds matrix dimensions."));

        return RectangularMatrixBase<ElementType>::template ConstructVectorReference<VectorOrientation::column>(_pData + index * _columnIncrement, _numRows, _rowIncrement);
    }

    template <typename ElementType, MatrixLayout Layout>
    VectorReference<ElementType, VectorOrientation::row> MatrixReference<ElementType, Layout>::GetRow(size_t index)
    {
        DEBUG_THROW(index >= _numRows, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "row index exceeds matrix dimensions."));

        return RectangularMatrixBase<ElementType>::template ConstructVectorReference<VectorOrientation::row>(_pData + index * _rowIncrement, _numColumns, _columnIncrement);
    }

    template <typename ElementType, MatrixLayout Layout>
    VectorReference<ElementType, VectorOrientation::column> MatrixReference<ElementType, Layout>::GetDiagonal()
    {
        auto size = std::min(NumColumns(), NumRows());
        return RectangularMatrixBase<ElementType>::template ConstructVectorReference<VectorOrientation::column>(_pData, size, _increment + 1);
    }

    //
    // Matrix
    //

    template <typename ElementType, MatrixLayout Layout>
    Matrix<ElementType, Layout>::Matrix(size_t numRows, size_t numColumns)
        : MatrixReference<ElementType, Layout>(numRows, numColumns), _data(numRows * numColumns)
    {
        _pData = _data.data();
    }

    template <typename ElementType, MatrixLayout Layout>
    Matrix<ElementType, Layout>::Matrix(std::initializer_list<std::initializer_list<ElementType>> list)
        : MatrixReference<ElementType, Layout>(list.size(), list.begin()->size()), _data(list.size() * list.begin()->size())
    {
        _pData = _data.data();

        size_t i = 0;
        for (auto rowIter = list.begin(); rowIter < list.end(); ++rowIter)
        {
            // check that the length of the row is the same as NumColumns();

            size_t j = 0;
            for (auto elementIter = rowIter->begin(); elementIter < rowIter->end(); ++elementIter)
            {
                (*this)(i, j) = *elementIter;
                ++j;
            }
            ++i;
        }
    }

    template <typename ElementType, MatrixLayout Layout>
    Matrix<ElementType, Layout>::Matrix(size_t numRows, size_t numColumns, const std::vector<ElementType>& data)
        : MatrixReference<ElementType, Layout>(numRows, numColumns), _data(data)
    {
        _pData = _data.data();
    }

    template <typename ElementType, MatrixLayout Layout>
    Matrix<ElementType, Layout>::Matrix(size_t numRows, size_t numColumns, std::vector<ElementType>&& data)
        : MatrixReference<ElementType, Layout>(numRows, numColumns), _data(std::move(data))
    {
        _pData = _data.data();
    }

    template <typename ElementType, MatrixLayout Layout>
    Matrix<ElementType, Layout>::Matrix(Matrix<ElementType, Layout>&& other)
        : MatrixReference<ElementType, Layout>(other.NumRows(), other.NumColumns()), _data(std::move(other._data))
    {
        _pData = _data.data();
    }

    template <typename ElementType, MatrixLayout Layout>
    Matrix<ElementType, Layout>::Matrix(const Matrix<ElementType, Layout>& other)
        : MatrixReference<ElementType, Layout>(other.NumRows(), other.NumColumns()), _data(other._data)
    {
        _pData = _data.data();
    }

    template <typename ElementType, MatrixLayout Layout>
    Matrix<ElementType, Layout>::Matrix(ConstMatrixReference<ElementType, _transposeLayout> other)
        : MatrixReference<ElementType, Layout>(other.NumRows(), other.NumColumns()), _data(other.NumRows() * other.NumColumns())
    {
        _pData = _data.data();
        for (size_t i = 0; i < _numRows; ++i)
        {
            for (size_t j = 0; j < _numColumns; ++j)
            {
                (*this)(i, j) = other(i, j);
            }
        }
    }

    template <typename ElementType, MatrixLayout Layout>
    Matrix<ElementType, Layout>& Matrix<ElementType, Layout>::operator=(Matrix<ElementType, Layout> other)
    {
        Swap(other);
        return *this;
    }

    template <typename ElementType, MatrixLayout Layout>
    void Matrix<ElementType, Layout>::Swap(Matrix<ElementType, Layout>& other)
    {
        RectangularMatrixBase<ElementType>::Swap(other);
        std::swap(_data, other._data);
    }

    template <typename ElementType, MatrixLayout Layout>
    void Matrix<ElementType, Layout>::Fill(ElementType value)
    {
        std::fill(_data.begin(), _data.end(), value);
    }

    template <typename ElementType, MatrixLayout Layout>
    template <typename GeneratorType>
    void Matrix<ElementType, Layout>::Generate(GeneratorType generator)
    {
        std::generate(_data.begin(), _data.end(), generator);
    }
}
}