////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:    Matrix.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Vector.h"

// utilities
#include "IArchivable.h"

// stl
#include <cstddef>
#include <limits>

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
    /// Usage: auto transposedLayout = TransposeMatrixLayout<layout>::value

    template <MatrixLayout>
    struct TransposeMatrixLayout;

    template <>
    struct TransposeMatrixLayout<MatrixLayout::columnMajor>
    {
        static constexpr MatrixLayout value = MatrixLayout::rowMajor;
    };

    template <>
    struct TransposeMatrixLayout<MatrixLayout::rowMajor>
    {
        static constexpr MatrixLayout value = MatrixLayout::columnMajor;
    };

    /// <summary> Forward declaration of a base class for matrices, for subsequent specialization according to layout. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    template <typename ElementType, MatrixLayout layout>
    class MatrixBase;

    /// <summary> Base class for column major rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template <typename ElementType>
    class MatrixBase<ElementType, MatrixLayout::columnMajor> : public RectangularMatrixBase<ElementType>
    {
    public:
        using RectangularMatrixBase<ElementType>::RectangularMatrixBase;

        /// <summary> Constructs a column major instance of MatrixBase> </summary>
        ///
        /// <param name="numRows"> Number of rows. </param>
        /// <param name="numColumns"> Number of columns. </param>
        /// <param name="pData"> (Optional) Pointer to the data. </param>
        MatrixBase(size_t numRows, size_t numColumns, ElementType* pData = nullptr);

    protected:
        void Swap(MatrixBase<ElementType, MatrixLayout::columnMajor>& other);
        using RectangularMatrixBase<ElementType>::_numRows;
        using RectangularMatrixBase<ElementType>::_numColumns;
        using RectangularMatrixBase<ElementType>::_increment;

        static constexpr VectorOrientation _intervalOrientation = VectorOrientation::column;

        const size_t& _numIntervals = _numColumns;
        const size_t& _intervalSize = _numRows;
        static constexpr size_t _rowIncrement = 1;
        const size_t& _columnIncrement = _increment;
    };

    /// <summary> Base class for row major rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template <typename ElementType>
    class MatrixBase<ElementType, MatrixLayout::rowMajor> : public RectangularMatrixBase<ElementType>
    {
    public:
        using RectangularMatrixBase<ElementType>::RectangularMatrixBase;

        /// <summary> Constructs a row major instance of MatrixBase> </summary>
        ///
        /// <param name="numRows"> Number of rows. </param>
        /// <param name="numColumns"> Number of columns. </param>
        /// <param name="pData"> (Optional) Pointer to the data. </param>
        MatrixBase(size_t numRows, size_t numColumns, ElementType* pData = nullptr);

    protected:
        void Swap(MatrixBase<ElementType, MatrixLayout::rowMajor>& other);

        using RectangularMatrixBase<ElementType>::_numRows;
        using RectangularMatrixBase<ElementType>::_numColumns;
        using RectangularMatrixBase<ElementType>::_increment;

        static constexpr VectorOrientation _intervalOrientation = VectorOrientation::row;

        const size_t& _numIntervals = _numRows;
        const size_t& _intervalSize = _numColumns;
        const size_t& _rowIncrement = _increment;
        static constexpr size_t _columnIncrement = 1;
    };

    /// <summary> Const reference to a dense matrix. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    template <typename ElementType, MatrixLayout layout>
    class ConstMatrixReference : public MatrixBase<ElementType, layout>
    {
    public:
        using MatrixBase<ElementType, layout>::MatrixBase;
        using RectangularMatrixBase<ElementType>::NumRows;
        using RectangularMatrixBase<ElementType>::NumColumns;

        /// <summary> Matrix element access operator. </summary>
        ///
        /// <returns> A copy of the element in a given position. </returns>
        inline ElementType operator()(size_t rowIndex, size_t columnIndex) const;

        /// <summary> Gets the number of elements in the matrix. </summary>
        ///
        /// <returns> The number of elements in the matrix. </returns>
        size_t Size() const { return NumRows() * NumColumns(); }

        /// <summary> Gets a const pointer to the underlying data storage. </summary>
        ///
        /// <returns> Const pointer to the data. </returns>
        const ElementType* GetDataPointer() const { return _pData; }

        /// <summary> Gets the size of the storage. </summary>
        ///
        /// <returns> The size of the storage. </returns>
        size_t GetDataSize() const { return _increment * _numIntervals; }

        /// <summary> Gets the matrix layout. </summary>
        ///
        /// <returns> The matrix layout. </returns>
        MatrixLayout GetLayout() const { return layout; }

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
        bool IsContiguous() const { return (_increment == _intervalSize); }

        /// <summary> Returns a copy of the contents of the Matrix. </summary>
        ///
        /// <returns> A std::vector with a copy of the contents of the Matrix. </returns>
        std::vector<ElementType> ToArray() const;

        /// <summary> Swaps the contents of this matrix with the contents of another matrix. </summary>
        ///
        /// <param name="other"> [in,out] The other matrix. </param>
        void Swap(ConstMatrixReference<ElementType, layout>& other);

        /// \name Comparison Functions
        /// @{

        /// <summary> Determines if two matrices with the same layout are equal. </summary>
        ///
        /// <param name="other"> The other matrix. </param>
        /// <param name="tolerance"> The element comparison tolerance. </param>
        ///
        /// <returns> true if the two matrices are equivalent. </returns>
        bool IsEqual(ConstMatrixReference<ElementType, layout> other, ElementType tolerance = static_cast<ElementType>(1.0e-8)) const;

        /// <summary> Determines if two matrices with opposite layouts are equal. </summary>
        ///
        /// <param name="other"> The other matrix. </param>
        /// <param name="tolerance"> The element comparison tolerance. </param>
        ///
        /// <returns> true if the two matrices are equivalent. </returns>
        bool IsEqual(ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value> other, ElementType tolerance = static_cast<ElementType>(1.0e-8)) const;

        /// <summary> Equality operator for matrices with the same layout. </summary>
        ///
        /// <param name="other"> The other matrix. </param>
        ///
        /// <returns> true if the two matrices are equivalent. </returns>
        bool operator==(const ConstMatrixReference<ElementType, layout>& other) const;

        /// <summary> Equality operator for matrices with opposite layout. </summary>
        ///
        /// <param name="other"> The other matrix. </param>
        ///
        /// <returns> true if the two matrices are equivalent. </returns>
        bool operator==(const ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value>& other) const;

        /// <summary> Inequality operator. </summary>
        ///
        /// <typeparam name="otherLayout"> The layout of the other matrix. </typeparam>
        /// <param name="other"> The other matrix. </param>
        ///
        /// <returns> true if the two matrices are not equivalent. </returns>
        template <MatrixLayout otherLayout>
        bool operator!=(const ConstMatrixReference<ElementType, otherLayout>& other);

        /// @}

        /// \name View Functions
        /// @{

        /// <summary> Gets a constant reference to this vector. </summary>
        ///
        /// <returns> A constant reference to this vector. </returns>
        ConstMatrixReference<ElementType, layout> GetConstReference() const { return *this; }

        /// <summary> Gets a constant reference to a block-shaped sub-matrix. </summary>
        ///
        /// <param name="firstRow"> The first row in the block. </param>
        /// <param name="firstColumn"> The first column in the block. </param>
        /// <param name="numRows"> Number of rows in the block. </param>
        /// <param name="numColumns"> Number of columns in the block. </param>
        ///
        /// <returns> The constant reference to a block. </returns>
        ConstMatrixReference<ElementType, layout> GetSubMatrix(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns) const;

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
            return ConstVectorReference<ElementType, MatrixBase<ElementType, layout>::_intervalOrientation>(GetMajorVectorBegin(index), _intervalSize, 1);
        }

        /// <summary> Gets a reference to the matrix transpose. </summary>
        ///
        /// <returns> A reference to the matrix transpose. </returns>
        auto Transpose() const -> ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value>;

        /// <summary> Returns a vew of this matrix as a vector reference (requires the matrix to be contiguous). </summary>
        ///
        /// <returns> A ConstVectorReference. </returns>
        ConstVectorReference<ElementType, VectorOrientation::column> ReferenceAsVector() const;

        /// @}

    protected:
        friend class ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value>;

        ElementType* GetMajorVectorBegin(size_t index) const;

        using RectangularMatrixBase<ElementType>::_pData;
        using RectangularMatrixBase<ElementType>::_numRows;
        using RectangularMatrixBase<ElementType>::_numColumns;
        using RectangularMatrixBase<ElementType>::_increment;

        using MatrixBase<ElementType, layout>::_numIntervals;
        using MatrixBase<ElementType, layout>::_intervalSize;
        using MatrixBase<ElementType, layout>::_rowIncrement;
        using MatrixBase<ElementType, layout>::_columnIncrement;
    };

    /// <summary> Prints a matrix in initializer list format. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    /// <param name="M"> The matrix. </param>
    /// <param name="stream"> [in,out] The output stream. </param>
    /// <param name="indent"> (Optional) How many indent to print before the tensor. </param>
    /// <param name="maxRows"> (Optional) The maximum number of rows to print. </param>
    /// <param name="maxElementsPerRow"> (Optional) The maximum number of elements to print per row. </param>
    template <typename ElementType, MatrixLayout layout>
    void Print(ConstMatrixReference<ElementType, layout> M, std::ostream& stream, size_t indent = 0, size_t maxRows = std::numeric_limits<size_t>::max(), size_t maxElementsPerRow = std::numeric_limits<size_t>::max());

    /// <summary> Prints a matrix in initializer list format. </summary>
    ///
    /// <param name="stream"> [in,out] The output stream. </param>
    /// <param name="M"> The const matrix reference to print. </param>
    ///
    /// <returns> Reference to the output stream. </returns>
    template <typename ElementType, MatrixLayout layout>
    std::ostream& operator<<(std::ostream& stream, ConstMatrixReference<ElementType, layout> M);

    /// <summary> Non-const reference to a dense matrix. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    template <typename ElementType, MatrixLayout layout>
    class MatrixReference : public ConstMatrixReference<ElementType, layout>
    {
    public:
        using ConstMatrixReference<ElementType, layout>::ConstMatrixReference;
        using ConstMatrixReference<ElementType, layout>::NumRows;
        using ConstMatrixReference<ElementType, layout>::NumColumns;
        using ConstMatrixReference<ElementType, layout>::operator();
        using ConstMatrixReference<ElementType, layout>::Size;
        using ConstMatrixReference<ElementType, layout>::GetDataPointer;
        using ConstMatrixReference<ElementType, layout>::GetLayout;
        using ConstMatrixReference<ElementType, layout>::GetIncrement;
        using ConstMatrixReference<ElementType, layout>::NumIntervals;
        using ConstMatrixReference<ElementType, layout>::IsContiguous;
        using ConstMatrixReference<ElementType, layout>::IsEqual;
        using ConstMatrixReference<ElementType, layout>::operator==;
        using ConstMatrixReference<ElementType, layout>::operator!=;
        using ConstMatrixReference<ElementType, layout>::GetConstReference;
        using ConstMatrixReference<ElementType, layout>::GetSubMatrix;
        using ConstMatrixReference<ElementType, layout>::GetColumn;
        using ConstMatrixReference<ElementType, layout>::GetRow;
        using ConstMatrixReference<ElementType, layout>::GetDiagonal;
        using ConstMatrixReference<ElementType, layout>::GetMajorVector;
        using ConstMatrixReference<ElementType, layout>::Transpose;
        using ConstMatrixReference<ElementType, layout>::ReferenceAsVector;

        /// <summary> Matrix element access operator. </summary>
        ///
        /// <returns> A reference to an element in a given position. </returns>
        inline ElementType& operator()(size_t rowIndex, size_t columnIndex);

        /// <summary> Gets a pointer to the underlying data storage. </summary>
        ///
        /// <returns> Pointer to the data. </returns>
        ElementType* GetDataPointer() const { return _pData; }

        /// <summary> Swaps the contents of this matrix with the contents of another matrix. </summary>
        ///
        /// <param name="other"> [in,out] The other matrix. </param>
        void Swap(MatrixReference<ElementType, layout>& other);

        /// \name  Content Manipulation Functions
        /// @{

        /// <summary> Copies values from another matrix into this matrix. </summary>
        ///
        /// <param name="other"> The other matrix. </param>
        void CopyFrom(ConstMatrixReference<ElementType, layout> other);

        /// <summary> Copies values from another matrix into this matrix. </summary>
        ///
        /// <param name="other"> The other matrix. </param>
        void CopyFrom(ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value> other);

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

        /// <summary> Applies a transfromation to each element of the matrix. </summary>
        ///
        /// <typeparam name="TransformationType"> Tranformation type. </typeparam>
        /// <param name="transformation"> The transfromation. </param>
        template <typename TransformationType>
        void Transform(TransformationType transformation);

        /// @}

        /// \name View Functions
        /// @{

        /// <summary> Gets a reference to this matrix. </summary>
        ///
        /// <returns> A reference to this matrix. </returns>
        MatrixReference<ElementType, layout> GetReference() { return *this; }

        /// <summary> Gets a reference to the matrix transpose. </summary>
        ///
        /// <returns> A reference to the matrix transpose. </returns>
        auto Transpose() const -> MatrixReference<ElementType, TransposeMatrixLayout<layout>::value>;

        /// <summary> Gets a const reference to a block-shaped sub-matrix. </summary>
        ///
        /// <param name="firstRow"> The first row in the block. </param>
        /// <param name="firstColumn"> The first column in the block. </param>
        /// <param name="numRows"> Number of rows in the block. </param>
        /// <param name="numColumns"> Number of columns in the block. </param>
        ///
        /// <returns> The constant reference to a block. </returns>
        MatrixReference<ElementType, layout> GetSubMatrix(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns);

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
            return VectorReference<ElementType, MatrixBase<ElementType, layout>::_intervalOrientation>(GetMajorVectorBegin(index), _intervalSize, 1);
        }

        /// @}

        /// \name Math Functions
        /// @{

        /// <summary> Adds a constant value to this matrix. </summary>
        ///
        /// <param name="other"> The constant value. </param>
        void operator+=(ElementType value);

        /// <summary> Subtracts a constant value from this matrix. </summary>
        ///
        /// <param name="other"> The constant value. </param>
        void operator-=(ElementType value);

        /// <summary> Multiplies this matrix by a constant value. </summary>
        ///
        /// <param name="other"> The constant value. </param>
        void operator*=(ElementType value);

        /// <summary> Divides each element of this matrix by a constant value. </summary>
        ///
        /// <param name="other"> The constant value. </param>
        void operator/=(ElementType value);

        /// @}

    protected:
        friend MatrixReference<ElementType, TransposeMatrixLayout<layout>::value>;

        using RectangularMatrixBase<ElementType>::_pData;
        using RectangularMatrixBase<ElementType>::_numRows;
        using RectangularMatrixBase<ElementType>::_numColumns;
        using RectangularMatrixBase<ElementType>::_increment;

        using MatrixBase<ElementType, layout>::_numIntervals;
        using MatrixBase<ElementType, layout>::_intervalSize;
        using MatrixBase<ElementType, layout>::_rowIncrement;
        using MatrixBase<ElementType, layout>::_columnIncrement;

        using ConstMatrixReference<ElementType, layout>::GetMajorVectorBegin;
    };

    /// <summary> A dense matrix. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    template <typename ElementType, MatrixLayout layout>
    class Matrix : public MatrixReference<ElementType, layout>
    {
    public:
        using MatrixReference<ElementType, layout>::NumRows;
        using MatrixReference<ElementType, layout>::NumColumns;
        using MatrixReference<ElementType, layout>::operator();
        using MatrixReference<ElementType, layout>::Size;
        using MatrixReference<ElementType, layout>::GetDataPointer;
        using MatrixReference<ElementType, layout>::GetLayout;
        using MatrixReference<ElementType, layout>::GetIncrement;
        using MatrixReference<ElementType, layout>::NumIntervals;
        using MatrixReference<ElementType, layout>::IsContiguous;
        using MatrixReference<ElementType, layout>::IsEqual;
        using MatrixReference<ElementType, layout>::operator==;
        using MatrixReference<ElementType, layout>::operator!=;
        using MatrixReference<ElementType, layout>::GetConstReference;
        using MatrixReference<ElementType, layout>::GetSubMatrix;
        using MatrixReference<ElementType, layout>::GetColumn;
        using MatrixReference<ElementType, layout>::GetRow;
        using MatrixReference<ElementType, layout>::GetDiagonal;
        using MatrixReference<ElementType, layout>::GetMajorVector;
        using MatrixReference<ElementType, layout>::Transpose;
        using MatrixReference<ElementType, layout>::ReferenceAsVector;
        using MatrixReference<ElementType, layout>::CopyFrom;
        using MatrixReference<ElementType, layout>::Reset;
        using MatrixReference<ElementType, layout>::Fill;
        using MatrixReference<ElementType, layout>::Generate;
        using MatrixReference<ElementType, layout>::Transform;
        using MatrixReference<ElementType, layout>::GetReference;
        using MatrixReference<ElementType, layout>::operator+=;
        using MatrixReference<ElementType, layout>::operator-=;
        using MatrixReference<ElementType, layout>::operator*=;
        using MatrixReference<ElementType, layout>::operator/=;

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
        Matrix(Matrix<ElementType, layout>&& other);

        /// <summary> Copy Constructor. </summary>
        ///
        /// <param name="other"> The matrix being copied. </param>
        Matrix(ConstMatrixReference<ElementType, layout>& other);

        /// <summary> Copy Constructor. </summary>
        ///
        /// <param name="other"> [in,out] The matrix being copied. </param>
        Matrix(const Matrix<ElementType, layout>& other);

        /// <summary> Copies a matrix of the opposite layout. </summary>
        ///
        /// <param name="other"> The matrix being copied. </param>
        Matrix(ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value> other);

        /// <summary> Assignment operator. </summary>
        ///
        /// <param name="other"> The other matrix. </param>
        ///
        /// <returns> A reference to this matrix. </returns>
        Matrix<ElementType, layout>& operator=(Matrix<ElementType, layout> other);

        Matrix<ElementType, layout>& operator=(Matrix<ElementType, layout>&& other) = default;

        /// <summary> Swaps the contents of this matrix with the contents of another matrix. </summary>
        ///
        /// <param name="other"> [in,out] The other matrix. </param>
        void Swap(Matrix<ElementType, layout>& other);

        /// <summary> Returns a copy of the contents of the Matrix. </summary>
        ///
        /// <returns> A std::vector with a copy of the contents of the Matrix. </returns>
        std::vector<ElementType> ToArray() const { return _data; }

    private:
        using RectangularMatrixBase<ElementType>::_pData;
        using RectangularMatrixBase<ElementType>::_numRows;
        using RectangularMatrixBase<ElementType>::_numColumns;
        using RectangularMatrixBase<ElementType>::_increment;
        std::vector<ElementType> _data;
    };

    /// <summary> A class that implements helper functions for archiving/unarchiving Matrix instances. </summary>
    class MatrixArchiver
    {
    public:
        /// <summary> Writes a matrix to the archiver. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix element type. </typeparam>
        /// <typeparam name="layout"> Matrix layout. </typeparam>
        /// <param name="tensor"> The matrix to add to the archiver. </param>
        /// <param name="name"> The name of the matrix value to add to the archiver. </param>
        /// <param name="archiver"> The `Archiver` to add the matrix to </param>
        template <typename ElementType, MatrixLayout layout>
        static void Write(const Matrix<ElementType, layout>& matrix, const std::string& name, utilities::Archiver& archiver);

        /// <summary> Reads a matrix from the archiver. </summary>
        ///
        /// <typeparam name="ElementType"> Matrix element type. </typeparam>
        /// <typeparam name="layout"> Matrix layout. </typeparam>
        /// <param name="tensor"> The matrix that will hold the result after it has been read from the archiver. </param>
        /// <param name="name"> The name of the matrix value in the archiver. </param>
        /// <param name="archiver"> The `Archiver` to add the matrix to </param>
        template <typename ElementType, MatrixLayout layout>
        static void Read(Matrix<ElementType, layout>& matrix, const std::string& name, utilities::Unarchiver& archiver);

    private:
        static std::string GetRowsName(const std::string& name) { return name + "_rows"; }
        static std::string GetColumnsName(const std::string& name) { return name + "_columns"; }
        static std::string GetValuesName(const std::string& name) { return name + "_values"; }
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
