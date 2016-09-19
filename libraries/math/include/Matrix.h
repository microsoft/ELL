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

    /// <summary> A helper class that constructs vector reference objects. </summary>
    class VectorReferenceConstructor
    {
    protected:
        /// <summary> Constructs a vector reference. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="Orientation"> Vector orientation. </typeparam>
        /// <param name="pData"> Pointer to the first element in the vector. </param>
        /// <param name="size"> The vector size. </param>
        /// <param name="increment"> The vector increment. </param>
        ///
        /// <returns> A vector reference; </returns>
        template<typename ElementType, VectorOrientation Orientation>
        VectorReference<ElementType, Orientation> ConstructVectorReference(ElementType* pData, size_t size, size_t increment);

        /// <summary> Constructs a const vector reference. </summary>
        ///
        /// <typeparam name="ElementType"> Vector element type. </typeparam>
        /// <typeparam name="Orientation"> Vector orientation. </typeparam>
        /// <param name="pData"> Pointer to the first element in the vector. </param>
        /// <param name="size"> The vector size. </param>
        /// <param name="increment"> The vector increment. </param>
        ///
        /// <returns> A const vector reference. </returns>
        template<typename ElementType, VectorOrientation Orientation>
        ConstVectorReference<ElementType, Orientation> ConstructConstVectorReference(ElementType* pData, size_t size, size_t increment) const;
    };

    /// <summary> Base class for rectangular dense matrices. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix Element type. </typeparam>
    template <typename ElementType>
    class RectangularMatrixBase : public VectorReferenceConstructor
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

        static constexpr MatrixLayout transposeLayout = MatrixLayout::rowMajor;
        static constexpr size_t _rowIncrement = 1;
        const size_t _numIntervals = _numColumns;
        const size_t _intervalLength = _numRows;
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

        static constexpr MatrixLayout transposeLayout = MatrixLayout::columnMajor;
        const size_t _numIntervals = _numRows;
        const size_t _intervalLength = _numColumns;
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
        auto Transpose() const -> ConstMatrixReference<ElementType, MatrixBase<ElementType, Layout>::transposeLayout>;

        /// <summary> Gets a constant reference to a block-shaped sub-matrix. </summary>
        ///
        /// <param name="firstRow"> The first row in the block. </param>
        /// <param name="firstColumn"> The first column in the block. </param>
        /// <param name="numRows"> Number of rows in the block. </param>
        /// <param name="numColumns"> Number of columns in the block. </param>
        ///
        /// <returns> The constant reference to a block. </returns>
        ConstMatrixReference<ElementType, Layout> GetBlock(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns) const;

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

        /// <summary> Gets a const reference to the matrix diagonal. </summary>
        ///
        /// <typeparam name="Orientation"> User must specify whether the return value is a column or row vector. </typeparam>
        ///
        /// <returns> A const reference to the matrix diagnoal. </returns>
        template<VectorOrientation Orientation>
        ConstVectorReference<ElementType, Orientation> GetDiagonal() const;

        /// <summary> Equality operator. </summary>
        ///
        /// <typeparam name="OtherLayout"> The layout of the other matrix. </typeparam>
        /// <param name="other"> The other matrix. </param>
        ///
        /// <returns> true if the two matrices are equivalent. </returns>
        template<MatrixLayout OtherLayout>
        bool operator==(const ConstMatrixReference<ElementType, OtherLayout>& other) const;

        /// <summary> Inequality operator. </summary>
        ///
        /// <typeparam name="OtherLayout"> The layout of the other matrix. </typeparam>
        /// <param name="other"> The other matrix. </param>
        ///
        /// <returns> true if the two matrices are not equivalent. </returns>
        template<MatrixLayout OtherLayout>
        bool operator !=(const ConstMatrixReference<ElementType, OtherLayout>& other);

    protected:
        friend class ConstMatrixReference<ElementType, MatrixBase<ElementType, Layout>::transposeLayout>;
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
        auto Transpose() const->MatrixReference<ElementType, MatrixBase<ElementType, Layout>::transposeLayout>;

        /// <summary> Gets a const reference to a block-shaped sub-matrix. </summary>
        ///
        /// <param name="firstRow"> The first row in the block. </param>
        /// <param name="firstColumn"> The first column in the block. </param>
        /// <param name="numRows"> Number of rows in the block. </param>
        /// <param name="numColumns"> Number of columns in the block. </param>
        ///
        /// <returns> The constant reference to a block. </returns>
        MatrixReference<ElementType, Layout> GetBlock(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns);

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

        /// <summary> Gets a reference to the matrix diagonal. </summary>
        ///
        /// <typeparam name="Orientation"> User must specify whether the return value is a column or row vector. </typeparam>
        ///
        /// <returns> A const reference to the matrix diagnoal. </returns>
        template<VectorOrientation Orientation>
        VectorReference<ElementType, Orientation> GetDiagonal();

        /// <summary> Applies an operation to each row of the matrix. </summary>
        ///
        /// <typeparam name="MapperType"> A mapper type, which is a functor that takes a row oriented VectorReference and returns void. </typeparam>
        /// <param name="mapper"> The mapper. </param>
        template<typename MapperType>
        void ForEachRow(MapperType mapper);

        /// <summary> Applies an operation to each column of the matrix. </summary>
        ///
        /// <typeparam name="MapperType"> A mapper type, which is a functor that takes a column oriented VectorReference and returns void. </typeparam>
        /// <param name="mapper"> The mapper. </param>
        template<typename MapperType>
        void ForEachColumn(MapperType mapper);

    protected:
        friend MatrixReference<ElementType, MatrixBase<ElementType, Layout>::transposeLayout>;
        using ConstMatrixReference<ElementType, Layout>::ConstMatrixReference;
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
