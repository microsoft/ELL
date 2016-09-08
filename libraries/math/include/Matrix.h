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
    enum class MatrixLayout { rowMajor, columnMajor };

    /// <summary> Const reference to a rectangular dense matrix. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template <typename ElementType, MatrixLayout Layout>
    class ConstMatrixReference
    {
    public:
        size_t NumRows() const { return _numRows; }
        size_t NumColumns() const { return _numColumns; }

        ElementType operator() (size_t rowIndex, size_t columnIndex) const 
        { 
            return _pData[IndexOf(rowIndex, columnIndex)];
        }

    protected:
        // allow operations defined in the Operations struct to access raw data vector
        ConstMatrixReference(ElementType* pData, size_t numRows, size_t numColumns, size_t rowStride, size_t columnStride) : _pData(pData), _numRows(numRows), _numColumns(numColumns), _rowStride(rowStride), _columnStride(columnStride)
        {}

        size_t IndexOf(size_t rowIndex, size_t columnIndex) const
        {
            // TODO check index < ...
            return rowIndex * _rowStride + columnIndex * _columnStride;
        }

        ElementType* _pData;
        size_t _numRows;
        size_t _numColumns;
        size_t _rowStride; // increment needed to get to the next row
        size_t _columnStride; // increment needed to get to the next column
    };

    /// <summary> Non-const reference to a column major rectangular dense matrix. </summary>
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

        MatrixReference<ElementType, Layout> GetSubMatrix(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns, size_t rowStride, size_t columnStride)
        {
            // TODO check inputs
            return MatrixReference<ElementType, Layout>(_pData + IndexOf(firstRow, firstColumn), numRows, numColumns, _rowStride*rowStride, _columnStride*columnStride);
        }

    protected:
        using ConstMatrixReference<ElementType, Layout>::ConstMatrixReference;
    };

    /// <summary> Forward declaration of A matrix. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="Layout"> Matrix layout. </typeparam>
    template <typename ElementType, MatrixLayout Layout>
    class Matrix;
    
    
    template <typename ElementType>
    class Matrix<ElementType, MatrixLayout::columnMajor> : public MatrixReference<ElementType, MatrixLayout::columnMajor>
    {
    public:
        Matrix(size_t numRows, size_t numColumns) : MatrixReference<ElementType, MatrixLayout::columnMajor>(nullptr, numRows, numColumns, 1, numRows), _data(numRows*numColumns) 
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
    //typedef Matrix<double, MatrixLayout::rowMajor> DoubleRowMatrix;
}

#include "../tcc/Matrix.tcc"
