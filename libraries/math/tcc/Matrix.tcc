////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Matrix.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Operations.h"

// stl
#include <algorithm> // for std::generate

namespace emll
{
namespace math
{
    //
    // RectangularMatrixBase
    // 

    template<typename ElementType>
    RectangularMatrixBase<ElementType>::RectangularMatrixBase(ElementType * pData, size_t numRows, size_t numColumns, size_t increment) : _pData(pData), _numRows(numRows), _numColumns(numColumns), _increment(increment)
    {}

    template<typename ElementType>
    template<VectorOrientation Orientation>
    VectorReference<ElementType, Orientation> RectangularMatrixBase<ElementType>::ConstructVectorReference(ElementType * pData, size_t size, size_t increment)
    {
        return VectorReference<ElementType, Orientation>(pData, size, increment);
    }

    template<typename ElementType>
    template<VectorOrientation Orientation>
    ConstVectorReference<ElementType, Orientation> RectangularMatrixBase<ElementType>::ConstructConstVectorReference(ElementType * pData, size_t size, size_t increment) const
    {
        return ConstVectorReference<ElementType, Orientation>(pData, size, increment);
    }

    //
    // MatrixBase
    // 

    template<typename ElementType>
    MatrixBase<ElementType, MatrixLayout::rowMajor>::MatrixBase(size_t numRows, size_t numColumns) : RectangularMatrixBase<ElementType>(nullptr, numRows, numColumns, numColumns)
    {}

    template<typename ElementType>
    MatrixBase<ElementType, MatrixLayout::columnMajor>::MatrixBase(size_t numRows, size_t numColumns) : RectangularMatrixBase<ElementType>(nullptr, numRows, numColumns, numRows)
    {}

    //
    // ConstMatrixReference
    // 

    template<typename ElementType, MatrixLayout Layout>
    ElementType ConstMatrixReference<ElementType, Layout>::operator() (size_t rowIndex, size_t columnIndex)  const
    {
        if (rowIndex >= _numRows || columnIndex >= _numColumns)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "(rowIndex, columnIndex) exceeds matrix dimensions.");
        }
        return _pData[rowIndex * _rowIncrement + columnIndex * _columnIncrement];
    }

    template<typename ElementType, MatrixLayout Layout>
    auto ConstMatrixReference<ElementType, Layout>::Transpose() const -> ConstMatrixReference<ElementType, MatrixBase<ElementType, Layout>::_transposeLayout>
    {
        return ConstMatrixReference<ElementType, MatrixBase<ElementType, Layout>::_transposeLayout>(_pData, _numColumns, _numRows, _increment);
    }

    template<typename ElementType, MatrixLayout Layout>
    ConstMatrixReference<ElementType, Layout> ConstMatrixReference<ElementType, Layout>::GetSubMatrix(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns) const
    {
        if (firstRow + numRows > _numRows || firstColumn + numColumns > _numColumns)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "block exceeds matrix dimensions.");
        }
        return ConstMatrixReference<ElementType, Layout>(_pData + firstRow * _rowIncrement + firstColumn * _columnIncrement, numRows, numColumns, _increment);
    }

    template<typename ElementType, MatrixLayout Layout>
    ConstVectorReference<ElementType, VectorOrientation::column> ConstMatrixReference<ElementType, Layout>::GetColumn(size_t index) const
    {
        if (index >= _numColumns)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "column index exceeds matrix dimensions.");
        }
        return RectangularMatrixBase<ElementType>::template ConstructConstVectorReference<VectorOrientation::column>(_pData + index * _columnIncrement, _numRows, _rowIncrement);
    }

    template<typename ElementType, MatrixLayout Layout>
    ConstVectorReference<ElementType, VectorOrientation::row> ConstMatrixReference<ElementType, Layout>::GetRow(size_t index) const
    {
        if (index >= _numRows)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "row index exceeds matrix dimensions.");
        }
        return RectangularMatrixBase<ElementType>::template ConstructConstVectorReference<VectorOrientation::row>(_pData + index * _rowIncrement, _numColumns, _columnIncrement);
    }

    template<typename ElementType, MatrixLayout Layout>
    auto ConstMatrixReference<ElementType, Layout>::GetMajorVector(size_t index) const -> ConstVectorReference<ElementType, MatrixBase<ElementType, Layout>::_intervalOrientation>
    {
        return RectangularMatrixBase<ElementType>::template ConstructConstVectorReference<MatrixBase<ElementType, Layout>::_intervalOrientation>(GetMajorVectorBegin(index), _intervalSize, 1);
    }

    template<typename ElementType, MatrixLayout Layout>
    template<VectorOrientation Orientation>
    ConstVectorReference<ElementType, Orientation> ConstMatrixReference<ElementType, Layout>::GetDiagonal() const
    {
        auto size = std::min(NumColumns(), NumRows());
        return RectangularMatrixBase<ElementType>::template ConstructConstVectorReference<Orientation>(_pData, size, _increment + 1);
    }

    template<typename ElementType, MatrixLayout Layout>
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

    template<typename ElementType, MatrixLayout Layout>
    bool ConstMatrixReference<ElementType, Layout>::operator==(const ConstMatrixReference<ElementType, MatrixBase<ElementType, Layout>::_transposeLayout>& other) const 
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

    template<typename ElementType, MatrixLayout Layout>
    template<MatrixLayout OtherLayout>
    bool ConstMatrixReference<ElementType, Layout>::operator!=(const ConstMatrixReference<ElementType, OtherLayout>& other)
    {
        return !(*this == other);
    }

    template<typename ElementType, MatrixLayout Layout>
    ElementType* ConstMatrixReference<ElementType, Layout>::GetMajorVectorBegin(size_t index) const
    {
        return _pData + index * _increment;
    }

    //
    // MatrixReference
    // 

    template<typename ElementType, MatrixLayout Layout>
    ElementType& MatrixReference<ElementType, Layout>::operator() (size_t rowIndex, size_t columnIndex)
    {
        if (rowIndex >= _numRows || columnIndex >= _numColumns)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "(rowIndex, columnIndex) exceeds matrix dimensions.");
        }
        return _pData[rowIndex * _rowIncrement + columnIndex * _columnIncrement];
    }

    template<typename ElementType, MatrixLayout Layout>
    void MatrixReference<ElementType, Layout>::Fill(ElementType value)
    {
        for (size_t i = 0; i < _numIntervals; ++i)
        {
            auto begin = GetMajorVectorBegin(i);
            std::fill(begin, begin + _intervalSize, value);
        }
    }

    template<typename ElementType, MatrixLayout Layout>
    template<typename GeneratorType>
    void MatrixReference<ElementType, Layout>::Generate(GeneratorType generator)
    {
        for (size_t i = 0; i < _numIntervals; ++i)
        {
            auto begin = GetMajorVectorBegin(i); 
            std::generate(begin, begin + _intervalSize, generator);
        }
    }

    template<typename ElementType, MatrixLayout Layout>
    MatrixReference<ElementType, Layout> MatrixReference<ElementType, Layout>::GetReference()
    {
        return MatrixReference<ElementType, Layout>(_pData, _numRows, _numColumns, _increment);
    }

    template<typename ElementType, MatrixLayout Layout>
    auto MatrixReference<ElementType, Layout>::Transpose() const -> MatrixReference<ElementType, MatrixBase<ElementType, Layout>::_transposeLayout>
    {
        return MatrixReference<ElementType, MatrixBase<ElementType, Layout>::_transposeLayout>(_pData, _numColumns, _numRows, _increment);
    }

    template<typename ElementType, MatrixLayout Layout>
    MatrixReference<ElementType, Layout> MatrixReference<ElementType, Layout>::GetSubMatrix(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns)
    {
        if (firstRow + numRows > _numRows || firstColumn + numColumns > _numColumns)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "block exceeds matrix dimensions.");
        }
        return MatrixReference<ElementType, Layout>(_pData + firstRow * _rowIncrement + firstColumn * _columnIncrement, numRows, numColumns, _increment);
    }

    template<typename ElementType, MatrixLayout Layout>
    VectorReference<ElementType, VectorOrientation::column> MatrixReference<ElementType, Layout>::GetColumn(size_t index)
    {
        if (index >= _numColumns)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "column index exceeds matrix dimensions.");
        }
        return RectangularMatrixBase<ElementType>::template ConstructVectorReference<VectorOrientation::column>(_pData + index * _columnIncrement, _numRows, _rowIncrement);
    }

    template<typename ElementType, MatrixLayout Layout>
    VectorReference<ElementType, VectorOrientation::row> MatrixReference<ElementType, Layout>::GetRow(size_t index)
    {
        if (index >= _numRows)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "row index exceeds matrix dimensions.");
        }
        return RectangularMatrixBase<ElementType>::template ConstructVectorReference<VectorOrientation::row>(_pData + index * _rowIncrement, _numColumns, _columnIncrement);
    }

    template<typename ElementType, MatrixLayout Layout>
    template<VectorOrientation Orientation>
    VectorReference<ElementType, Orientation> MatrixReference<ElementType, Layout>::GetDiagonal()
    {
        auto size = std::min(NumColumns(), NumRows());
        return RectangularMatrixBase<ElementType>::template ConstructVectorReference<Orientation>(_pData, size, _increment + 1);
    }

    template<typename ElementType, MatrixLayout Layout>
    auto MatrixReference<ElementType, Layout>::GetMajorVector(size_t index) -> VectorReference<ElementType, MatrixBase<ElementType, Layout>::_intervalOrientation>
    {
        return RectangularMatrixBase<ElementType>::template ConstructVectorReference<MatrixBase<ElementType, Layout>::_intervalOrientation>(GetMajorVectorBegin(index), _intervalSize, 1);
    }

    //
    // Matrix
    // 

    template<typename ElementType, MatrixLayout Layout>
    Matrix<ElementType, Layout>::Matrix(size_t numRows, size_t numColumns) : MatrixReference<ElementType, Layout>(numRows, numColumns), _data(numRows*numColumns)
    {
        _pData = _data.data();
    }

    template<typename ElementType, MatrixLayout Layout>
    Matrix<ElementType, Layout>::Matrix(std::initializer_list<std::initializer_list<ElementType>> list) : MatrixReference<ElementType, Layout>(list.size(), list.begin()->size()), _data(list.size() * list.begin()->size())
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

    template<typename ElementType, MatrixLayout Layout>
    void Matrix<ElementType, Layout>::Fill(ElementType value)
    {
        std::fill(_data.begin(), _data.end(), value);
    }

    template<typename ElementType, MatrixLayout Layout>
    template<typename GeneratorType>
    void Matrix<ElementType, Layout>::Generate(GeneratorType generator)
    {
        std::generate(_data.begin(), _data.end(), generator);
    }
}
}