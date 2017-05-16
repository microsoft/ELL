////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Tensor.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Matrix.h"
#include "Vector.h"

// stl
#include <cmath>
#include <iostream>
#include <vector>
#include <array>

// utilities
#include "Debug.h"
#include "Exception.h"

namespace ell
{
namespace math
{
    /// <summary> Enum that represents the dimensions of a tensor. </summary>
    enum class Dimension{ row, column, channel };

    // abbreviations
    using Triplet = std::array<size_t, 3>;

    /// <summary>
    /// Forward declaration of TensorLayout. This class helps the tensor move back and forth from canonical coordinates (row, column, channel)
    /// to the layout coordinates (which depend on how the tensor is layed out in memory)
    /// </summary>
    ///
    /// <typeparam name="dimension0"> Identity of the tensor dimension that occupies contiguous memory
    /// (increment of 1). </typeparam>
    /// <typeparam name="dimension1"> Identity of the tensor dimension with a minor memory increment. </typeparam>
    /// <typeparam name="dimension2"> Identity of the tensor dimension with a major memory increment. </typeparam>
    template<Dimension dimension0, Dimension dimension1, Dimension dimension2>
    struct TensorLayout; 

    /// <summary>
    /// Specialization to column/row/channel memory layout. This means that channels are stored
    /// one after the other and each channel is stored as a row-major matrix.
    /// </summary>
    template<>
    struct TensorLayout<Dimension::column, Dimension::row, Dimension::channel>
    {
        static Triplet CanonicalToLayout(Triplet canonical) { return{ canonical[1], canonical[0], canonical[2] }; }
        static const size_t rowPosition = 1;
        static const size_t columnPosition = 0;
        static const size_t channelPosition = 2;
    };

    /// <summary>
    /// Specialization to channel/column/row memory layout. For example, if the tensor represents an
    /// RGB image, the entries will be stored as R_00, G_00, B_00, R_01, G_01, B_01, ...
    /// </summary>
    template<>
    struct TensorLayout<Dimension::channel, Dimension::column, Dimension::row>
    {
        static Triplet CanonicalToLayout(Triplet canonical) { return{ canonical[2], canonical[1], canonical[0] }; }
        static const size_t rowPosition = 2;
        static const size_t columnPosition = 1;
        static const size_t channelPosition = 0;
    };

    /// <summary> Helper struct that incudes all of the TensorReference internals. </summary>
    ///
    /// <typeparam name="ElementType"> Tensor element type. </typeparam>
    template<typename ElementType>
    struct TensorContents
    {
        Triplet layout;
        std::array<size_t, 2> increments;
        ElementType* pData;
    };

    /// <summary>
    /// A const reference to a tensor. This class implements all the operations that do not modify tensor
    /// elements. A ConstTensorReference does not own its own memory.
    /// </summary>
    ///
    /// <typeparam name="ElementType"> Tensor element type. </typeparam>
    /// <typeparam name="dimension0"> Identity of the tensor dimension that occupies contiguous memory
    /// (increment of 1). </typeparam>
    /// <typeparam name="dimension1"> Identity of the tensor dimension with a minor memory increment. </typeparam>
    /// <typeparam name="dimension2"> Identity of the tensor dimension with a major memory increment. </typeparam>
    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    class ConstTensorReference 
    {
    public:
        /// <summary> Constructs an instance of ConstTensorReference. </summary>
        ///
        /// <param name="shape"> The tensor shape (always given in canonical coordinates: row, column, channel). </param>
        ConstTensorReference(Triplet shape);

        /// <summary> Gets a constant reference to this tensor. </summary>
        ///
        /// <returns> A constant tensor reference. </returns>
        ConstTensorReference GetConstTensorReference() const { return *this; }

        /// <summary> Gets the total number of elements in the tensor. </summary>
        ///
        /// <returns> The total number of elements. </returns>
        size_t NumElements() const;

        /// <summary> Gets the number of rows. </summary>
        ///
        /// <returns> The number of rows. </returns>
        size_t NumRows() const { return _contents.layout[TensorLayoutT::rowPosition]; }

        /// <summary> Gets the number of columns. </summary>
        ///
        /// <returns> The number of columns. </returns>
        size_t NumColumns() const { return _contents.layout[TensorLayoutT::columnPosition]; }

        /// <summary> Gets the number of channels. </summary>
        ///
        /// <returns> The number of channels. </returns>
        size_t NumChannels() const { return _contents.layout[TensorLayoutT::channelPosition]; }

        /// <summary> Gets the three sizes that make up the tensor's layout shape. </summary>
        ///
        /// <returns> The layout shape. </returns>
        Triplet GetLayout() const { return _contents.layout; }

        /// <summary> Gets the three dimenions of the tensor in canonical order (row, column, channel). </summary>
        ///
        /// <returns> The shape of the dimensions in canonical order. </returns>
        Triplet GetShape() const { return {NumRows(), NumColumns(), NumChannels()}; }

        /// <summary> Element access operator. </summary>
        ///
        /// <param name="row"> The row. </param>
        /// <param name="column"> The column. </param>
        /// <param name="channel"> The channel. </param>
        ///
        /// <returns> A copy of a tensor element. </returns>
        ElementType operator()(size_t row, size_t column, size_t channel) const;

        /// <summary> Element access operator. </summary>
        ///
        /// <param name="coordinate"> The coordinate to access. </param>
        ///
        /// <returns> A copy of a tensor element. </returns>
        ElementType operator()(Triplet coordinate) const;

        /// <summary> Determines if two tensors are equal. </summary>
        ///
        /// <param name="other"> The other tensor. </param>
        /// <param name="tolerance"> The element comparison tolerance. </param>
        ///
        /// <returns> true if the two tensors are equivalent. </returns>
        template<Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
        bool IsEqual(ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2> other, ElementType tolerance = 1.0e-8) const;

        /// <summary> Equality operator for tensors. </summary>
        ///
        /// <param name="other"> The other tensor. </param>
        ///
        /// <returns> true if the two tensors are equivalent. </returns>
        template<Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
        bool operator==(const ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2>& other) const;

        /// <summary> Flattens a tensor into a row vector. Works only when the tensor dimensions are full </summary>
        ///
        /// <returns> A ConstVectorReference that which includes all of the tensor elements. </returns>
        ConstVectorReference<ElementType, VectorOrientation::row> ReferenceAsVector() const;

        /// <summary>
        /// Returns a const matrix reference that includes all of the tensor elements, by flattening the first
        /// tensor dimension. Works only when the first dimension is full.
        /// </summary>
        ///
        /// <returns>
        /// A const reference to a row major matrix, which includes all of the tensor elements.
        /// </returns>
        ConstMatrixReference<ElementType, MatrixLayout::rowMajor> ReferenceAsMatrix() const;

        /// <summary>
        /// Gets a reference to a slice of the tensor. Note that only four of the six possible slice
        /// configutations are possible. Namely, dimension 0 must be either the row dimension or the
        /// column dimension. In the former case, the outcome will be a column major matrix; in the
        /// latter case it will be row major.
        /// </summary>
        ///
        /// <typeparam name="rowDimension"> Which tensor dimension to use for the rows of the matrix. </typeparam>
        /// <typeparam name="columnDimension"> Which tensor dimension to use for the columns of the
        /// matrix. </typeparam>
        /// <param name="index"> Slice index. </param>
        ///
        /// <returns> The resulting slice. </returns>
        template<Dimension rowDimension, Dimension columnDimension>
        auto GetSlice(size_t index) const;

        /// <summary> Gets a slice of the tensor with repsect to its primary dimensions. </summary>
        ///
        /// <param name="index"> Slice index. </param>
        ///
        /// <returns> The primary slice. </returns>
        auto GetPrimarySlice(size_t index) const;

        /// <summary> Gets a const reference to a subtensor (a block). </summary>
        ///
        /// <param name="firstRow"> The first row in the block. </param>
        /// <param name="firstColumn"> The first column in the block. </param>
        /// <param name="firstChannel"> The first channel in the block. </param>
        /// <param name="numRows"> Number of rows in the block. </param>
        /// <param name="numColumns"> Number of columns in the block. </param>
        /// <param name="numChannels"> Number of channels in the block. </param>
        ///
        /// <returns> The resulting TensorReference. </returns>
        ConstTensorReference<ElementType, dimension0, dimension1, dimension2> GetSubTensor(size_t firstRow, size_t firstColumn, size_t firstChannel, size_t numRows, size_t numColumns, size_t numChannels) const;

        /// <summary> Gets a const reference to a subtensor (a block). </summary>
        ///
        /// <param name="firstCoordinate"> The first coordinate of the block. </param>
        /// <param name="shape"> The shape of the block (always given in canonical coordinates: row, column, channel). </param>
        ///
        /// <returns> The resulting ConstTensorReference. </returns>
        ConstTensorReference<ElementType, dimension0, dimension1, dimension2> GetSubTensor(Triplet firstCoordinate, Triplet shape) const;

    protected:
        // other protected member functions
        ConstTensorReference(TensorContents<ElementType> contents);
        size_t GetOffset(Triplet coordinate) const;

        // variables
        using TensorLayoutT = TensorLayout<dimension0, dimension1, dimension2>;
        TensorContents<ElementType> _contents;
    };

    /// <summary>
    /// A reference to a tensor. This class implements all the operations that modify tensor
    /// elements. A tensor reference does not own its own memory.
    /// </summary>
    ///
    /// <typeparam name="ElementType"> Tensor element type. </typeparam>
    /// <typeparam name="dimension0"> Identity of the tensor dimension that occupies contiguous memory
    /// (increment of 1). </typeparam>
    /// <typeparam name="dimension1"> Identity of the tensor dimension with a minor memory increment. </typeparam>
    /// <typeparam name="dimension2"> Identity of the tensor dimension with a major memory increment. </typeparam>
    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    class TensorReference : public ConstTensorReference<ElementType, dimension0, dimension1, dimension2>
    {
    public:
        /// <summary> Element access operator. </summary>
        ///
        /// <param name="row"> The row. </param>
        /// <param name="column"> The column. </param>
        /// <param name="channel"> The channel. </param>
        ///
        /// <returns> A reference to a tensor element. </returns>
        ElementType& operator()(size_t row, size_t column, size_t channel);

        /// <summary> Element access operator. </summary>
        ///
        /// <param name="coordinate"> The coordinate to access. </param>
        ///
        /// <returns> A reference to a tensor element. </returns>
        ElementType& operator()(Triplet coordinate);

        /// <summary> Flattens a tensor into a row vector. Works only when the tensor dimensions are full
        /// (namely, not a subtensor). </summary>
        ///
        /// <returns> A VectorReference that which includes all of the tensor elements. </returns>
        VectorReference<ElementType, VectorOrientation::row> ReferenceAsVector();

        /// <summary>
        /// Returns a matrix reference that includes all of the tensor elements, by flattening the first
        /// tensor dimension.
        /// </summary>
        ///
        /// <returns>
        /// A reference to a row major matrix, which includes all of the tensor elements.
        /// </returns>
        MatrixReference<ElementType, MatrixLayout::rowMajor> ReferenceAsMatrix();

        /// <summary>
        /// Gets a reference to a slice of the tensor. Note that only four of the six possible slice
        /// configutations are possible. Namely, dimension 0 must be either the row dimension or the
        /// column dimension. In the former case, the outcome will be a column major matrix; in the
        /// latter case it will be row major.
        /// </summary>
        ///
        /// <typeparam name="rowDimension"> Which tensor dimension to use for the rows of the matrix. </typeparam>
        /// <typeparam name="columnDimension"> Which tensor dimension to use for the columns of the
        /// matrix. </typeparam>
        /// <param name="index"> Slice index. </param>
        ///
        /// <returns> The resulting slice. </returns>
        template<Dimension rowDimension, Dimension columnDimension>
        auto GetSlice(size_t index);

        /// <summary> Gets a slice of the tensor with repsect to its primary dimensions. </summary>
        ///
        /// <param name="index"> Slice index. </param>
        ///
        /// <returns> The primary slice. </returns>
        auto GetPrimarySlice(size_t index);

        /// <summary> Gets a reference to a subtensor (a block). </summary>
        ///
        /// <param name="firstRow"> The first row in the block. </param>
        /// <param name="firstColumn"> The first column in the block. </param>
        /// <param name="firstChannel"> The first channel in the block. </param>
        /// <param name="numRows"> Number of rows in the block. </param>
        /// <param name="numColumns"> Number of columns in the block. </param>
        /// <param name="numChannels"> Number of channels in the block. </param>
        ///
        /// <returns> The resulting TensorReference. </returns>
        TensorReference<ElementType, dimension0, dimension1, dimension2> GetSubTensor(size_t firstRow, size_t firstColumn, size_t firstChannel, size_t numRows, size_t numColumns, size_t numChannels);

        /// <summary> Gets a reference to a subtensor (a block). </summary>
        ///
        /// <param name="firstCoordinate"> The first coordinate of the block. </param>
        /// <param name="shape"> The shape of the block (always given in canonical coordinates: row, column, channel). </param>
        ///
        /// <returns> The resulting TensorReference. </returns>
        TensorReference<ElementType, dimension0, dimension1, dimension2> GetSubTensor(Triplet firstCoordinate, Triplet shape);

        /// <summary> Fills the Tensor with a given value. </summary>
        ///
        /// <param name="value"> The value. </param>
        void Fill(ElementType value);

        /// <summary>
        /// Generates elements of the tensor by repeatedly calling a generator function (such as a random
        /// number generator).
        /// </summary>
        ///
        /// <typeparam name="GeneratorType"> Type of lambda or functor to use as a generator. </typeparam>
        /// <param name="generator"> The generator function. </param>
        template <typename GeneratorType>
        void Generate(GeneratorType generator);

    protected:
        // abbreviations
        using ConstTensorRef = ConstTensorReference<ElementType, dimension0, dimension1, dimension2>;
        using TensorLayoutT = TensorLayout<dimension0, dimension1, dimension2>;

        // protected ctors can only be accessed by derived classes
        using ConstTensorRef::ConstTensorReference;

        // variables
        using ConstTensorRef::_contents;
    };

    /// <summary> Implements a tensor that owns its own memory. </summary>
    ///
    /// <typeparam name="ElementType"> Tensor element type. </typeparam>
    /// <typeparam name="dimension0"> Identity of the tensor dimension that occupies contiguous memory
    /// (increment of 1). </typeparam>
    /// <typeparam name="dimension1"> Identity of the tensor dimension with a minor memory increment. </typeparam>
    /// <typeparam name="dimension2"> Identity of the tensor dimension with a major memory increment. </typeparam>
    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    class Tensor : public TensorReference<ElementType, dimension0, dimension1, dimension2>
    {
    public:
        /// <summary> Constructs a the zero tensor of given shape. </summary>
        ///
        /// <param name="numRows"> Number of rows. </param>
        /// <param name="numColumns"> Number of columns. </param>
        /// <param name="numChannels"> Number of channels. </param>
        Tensor(size_t numRows, size_t numColumns, size_t numChannels);

        /// <summary> Constructs a the zero tensor of given shape. </summary>
        ///
        /// <param name="shape"> The tensor shape (given in canonical coordinates: rows, columns, channels). </param>
        Tensor(Triplet shape);

        /// <summary> Copy Constructor. </summary>
        ///
        /// <param name="other"> The other tensor. </param>
        Tensor(const Tensor<ElementType, dimension0, dimension1, dimension2>& other);

        /// <summary> Copies a tensor of a different layout. </summary>
        ///
        /// <param name="other"> The other tensor. </param>
        template<Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
        Tensor(ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2> other);

        /// <summary> Constructs a tensor from a triply nested initializer list, given in canonical order. </summary>
        ///
        /// <param name="list"> A triply nested initalizer list. </param>
        Tensor(std::initializer_list<std::initializer_list<std::initializer_list<ElementType>>> list);


        /// <summary> Fills the Tensor with a given value. </summary>
        ///
        /// <param name="value"> The value. </param>
        void Fill(ElementType value);

        /// <summary>
        /// Generates elements of the tensor by repeatedly calling a generator function (such as a random
        /// number generator).
        /// </summary>
        ///
        /// <typeparam name="GeneratorType"> Type of lambda or functor to use as a generator. </typeparam>
        /// <param name="generator"> The generator function. </param>
        template <typename GeneratorType>
        void Generate(GeneratorType generator);

    private:
        // abbreviation
        using ConstTensorRef = ConstTensorReference<ElementType, dimension0, dimension1, dimension2>;
        using TensorRef = TensorReference<ElementType, dimension0, dimension1, dimension2>;
        
        // the array used to store the tensor
        using ConstTensorRef::_contents;
        std::vector<ElementType> _data;
    };

    //
    // friendly names
    //
    template <typename ElementType>
    using ChannelColumnRowTensor = Tensor<ElementType, Dimension::channel, Dimension::column, Dimension::row>;

    template <typename ElementType>
    using ColumnRowChannelTensor = Tensor<ElementType, Dimension::column, Dimension::row, Dimension::channel>;
}
}
#include "../tcc/Tensor.tcc"
