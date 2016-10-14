////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:    Matrix.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Vector.h"

namespace emll
{
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

    /// <summary> Base class for rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template <typename ElementType>
    class RectangularMatrixBase 
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
        // protected ctor accessible only through derived classes
        RectangularMatrixBase(ElementType* pData, size_t numRows, size_t numColumns, size_t increment);

        template<VectorOrientation Orientation>
        VectorReference<ElementType, Orientation> ConstructVectorReference(ElementType* pData, size_t size, size_t increment);

        template<VectorOrientation Orientation>
        ConstVectorReference<ElementType, Orientation> ConstructConstVectorReference(ElementType* pData, size_t size, size_t increment) const;

        ElementType* _pData;
        size_t _numRows;
        size_t _numColumns;
        size_t _increment;
    };

    /// <summary> Base class for column major rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template<typename ElementType>
    class MatrixBase<ElementType, MatrixLayout::columnMajor> : public RectangularMatrixBase<ElementType>
    {
    protected:
        using RectangularMatrixBase<ElementType>::RectangularMatrixBase;
        MatrixBase(size_t numRows, size_t numColumns);

        using RectangularMatrixBase<ElementType>::_pData;
        using RectangularMatrixBase<ElementType>::_numRows;
        using RectangularMatrixBase<ElementType>::_numColumns;
        using RectangularMatrixBase<ElementType>::_increment;
        static constexpr MatrixLayout _transposeLayout = MatrixLayout::rowMajor;
        static constexpr VectorOrientation _intervalOrientation = VectorOrientation::column;
        const size_t _numIntervals = _numColumns;
        const size_t _intervalSize = _numRows;
        static constexpr size_t _rowIncrement = 1;
        const size_t _columnIncrement = _increment;
    };

    /// <summary> Base class for row major rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template<typename ElementType>
    class MatrixBase<ElementType, MatrixLayout::rowMajor> : public RectangularMatrixBase<ElementType>
    {
    protected:
        using RectangularMatrixBase<ElementType>::RectangularMatrixBase;
        MatrixBase(size_t numRows, size_t numColumns);

        using RectangularMatrixBase<ElementType>::_pData;
        using RectangularMatrixBase<ElementType>::_numRows;
        using RectangularMatrixBase<ElementType>::_numColumns;
        using RectangularMatrixBase<ElementType>::_increment;
        static constexpr MatrixLayout _transposeLayout = MatrixLayout::columnMajor;
        static constexpr VectorOrientation _intervalOrientation = VectorOrientation::row;
        const size_t _numIntervals = _numRows;
        const size_t _intervalSize = _numColumns;
        const size_t _rowIncrement = _increment;
        static constexpr size_t _columnIncrement = 1;
    };

    /// <summary> Const reference to a dense matrix. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="Layout"> Matrix layout. </typeparam>
    template <typename ElementType, MatrixLayout Layout>
    class ConstMatrixReference : public MatrixBase<ElementType, Layout>
    {
    public:
        /// <summary> Gets a const pointer to the underlying data storage. </summary>
        ///
        /// <returns> Const pointer to the data. </returns>
        const ElementType* GetDataPointer() const { return _pData; }

        /// <summary> Gets the increment used in the underlying data storage. </summary>
        ///
        /// <returns> The increment. </returns>
        size_t GetIncrement() const { return _increment; }

        /// <summary> Gets the number of columns of a column major matrix or rows of a row major matrix. </summary>
        ///
        /// <returns> The number of columns. </returns>
        size_t NumIntervals() const { return _numIntervals; }

        /// <summary> Matrix element access operator. </summary>
        ///
        /// <returns> A copy of the element in a given position. </returns>
        ElementType operator() (size_t rowIndex, size_t columnIndex)  const;

        /// <summary> Gets the matrix layout. </summary>
        ///
        /// <returns> The matrix layout. </returns>
        MatrixLayout GetLayout() const { return Layout; }

        /// <summary> Gets a reference to the matrix transpose. </summary>
        ///
        /// <returns> A reference to the matrix transpose. </returns>
        auto Transpose() const -> ConstMatrixReference<ElementType, MatrixBase<ElementType, Layout>::_transposeLayout>
        {
            return ConstMatrixReference<ElementType, MatrixBase<ElementType, Layout>::_transposeLayout>(_pData, _numColumns, _numRows, _increment);
        }

        /// <summary> Gets a constant reference to a block-shaped sub-matrix. </summary>
        ///
        /// <param name="firstRow"> The first row in the block. </param>
        /// <param name="firstColumn"> The first column in the block. </param>
        /// <param name="numRows"> Number of rows in the block. </param>
        /// <param name="numColumns"> Number of columns in the block. </param>
        ///
        /// <returns> The constant reference to a block. </returns>
        ConstMatrixReference<ElementType, Layout> GetSubMatrix(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns) const;

        /// <summary> Gets a const reference to a column of the matrix. </summary>
        ///
        /// <param name="index"> Zero-based column index. </param>
        ///
        /// <returns> A const reference to the matrix column. </returns>
        ConstVectorReference<ElementType, VectorOrientation::column> GetColumn(size_t index) const;

        /// <summary> Gets a const reference to a row of the matrix. </summary>
        ///
        /// <param name="index"> Zero-based row index. </param>
        ///
        /// <returns> A const reference to the matrix row. </returns>
        ConstVectorReference<ElementType, VectorOrientation::row> GetRow(size_t index) const;

        /// <summary> Gets a const reference to the matrix diagonal as a column vector. </summary>
        ///
        /// <returns> A const reference to the matrix diagnoal. </returns>
        ConstVectorReference<ElementType, VectorOrientation::column> GetDiagonal() const;

        /// <summary> Gets a constant reference to a row of a row major matrix or to a column of a column major matrix. </summary>
        ///
        /// <param name="index"> The interval index. </param>
        ///
        /// <returns> Constant reference to the interval. </returns>
        auto GetMajorVector(size_t index) const->ConstVectorReference<ElementType, MatrixBase<ElementType, Layout>::_intervalOrientation>
        {
            return RectangularMatrixBase<ElementType>::template ConstructConstVectorReference<MatrixBase<ElementType, Layout>::_intervalOrientation>(GetMajorVectorBegin(index), _intervalSize, 1);
        }

        /// <summary> Equality operator for matrices with the same layout. </summary>
        ///
        /// <param name="other"> The other matrix. </param>
        ///
        /// <returns> true if the two matrices are equivalent. </returns>
        bool operator==(const ConstMatrixReference<ElementType, Layout>& other) const;

        /// <summary> Equality operator for matrices with opposite layout. </summary>
        ///
        /// <param name="other"> The other matrix. </param>
        ///
        /// <returns> true if the two matrices are equivalent. </returns>
        bool operator==(const ConstMatrixReference<ElementType, MatrixBase<ElementType, Layout>::_transposeLayout>& other) const
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

        /// <summary> Inequality operator. </summary>
        ///
        /// <typeparam name="OtherLayout"> The layout of the other matrix. </typeparam>
        /// <param name="other"> The other matrix. </param>
        ///
        /// <returns> true if the two matrices are not equivalent. </returns>
        template<MatrixLayout OtherLayout>
        bool operator !=(const ConstMatrixReference<ElementType, OtherLayout>& other);

        using RectangularMatrixBase<ElementType>::NumRows;
        using RectangularMatrixBase<ElementType>::NumColumns;

    protected:
        friend class ConstMatrixReference<ElementType, MatrixBase<ElementType, Layout>::_transposeLayout>;
        using MatrixBase<ElementType, Layout>::MatrixBase;

        ElementType* GetMajorVectorBegin(size_t index) const;


        using RectangularMatrixBase<ElementType>::_pData;
        using RectangularMatrixBase<ElementType>::_numRows;
        using RectangularMatrixBase<ElementType>::_numColumns;
        using RectangularMatrixBase<ElementType>::_increment;

        using MatrixBase<ElementType, Layout>::_numIntervals;
        using MatrixBase<ElementType, Layout>::_intervalSize;
        using MatrixBase<ElementType, Layout>::_rowIncrement;
        using MatrixBase<ElementType, Layout>::_columnIncrement;
    };

    /// <summary> Non-const reference to a dense matrix. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="Layout"> Matrix layout. </typeparam>
    template <typename ElementType, MatrixLayout Layout>
    class MatrixReference : public ConstMatrixReference<ElementType, Layout>
    {
    public:
        /// <summary> Gets a pointer to the underlying data storage. </summary>
        ///
        /// <returns> Pointer to the data. </returns>
        ElementType* GetDataPointer() const { return _pData; }

        /// <summary> Matrix element access operator. </summary>
        ///
        /// <returns> A reference to an element in a given position. </returns>
        ElementType& operator() (size_t rowIndex, size_t columnIndex);

        /// <summary> Sets all matrix elements to zero. </summary>
        void Reset() { Fill(0); }

        /// <summary> Sets all matrix elements to a given value. </summary>
        ///
        /// <param name="value"> The value. </param>
        void Fill(ElementType value);

        /// <summary>
        /// Generates elements of the vector by repeatedly calling a generator function (such as a random
        /// number generator).
        /// </summary>
        ///
        /// <typeparam name="GeneratorType"> Type of lambda or functor to use as a generator. </typeparam>
        /// <param name="generator"> The generator function. </param>
        template<typename GeneratorType>
        void Generate(GeneratorType generator);

        /// <summary> Gets a reference to this matrix. </summary>
        ///
        /// <returns> A reference to this matrix. </returns>
        MatrixReference<ElementType, Layout> GetReference();

        ///// <summary> Gets a const reference to this matrix. </summary>
        /////
        ///// <returns> A const reference to this matrix. </returns>
        //ConstMatrixReference<ElementType, Layout> GetConstReference() const;

        /// <summary> Gets a reference to the matrix transpose. </summary>
        ///
        /// <returns> A reference to the matrix transpose. </returns>
        auto Transpose() const->MatrixReference<ElementType, MatrixBase<ElementType, Layout>::_transposeLayout>
        {
            return MatrixReference<ElementType, MatrixBase<ElementType, Layout>::_transposeLayout>(_pData, _numColumns, _numRows, _increment);
        }

        /// <summary> Gets a const reference to a block-shaped sub-matrix. </summary>
        ///
        /// <param name="firstRow"> The first row in the block. </param>
        /// <param name="firstColumn"> The first column in the block. </param>
        /// <param name="numRows"> Number of rows in the block. </param>
        /// <param name="numColumns"> Number of columns in the block. </param>
        ///
        /// <returns> The constant reference to a block. </returns>
        MatrixReference<ElementType, Layout> GetSubMatrix(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns);

        /// <summary> Gets a reference to a column of the matrix. </summary>
        ///
        /// <param name="index"> Zero-based column index. </param>
        ///
        /// <returns> A reference to the matrix column. </returns>
        VectorReference<ElementType, VectorOrientation::column> GetColumn(size_t index);

        /// <summary> Gets a reference to a row of the matrix. </summary>
        ///
        /// <param name="index"> Zero-based row index. </param>
        ///
        /// <returns> A reference to the matrix row. </returns>
        VectorReference<ElementType, VectorOrientation::row> GetRow(size_t index);

        /// <summary> Gets a reference to the matrix diagonal as a column vector. </summary>
        ///
        /// <returns> A const reference to the matrix diagnoal. </returns>
        VectorReference<ElementType, VectorOrientation::column> GetDiagonal();

        /// <summary> Gets a reference to a row of a row major matrix or to a column of a column major matrix. </summary>
        ///
        /// <param name="index"> The interval index. </param>
        ///
        /// <returns> Reference to the interval. </returns>
        auto GetMajorVector(size_t index)->VectorReference<ElementType, MatrixBase<ElementType, Layout>::_intervalOrientation>
        {
            return RectangularMatrixBase<ElementType>::template ConstructVectorReference<MatrixBase<ElementType, Layout>::_intervalOrientation>(GetMajorVectorBegin(index), _intervalSize, 1);
        }

        using RectangularMatrixBase<ElementType>::NumRows;
        using RectangularMatrixBase<ElementType>::NumColumns;

    protected:
        friend MatrixReference<ElementType, MatrixBase<ElementType, Layout>::_transposeLayout>;
        using ConstMatrixReference<ElementType, Layout>::ConstMatrixReference;

        using RectangularMatrixBase<ElementType>::_pData;
        using RectangularMatrixBase<ElementType>::_numRows;
        using RectangularMatrixBase<ElementType>::_numColumns;
        using RectangularMatrixBase<ElementType>::_increment;

        using MatrixBase<ElementType, Layout>::_numIntervals;
        using MatrixBase<ElementType, Layout>::_intervalSize;
        using MatrixBase<ElementType, Layout>::_rowIncrement;
        using MatrixBase<ElementType, Layout>::_columnIncrement;

        using ConstMatrixReference<ElementType, Layout>::GetMajorVectorBegin;
    };

    /// <summary> A dense matrix. </summary>
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
        Matrix(size_t numRows, size_t numColumns);

        /// <summary> Constructs a matrix from an initialization list. </summary>
        ///
        /// <param name="list"> A list of initialization lists (row by row). </param>
        Matrix(std::initializer_list<std::initializer_list<ElementType>> list);

        /// <summary> Sets all matrix elements to zero. </summary>
        void Reset() { Fill(0); }

        /// <summary> Sets all matrix elements to a given value. </summary>
        ///
        /// <param name="value"> The value. </param>
        void Fill(ElementType value);

        /// <summary>
        /// Generates elements of the vector by repeatedly calling a generator function (such as a random
        /// number generator).
        /// </summary>
        ///
        /// <typeparam name="GeneratorType"> Type of lambda or functor to use as a generator. </typeparam>
        /// <param name="generator"> The generator function. </param>
        template<typename GeneratorType>
        void Generate(GeneratorType generator);

    private:
        using RectangularMatrixBase<ElementType>::_pData;
        using RectangularMatrixBase<ElementType>::_numRows;
        using RectangularMatrixBase<ElementType>::_numColumns;
        using RectangularMatrixBase<ElementType>::_increment;
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
}
#include "../tcc/Matrix.tcc"
