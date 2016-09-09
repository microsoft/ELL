////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Matrix.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace math
{
    /// <summary> Enum of possible matrix layouts. </summary>
    enum class MatrixLayout { columnMajor,  rowMajor };

    /// <summary> Forward declaration of a base class for matrices, for subsequent specialization. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="Layout"> Type of the layout. </typeparam>
    template<typename ElementType, MatrixLayout Layout>
    class MatrixBase;

    /// <summary> Base class for rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template <typename ElementType>
    class RectangularMatrixBase
    {
    public:
        size_t NumRows() const { return _numRows; }
        size_t NumColumns() const { return _numColumns; }

    protected:
        // allow operations defined in the Operations struct to access raw data vector
        RectangularMatrixBase(ElementType* pData, size_t numRows, size_t numColumns, size_t increment) : _pData(pData), _numRows(numRows), _numColumns(numColumns), _increment(increment)
        {}

        ElementType* _pData;
        size_t _numRows;
        size_t _numColumns;
        size_t _increment; // Increment needed to get to the next row/column
    };

    /// <summary> Base class for column major rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template<typename ElementType>
    class MatrixBase<ElementType, MatrixLayout::columnMajor> : public RectangularMatrixBase<ElementType>
    {
    protected:
        using RectangularMatrixBase<ElementType>::RectangularMatrixBase;
        MatrixBase(size_t numRows, size_t numColumns) : RectangularMatrixBase<ElementType>(nullptr, numRows, numColumns, numRows)
        {}

        size_t IndexOf(size_t rowIndex, size_t columnIndex) const
        {
            // TODO check indices
            return rowIndex + columnIndex * _increment;
        }

    };

    /// <summary> Base class for row major rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template<typename ElementType>
    class MatrixBase<ElementType, MatrixLayout::rowMajor> : public RectangularMatrixBase<ElementType>
    {
    protected:
        using RectangularMatrixBase<ElementType>::RectangularMatrixBase;
        MatrixBase(size_t numRows, size_t numColumns) : RectangularMatrixBase<ElementType>(nullptr, numRows, numColumns, numColumns)
        {}

        size_t IndexOf(size_t rowIndex, size_t columnIndex) const
        {
            // TODO check indices
            return rowIndex * _increment + columnIndex;
        }

    };

    /// <summary> Const reference to a dense matrix. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="Layout"> Matrix layout. </typeparam>
    template <typename ElementType, MatrixLayout Layout>
    class ConstMatrixReference : public MatrixBase<ElementType, Layout>
    {
    public:
        ElementType operator() (size_t rowIndex, size_t columnIndex)  const
        { 
            return _pData[IndexOf(rowIndex, columnIndex)];
        }

        ConstMatrixReference<ElementType, Layout> GetBlock(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns) const
        {
            // TODO check inputs
            return ConstMatrixReference<ElementType, Layout>(_pData + IndexOf(firstRow, firstColumn), numRows, numColumns, _increment);
        }

    protected:
        using MatrixBase<ElementType, Layout>::MatrixBase;

    };

    /// <summary> Non-const reference to a dense matrix. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="Layout"> Matrix layout. </typeparam>
    template <typename ElementType, MatrixLayout Layout>
    class MatrixReference : public ConstMatrixReference<ElementType, Layout>
    {
    public:
        ElementType& operator() (size_t rowIndex, size_t columnIndex)  
        { 
            return _pData[IndexOf(rowIndex, columnIndex)];
        }

        MatrixReference<ElementType, Layout> GetBlock(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns)
        {
            // TODO check inputs
            return MatrixReference<ElementType, Layout>(_pData + IndexOf(firstRow, firstColumn), numRows, numColumns, _increment);
        }


    protected:
        using ConstMatrixReference<ElementType, Layout>::ConstMatrixReference;
    };


    /// <summary> Forward declaration of A matrix. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="Layout"> Matrix layout. </typeparam>
    template <typename ElementType, MatrixLayout Layout>
    class Matrix : public MatrixReference<ElementType, Layout>
    {
    public:
        Matrix(size_t numRows, size_t numColumns) : MatrixReference<ElementType, Layout>(numRows, numColumns), _data(numRows*numColumns) 
        {
            _pData = _data.data();
        }

    private:
        std::vector<ElementType> _data;
    };

    //
    // friendly names
    // 
    typedef Matrix<double, MatrixLayout::columnMajor> DoubleColumnMatrix;
    typedef Matrix<double, MatrixLayout::rowMajor> DoubleRowMatrix;
}

#include "../tcc/Matrix.tcc"
