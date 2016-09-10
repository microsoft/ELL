////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Matrix.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Vector.h"

namespace math
{
    /// <summary> Enum of possible matrix layouts. </summary>
    enum class MatrixLayout { columnMajor, rowMajor };

    /// <summary> Forward declaration of a base class for matrices, for subsequent specialization according to layout. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="Layout"> Type of the layout. </typeparam>
    template<typename ElementType, MatrixLayout Layout>
    class MatrixBase;

    class VectorConstructor
    {
    protected:
        template<typename ElementType, VectorOrientation Orientation>
        VectorReference<ElementType, Orientation> ConstructVector(ElementType* pData, size_t size, size_t increment)
        {
            return VectorReference<ElementType, Orientation>(pData, size, increment);
        }

        template<typename ElementType, VectorOrientation Orientation>
        ConstVectorReference<ElementType, Orientation> ConstructVector(ElementType* pData, size_t size, size_t increment) const
        {
            return ConstVectorReference<ElementType, Orientation>(pData, size, increment);
        }
    };

    /// <summary> Base class for rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template <typename ElementType>
    class RectangularMatrixBase : public VectorConstructor
    {
    public:
        /// <summary> Gets the number of rows. </summary>
        ///
        /// <returns> The number of rows. </returns>
        size_t NumRows() const { return _numRows; }

        /// <summary> Gets the number of columns. </summary>
        ///
        /// <returns> The number of columns. </returns>
        size_t NumColumns() const { return _numColumns; }

    protected:
        // allow operations defined in the Operations struct to access raw data vector
        RectangularMatrixBase(ElementType* pData, size_t numRows, size_t numColumns) : _pData(pData), _numRows(numRows), _numColumns(numColumns)
        {}

        ElementType* _pData;
        size_t _numRows;
        size_t _numColumns;
    };

    /// <summary> Base class for column major rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template<typename ElementType>
    class MatrixBase<ElementType, MatrixLayout::columnMajor> : public RectangularMatrixBase<ElementType>
    {
    public:
        static constexpr MatrixLayout transposeLayout = MatrixLayout::rowMajor;

    protected:
        MatrixBase(size_t numRows, size_t numColumns) : RectangularMatrixBase<ElementType>(nullptr, numRows, numColumns), _columnIncrement(numRows)
        {}

        MatrixBase(ElementType* pData, size_t numRows, size_t numColumns, size_t increment) : RectangularMatrixBase<ElementType>(pData, numRows, numColumns), _columnIncrement(increment)
        {}

        static constexpr MatrixLayout GetLayout() 
        {
            return MatrixLayout::columnMajor;
        }

        size_t GetIncrement() const
        {
            return _columnIncrement;
        }

        size_t GetRowIncrement() const
        {
            return 1;
        }

        size_t GetColumnIncrement() const
        {
            return _columnIncrement;
        }

    private:
        size_t _columnIncrement;
    };

    /// <summary> Base class for row major rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template<typename ElementType>
    class MatrixBase<ElementType, MatrixLayout::rowMajor> : public RectangularMatrixBase<ElementType>
    {
    public:
        static constexpr MatrixLayout transposeLayout = MatrixLayout::columnMajor;

    protected:
        MatrixBase(size_t numRows, size_t numColumns) : RectangularMatrixBase<ElementType>(nullptr, numRows, numColumns), _rowIncrement(numColumns)
        {}

        MatrixBase(ElementType* pData, size_t numRows, size_t numColumns, size_t increment) : RectangularMatrixBase<ElementType>(pData, numRows, numColumns), _rowIncrement(increment)
        {}

        static constexpr MatrixLayout GetLayout()
        {
            return MatrixLayout::rowMajor;
        }


        size_t GetIncrement() const
        {
            return _rowIncrement;
        }

        size_t GetRowIncrement() const
        {
            return _rowIncrement;
        }

        size_t GetColumnIncrement() const
        {
            return 1;
        }

    private:
        size_t _rowIncrement;
    };

    /// <summary> Const reference to a dense matrix. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="Layout"> Matrix layout. </typeparam>
    template <typename ElementType, MatrixLayout Layout>
    class ConstMatrixReference : public MatrixBase<ElementType, Layout>
    {
    public:

        /// <summary> Matrix element access operator. </summary>
        ///
        /// <returns> A copy of the element in a given position. </returns>
        ElementType operator() (size_t rowIndex, size_t columnIndex)  const
        { 
            return _pData[rowIndex * GetRowIncrement() + columnIndex * GetColumnIncrement()];
        }

        auto Transpose() const -> ConstMatrixReference<ElementType, MatrixBase<ElementType, Layout>::transposeLayout>
        {
            // TODO check inputs
            return ConstMatrixReference<ElementType, MatrixBase<ElementType, Layout>::transposeLayout>(_pData, _numColumns, _numRows, GetIncrement());
        }




        /// <summary> Gets a constant reference to a block-shaped sub-matrix. </summary>
        ///
        /// <param name="firstRow"> The first row in the block. </param>
        /// <param name="firstColumn"> The first column in the block. </param>
        /// <param name="numRows"> Number of rows in the block. </param>
        /// <param name="numColumns"> Number of columns in the block. </param>
        ///
        /// <returns> The constant reference to a block. </returns>
        ConstMatrixReference<ElementType, Layout> GetBlock(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns) const
        {
            // TODO check inputs
            return ConstMatrixReference<ElementType, Layout>(_pData + firstRow * GetRowIncrement() + firstColumn * GetColumnIncrement(), numRows, numColumns, GetIncrement());
        }

        ConstVectorReference<ElementType, VectorOrientation::column> GetColumn(size_t index) const
        {
            return ConstructVector<ElementType, VectorOrientation::column>(_pData + index * GetColumnIncrement(), NumRows(), GetRowIncrement());
        }

        ConstVectorReference<ElementType, VectorOrientation::row> GetRow(size_t index) const
        {
            return ConstructVector<ElementType, VectorOrientation::row>(_pData + index * GetRowIncrement(), NumColumns(), GetColumnIncrement());
        }

        template<VectorOrientation Orientation>
        ConstVectorReference<ElementType, Orientation> GetDiagonal() const
        {
            auto size = std::min(NumColumns(), NumRows());
            return <ElementType, Orientation>(_pData, size, GetIncrement()+1);
        }

        template<MatrixLayout OtherLayout>
        bool operator==(const ConstMatrixReference<ElementType, OtherLayout>& other) const // TODO, compare matrices with different layout
        {
            if(NumRows() != other.NumRows() || NumColumns() != other.NumColumns())
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

        template<MatrixLayout OtherLayout>
        bool operator !=(const ConstMatrixReference<ElementType, OtherLayout>& other)
        {
            return !(*this == other);
        }

    protected:
        friend ConstMatrixReference<ElementType, MatrixBase<ElementType, Layout>::transposeLayout>;
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
        /// <summary> Matrix element access operator. </summary>
        ///
        /// <returns> A reference to an element in a given position. </returns>
        ElementType& operator() (size_t rowIndex, size_t columnIndex)  
        { 
            return _pData[rowIndex * GetRowIncrement() + columnIndex * GetColumnIncrement()];
        }

        auto Transpose() const -> MatrixReference<ElementType, MatrixBase<ElementType, Layout>::transposeLayout>
        {
            // TODO check inputs
            return MatrixReference<ElementType, MatrixBase<ElementType, Layout>::transposeLayout>(_pData, _numColumns, _numRows, GetIncrement());
        }

        /// <summary> Gets a constant reference to a block-shaped sub-matrix. </summary>
        ///
        /// <param name="firstRow"> The first row in the block. </param>
        /// <param name="firstColumn"> The first column in the block. </param>
        /// <param name="numRows"> Number of rows in the block. </param>
        /// <param name="numColumns"> Number of columns in the block. </param>
        ///
        /// <returns> The constant reference to a block. </returns>
        MatrixReference<ElementType, Layout> GetBlock(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns)
        {
            // TODO check inputs
            return MatrixReference<ElementType, Layout>(_pData + firstRow * GetRowIncrement() + firstColumn * GetColumnIncrement(), numRows, numColumns, GetIncrement());
        }

        VectorReference<ElementType, VectorOrientation::column> GetColumn(size_t index) 
        {
            return ConstructVector<ElementType, VectorOrientation::column>(_pData + index * GetColumnIncrement(), NumRows(), GetRowIncrement());
        }

        VectorReference<ElementType, VectorOrientation::row> GetRow(size_t index)
        {
            return ConstructVector<ElementType, VectorOrientation::row>(_pData + index * GetRowIncrement(), NumColumns(), GetColumnIncrement());
        }

        template<VectorOrientation Orientation>
        VectorReference<ElementType, Orientation> GetDiagonal() 
        {
            auto size = std::min(NumColumns(), NumRows());
            return ConstructVector<ElementType, Orientation>(_pData, size, GetIncrement() + 1);
        }

    protected:
        friend MatrixReference<ElementType, MatrixBase<ElementType, Layout>::transposeLayout>;
        using ConstMatrixReference<ElementType, Layout>::ConstMatrixReference;
    };


    /// <summary> A matrix. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="Layout"> Matrix layout. </typeparam>
    template <typename ElementType, MatrixLayout Layout>
    class Matrix : public MatrixReference<ElementType, Layout>
    {
    public:

        /// <summary> Constructs an all-zeros matrix of a given size. </summary>
        ///
        /// <param name="numRows"> Number of rows in the matrix. </param>
        /// <param name="numColumns"> Number of columns in the matrix. </param>
        Matrix(size_t numRows, size_t numColumns) : MatrixReference<ElementType, Layout>(numRows, numColumns), _data(numRows*numColumns) 
        {
            _pData = _data.data();
        }

        Matrix(std::initializer_list<std::initializer_list<ElementType>> list) : MatrixReference<ElementType, Layout>(list.size(), list.begin()->size()), _data(list.size() * list.begin()->size())
        {
            _pData = _data.data();
           
            size_t i = 0;
            for(auto rowIter = list.begin(); rowIter < list.end(); ++rowIter)
            {
                // check that the length of the row is the same as NumColumns();

                size_t j = 0;
                for(auto elementIter = rowIter->begin(); elementIter < rowIter->end(); ++elementIter)
                {
                    (*this)(i, j) = *elementIter;
                    ++j;
                }
                ++i;
            }
        }

    private:
        std::vector<ElementType> _data;
    };

    //
    // friendly names
    // 
    template<typename ElementType>
    using ColumnMatrix = Matrix<ElementType, MatrixLayout::columnMajor>;

    template<typename ElementType>
    using RowMatrix = Matrix<ElementType, MatrixLayout::rowMajor>;
}

#include "../tcc/Matrix.tcc"
