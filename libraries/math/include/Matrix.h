////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:    Matrix.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Vector.h"

#ifndef MATRIX_H
#define MATRIX_H

namespace ell
{
namespace math
{
    /// <summary> Base class for rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template <typename ElementType>
    class RectangularMatrixBase
    {
    public:
        /// <summary> Constructs an instance of RectangularMatrixBase. </summary>
        ///
        /// <param name="numRows"> Number of matrix rows. </param>
        /// <param name="numColumns"> Number of matrix columns. </param>
        /// <param name="increment"> The matrix increment. </param>
        /// <param name="pData"> Pointer to the underlying std::vector that contains the tensor data. </param>
        RectangularMatrixBase(size_t numRows, size_t numColumns, size_t increment, ElementType* pData = nullptr);

        /// <summary> Gets the number of rows. </summary>
        ///
        /// <returns> The number of rows. </returns>
        size_t NumRows() const { return _numRows; }

        /// <summary> Gets the number of columns. </summary>
        ///
        /// <returns> The number of columns. </returns>
        size_t NumColumns() const { return _numColumns; }

    protected:
        void Swap(RectangularMatrixBase<ElementType>& other);

        ElementType* _pData;
        size_t _numRows;
        size_t _numColumns;
        size_t _increment;
    };

    /// <summary> Enum of possible matrix layouts. </summary>
    enum class MatrixLayout
    {
        columnMajor,
        rowMajor
    };

    /// <summary> Helper class to obtain the transpose of a MatrixLayout </summary>
    ///
    /// Usage: auto transposedLayout = TransposeMatrixLayout<Layout>::layout
    
    template <MatrixLayout>
    struct TransposeMatrixLayout;

    template <>
    struct TransposeMatrixLayout<MatrixLayout::columnMajor>
    {
        static constexpr MatrixLayout layout = MatrixLayout::rowMajor;
    };

    template <>
    struct TransposeMatrixLayout<MatrixLayout::rowMajor>
    {
        static constexpr MatrixLayout layout = MatrixLayout::columnMajor;
    };

    /// <summary> Forward declaration of a base class for matrices, for subsequent specialization according to layout. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="Layout"> Type of the layout. </typeparam>
    template <typename ElementType, MatrixLayout Layout>
    class MatrixBase;

    /// <summary> Base class for column major rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template <typename ElementType>
    class MatrixBase<ElementType, MatrixLayout::columnMajor> : public RectangularMatrixBase<ElementType>
    {
    protected:
        using RectangularMatrixBase<ElementType>::RectangularMatrixBase;
        MatrixBase(size_t numRows, size_t numColumns, ElementType* pData = nullptr);

        using RectangularMatrixBase<ElementType>::_numRows;
        using RectangularMatrixBase<ElementType>::_numColumns;
        using RectangularMatrixBase<ElementType>::_increment;

        static constexpr VectorOrientation _intervalOrientation = VectorOrientation::column;

        const size_t _numIntervals = _numColumns;
        const size_t _intervalSize = _numRows;
        static constexpr size_t _rowIncrement = 1;
        const size_t _columnIncrement = _increment;
    };

    /// <summary> Base class for row major rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template <typename ElementType>
    class MatrixBase<ElementType, MatrixLayout::rowMajor> : public RectangularMatrixBase<ElementType>
    {
    protected:
        using RectangularMatrixBase<ElementType>::RectangularMatrixBase;
        MatrixBase(size_t numRows, size_t numColumns, ElementType* pData = nullptr);

        using RectangularMatrixBase<ElementType>::_numRows;
        using RectangularMatrixBase<ElementType>::_numColumns;
        using RectangularMatrixBase<ElementType>::_increment;

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

        /// <summary> Determines of this Matrix is stored in contiguous memory. </summary>
        ///
        /// <returns> True if contiguous, false if not. </returns>
        bool IsContiguous() const;

        /// <summary> Matrix element access operator. </summary>
        ///
        /// <returns> A copy of the element in a given position. </returns>
        ElementType operator()(size_t rowIndex, size_t columnIndex) const;

        /// <summary> Gets the matrix layout. </summary>
        ///
        /// <returns> The matrix layout. </returns>
        MatrixLayout GetLayout() const { return Layout; }

        /// <summary> Gets a reference to the matrix transpose. </summary>
        ///
        /// <returns> A reference to the matrix transpose. </returns>
        auto Transpose() const -> ConstMatrixReference<ElementType, TransposeMatrixLayout<Layout>::layout>;

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
        auto GetMajorVector(size_t index) const
        {
            // STYLE intentional deviation from project style
            return ConstVectorReference<ElementType, MatrixBase<ElementType, Layout>::_intervalOrientation>(GetMajorVectorBegin(index), _intervalSize, 1);
        }

        /// <summary> Returns a vew of this matrix as a vector reference (requires the matrix to be contiguous). </summary>
        ///
        /// <returns> A ConstVectorReference. </returns>
        ConstVectorReference<ElementType, VectorOrientation::column> ReferenceAsVector() const;

        /// <summary> Determines if two matrices with the same layout are equal. </summary>
        ///
        /// <param name="other"> The other matrix. </param>
        /// <param name="tolerance"> The element comparison tolerance. </param>
        ///
        /// <returns> true if the two matrices are equivalent. </returns>
        bool IsEqual(ConstMatrixReference<ElementType, Layout> other, ElementType tolerance = 1.0e-8) const;

        /// <summary> Determines if two matrices with opposite layouts are equal. </summary>
        ///
        /// <param name="other"> The other matrix. </param>
        /// <param name="tolerance"> The element comparison tolerance. </param>
        ///
        /// <returns> true if the two matrices are equivalent. </returns>
        bool IsEqual(ConstMatrixReference<ElementType, TransposeMatrixLayout<Layout>::layout> other, ElementType tolerance = 1.0e-8) const;

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
        bool operator==(const ConstMatrixReference<ElementType, TransposeMatrixLayout<Layout>::layout>& other) const;

        /// <summary> Inequality operator. </summary>
        ///
        /// <typeparam name="OtherLayout"> The layout of the other matrix. </typeparam>
        /// <param name="other"> The other matrix. </param>
        ///
        /// <returns> true if the two matrices are not equivalent. </returns>
        template <MatrixLayout OtherLayout>
        bool operator!=(const ConstMatrixReference<ElementType, OtherLayout>& other);

        using RectangularMatrixBase<ElementType>::NumRows;
        using RectangularMatrixBase<ElementType>::NumColumns;

    protected:
        friend class ConstMatrixReference<ElementType, TransposeMatrixLayout<Layout>::layout>;
        using MatrixBase<ElementType, Layout>::MatrixBase;

        ConstMatrixReference(size_t numRows, size_t numColumns, ElementType* pData) : MatrixBase<ElementType, Layout>(numRows, numColumns, pData) {}

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
        /// <summary> Constructs a matrix that uses a pointer to an external buffer as the element data. 
        /// This allows the matrix to use data provided by some other source, and this matrix does not
        /// own the data buffer.
        /// The buffer has (numRows * numColumns) number of elements. </summary>
        ///
        /// <param name="numRows"> Number of rows in the matrix. </param>
        /// <param name="numColumns"> Number of columns in the matrix. </param>
        /// <param name="pData"> A pointer where the elements are stored. </param>
        MatrixReference(size_t numRows, size_t numColumns, ElementType* pData);

        /// <summary> Gets a pointer to the underlying data storage. </summary>
        ///
        /// <returns> Pointer to the data. </returns>
        ElementType* GetDataPointer() const { return _pData; }

        /// <summary> Matrix element access operator. </summary>
        ///
        /// <returns> A reference to an element in a given position. </returns>
        ElementType& operator()(size_t rowIndex, size_t columnIndex);

        /// <summary> Sets all matrix elements to zero. </summary>
        void Reset() { Fill(0); }

        /// <summary> Sets all matrix elements to a given value. </summary>
        ///
        /// <param name="value"> The value. </param>
        void Fill(ElementType value);

        /// <summary>
        /// Generates elements of the matrix by repeatedly calling a generator function (such as a random
        /// number generator).
        /// </summary>
        ///
        /// <typeparam name="GeneratorType"> Type of lambda or functor to use as a generator. </typeparam>
        /// <param name="generator"> The generator function. </param>
        template <typename GeneratorType>
        void Generate(GeneratorType generator);

        /// <summary> Gets a reference to this matrix. </summary>
        ///
        /// <returns> A reference to this matrix. </returns>
        MatrixReference<ElementType, Layout> GetReference();

        /// <summary> Gets a const reference to this matrix. </summary>
        ///
        /// <returns> A const reference to this matrix. </returns>
        ConstMatrixReference<ElementType, Layout> GetConstReference() const;

        /// <summary> Gets a reference to the matrix transpose. </summary>
        ///
        /// <returns> A reference to the matrix transpose. </returns>
        auto Transpose() const -> MatrixReference<ElementType, TransposeMatrixLayout<Layout>::layout>;

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

        /// <summary> Returns a vew of this matrix as a vector reference (requires the matrix to be contiguous). </summary>
        ///
        /// <returns> A VectorReference. </returns>
        VectorReference<ElementType, VectorOrientation::column> ReferenceAsVector();

        /// <summary> Gets a reference to a row of a row major matrix or to a column of a column major matrix. </summary>
        ///
        /// <param name="index"> The interval index. </param>
        ///
        /// <returns> Reference to the interval. </returns>
        auto GetMajorVector(size_t index)
        {
            // STYLE intentional deviation from project style
            return VectorReference<ElementType, MatrixBase<ElementType, Layout>::_intervalOrientation>(GetMajorVectorBegin(index), _intervalSize, 1);
        }

        /// <summary> Swaps the contents of this matrix with the contents of another matrix. </summary>
        ///
        /// <param name="other"> [in,out] The other matrix. </param>
        void Swap(MatrixReference<ElementType, Layout>& other);

        using RectangularMatrixBase<ElementType>::NumRows;
        using RectangularMatrixBase<ElementType>::NumColumns;

    protected:
        friend MatrixReference<ElementType, TransposeMatrixLayout<Layout>::layout>;
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

        /// <summary> Constructs a matrix from an vector. The vector has
        /// (numRows * numColumns) number of elements. </summary>
        ///
        /// <param name="numRows"> Number of rows in the matrix. </param>
        /// <param name="numColumns"> Number of columns in the matrix. </param>
        /// <param name="list"> A list of elements. These elements are expected to be in the layout order of this matrix's layout type. </param>
        Matrix(size_t numRows, size_t numColumns, const std::vector<ElementType>& data);

        /// <summary> Constructs a matrix from an vector. The vector has
        /// (numRows * numColumns) number of elements. </summary>
        ///
        /// <param name="numRows"> Number of rows in the matrix. </param>
        /// <param name="numColumns"> Number of columns in the matrix. </param>
        /// <param name="list"> A list of elements. These elements are expected to be in the layout order of this matrix's layout type. </param>
        Matrix(size_t numRows, size_t numColumns, std::vector<ElementType>&& data);

        /// <summary> Move Constructor. </summary>
        ///
        /// <param name="other"> [in,out] The matrix being moved. </param>
        Matrix(Matrix<ElementType, Layout>&& other);

        /// <summary> Copy Constructor. </summary>
        ///
        /// <param name="other"> The matrix being copied. </param>
        Matrix(ConstMatrixReference<ElementType, Layout>& other);

        /// <summary> Copy Constructor. </summary>
        ///
        /// <param name="other"> [in,out] The matrix being copied. </param>
        Matrix(const Matrix<ElementType, Layout>& other);

        /// <summary> Copies a matrix of the opposite layout. </summary>
        ///
        /// <param name="other"> The matrix being copied. </param>
        Matrix(ConstMatrixReference<ElementType, TransposeMatrixLayout<Layout>::layout> other);

        /// <summary> Assignment operator. </summary>
        ///
        /// <param name="other"> The other matrix. </param>
        ///
        /// <returns> A reference to this matrix. </returns>
        Matrix<ElementType, Layout>& operator=(Matrix<ElementType, Layout> other);

        /// <summary> Swaps the contents of this matrix with the contents of another matrix. </summary>
        ///
        /// <param name="other"> [in,out] The other matrix. </param>
        void Swap(Matrix<ElementType, Layout>& other);

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
        template <typename GeneratorType>
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
    template <typename ElementType>
    using ColumnMatrix = Matrix<ElementType, MatrixLayout::columnMajor>;

    template <typename ElementType>
    using ColumnMatrixReference = Matrix<ElementType, MatrixLayout::columnMajor>;

    template <typename ElementType>
    using RowMatrix = Matrix<ElementType, MatrixLayout::rowMajor>;

    template <typename ElementType>
    using RowMatrixReference = MatrixReference<ElementType, MatrixLayout::rowMajor>;
}
}

#include "../tcc/Matrix.tcc"

#endif // MATRIX_H
