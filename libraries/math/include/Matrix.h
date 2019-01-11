////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:    Matrix.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Vector.h"

#include <utilities/include/IArchivable.h>

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
        CommonMatrixBase(const ElementType* pData, size_t numRows, size_t numColumns, size_t increment);

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
        MatrixBase(const ElementType* pData, size_t numRows, size_t numColumns);
        MatrixBase(const ElementType* pData, size_t numRows, size_t numColumns, size_t increment);
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
        MatrixBase(const ElementType* pData, size_t numRows, size_t numColumns);
        MatrixBase(const ElementType* pData, size_t numRows, size_t numColumns, size_t increment);
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
        /// <summary> Constructs an instance of ConstMatrixReference. </summary>
        ///
        /// <param name="pData"> (Optional) Pointer to the data. </param>
        /// <param name="numRows"> Number of rows. </param>
        /// <param name="numColumns"> Number of columns. </param>
        ConstMatrixReference(const ElementType* pData, size_t numRows, size_t numColumns);

        /// <summary> Constructs an instance of ConstMatrixReference. </summary>
        ///
        /// <param name="pData"> Pointer to the underlying memory that contains the tensor data. </param>
        /// <param name="numRows"> Number of matrix rows. </param>
        /// <param name="numColumns"> Number of matrix columns. </param>
        /// <param name="increment"> The matrix increment. </param>
        ConstMatrixReference(const ElementType* pData, size_t numRows, size_t numColumns, size_t increment);

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
} // namespace math
} // namespace ell

#pragma region implementation

#include "VectorOperations.h"

#include <utilities/include/Debug.h>
#include <utilities/include/Exception.h>
#include <utilities/include/Unused.h>

#include <algorithm>

namespace ell
{
namespace math
{
    //
    // CommonMatrixBase
    //

    template <typename ElementType>
    CommonMatrixBase<ElementType>::CommonMatrixBase(const ElementType* pData, size_t numRows, size_t numColumns, size_t increment) :
        _pData(pData),
        _numRows(numRows),
        _numColumns(numColumns),
        _increment(increment)
    {
    }

    template <typename ElementType>
    void CommonMatrixBase<ElementType>::Swap(CommonMatrixBase<ElementType>& other)
    {
        using std::swap;
        swap(_pData, other._pData);
        swap(_numRows, other._numRows);
        swap(_numColumns, other._numColumns);
        swap(_increment, other._increment);
    }

    //
    // MatrixBase
    //

    // Row-major
    template <typename ElementType>
    MatrixBase<ElementType, MatrixLayout::rowMajor>::MatrixBase(const ElementType* pData, size_t numRows, size_t numColumns) :
        CommonMatrixBase<ElementType>(pData, numRows, numColumns, numColumns)
    {
    }

    template <typename ElementType>
    MatrixBase<ElementType, MatrixLayout::rowMajor>::MatrixBase(const ElementType* pData, size_t numRows, size_t numColumns, size_t increment) :
        CommonMatrixBase<ElementType>(pData, numRows, numColumns, increment)
    {
    }

    template <typename ElementType>
    void MatrixBase<ElementType, MatrixLayout::rowMajor>::Swap(MatrixBase<ElementType, MatrixLayout::rowMajor>& other)
    {
        CommonMatrixBase<ElementType>::Swap(other);
    }

    // Column-major
    template <typename ElementType>
    MatrixBase<ElementType, MatrixLayout::columnMajor>::MatrixBase(const ElementType* pData, size_t numRows, size_t numColumns) :
        CommonMatrixBase<ElementType>(pData, numRows, numColumns, numRows)
    {
    }

    template <typename ElementType>
    MatrixBase<ElementType, MatrixLayout::columnMajor>::MatrixBase(const ElementType* pData, size_t numRows, size_t numColumns, size_t increment) :
        CommonMatrixBase<ElementType>(pData, numRows, numColumns, increment)
    {
    }

    template <typename ElementType>
    void MatrixBase<ElementType, MatrixLayout::columnMajor>::Swap(MatrixBase<ElementType, MatrixLayout::columnMajor>& other)
    {
        CommonMatrixBase<ElementType>::Swap(other);
    }

    //
    // ConstMatrixReference
    //
    template <typename ElementType, MatrixLayout layout>
    ConstMatrixReference<ElementType, layout>::ConstMatrixReference(const ElementType* pData, size_t numRows, size_t numColumns, size_t increment) :
        MatrixBase<ElementType, layout>(pData, numRows, numColumns, increment)
    {
    }

    template <typename ElementType, MatrixLayout layout>
    ConstMatrixReference<ElementType, layout>::ConstMatrixReference(const ElementType* pData, size_t numRows, size_t numColumns) :
        MatrixBase<ElementType, layout>(pData, numRows, numColumns)
    {
    }

    template <typename ElementType, MatrixLayout layout>
    ElementType ConstMatrixReference<ElementType, layout>::operator()(size_t rowIndex, size_t columnIndex) const
    {
        using namespace std::string_literals;
        using std::to_string;
        DEBUG_THROW(rowIndex >= this->NumRows() || columnIndex >= this->NumColumns(),
                    utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange,
                                              "("s + to_string(rowIndex) + ", " + to_string(columnIndex) + ") exceeds matrix dimensions (" +
                                                  to_string(this->NumRows()) + " x " + to_string(this->NumColumns()) + ")."));

        return GetConstDataPointer()[rowIndex * this->GetRowIncrement() + columnIndex * this->GetColumnIncrement()];
    }

    template <typename ElementType, MatrixLayout layout>
    std::vector<ElementType> ConstMatrixReference<ElementType, layout>::ToArray() const
    {
        std::vector<ElementType> v(this->Size());
        auto vIterator = v.begin();
        for (size_t i = 0; i < this->GetMinorSize(); ++i)
        {
            auto pIntervalData = GetMajorVectorBegin(i);
            std::copy(pIntervalData, pIntervalData + this->GetMajorSize(), vIterator);
            vIterator += this->GetMajorSize();
        }
        return v;
    }

    template <typename ElementType, MatrixLayout layout>
    void ConstMatrixReference<ElementType, layout>::Swap(ConstMatrixReference<ElementType, layout>& other)
    {
        MatrixBase<ElementType, layout>::Swap(other);
    }

    template <typename ElementType, MatrixLayout layout>
    bool ConstMatrixReference<ElementType, layout>::IsEqual(ConstMatrixReference<ElementType, layout> other, ElementType tolerance) const
    {
        if (this->NumRows() != other.NumRows() || this->NumColumns() != other.NumColumns())
        {
            return false;
        }

        for (size_t i = 0; i < this->GetMinorSize(); ++i)
        {
            if (!GetMajorVector(i).IsEqual(other.GetMajorVector(i), tolerance))
            {
                return false;
            }
        }
        return true;
    }

    template <typename ElementType, MatrixLayout layout>
    bool ConstMatrixReference<ElementType, layout>::IsEqual(ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value> other, ElementType tolerance) const
    {
        if (this->NumRows() != other.NumRows() || this->NumColumns() != other.NumColumns())
        {
            return false;
        }

        for (size_t i = 0; i < this->NumRows(); ++i)
        {
            if (!GetRow(i).IsEqual(other.GetRow(i), tolerance))
            {
                return false;
            }
        }
        return true;
    }

    template <typename ElementType, MatrixLayout layout>
    bool ConstMatrixReference<ElementType, layout>::operator==(const ConstMatrixReference<ElementType, layout>& other) const
    {
        return IsEqual(other);
    }

    template <typename ElementType, MatrixLayout layout>
    bool ConstMatrixReference<ElementType, layout>::operator==(const ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value>& other) const
    {
        return IsEqual(other);
    }

    template <typename ElementType, MatrixLayout layout>
    template <MatrixLayout otherLayout>
    bool ConstMatrixReference<ElementType, layout>::operator!=(const ConstMatrixReference<ElementType, otherLayout>& other)
    {
        return !(*this == other);
    }

    template <typename ElementType, MatrixLayout layout>
    ConstMatrixReference<ElementType, layout> ConstMatrixReference<ElementType, layout>::GetSubMatrix(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns) const
    {
        DEBUG_THROW(firstRow + numRows > this->NumRows() || firstColumn + numColumns > this->NumColumns(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "block exceeds matrix dimensions."));

        return ConstMatrixReference<ElementType, layout>(GetConstDataPointer() + firstRow * this->GetRowIncrement() + firstColumn * this->GetColumnIncrement(), numRows, numColumns, this->GetIncrement());
    }

    template <typename ElementType, MatrixLayout layout>
    ConstColumnVectorReference<ElementType> ConstMatrixReference<ElementType, layout>::GetColumn(size_t index) const
    {
        DEBUG_THROW(index >= this->NumColumns(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "column index exceeds matrix dimensions."));

        return ConstColumnVectorReference<ElementType>(GetConstDataPointer() + index * this->GetColumnIncrement(), this->NumRows(), this->GetRowIncrement());
    }

    template <typename ElementType, MatrixLayout layout>
    ConstRowVectorReference<ElementType> ConstMatrixReference<ElementType, layout>::GetRow(size_t index) const
    {
        DEBUG_THROW(index >= this->NumRows(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "row index exceeds matrix dimensions."));

        return ConstRowVectorReference<ElementType>(GetConstDataPointer() + index * this->GetRowIncrement(), this->NumColumns(), this->GetColumnIncrement());
    }

    template <typename ElementType, MatrixLayout layout>
    ConstColumnVectorReference<ElementType> ConstMatrixReference<ElementType, layout>::GetDiagonal() const
    {
        auto size = std::min(this->NumColumns(), this->NumRows());
        return ConstColumnVectorReference<ElementType>(GetConstDataPointer(), size, this->GetIncrement() + 1);
    }

    template <typename ElementType, MatrixLayout layout>
    ConstColumnVectorReference<ElementType> ConstMatrixReference<ElementType, layout>::ReferenceAsVector() const
    {
        DEBUG_THROW(!IsContiguous(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Can only flatten a matrix when its memory is contiguous"));
        return ConstColumnVectorReference<ElementType>(GetConstDataPointer(), this->NumRows() * this->NumColumns(), 1);
    }

    template <typename ElementType, MatrixLayout layout>
    auto ConstMatrixReference<ElementType, layout>::Transpose() const -> ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value>
    {
        return ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value>(GetConstDataPointer(), this->NumColumns(), this->NumRows(), this->GetIncrement());
    }

    template <typename ElementType, MatrixLayout layout>
    const ElementType* ConstMatrixReference<ElementType, layout>::GetMajorVectorBegin(size_t index) const
    {
        return GetConstDataPointer() + index * this->GetIncrement();
    }

    //
    // MatrixReference
    //

    template <typename ElementType, MatrixLayout layout>
    MatrixReference<ElementType, layout>::MatrixReference(ElementType* pData, size_t numRows, size_t numColumns, size_t increment) :
        ConstMatrixReference<ElementType, layout>(pData, numRows, numColumns, increment)
    {
    }

    template <typename ElementType, MatrixLayout layout>
    MatrixReference<ElementType, layout>::MatrixReference(ElementType* pData, size_t numRows, size_t numColumns) :
        ConstMatrixReference<ElementType, layout>(pData, numRows, numColumns)
    {
    }

    template <typename ElementType, MatrixLayout layout>
    ElementType& MatrixReference<ElementType, layout>::operator()(size_t rowIndex, size_t columnIndex)
    {
        using namespace std::string_literals;
        using std::to_string;
        DEBUG_THROW(rowIndex >= this->NumRows() || columnIndex >= this->NumColumns(),
                    utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange,
                                              "("s + to_string(rowIndex) + ", " + to_string(columnIndex) + ") exceeds matrix dimensions (" +
                                                  to_string(this->NumRows()) + " x " + to_string(this->NumColumns()) + ")."));

        return GetDataPointer()[rowIndex * this->GetRowIncrement() + columnIndex * this->GetColumnIncrement()];
    }

    template <typename ElementType, MatrixLayout layout>
    void MatrixReference<ElementType, layout>::CopyFrom(ConstMatrixReference<ElementType, layout> other)
    {
        if (this->NumRows() != other.NumRows() || this->NumColumns() != other.NumColumns())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Matrix dimensions are not the same.");
        }

        for (size_t i = 0; i < other.GetMinorSize(); ++i)
        {
            GetMajorVector(i).CopyFrom(other.GetMajorVector(i));
        }
    }

    template <typename ElementType, MatrixLayout layout>
    void MatrixReference<ElementType, layout>::CopyFrom(ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value> other)
    {
        if (this->NumRows() != other.NumRows() || this->NumColumns() != other.NumColumns())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Matrix dimensions are not the same.");
        }

        for (size_t i = 0; i < other.NumRows(); ++i)
        {
            GetRow(i).CopyFrom(other.GetRow(i));
        }
    }

    template <typename ElementType, MatrixLayout layout>
    void MatrixReference<ElementType, layout>::Swap(MatrixReference<ElementType, layout>& other)
    {
        ConstMatrixReference<ElementType, layout>::Swap(other);
    }

    template <typename ElementType, MatrixLayout layout>
    void MatrixReference<ElementType, layout>::Fill(ElementType value)
    {
        for (size_t i = 0; i < this->GetMinorSize(); ++i)
        {
            auto vector = GetMajorVector(i);
            vector.Fill(value);
        }
    }

    template <typename ElementType, MatrixLayout layout>
    template <typename GeneratorType>
    void MatrixReference<ElementType, layout>::Generate(GeneratorType generator)
    {
        for (size_t i = 0; i < this->GetMinorSize(); ++i)
        {
            GetMajorVector(i).Generate(generator);
        }
    }

    template <typename ElementType, MatrixLayout layout>
    template <typename TransformationType>
    void MatrixReference<ElementType, layout>::Transform(TransformationType transformation)
    {
        for (size_t i = 0; i < this->GetMinorSize(); ++i)
        {
            TransformUpdate(transformation, GetMajorVector(i));
        }
    }

    template <typename ElementType, MatrixLayout layout>
    auto MatrixReference<ElementType, layout>::Transpose() -> MatrixReference<ElementType, TransposeMatrixLayout<layout>::value>
    {
        return MatrixReference<ElementType, TransposeMatrixLayout<layout>::value>(GetDataPointer(), this->NumColumns(), this->NumRows(), this->GetIncrement());
    }

    template <typename ElementType, MatrixLayout layout>
    MatrixReference<ElementType, layout> MatrixReference<ElementType, layout>::GetSubMatrix(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns)
    {
        DEBUG_THROW(firstRow + numRows > this->NumRows() || firstColumn + numColumns > this->NumColumns(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "block exceeds matrix dimensions."));

        return MatrixReference<ElementType, layout>(GetDataPointer() + firstRow * this->GetRowIncrement() + firstColumn * this->GetColumnIncrement(), numRows, numColumns, this->GetIncrement());
    }

    template <typename ElementType, MatrixLayout layout>
    ColumnVectorReference<ElementType> MatrixReference<ElementType, layout>::GetColumn(size_t index)
    {
        DEBUG_THROW(index >= this->NumColumns(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "column index exceeds matrix dimensions."));

        return ColumnVectorReference<ElementType>(GetDataPointer() + index * this->GetColumnIncrement(), this->NumRows(), this->GetRowIncrement());
    }

    template <typename ElementType, MatrixLayout layout>
    RowVectorReference<ElementType> MatrixReference<ElementType, layout>::GetRow(size_t index)
    {
        DEBUG_THROW(index >= this->NumRows(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "row index exceeds matrix dimensions."));

        return RowVectorReference<ElementType>(GetDataPointer() + index * this->GetRowIncrement(), this->NumColumns(), this->GetColumnIncrement());
    }

    template <typename ElementType, MatrixLayout layout>
    ColumnVectorReference<ElementType> MatrixReference<ElementType, layout>::GetDiagonal()
    {
        auto size = std::min(this->NumColumns(), this->NumRows());
        return ColumnVectorReference<ElementType>(GetDataPointer(), size, this->GetIncrement() + 1);
    }

    template <typename ElementType, MatrixLayout layout>
    ColumnVectorReference<ElementType> MatrixReference<ElementType, layout>::ReferenceAsVector()
    {
        DEBUG_THROW(!IsContiguous(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Can only flatten a matrix when its memory is contiguous"));
        return ColumnVectorReference<ElementType>(GetDataPointer(), this->NumRows() * this->NumColumns(), 1);
    }

    //
    // Matrix
    //

    template <typename ElementType, MatrixLayout layout>
    Matrix<ElementType, layout>::Matrix(size_t numRows, size_t numColumns) :
        MatrixReference<ElementType, layout>(nullptr, numRows, numColumns),
        _data(numRows * numColumns)
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, MatrixLayout layout>
    Matrix<ElementType, layout>::Matrix(std::initializer_list<std::initializer_list<ElementType>> list) :
        MatrixReference<ElementType, layout>(nullptr, list.size(), list.begin()->size()),
        _data(list.size() * list.begin()->size())
    {
        this->_pData = _data.data();
        auto numColumns = list.begin()->size();
        DEBUG_USED(numColumns);

        size_t i = 0;
        for (auto rowIter = list.begin(); rowIter < list.end(); ++rowIter)
        {
            DEBUG_THROW(rowIter->size() != numColumns, utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "incorrect number of elements in initializer list"));

            size_t j = 0;
            for (auto elementIter = rowIter->begin(); elementIter < rowIter->end(); ++elementIter)
            {
                (*this)(i, j) = *elementIter;
                ++j;
            }
            ++i;
        }
    }

    template <typename ElementType, MatrixLayout layout>
    Matrix<ElementType, layout>::Matrix(size_t numRows, size_t numColumns, const std::vector<ElementType>& data) :
        MatrixReference<ElementType, layout>(nullptr, numRows, numColumns),
        _data(data)
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, MatrixLayout layout>
    Matrix<ElementType, layout>::Matrix(size_t numRows, size_t numColumns, std::vector<ElementType>&& data) :
        MatrixReference<ElementType, layout>(nullptr, numRows, numColumns),
        _data(std::move(data))
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, MatrixLayout layout>
    Matrix<ElementType, layout>::Matrix(Matrix<ElementType, layout>&& other) :
        MatrixReference<ElementType, layout>(nullptr, other.NumRows(), other.NumColumns()),
        _data(std::move(other._data))
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, MatrixLayout layout>
    Matrix<ElementType, layout>::Matrix(const Matrix<ElementType, layout>& other) :
        MatrixReference<ElementType, layout>(nullptr, other.NumRows(), other.NumColumns()),
        _data(other._data)
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, MatrixLayout layout>
    Matrix<ElementType, layout>::Matrix(ConstMatrixReference<ElementType, layout>& other) :
        MatrixReference<ElementType, layout>(nullptr, other.NumRows(), other.NumColumns()),
        _data(other.NumRows() * other.NumColumns())
    {
        this->_pData = _data.data();
        for (size_t i = 0; i < this->NumRows(); ++i)
        {
            for (size_t j = 0; j < this->NumColumns(); ++j)
            {
                (*this)(i, j) = other(i, j);
            }
        }
    }

    template <typename ElementType, MatrixLayout layout>
    Matrix<ElementType, layout>::Matrix(ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value> other) :
        MatrixReference<ElementType, layout>(nullptr, other.NumRows(), other.NumColumns()),
        _data(other.NumRows() * other.NumColumns())
    {
        this->_pData = _data.data();
        for (size_t i = 0; i < this->NumRows(); ++i)
        {
            for (size_t j = 0; j < this->NumColumns(); ++j)
            {
                (*this)(i, j) = other(i, j);
            }
        }
    }

    template <typename ElementType, MatrixLayout layout>
    Matrix<ElementType, layout>& Matrix<ElementType, layout>::operator=(Matrix<ElementType, layout> other)
    {
        Swap(other);
        return *this;
    }

    template <typename ElementType, MatrixLayout layout>
    void Matrix<ElementType, layout>::Swap(Matrix<ElementType, layout>& other)
    {
        MatrixReference<ElementType, layout>::Swap(other);
        std::swap(_data, other._data);
    }

    template <typename ElementType, MatrixLayout layout>
    void MatrixArchiver::Write(const Matrix<ElementType, layout>& matrix, const std::string& name, utilities::Archiver& archiver)
    {
        archiver[GetRowsName(name)] << matrix.NumRows();
        archiver[GetColumnsName(name)] << matrix.NumColumns();
        archiver[GetValuesName(name)] << matrix.ToArray();
    }

    template <typename ElementType, MatrixLayout layout>
    void MatrixArchiver::Read(Matrix<ElementType, layout>& matrix, const std::string& name, utilities::Unarchiver& archiver)
    {
        size_t rows = 0;
        size_t columns = 0;
        std::vector<ElementType> values;

        archiver[GetRowsName(name)] >> rows;
        archiver[GetColumnsName(name)] >> columns;
        archiver[GetValuesName(name)] >> values;

        Matrix<ElementType, layout> value(rows, columns, std::move(values));

        matrix = std::move(value);
    }
} // namespace math
} // namespace ell

#pragma endregion implementation
