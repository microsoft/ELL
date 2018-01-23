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
    class CommonMatrixBase
    {
    public:
        /// <summary> Constructs an instance of CommonMatrixBase. </summary>
        ///
        /// <param name="pData"> Pointer to the underlying memory that contains the tensor data. </param>
        /// <param name="numRows"> Number of matrix rows. </param>
        /// <param name="numColumns"> Number of matrix columns. </param>
        /// <param name="increment"> The matrix increment. </param>
        CommonMatrixBase(const ElementType* pData, size_t numRows, size_t numColumns, size_t increment);

        /// <summary> Gets the number of rows. </summary>
        ///
        /// <returns> The number of rows. </returns>
        size_t NumRows() const { return _numRows; }

        /// <summary> Gets the number of columns. </summary>
        ///
        /// <returns> The number of columns. </returns>
        size_t NumColumns() const { return _numColumns; }

        /// <summary> Gets the number of elements in the matrix. </summary>
        ///
        /// <returns> The number of elements in the matrix. </returns>
        size_t Size() const { return NumRows() * NumColumns(); }

        /// <summary> Gets the increment used in the underlying data storage. </summary>
        ///
        /// <returns> The increment. </returns>
        size_t GetIncrement() const { return _increment; }

    protected:
        void Swap(CommonMatrixBase<ElementType>& other);
        const ElementType* _pData;
        size_t _numRows;
        size_t _numColumns;
        size_t _increment;
    };

    /// <summary> Enum of possible matrix layouts.
    /// This is a standalone definition separate from OpenBLAS' cblas.h so that the ELL
    /// code base can compile without OpenBLAS. Use GetCBlasMatrixOrder if you need to
    /// map these values back to OpenBLAS' definition of CBLAS_ORDER.
    /// </summary>
    enum class MatrixLayout
    {
        columnMajor,
        rowMajor
    };

    /// <summary> Enum of possible matrix transposes.
    /// This is a standalone definition separate from OpenBLAS' cblas.h so that the ELL
    /// code base can compile without OpenBLAS. Use GetCBlasMatrixTranspose if you need to
    /// map these values back to OpenBLAS' definition of CBLAS_TRANSPOSE.</summary>
    enum class MatrixTranspose
    {
        noTranspose,
        transpose
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
    class MatrixBase<ElementType, MatrixLayout::columnMajor> : public CommonMatrixBase<ElementType>
    {
    public:
        using CommonMatrixBase<ElementType>::CommonMatrixBase;

        /// <summary> Constructs a column major instance of MatrixBase> </summary>
        ///
        /// <param name="pData"> (Optional) Pointer to the data. </param>
        /// <param name="numRows"> Number of rows. </param>
        /// <param name="numColumns"> Number of columns. </param>
        MatrixBase(const ElementType* pData, size_t numRows, size_t numColumns);

        /// \name Accessor Functions
        /// @{

        /// <summary> Gets the minor size of the matrix (number of columns of a column major matrix or number of rows of a row major matrix). </summary>
        ///
        /// <returns> The minor size. </returns>
        size_t GetMinorSize() const { return this->NumColumns(); }

        /// <summary> Gets the major size of a matrix (size of each column of a column major matrix or size of each row of a row major matrix). </summary>
        ///
        /// <returns> The major size. </returns>
        size_t GetMajorSize() const { return this->NumRows(); }

        /// <summary> Gets the row increment. </summary>
        ///
        /// <returns> The row increment. </returns>
        size_t GetRowIncrement() const { return 1; }

        /// <summary> Gets the column increment. </summary>
        ///
        /// <returns> The column increment. </returns>
        size_t GetColumnIncrement() const { return this->GetIncrement(); }

        /// @}

    protected:
        void Swap(MatrixBase<ElementType, MatrixLayout::columnMajor>& other);
        static constexpr VectorOrientation _intervalOrientation = VectorOrientation::column;
    };

    /// <summary> Base class for row major rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template <typename ElementType>
    class MatrixBase<ElementType, MatrixLayout::rowMajor> : public CommonMatrixBase<ElementType>
    {
    public:
        using CommonMatrixBase<ElementType>::CommonMatrixBase;

        /// <summary> Constructs a row major instance of MatrixBase> </summary>
        ///
        /// <param name="pData"> (Optional) Pointer to the data. </param>
        /// <param name="numRows"> Number of rows. </param>
        /// <param name="numColumns"> Number of columns. </param>
        MatrixBase(const ElementType* pData, size_t numRows, size_t numColumns);

        /// \name Accessor Functions
        /// @{

        /// <summary> Gets the minor size of the matrix (number of columns of a column major matrix or number of rows of a row major matrix). </summary>
        ///
        /// <returns> The minor size. </returns>
        size_t GetMinorSize() const { return this->NumRows(); }

        /// <summary> Gets the major size of a matrix (size of each column of a column major matrix or size of each row of a row major matrix). </summary>
        ///
        /// <returns> The major size. </returns>
        size_t GetMajorSize() const { return this->NumColumns(); }

        /// <summary> Gets the row increment. </summary>
        ///
        /// <returns> The row increment. </returns>
        size_t GetRowIncrement() const { return this->GetIncrement(); }

        /// <summary> Gets the column increment. </summary>
        ///
        /// <returns> The column increment. </returns>
        size_t GetColumnIncrement() const { return 1; }

        /// @}

    protected:
        void Swap(MatrixBase<ElementType, MatrixLayout::rowMajor>& other);
        static constexpr VectorOrientation _intervalOrientation = VectorOrientation::row;
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

        /// \name Accessor Functions
        /// @{

        /// <summary> Gets the matrix layout. </summary>
        ///
        /// <returns> The matrix layout. </returns>
        MatrixLayout GetLayout() const { return layout; }

        /// <summary> Matrix element access operator. </summary>
        ///
        /// <returns> A copy of the element in a given position. </returns>
        inline ElementType operator()(size_t rowIndex, size_t columnIndex) const;

        /// <summary> Gets a const pointer to the underlying data storage. </summary>
        ///
        /// <returns> Const pointer to the data. </returns>
        const ElementType* GetConstDataPointer() const { return this->_pData; }

        /// @}

        /// \name Utility Functions
        /// @{

        /// <summary> Returns a copy of the contents of the Matrix. </summary>
        ///
        /// <returns> A std::vector with a copy of the contents of the Matrix. </returns>
        std::vector<ElementType> ToArray() const;

        /// <summary> Swaps the contents of this matrix with the contents of another matrix. </summary>
        ///
        /// <param name="other"> [in,out] The other matrix. </param>
        void Swap(ConstMatrixReference<ElementType, layout>& other);

        /// <summary> Determines if this Matrix is stored in contiguous memory. </summary>
        ///
        /// <returns> True if contiguous, false if not. </returns>
        bool IsContiguous() const { return this->GetIncrement() == this->GetMajorSize(); }

        /// @}

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
        ConstColumnVectorReference<ElementType> GetColumn(size_t index) const;

        /// <summary> Gets a const reference to a row of the matrix. </summary>
        ///
        /// <param name="index"> Zero-based row index. </param>
        ///
        /// <returns> A const reference to the matrix row. </returns>
        ConstRowVectorReference<ElementType> GetRow(size_t index) const;

        /// <summary> Gets a const reference to the matrix diagonal as a column vector. </summary>
        ///
        /// <returns> A const reference to the matrix diagnoal. </returns>
        ConstColumnVectorReference<ElementType> GetDiagonal() const;

        /// <summary> Gets a constant reference to a row of a row major matrix or to a column of a column major matrix. </summary>
        ///
        /// <param name="index"> The interval index. </param>
        ///
        /// <returns> Constant reference to the interval. </returns>
        auto GetMajorVector(size_t index) const
        {
            // STYLE intentional deviation from project style
            return ConstVectorReference<ElementType, MatrixBase<ElementType, layout>::_intervalOrientation>(this->GetMajorVectorBegin(index), this->GetMajorSize(), 1);
        }

        /// <summary> Gets a reference to the transposed matrix. </summary>
        ///
        /// <returns> A reference to the transposed matrix. </returns>
        auto Transpose() const -> ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value>;

        /// <summary> Returns a vew of this matrix as a vector reference (requires the matrix to be contiguous). </summary>
        ///
        /// <returns> A ConstVectorReference. </returns>
        ConstColumnVectorReference<ElementType> ReferenceAsVector() const;

        /// @}

    protected:
        friend class ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value>;
        const ElementType* GetMajorVectorBegin(size_t index) const;
    };

    /// <summary> Non-const reference to a dense matrix. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    template <typename ElementType, MatrixLayout layout>
    class MatrixReference : public ConstMatrixReference<ElementType, layout>
    {
    public:
        /// <summary> Constructs an instance of MatrixReference. </summary>
        ///
        /// <param name="pData"> Pointer to the underlying std::vector that contains the tensor data. </param>
        /// <param name="numRows"> Number of matrix rows. </param>
        /// <param name="numColumns"> Number of matrix columns. </param>
        /// <param name="increment"> The matrix increment. </param>
        MatrixReference(ElementType* pData, size_t numRows, size_t numColumns, size_t increment);

        /// <summary> Constructs an instance of MatrixReference. </summary>
        ///
        /// <param name="pData"> Pointer to the underlying std::vector that contains the tensor data. </param>
        /// <param name="numRows"> Number of matrix rows. </param>
        /// <param name="numColumns"> Number of matrix columns. </param>
        MatrixReference(ElementType* pData, size_t numRows, size_t numColumns);

        /// \name Accessor Functions
        /// @{

        using ConstMatrixReference<ElementType, layout>::operator();

        /// <summary> Matrix element access operator. </summary>
        ///
        /// <returns> A reference to an element in a given position. </returns>
        inline ElementType& operator()(size_t rowIndex, size_t columnIndex);

        /// <summary> Gets a pointer to the underlying data storage. </summary>
        ///
        /// <returns> Pointer to the data. </returns>
        ElementType* GetDataPointer() { return const_cast<ElementType*>(this->_pData); }

        /// @}

        /// \name Utility Functions
        /// @{

        using ConstMatrixReference<ElementType, layout>::IsContiguous;

        /// <summary> Copies values from another matrix into this matrix. </summary>
        ///
        /// <param name="other"> The other matrix. </param>
        void CopyFrom(ConstMatrixReference<ElementType, layout> other);

        /// <summary> Copies values from another matrix into this matrix. </summary>
        ///
        /// <param name="other"> The other matrix. </param>
        void CopyFrom(ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value> other);

        /// <summary> Swaps the contents of this matrix with the contents of another matrix. </summary>
        ///
        /// <param name="other"> [in,out] The other matrix. </param>
        void Swap(MatrixReference<ElementType, layout>& other);

        /// @}

        /// \name  Content Manipulation Functions
        /// @{

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

        using ConstMatrixReference<ElementType, layout>::GetSubMatrix;
        using ConstMatrixReference<ElementType, layout>::GetColumn;
        using ConstMatrixReference<ElementType, layout>::GetRow;
        using ConstMatrixReference<ElementType, layout>::GetDiagonal;
        using ConstMatrixReference<ElementType, layout>::GetMajorVector;
        using ConstMatrixReference<ElementType, layout>::Transpose;
        using ConstMatrixReference<ElementType, layout>::ReferenceAsVector;

        /// <summary> Gets a reference to this matrix. </summary>
        ///
        /// <returns> A reference to this matrix. </returns>
        MatrixReference<ElementType, layout> GetReference() { return *this; }

        /// <summary> Gets a reference to the transposed matrix. </summary>
        ///
        /// <returns> A reference to the transposed matrix. </returns>
        auto Transpose() -> MatrixReference<ElementType, TransposeMatrixLayout<layout>::value>;

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
        ColumnVectorReference<ElementType> GetColumn(size_t index);

        /// <summary> Gets a reference to a row of the matrix. </summary>
        ///
        /// <param name="index"> Zero-based row index. </param>
        ///
        /// <returns> A reference to the matrix row. </returns>
        RowVectorReference<ElementType> GetRow(size_t index);

        /// <summary> Gets a reference to the matrix diagonal as a column vector. </summary>
        ///
        /// <returns> A const reference to the matrix diagnoal. </returns>
        ColumnVectorReference<ElementType> GetDiagonal();

        /// <summary> Returns a vew of this matrix as a vector reference (requires the matrix to be contiguous). </summary>
        ///
        /// <returns> A VectorReference. </returns>
        ColumnVectorReference<ElementType> ReferenceAsVector();

        /// <summary> Gets a reference to a row of a row major matrix or to a column of a column major matrix. </summary>
        ///
        /// <param name="index"> The interval index. </param>
        ///
        /// <returns> Reference to the interval. </returns>
        auto GetMajorVector(size_t index)
        {
            // STYLE intentional deviation from project style
            return VectorReference<ElementType, MatrixBase<ElementType, layout>::_intervalOrientation>(this->GetMajorVectorBegin(index), this->GetMajorSize(), 1);
        }

        /// @}

    protected:
        friend MatrixReference<ElementType, TransposeMatrixLayout<layout>::value>;
    };

    /// <summary> A dense matrix. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    /// <typeparam name="layout"> Matrix layout. </typeparam>
    template <typename ElementType, MatrixLayout layout>
    class Matrix : public MatrixReference<ElementType, layout>
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
        Matrix(Matrix<ElementType, layout>&& other);

        /// <summary> Copy Constructor. </summary>
        ///
        /// <param name="other"> The matrix being copied. </param>
        Matrix(ConstMatrixReference<ElementType, layout>& other);

        /// <summary> Copy Constructor. </summary>
        ///
        /// <param name="other"> The matrix being copied. </param>
        Matrix(const Matrix<ElementType, layout>& other);

        /// <summary> Copies a matrix of the opposite layout. </summary>
        ///
        /// <param name="other"> The matrix being copied. </param>
        Matrix(ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value> other);

        /// \name Utility Functions
        /// @{

        /// <summary> Assignment operator. </summary>
        ///
        /// <param name="other"> The other matrix. </param>
        ///
        /// <returns> A reference to this matrix. </returns>
        Matrix<ElementType, layout>& operator=(Matrix<ElementType, layout> other);

        /// <summary> Returns a copy of the contents of the Matrix. </summary>
        ///
        /// <returns> A std::vector with a copy of the contents of the Matrix. </returns>
        std::vector<ElementType> ToArray() const { return _data; }

        /// <summary> Swaps the contents of this matrix with the contents of another matrix. </summary>
        ///
        /// <param name="other"> [in,out] The other matrix. </param>
        void Swap(Matrix<ElementType, layout>& other);

        /// @}

    private:
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
        static std::string GetRowsName(const std::string& name) { return name + "_rows"; } // STYLE discrepancy
        static std::string GetColumnsName(const std::string& name) { return name + "_columns"; } // STYLE discrepancy
        static std::string GetValuesName(const std::string& name) { return name + "_values"; } // STYLE discrepancy
    };

    //
    // friendly names
    //
    template <typename ElementType>
    using ColumnMatrix = Matrix<ElementType, MatrixLayout::columnMajor>;

    template <typename ElementType>
    using ColumnMatrixReference = MatrixReference<ElementType, MatrixLayout::columnMajor>;

    template <typename ElementType>
    using ConstColumnMatrixReference = ConstMatrixReference<ElementType, MatrixLayout::columnMajor>;

    template <typename ElementType>
    using RowMatrix = Matrix<ElementType, MatrixLayout::rowMajor>;

    template <typename ElementType>
    using RowMatrixReference = MatrixReference<ElementType, MatrixLayout::rowMajor>;

    template <typename ElementType>
    using ConstRowMatrixReference = ConstMatrixReference<ElementType, MatrixLayout::rowMajor>;
}
}

#include "../tcc/Matrix.tcc"
