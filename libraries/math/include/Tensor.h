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
#include "IArchivable.h"

// stl
#include <array>
#include <initializer_list>
#include <ostream>
#include <tuple>
#include <vector>

namespace ell
{
namespace math
{
    /// <summary> Enum that represents the dimensions of a tensor. </summary>
    enum class Dimension { row, column, channel };

    // abbreviations
    using Triplet = std::array<size_t, 3>;

    /// <summary> Helper functions for TensorLayout definition </summary>
    template <Dimension dimension0, Dimension dimension1, Dimension dimension2>
    constexpr int IndexOfDimension(Dimension dimension)
    {
        return dimension == dimension0 ? 0 : (dimension == dimension1 ? 1 : (dimension == dimension2 ? 2 : -1));
    }

    /// <summary>
    /// Forward declaration of TensorLayout. This class helps the tensor move back and forth from canonical coordinates (row, column, channel)
    /// to the layout coordinates (which depend on how the tensor is layed out in memory)
    /// </summary>
    ///
    /// <typeparam name="dimension0"> Identity of the tensor dimension that occupies contiguous memory
    /// (increment of 1). </typeparam>
    /// <typeparam name="dimension1"> Identity of the tensor dimension with a minor memory increment. </typeparam>
    /// <typeparam name="dimension2"> Identity of the tensor dimension with a major memory increment. </typeparam>
    template <Dimension dimension0, Dimension dimension1, Dimension dimension2>
    struct TensorLayout
    {
        static Triplet CanonicalToLayout(Triplet canonical) { return{ canonical[rowPosition], canonical[columnPosition], canonical[channelPosition] }; }
        static const size_t rowPosition = IndexOfDimension<dimension0, dimension1, dimension2>(Dimension::row);
        static const size_t columnPosition = IndexOfDimension<dimension0, dimension1, dimension2>(Dimension::column);
        static const size_t channelPosition = IndexOfDimension<dimension0, dimension1, dimension2>(Dimension::channel);
        static_assert(rowPosition != -1, "Invalid layout");
        static_assert(columnPosition != -1, "Invalid layout");
        static_assert(channelPosition != -1, "Invalid layout");
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
        
        /// <summary> Constructs an instance of ConstTensorReference. </summary>
        ///
        /// <param name="shape"> The tensor shape (always given in canonical coordinates: row, column, channel). </param>
        /// <param name="pData"> A pointer to the data to reference. </param>
        ConstTensorReference(Triplet shape, ElementType* pData);

        /// <summary> Element access operator. </summary>
        ///
        /// <param name="row"> The row. </param>
        /// <param name="column"> The column. </param>
        /// <param name="channel"> The channel. </param>
        ///
        /// <returns> A copy of a tensor element. </returns>
        inline ElementType operator()(size_t row, size_t column, size_t channel) const;

        /// <summary> Element access operator. </summary>
        ///
        /// <param name="coordinate"> The coordinate to access. </param>
        ///
        /// <returns> A copy of a tensor element. </returns>
        inline ElementType operator()(Triplet coordinate) const;

        /// <summary> Gets the total number of elements in the tensor. </summary>
        ///
        /// <returns> The total number of elements. </returns>
        size_t Size() const;

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

        /// <summary>
        /// Gets the number of slices that can be returned by GetSlice.
        /// </summary>
        ///
        /// <typeparam name="rowDimension"> Which tensor dimension to use for the rows of the matrix. </typeparam>
        /// <typeparam name="columnDimension"> Which tensor dimension to use for the columns of the
        /// matrix. </typeparam>
        ///
        /// <returns> The number of slices. </returns>
        template<Dimension rowDimension, Dimension columnDimension>
        size_t NumSlices() const;

        /// <summary> Gets the number of primary slices. </summary>
        ///
        /// <returns> The number of primary slices. </returns>
        size_t NumPrimarySlices() const;

        /// \name Comparison Functions
        /// @{

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

        /// <summary> Inequality operator for tensors. </summary>
        ///
        /// <param name="other"> The other tensor. </param>
        ///
        /// <returns> true if the two tensors are not equivalent. </returns>
        template<Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
        bool operator!=(const ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2>& other) const;

        /// @}

        /// \name View Functions
        /// @{

        /// <summary> Gets a const reference to this vector. </summary>
        ///
        /// <returns> A const reference to this vector. </returns>
        ConstTensorReference<ElementType, dimension0, dimension1, dimension2> GetConstReference() const { return *this; }

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

        /// <summary>
        /// Gets a reference to a slice of the tensor. Note that only four of the six possible slice
        /// configurations are possible. Namely, dimension 0 must be either the row dimension or the
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

        /// <summary> Gets a constant reference to this tensor. </summary>
        ///
        /// <returns> A constant tensor reference. </returns>
        ConstTensorReference GetConstTensorReference() const { return *this; }

        /// <summary> Gets a slice of the tensor with repsect to its primary dimensions. </summary>
        ///
        /// <param name="index"> Slice index. </param>
        ///
        /// <returns> The primary slice. </returns>
        auto GetPrimarySlice(size_t index) const;

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

        /// @}

        /// \name Utility Functions
        /// @{

        /// <summary> Swaps the contents of this with the contents of another const tensor reference. </summary>
        ///
        /// <param name="other"> [in,out] The other const tensor reference. </param>
        void Swap(ConstTensorReference<ElementType, dimension0, dimension1, dimension2>& other);

        /// @}

        /// <summary> Returns a copy of the contents of the Tensor. </summary>
        ///
        /// <returns> A std::vector with a copy of the contents of the Tensor. </returns>
        std::vector<ElementType> ToArray() const;

        /// <summary> Gets a const pointer to the underlying data storage. </summary>
        ///
        /// <returns> Const pointer to the data. </returns>
        const ElementType* GetDataPointer() const { return _contents.pData; }

    protected:
        // other protected member functions
        ConstTensorReference(TensorContents<ElementType> contents);
        size_t GetOffset(Triplet coordinate) const;

        // variables
        using TensorLayoutT = TensorLayout<dimension0, dimension1, dimension2>;
        TensorContents<ElementType> _contents;
    };

    /// <summary> Helper function to get the number of slices in a Tensor. </summary>
    ///
    /// <typeparam name="rowDimension"> Which tensor dimension to use for the rows of the matrix. </typeparam>
    /// <typeparam name="columnDimension"> Which tensor dimension to use for the columns of the
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> Tensor dimension 0. </typeparam>
    /// <typeparam name="dimension1"> Tensor dimension 1. </typeparam>
    /// <typeparam name="dimension2"> Tensor dimension 2. </typeparam>
    /// <param name="T"> The Tensor. </param>
    ///
    /// <returns> The number slices. </returns>
    template<Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t NumSlices(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> T);

    /// <summary> Helper function to get a const tensor slice. </summary>
    ///
    /// <typeparam name="rowDimension"> Which tensor dimension to use for the rows of the matrix. </typeparam>
    /// <typeparam name="columnDimension"> Which tensor dimension to use for the columns of the
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> Tensor dimension 0. </typeparam>
    /// <typeparam name="dimension1"> Tensor dimension 1. </typeparam>
    /// <typeparam name="dimension2"> Tensor dimension 2. </typeparam>
    /// <param name="T"> The Tensor. </param>
    /// <param name="index"> Slice index </param>
    ///
    /// <returns> The number slices. </returns>
    template<Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    auto GetConstSlice(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> T, size_t index);

    /// <summary> Prints a tensor in initializer list format. </summary>
    ///
    /// <typeparam name="ElementType"> Tensor element type. </typeparam>
    /// <typeparam name="dimension0"> Tensor first dimension. </typeparam>
    /// <typeparam name="dimension1"> Tensor second dimension. </typeparam>
    /// <typeparam name="dimension2"> Tensor third dimension. </typeparam>
    /// <param name="T"> The tensor. </param>
    /// <param name="stream"> [in,out] The output stream. </param>
    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void Print(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> T, std::ostream& stream);

    /// <summary> Prints a tensor in initializer list format. </summary>
    ///
    /// <param name="stream"> [in,out] The output stream. </param>
    /// <param name="v"> The const tensor reference to print. </param>
    ///
    /// <returns> Reference to the output stream. </returns>
    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    std::ostream& operator<<(std::ostream& stream, ConstTensorReference<ElementType, dimension0, dimension1, dimension2> T);

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
        using ConstTensorRef = ConstTensorReference<ElementType, dimension0, dimension1, dimension2>;
        using ConstTensorRef::ConstTensorReference;
        using ConstTensorRef::operator();
        using ConstTensorRef::Size;
        using ConstTensorRef::NumRows;
        using ConstTensorRef::NumColumns;
        using ConstTensorRef::NumChannels;
        using ConstTensorRef::GetLayout;
        using ConstTensorRef::GetShape;
        using ConstTensorRef::NumSlices;
        using ConstTensorRef::NumPrimarySlices;
        using ConstTensorRef::IsEqual;
        using ConstTensorRef::operator==;
        using ConstTensorRef::operator!=;
        using ConstTensorRef::GetConstReference;
        using ConstTensorRef::GetSubTensor;
        using ConstTensorRef::GetSlice;
        using ConstTensorRef::GetPrimarySlice;
        using ConstTensorRef::ReferenceAsVector;
        using ConstTensorRef::ReferenceAsMatrix;

        /// <summary> Constructs tensor of given shape that uses a pointer to an external buffer as the element data.
        /// This allows the Tensor to use data provided by some other source, and this Tensor does not
        /// own the data buffer.
        /// The buffer has (numRows * numColumns * numChannels) number of elements. </summary>
        /// </summary>
        ///
        /// <param name="numRows"> Number of rows. </param>
        /// <param name="numColumns"> Number of columns. </param>
        /// <param name="numChannels"> Number of channels. </param>
        /// <param name="pData"> A pointer where the elements are stored. </param>
        TensorReference(size_t numRows, size_t numColumns, size_t numChannels, ElementType* pData);

        /// <summary> Element access operator. </summary>
        ///
        /// <param name="row"> The row. </param>
        /// <param name="column"> The column. </param>
        /// <param name="channel"> The channel. </param>
        ///
        /// <returns> A reference to a tensor element. </returns>
        inline ElementType& operator()(size_t row, size_t column, size_t channel);

        /// <summary> Element access operator. </summary>
        ///
        /// <param name="coordinate"> The coordinate to access. </param>
        ///
        /// <returns> A reference to a tensor element. </returns>
        inline ElementType& operator()(Triplet coordinate);

        /// <summary> Swaps the contents of this tensor reference with the contents of another tensor reference. </summary>
        ///
        /// <param name="other"> [in,out] The other tensor reference. </param>
        void Swap(TensorReference<ElementType, dimension0, dimension1, dimension2>& other);

        /// \name Content Manipulation
        /// @{

        /// <summary> Copies values from another tensor into this tensor. </summary>
        ///
        /// <param name="other"> The other tensor. </param>
        void CopyFrom(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> other);

        /// <summary> Copies values from another tensor into this tensor. </summary>
        ///
        /// <param name="other"> The other tensor. </param>
        void CopyFrom(ConstTensorReference<ElementType, dimension0, dimension2, dimension1> other);

        /// <summary> Copies values from another tensor into this tensor. </summary>
        ///
        /// <typeparam name="otherDimension0"> Dimension 0 of the other Tensor. </typeparam>
        /// <typeparam name="otherDimension1"> Dimension 1 of the other Tensor. </typeparam>
        /// <typeparam name="otherDimension2"> Dimension 2 of the other Tensor. </typeparam>
        /// <param name="other"> The other tensor. </param>
        template<Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
        void CopyFrom(ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2> other);

        /// <summary> Sets all Tensor elements to zero. </summary>
        void Reset() { Fill(0); }

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

        /// <summary> Applies a transfromation to each element of the tensor. </summary>
        ///
        /// <typeparam name="TransformationType"> Tranformation type. </typeparam>
        /// <param name="transformation"> The transfromation. </param>
        template <typename TransformationType>
        void Transform(TransformationType transformation);

        /// @}

        /// \name View Functions
        /// @{

        /// <summary> Gets a reference to this vector. </summary>
        ///
        /// <returns> A reference to this vector. </returns>
        TensorReference<ElementType, dimension0, dimension1, dimension2> GetReference() const { return *this; }

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

        /// @}

        /// \name Math Functions
        /// @{

        /// <summary> Adds a constant value to this tensor. </summary>
        ///
        /// <param name="other"> The constant value. </param>
        void operator+=(ElementType value);

        /// <summary> Subtracts a constant value from this tensor. </summary>
        ///
        /// <param name="other"> The constant value. </param>
        void operator-=(ElementType value);

        /// <summary> Multiplies this tensor by a constant value. </summary>
        ///
        /// <param name="other"> The constant value. </param>
        void operator*=(ElementType value);

        /// <summary> Divides each element of this tensor by a constant value. </summary>
        ///
        /// <param name="other"> The constant value. </param>
        void operator/=(ElementType value);

        /// @}

    protected:
        // abbreviations
        using TensorLayoutT = TensorLayout<dimension0, dimension1, dimension2>;

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
        using TensorRef = TensorReference<ElementType, dimension0, dimension1, dimension2>;
        using TensorRef::TensorReference;
        using TensorRef::operator();
        using TensorRef::Size;
        using TensorRef::NumRows;
        using TensorRef::NumColumns;
        using TensorRef::NumChannels;
        using TensorRef::GetLayout;
        using TensorRef::GetShape;
        using TensorRef::NumSlices;
        using TensorRef::NumPrimarySlices;
        using TensorRef::IsEqual;
        using TensorRef::operator==;
        using TensorRef::operator!=;
        using TensorRef::GetConstReference;
        using TensorRef::GetSubTensor;
        using TensorRef::GetSlice;
        using TensorRef::GetPrimarySlice;
        using TensorRef::ReferenceAsVector;
        using TensorRef::ReferenceAsMatrix;
        using TensorRef::CopyFrom;
        using TensorRef::Reset;
        using TensorRef::Fill;
        using TensorRef::Generate;
        using TensorRef::GetReference;
        using TensorRef::operator+=;
        using TensorRef::operator-=;
        using TensorRef::operator*=;
        using TensorRef::operator/=;

        /// <summary> Constructs an empty tensor. </summary>
        Tensor();

        /// <summary> Constructs a the zero tensor of given shape. </summary>
        ///
        /// <param name="numRows"> Number of rows. </param>
        /// <param name="numColumns"> Number of columns. </param>
        /// <param name="numChannels"> Number of channels. </param>
        Tensor(size_t numRows, size_t numColumns, size_t numChannels);

        /// <summary> Constructs a tensor of the given shape with the specified data. </summary>
        ///
        /// <param name="numRows"> Number of rows. </param>
        /// <param name="numColumns"> Number of columns. </param>
        /// <param name="numChannels"> Number of channels. </param>
        /// <param name="data"> Vector of data elements that will be copied to this Tensor. </param>
        Tensor(size_t numRows, size_t numColumns, size_t numChannels, const std::vector<ElementType>& data);

        /// <summary> Constructs a tensor of the given shape with the specified data. </summary>
        ///
        /// <param name="numRows"> Number of rows. </param>
        /// <param name="numColumns"> Number of columns. </param>
        /// <param name="numChannels"> Number of channels. </param>
        /// <param name="data"> Vector of data elements that will be moved to this Tensor. </param>
        Tensor(size_t numRows, size_t numColumns, size_t numChannels, std::vector<ElementType>&& data);

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

        /// <summary> Assignment operator. </summary>
        ///
        /// <param name="other"> The other tensor. </param>
        ///
        /// <returns> A reference to this tensor. </returns>
        Tensor<ElementType, dimension0, dimension1, dimension2>& operator=(Tensor<ElementType, dimension0, dimension1, dimension2> other);

        /// \name Utility Functions
        /// @{

        /// <summary> Swaps the contents of this tensor with the contents of another tensor. </summary>
        ///
        /// <param name="other"> [in,out] The other tensor. </param>
        void Swap(Tensor<ElementType, dimension0, dimension1, dimension2>& other);

        /// @}

        /// <summary> Returns a copy of the contents of the Tensor. </summary>
        ///
        /// <returns> A std::vector with a copy of the contents of the Tensor. </returns>
        std::vector<ElementType> ToArray() const { return _data; }

    private:
        Tensor(size_t numRows, size_t numColumns, size_t numChannels, ElementType* pData) : TensorReference<ElementType, dimension0, dimension1, dimension2>(numRows, numColumns, numChannels, pData) {};

        // abbreviation
        using ConstTensorRef = ConstTensorReference<ElementType, dimension0, dimension1, dimension2>;
        
        // the array used to store the tensor
        using ConstTensorRef::_contents;
        std::vector<ElementType> _data;
    };

    /// <summary> A class that implements helper functions for archiving/unarchiving Tensor instances. </summary>
    class TensorArchiver
    {
    public:
        /// <summary> Writes a tensor to the archive. </summary>
        ///
        /// <typeparam name="ElementType"> Tensor element type. </typeparam>
        /// <typeparam name="dimension0"> Identity of the tensor dimension that occupies contiguous memory
        /// (increment of 1). </typeparam>
        /// <typeparam name="dimension1"> Identity of the tensor dimension with a minor memory increment. </typeparam>
        /// <typeparam name="dimension2"> Identity of the tensor dimension with a major memory increment. </typeparam>
        /// <param name="tensor"> The tensor to add to the archiver. </param>
        /// <param name="name"> The name of the tensor value to store in the archiver. </param>
        /// <param name="archiver"> The `Archiver` to add the tensor to </param>
        template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
        static void Write(const Tensor<ElementType, dimension0, dimension1, dimension2>& tensor, const std::string& name, utilities::Archiver& archiver);

        /// <summary> Reads a tensor from the archive. </summary>
        ///
        /// <typeparam name="ElementType"> Tensor element type. </typeparam>
        /// <typeparam name="dimension0"> Identity of the tensor dimension that occupies contiguous memory
        /// (increment of 1). </typeparam>
        /// <typeparam name="dimension1"> Identity of the tensor dimension with a minor memory increment. </typeparam>
        /// <typeparam name="dimension2"> Identity of the tensor dimension with a major memory increment. </typeparam>
        /// <param name="tensor"> The tensor that will hold the result after it has been read from the archiver. </param>
        /// <param name="name"> The name of the tensor value in the archiver. </param>
        /// <param name="archiver"> The `Archiver` to read the tensor from  </param>
        template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
        static void Read(Tensor<ElementType, dimension0, dimension1, dimension2>& tensor, const std::string& name, utilities::Unarchiver& archiver);

    private:
        static std::string GetRowsName(const std::string& name) { return name + "_rows"; }
        static std::string GetColumnsName(const std::string& name) { return name + "_columns"; }
        static std::string GetChannelsName(const std::string& name) { return name + "_channels"; }
        static std::string GetValuesName(const std::string& name) { return name + "_values"; }
    };

    //
    // friendly names
    //

    /// <summary>
    /// Specialization to channel/column/row memory layout. For example, if the tensor represents an
    /// RGB image, the entries will be stored as R_00, G_00, B_00, R_01, G_01, B_01, ...
    /// </summary>
    template <typename ElementType>
    using ChannelColumnRowTensor = Tensor<ElementType, Dimension::channel, Dimension::column, Dimension::row>;

    /// <summary>
    /// Specialization to column/row/channel memory layout. This means that channels are stored
    /// one after the other and each channel is stored as a row-major matrix.
    /// </summary>
    template <typename ElementType>
    using ColumnRowChannelTensor = Tensor<ElementType, Dimension::column, Dimension::row, Dimension::channel>;
}
}
#include "../tcc/Tensor.tcc"
