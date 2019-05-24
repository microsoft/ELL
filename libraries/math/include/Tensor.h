////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Tensor.h (math)
//  Authors:  Ofer Dekel, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Matrix.h"
#include "Vector.h"

#include <utilities/include/Debug.h>
#include <utilities/include/Exception.h>
#include <utilities/include/IArchivable.h>

#include <array>
#include <cmath>
#include <initializer_list>
#include <ostream>
#include <tuple>
#include <vector>

namespace ell
{
namespace math
{
    /// <summary> Enum that represents the dimensions of a tensor. </summary>
    enum class Dimension
    {
        row,
        column,
        channel
    };

    // abbreviations
    using IntegerTriplet = std::array<size_t, 3>;

    /// <summary> Base class for TensorCoordinate and TensorShape. </summary>
    class TensorCoordinateBase
    {
    public:
        /// <summary> Constructs a TensorCoordinateBase from an IntegerTriplet. </summary>
        ///
        /// <param name="values"> The triplet. </param>
        TensorCoordinateBase(IntegerTriplet values);

        /// <summary> Constructs a TensorCoordinateBase from a vector equivalent of IntegerTriplet. </summary>
        ///
        /// <param name="values"> The vector. </param>
        TensorCoordinateBase(const std::vector<size_t>& values);

        /// <summary> Casting operator to an IntegerTriplet. </summary>
        ///
        /// <returns> The result of the cast. </returns>
        operator IntegerTriplet() const { return { _rowValue, _columnValue, _channelValue }; }

        /// <summary> Casting operator to a vector equivalent to IntegerTriplet. </summary>
        ///
        /// <returns> The result of the cast. </returns>
        operator std::vector<size_t>() const { return { _rowValue, _columnValue, _channelValue }; }

        /// <summary> Equality operator. </summary>
        ///
        /// <param name="other"> The other TensorCoordinateBase. </param>
        ///
        /// <returns> True if the two TensorCoordinateBases are equivalent. </returns>
        bool operator==(const TensorCoordinateBase& other) const;

        /// <summary> Inequality operator. </summary>
        ///
        /// <param name="other"> The other TensorCoordinateBase. </param>
        ///
        /// <returns> True if the two TensorCoordinateBases are not equivalent. </returns>
        bool operator!=(const TensorCoordinateBase& other) const;

        /// <summary> Gets one of the values. </summary>
        ///
        /// <typeparam name="dimension"> The dimension to get. </typeparam>
        ///
        /// <returns> The value. </returns>
        template <Dimension dimension>
        size_t GetValue() const;

        friend std::ostream& operator<<(std::ostream& stream, const TensorCoordinateBase& tensor)
        {
            stream << "(" << tensor._rowValue << ", " << tensor._columnValue << ", " << tensor._channelValue << ")";
            return stream;
        }

    protected:
        TensorCoordinateBase(size_t rowValue, size_t columnValue, size_t channelValue);

        size_t _rowValue;
        size_t _columnValue;
        size_t _channelValue;
    };

    /// <summary> A class that defines the shape of a tensor. </summary>
    class TensorShape : public TensorCoordinateBase
    {
    public:
        /// <summary> Constructs a TensorShape from row, column, channel values. </summary>
        ///
        /// <param name="rowValue"> The row value. </param>
        /// <param name="columnValue"> The column value. </param>
        /// <param name="channelValue"> The channel value. </param>
        TensorShape(size_t rowValue, size_t columnValue, size_t channelValue);

        /// <summary> Constructs a TensorShape from an IntegerTriplet. </summary>
        ///
        /// <param name="values"> The triplet. </param>
        TensorShape(IntegerTriplet values);

        /// <summary> Constructs a TensorShape from a vector equivalent of IntegerTriplet. </summary>
        ///
        /// <param name="values"> The vector. </param>
        TensorShape(const std::vector<size_t>& values);

        /// <summary> Gets the number rows in the tensor. </summary>
        ///
        /// <returns> The number of rows in the tensor. </returns>
        inline size_t NumRows() const { return _rowValue; }

        /// <summary> Gets the number columns in the tensor. </summary>
        ///
        /// <returns> The number of columns in the tensor. </returns>
        inline size_t NumColumns() const { return _columnValue; }

        /// <summary> Gets the number channels in the tensor. </summary>
        ///
        /// <returns> The number of channels in the tensor. </returns>
        inline size_t NumChannels() const { return _channelValue; }

        /// <summary> Gets the total number of elements in the tensor. </summary>
        ///
        /// <returns> The total number of elements in the tensor. </returns>
        size_t Size() const { return NumRows() * NumColumns() * NumChannels(); }
    };

    /// <summary> Represents a tensor coordinate. </summary>
    class TensorCoordinate : public TensorCoordinateBase
    {
    public:
        /// <summary> Constructs a TensorCoordinate from row, column, channel values. </summary>
        ///
        /// <param name="rowValue"> The row value. </param>
        /// <param name="columnValue"> The column value. </param>
        /// <param name="channelValue"> The channel value. </param>
        TensorCoordinate(size_t rowValue, size_t columnValue, size_t channelValue);

        /// <summary> Constructs a TensorCoordinate from an IntegerTriplet. </summary>
        ///
        /// <param name="values"> The triplet. </param>
        TensorCoordinate(IntegerTriplet values);

        /// <summary> Constructs a TensorCoordinate from a vector equivalent of IntegerTriplet. </summary>
        ///
        /// <param name="values"> The vector. </param>
        TensorCoordinate(const std::vector<size_t>& values);

        /// <summary> Gets the row index. </summary>
        ///
        /// <returns> The row index. </returns>
        inline size_t GetRowIndex() const { return _rowValue; }

        /// <summary> Gets the column index. </summary>
        ///
        /// <returns> The column index. </returns>
        inline size_t GetColumnIndex() const { return _columnValue; }

        /// <summary> Gets the channel index. </summary>
        ///
        /// <returns> The channel index. </returns>
        inline size_t GetChannelIndex() const { return _channelValue; }
    };

    /// <summary> TensorMatrixSlicer is a helper class in lieu of the ability to specialize the GetSlice() function </summary>
    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2, Dimension rowDimension, Dimension columnDimension>
    struct TensorMatrixSlicer;

    /// <summary> TensorVectorSlicer is a helper class in lieu of the ability to specialize the GetSlice() function </summary>
    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2, Dimension vectorDimension>
    struct TensorVectorSlicer;

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
    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    class ConstTensorReference
    {
    public:
        using TensorElementType = ElementType;

        /// <summary> Constructs an instance of ConstTensorReference. </summary>
        ///
        /// <param name="shape"> The tensor shape (always given in logical coordinates: row, column, channel). </param>
        ConstTensorReference(TensorShape shape);

        /// <summary> Constructs an instance of ConstTensorReference. </summary>
        ///
        /// <param name="pData"> A pointer to the data to reference. </param>
        /// <param name="shape"> The tensor shape (always given in logical coordinates: row, column, channel). </param>
        ConstTensorReference(const ElementType* pData, TensorShape shape);

        /// \name Accessor Functions
        /// @{

        /// <summary> Gets the number of rows. </summary>
        ///
        /// <returns> The number of rows. </returns>
        size_t NumRows() const { return _shape.NumRows(); }

        /// <summary> Gets the number of columns. </summary>
        ///
        /// <returns> The number of columns. </returns>
        size_t NumColumns() const { return _shape.NumColumns(); }

        /// <summary> Gets the number of channels. </summary>
        ///
        /// <returns> The number of channels. </returns>
        size_t NumChannels() const { return _shape.NumChannels(); }

        /// <summary> Gets the total number of elements in the tensor. </summary>
        ///
        /// <returns> The total number of elements. </returns>
        size_t Size() const { return _shape.Size(); }

        /// <summary> Gets the size of the dimension specified by its index. A template argument of 0 will return the size of dimension0, etc. </summary>
        ///
        /// <typeparam name="dimensionIndex"> The dimension index. </typeparam>
        ///
        /// <returns> The size of the specified dimension. </returns>
        template <size_t dimensionIndex>
        size_t GetSize() const;

        /// <summary> Gets the size of the first dimension. </summary>
        ///
        /// <returns> The size of the first dimension. </returns>
        size_t GetSize0() const { return GetSize<0>(); }

        /// <summary> Gets the size of the second dimension. </summary>
        ///
        /// <returns> The size of the second dimension. </returns>
        size_t GetSize1() const { return GetSize<1>(); }

        /// <summary> Gets the size of the third dimension. </summary>
        ///
        /// <returns> The size of the third dimension. </returns>
        size_t GetSize2() const { return GetSize<2>(); }

        /// <summary> Gets the memory offset needed to advance dimension1. </summary>
        ///
        /// <returns> The memory offset. </returns>
        size_t GetIncrement1() const { return _increment1; }

        /// <summary> Gets the memory offset needed to advance dimension2. </summary>
        ///
        /// <returns> The memory offset. </returns>
        size_t GetIncrement2() const { return _increment2; }

        /// <summary> Gets the three dimenions of the tensor in logical order (row, column, channel). </summary>
        ///
        /// <returns> The shape of the Tensor. </returns>
        TensorShape GetShape() const { return _shape; }

        /// <summary> Gets a const pointer to the underlying data storage. </summary>
        ///
        /// <returns> Const pointer to the data. </returns>
        const ElementType* GetConstDataPointer() const { return _pData; }

        /// <summary>
        /// Gets the number of 2D slices that can be returned by GetSlice.
        /// </summary>
        ///
        /// <typeparam name="rowDimension"> Which tensor dimension to use for the rows of the matrix. </typeparam>
        /// <typeparam name="columnDimension"> Which tensor dimension to use for the columns of the
        /// matrix. </typeparam>
        ///
        /// <returns> The number of 2D slices. </returns>
        template <Dimension rowDimension, Dimension columnDimension>
        size_t NumSlices() const;

        /// <summary>
        /// Gets the number of 1D slices that can be returned by GetSlice.
        /// </summary>
        ///
        /// <typeparam name="dimension"> Which tensor dimension to use for the slice. </typeparam>
        ///
        /// <returns> The number of 1D slices. </returns>
        template <Dimension dimension>
        size_t NumSlices() const;

        /// <summary> Gets the number of primary slices. </summary>
        ///
        /// <returns> The number of primary slices. </returns>
        size_t NumPrimarySlices() const;

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
        inline ElementType operator()(TensorCoordinate coordinate) const;

        /// @}

        /// \name Utility Functions
        /// @{

        /// <summary> Returns a copy of the contents of the Tensor. </summary>
        ///
        /// <returns> A std::vector with a copy of the contents of the Tensor. </returns>
        std::vector<ElementType> ToArray() const;

        /// <summary> Swaps the contents of this with the contents of another const tensor reference. </summary>
        ///
        /// <param name="other"> [in,out] The other const tensor reference. </param>
        void Swap(ConstTensorReference<ElementType, dimension0, dimension1, dimension2>& other);

        /// <summary> Determines if this Tensor is stored in contiguous memory. </summary>
        ///
        /// <returns> True if contiguous, false if not. </returns>
        bool IsContiguous() const;

        /// <summary>
        /// Visits elements of the tensor by repeatedly calling a visitor function.
        /// </summary>
        ///
        /// <typeparam name="VisitorType"> Type of lambda or functor to use as a visitor. </typeparam>
        /// <param name="visitor"> The visitor function. </param>
        template <typename VisitorType>
        void Visit(VisitorType visitor) const;

        /// @}

        /// \name Comparison Functions
        /// @{

        /// <summary> Determines if two tensors are equal. </summary>
        ///
        /// <param name="other"> The other tensor. </param>
        /// <param name="tolerance"> The element comparison tolerance. </param>
        ///
        /// <returns> true if the two tensors are equivalent. </returns>
        template <Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
        bool IsEqual(ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2> other, ElementType tolerance = 1.0e-8) const;

        /// <summary> Equality operator for tensors. </summary>
        ///
        /// <param name="other"> The other tensor. </param>
        ///
        /// <returns> true if the two tensors are equivalent. </returns>
        template <Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
        bool operator==(const ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2>& other) const;

        /// <summary> Inequality operator for tensors. </summary>
        ///
        /// <param name="other"> The other tensor. </param>
        ///
        /// <returns> true if the two tensors are not equivalent. </returns>
        template <Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
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
        /// <param name="shape"> The shape of the block (always given in logical coordinates: row, column, channel). </param>
        ///
        /// <returns> The resulting ConstTensorReference. </returns>
        ConstTensorReference<ElementType, dimension0, dimension1, dimension2> GetSubTensor(TensorCoordinate firstCoordinate, TensorShape shape) const;

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
        template <Dimension rowDimension, Dimension columnDimension>
        auto GetSlice(size_t index) const -> typename TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, rowDimension, columnDimension>::ConstSliceType;

        /// <summary>
        /// Gets a reference to a slice of the tensor. Note that the order of the indices is always
        /// in the logical order (row, col, channel), despite the memory order layout of the tensor.
        /// </summary>
        ///
        /// <typeparam name="dimension"> Which tensor dimension to use for the vector. </typeparam>
        /// <param name="index1"> Slice index for the first dimension. </param>
        /// <param name="index2"> Slice index for the second dimension. </param>
        ///
        /// <returns> The resulting slice. </returns>
        template <Dimension dimension>
        auto GetSlice(size_t index1, size_t index2) const -> typename TensorVectorSlicer<ElementType, dimension0, dimension1, dimension2, dimension>::ConstSliceType;

        /// <summary> Gets a slice of the tensor with repsect to its primary dimensions. </summary>
        ///
        /// <param name="index"> Slice index. </param>
        ///
        /// <returns> The primary slice. </returns>
        auto GetPrimarySlice(size_t index) const -> typename TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, dimension0, dimension1>::ConstSliceType;

        /// <summary> Flattens a tensor into a row vector. Works only when the tensor dimensions are full </summary>
        ///
        /// <returns> A ConstVectorReference that which includes all of the tensor elements. </returns>
        ConstRowVectorReference<ElementType> ReferenceAsVector() const;

        /// <summary>
        /// Returns a const matrix reference that includes all of the tensor elements, by flattening the first
        /// tensor dimension. Works only when the first dimension is full.
        /// </summary>
        ///
        /// <returns>
        /// A const reference to a row major matrix, which includes all of the tensor elements.
        /// </returns>
        ConstRowMatrixReference<ElementType> ReferenceAsMatrix() const;

        /// @}

    protected:
        ConstTensorReference(const ElementType* pData, TensorShape shape, size_t increment1, size_t increment2);
        size_t GetOffset(TensorCoordinate coordinate) const;

        const ElementType* _pData;
        TensorShape _shape;
        size_t _increment1;
        size_t _increment2;
    };

    /// <summary> Helper function to get the number of 2D slices along a dimension of a tensor. </summary>
    ///
    /// <typeparam name="rowDimension"> Which tensor dimension to use for the rows of the matrix. </typeparam>
    /// <typeparam name="columnDimension"> Which tensor dimension to use for the columns of the matrix. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> Tensor dimension 0. </typeparam>
    /// <typeparam name="dimension1"> Tensor dimension 1. </typeparam>
    /// <typeparam name="dimension2"> Tensor dimension 2. </typeparam>
    /// <param name="tensor"> The tensor. </param>
    ///
    /// <returns> The number of 2D slices. </returns>
    template <Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t NumSlices(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> tensor);

    /// <summary> Helper function to get the number of 1D slices along two dimensions of a tensor. </summary>
    ///
    /// <typeparam name="dimension"> Which tensor dimension to use for the slice of the matrix. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> Tensor dimension 0. </typeparam>
    /// <typeparam name="dimension1"> Tensor dimension 1. </typeparam>
    /// <typeparam name="dimension2"> Tensor dimension 2. </typeparam>
    /// <param name="tensor"> The tensor. </param>
    ///
    /// <returns> The number of 1D slices. </returns>
    template <Dimension dimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t NumSlices(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> tensor);

    /// <summary> Helper function to get a const tensor slice. </summary>
    ///
    /// <typeparam name="rowDimension"> Which tensor dimension to use for the rows of the matrix. </typeparam>
    /// <typeparam name="columnDimension"> Which tensor dimension to use for the columns of the matrix. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> Tensor dimension 0. </typeparam>
    /// <typeparam name="dimension1"> Tensor dimension 1. </typeparam>
    /// <typeparam name="dimension2"> Tensor dimension 2. </typeparam>
    /// <param name="tensor"> The tensor. </param>
    /// <param name="index"> Slice index </param>
    ///
    /// <returns> The requested slice. </returns>
    template <Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    auto GetSlice(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> tensor, size_t index);

    /// <summary> Helper function to get a const tensor slice. </summary>
    ///
    /// <typeparam name="dimension"> Which tensor dimension to use for the slice of the matrix. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> Tensor dimension 0. </typeparam>
    /// <typeparam name="dimension1"> Tensor dimension 1. </typeparam>
    /// <typeparam name="dimension2"> Tensor dimension 2. </typeparam>
    /// <param name="tensor"> The tensor. </param>
    /// <param name="index1"> Slice index for the first dimension. </param>
    /// <param name="index2"> Slice index for the second dimension. </param>
    ///
    /// <returns> The requested slice. </returns>
    template <Dimension dimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    auto GetSlice(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> tensor, size_t index1, size_t index2);

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
    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    class TensorReference : public ConstTensorReference<ElementType, dimension0, dimension1, dimension2>
    {
    public:
        using ConstTensorRef = ConstTensorReference<ElementType, dimension0, dimension1, dimension2>;
        using TensorElementType = typename ConstTensorRef::TensorElementType;

        /// <summary> Constructs an instance of TensorReference. </summary>
        ///
        /// <param name="shape"> The tensor shape (always given in logical coordinates: row, column, channel). </param>
        TensorReference(TensorShape shape);

        /// <summary> Constructs tensor of given shape that uses a pointer to an external buffer as the element data.
        /// This allows the Tensor to use data provided by some other source, and this Tensor does not
        /// own the data buffer.
        /// The buffer has (numRows * numColumns * numChannels) number of elements. </summary>
        /// </summary>
        ///
        /// <param name="pData"> A pointer where the elements are stored. </param>
        /// <param name="numRows"> Number of rows. </param>
        /// <param name="numColumns"> Number of columns. </param>
        /// <param name="numChannels"> Number of channels. </param>
        TensorReference(ElementType* pData, size_t numRows, size_t numColumns, size_t numChannels);

        /// \name Accessor Functions
        /// @{

        using ConstTensorRef::operator();

        /// <summary> Gets a pointer to the underlying data storage. </summary>
        ///
        /// <returns> Pointer to the data. </returns>
        ElementType* GetDataPointer() { return const_cast<ElementType*>(this->_pData); }

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
        inline ElementType& operator()(TensorCoordinate coordinate);

        /// @}

        /// \name Utility Functions
        /// @{

        /// <summary> Swaps the contents of this tensor reference with the contents of another tensor reference. </summary>
        ///
        /// <param name="other"> [in,out] The other tensor reference. </param>
        void Swap(TensorReference<ElementType, dimension0, dimension1, dimension2>& other);

        /// @}

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
        template <Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
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

        using ConstTensorRef::GetPrimarySlice;
        using ConstTensorRef::GetSlice;
        using ConstTensorRef::GetSubTensor;
        using ConstTensorRef::ReferenceAsMatrix;
        using ConstTensorRef::ReferenceAsVector;

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
        /// <param name="shape"> The shape of the block (always given in logical coordinates: row, column, channel). </param>
        ///
        /// <returns> The resulting TensorReference. </returns>
        TensorReference<ElementType, dimension0, dimension1, dimension2> GetSubTensor(TensorCoordinate firstCoordinate, TensorShape shape);

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
        template <Dimension rowDimension, Dimension columnDimension>
        auto GetSlice(size_t index) -> typename TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, rowDimension, columnDimension>::SliceType;

        /// <summary>
        /// Gets a reference to a slice of the tensor. Note that the order of the indices is always
        /// in the logical order (row, col, channel), despite the memory order layout of the tensor.
        /// </summary>
        ///
        /// <typeparam name="dimension"> Which tensor dimension to use for the vector. </typeparam>
        /// <param name="index1"> Slice index for the first dimension. </param>
        /// <param name="index2"> Slice index for the second dimension. </param>
        ///
        /// <returns> The resulting slice. </returns>
        template <Dimension dimension>
        auto GetSlice(size_t index1, size_t index2) -> typename TensorVectorSlicer<ElementType, dimension0, dimension1, dimension2, dimension>::SliceType;

        /// <summary> Gets a slice of the tensor with repsect to its primary dimensions. </summary>
        ///
        /// <param name="index"> Slice index. </param>
        ///
        /// <returns> The primary slice. </returns>
        auto GetPrimarySlice(size_t index) -> typename TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, dimension0, dimension1>::SliceType;

        /// <summary> Flattens a tensor into a row vector. Works only when the tensor dimensions are full
        /// (namely, not a subtensor). </summary>
        ///
        /// <returns> A VectorReference that which includes all of the tensor elements. </returns>
        RowVectorReference<ElementType> ReferenceAsVector();

        /// <summary>
        /// Returns a matrix reference that includes all of the tensor elements, by flattening the first
        /// tensor dimension.
        /// </summary>
        ///
        /// <returns>
        /// A reference to a row major matrix, which includes all of the tensor elements.
        /// </returns>
        RowMatrixReference<ElementType> ReferenceAsMatrix();

        /// @}

    protected:
        TensorReference(ElementType* pData, TensorShape shape, size_t increment1, size_t increment2);
    };

    /// <summary> Helper function to get the number of 2D slices along a dimension of a tensor. </summary>
    ///
    /// <typeparam name="rowDimension"> Which tensor dimension to use for the rows of the matrix. </typeparam>
    /// <typeparam name="columnDimension"> Which tensor dimension to use for the columns of the matrix. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> Tensor dimension 0. </typeparam>
    /// <typeparam name="dimension1"> Tensor dimension 1. </typeparam>
    /// <typeparam name="dimension2"> Tensor dimension 2. </typeparam>
    /// <param name="tensor"> The tensor. </param>
    ///
    /// <returns> The number of 2D slices. </returns>
    template <Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t NumSlices(TensorReference<ElementType, dimension0, dimension1, dimension2> tensor);

    /// <summary> Helper function to get the number of 1D slices along two dimensions of a tensor. </summary>
    ///
    /// <typeparam name="dimension"> Which tensor dimension to use for the slice of the matrix. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> Tensor dimension 0. </typeparam>
    /// <typeparam name="dimension1"> Tensor dimension 1. </typeparam>
    /// <typeparam name="dimension2"> Tensor dimension 2. </typeparam>
    /// <param name="tensor"> The tensor. </param>
    ///
    /// <returns> The number of 1D slices. </returns>
    template <Dimension dimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t NumSlices(TensorReference<ElementType, dimension0, dimension1, dimension2> tensor);

    /// <summary> Helper function to get a tensor slice. </summary>
    ///
    /// <typeparam name="rowDimension"> Which tensor dimension to use for the rows of the matrix. </typeparam>
    /// <typeparam name="columnDimension"> Which tensor dimension to use for the columns of the matrix. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> Tensor dimension 0. </typeparam>
    /// <typeparam name="dimension1"> Tensor dimension 1. </typeparam>
    /// <typeparam name="dimension2"> Tensor dimension 2. </typeparam>
    /// <param name="tensor"> The tensor. </param>
    /// <param name="index"> Slice index </param>
    ///
    /// <returns> The requested slice. </returns>
    template <Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    auto GetSlice(TensorReference<ElementType, dimension0, dimension1, dimension2> tensor, size_t index);

    /// <summary> Helper function to get a tensor slice. </summary>
    ///
    /// <typeparam name="dimension"> Which tensor dimension to use for the slice of the matrix. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> Tensor dimension 0. </typeparam>
    /// <typeparam name="dimension1"> Tensor dimension 1. </typeparam>
    /// <typeparam name="dimension2"> Tensor dimension 2. </typeparam>
    /// <param name="tensor"> The tensor. </param>
    /// <param name="index"> Slice index </param>
    ///
    /// <returns> The requested slice. </returns>
    template <Dimension dimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    auto GetSlice(TensorReference<ElementType, dimension0, dimension1, dimension2> tensor, size_t index1, size_t index2);

    /// <summary> Implements a tensor that owns its own memory. </summary>
    ///
    /// <typeparam name="ElementType"> Tensor element type. </typeparam>
    /// <typeparam name="dimension0"> Identity of the tensor dimension that occupies contiguous memory
    /// (increment of 1). </typeparam>
    /// <typeparam name="dimension1"> Identity of the tensor dimension with a minor memory increment. </typeparam>
    /// <typeparam name="dimension2"> Identity of the tensor dimension with a major memory increment. </typeparam>
    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    class Tensor : public TensorReference<ElementType, dimension0, dimension1, dimension2>
    {
    public:
        using TensorRef = TensorReference<ElementType, dimension0, dimension1, dimension2>;
        using TensorElementType = typename TensorRef::TensorElementType;

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
        /// <param name="shape"> The tensor shape (given in logical coordinates: rows, columns, channels). </param>
        Tensor(TensorShape shape);

        /// <summary> Copy Constructor. </summary>
        ///
        /// <param name="other"> The other tensor. </param>
        Tensor(const Tensor<ElementType, dimension0, dimension1, dimension2>& other);

        /// <summary> Copies a tensor of a different layout. </summary>
        ///
        /// <param name="other"> The other tensor. </param>
        template <Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
        Tensor(ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2> other);

        /// <summary> Constructs a tensor from a triply nested initializer list, given in logical order (row, column, channel). </summary>
        ///
        /// <param name="list"> A triply nested initalizer list. </param>
        Tensor(std::initializer_list<std::initializer_list<std::initializer_list<ElementType>>> list);

        /// \name Utility Functions
        /// @{

        /// <summary> Assignment operator. </summary>
        ///
        /// <param name="other"> The other tensor. </param>
        ///
        /// <returns> A reference to this tensor. </returns>
        Tensor<ElementType, dimension0, dimension1, dimension2>& operator=(Tensor<ElementType, dimension0, dimension1, dimension2> other);

        /// <summary> Returns a copy of the contents of the Tensor. </summary>
        ///
        /// <returns> A std::vector with a copy of the contents of the Tensor. </returns>
        std::vector<ElementType> ToArray() const { return _data; }

        /// <summary> Swaps the contents of this tensor with the contents of another tensor. </summary>
        ///
        /// <param name="other"> [in,out] The other tensor. </param>
        void Swap(Tensor<ElementType, dimension0, dimension1, dimension2>& other);

        /// @}

    private:
        using ConstTensorRef = ConstTensorReference<ElementType, dimension0, dimension1, dimension2>;
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
        template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
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
        template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
        static void Read(Tensor<ElementType, dimension0, dimension1, dimension2>& tensor, const std::string& name, utilities::Unarchiver& archiver);

    private:
        static std::string GetRowsName(const std::string& name) { return name + "_rows"; } // STYLE discrepancy
        static std::string GetColumnsName(const std::string& name) { return name + "_columns"; } // STYLE discrepancy
        static std::string GetChannelsName(const std::string& name) { return name + "_channels"; } // STYLE discrepancy
        static std::string GetValuesName(const std::string& name) { return name + "_values"; } // STYLE discrepancy
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
    /// Specialization to channel/column/row memory layout. For example, if the tensor represents an
    /// RGB image, the entries will be stored as R_00, G_00, B_00, R_01, G_01, B_01, ...
    /// </summary>
    template <typename ElementType>
    using ChannelColumnRowTensorReference = TensorReference<ElementType, Dimension::channel, Dimension::column, Dimension::row>;

    /// <summary>
    /// Specialization to channel/column/row memory layout. For example, if the tensor represents an
    /// RGB image, the entries will be stored as R_00, G_00, B_00, R_01, G_01, B_01, ...
    /// </summary>
    template <typename ElementType>
    using ConstChannelColumnRowTensorReference = ConstTensorReference<ElementType, Dimension::channel, Dimension::column, Dimension::row>;

    /// <summary>
    /// Specialization to column/row/channel memory layout. This means that channels are stored
    /// one after the other and each channel is stored as a row-major matrix.
    /// </summary>
    template <typename ElementType>
    using ColumnRowChannelTensor = Tensor<ElementType, Dimension::column, Dimension::row, Dimension::channel>;

    /// <summary>
    /// Specialization to column/row/channel memory layout. This means that channels are stored
    /// one after the other and each channel is stored as a row-major matrix.
    /// </summary>
    template <typename ElementType>
    using ColumnRowChannelTensorReference = TensorReference<ElementType, Dimension::column, Dimension::row, Dimension::channel>;

    /// <summary>
    /// Specialization to column/row/channel memory layout. This means that channels are stored
    /// one after the other and each channel is stored as a row-major matrix.
    /// </summary>
    template <typename ElementType>
    using ConstColumnRowChannelTensorReference = ConstTensorReference<ElementType, Dimension::column, Dimension::row, Dimension::channel>;
} // namespace math
} // namespace ell

#pragma region implementation

#include <algorithm>

namespace ell
{
namespace math
{
    //
    // TensorMatrixSlicers
    //

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2, Dimension rowDimension, Dimension columnDimension>
    struct TensorMatrixSlicer;

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    struct TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, dimension0, dimension1>
    {
        using SliceType = ColumnMatrixReference<ElementType>;
        using ConstSliceType = ConstColumnMatrixReference<ElementType>;

        inline static size_t NumSlices(TensorShape shape)
        {
            return shape.GetValue<dimension2>();
        }

        static ConstSliceType GetConstSlice(const ElementType* pData, TensorShape shape, size_t increment1, size_t increment2, size_t index)
        {
            DEBUG_THROW(index >= NumSlices(shape), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

            return ConstSliceType(pData + index * increment2, shape.GetValue<dimension0>(), shape.GetValue<dimension1>(), increment1);
        }

        static SliceType GetSlice(ElementType* pData, TensorShape shape, size_t increment1, size_t increment2, size_t index)
        {
            DEBUG_THROW(index >= NumSlices(shape), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

            return SliceType(pData + index * increment2, shape.GetValue<dimension0>(), shape.GetValue<dimension1>(), increment1);
        }
    };

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    struct TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, dimension0, dimension2>
    {
        using SliceType = ColumnMatrixReference<ElementType>;
        using ConstSliceType = ConstColumnMatrixReference<ElementType>;

        inline static size_t NumSlices(TensorShape shape)
        {
            return shape.GetValue<dimension1>();
        }

        static ConstSliceType GetConstSlice(const ElementType* pData, TensorShape shape, size_t increment1, size_t increment2, size_t index)
        {
            DEBUG_THROW(index >= NumSlices(shape), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

            return ConstSliceType(pData + index * increment1, shape.GetValue<dimension0>(), shape.GetValue<dimension2>(), increment2);
        }

        static SliceType GetSlice(ElementType* pData, TensorShape shape, size_t increment1, size_t increment2, size_t index)
        {
            DEBUG_THROW(index >= NumSlices(shape), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

            return SliceType(pData + index * increment1, shape.GetValue<dimension0>(), shape.GetValue<dimension2>(), increment2);
        }
    };

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    struct TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, dimension1, dimension0>
    {
        using SliceType = RowMatrixReference<ElementType>;
        using ConstSliceType = ConstRowMatrixReference<ElementType>;

        inline static size_t NumSlices(TensorShape shape)
        {
            return shape.GetValue<dimension2>();
        }

        static ConstSliceType GetConstSlice(const ElementType* pData, TensorShape shape, size_t increment1, size_t increment2, size_t index)
        {
            DEBUG_THROW(index >= NumSlices(shape), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

            return ConstSliceType(pData + index * increment2, shape.GetValue<dimension1>(), shape.GetValue<dimension0>(), increment1);
        }

        static SliceType GetSlice(ElementType* pData, TensorShape shape, size_t increment1, size_t increment2, size_t index)
        {
            DEBUG_THROW(index >= NumSlices(shape), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

            return SliceType(pData + index * increment2, shape.GetValue<dimension1>(), shape.GetValue<dimension0>(), increment1);
        }
    };

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    struct TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, dimension2, dimension0>
    {
        using SliceType = RowMatrixReference<ElementType>;
        using ConstSliceType = ConstRowMatrixReference<ElementType>;

        inline static size_t NumSlices(TensorShape shape)
        {
            return shape.GetValue<dimension1>();
        }

        static ConstSliceType GetConstSlice(const ElementType* pData, TensorShape shape, size_t increment1, size_t increment2, size_t index)
        {
            DEBUG_THROW(index >= NumSlices(shape), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

            return ConstSliceType(pData + index * increment1, shape.GetValue<dimension2>(), shape.GetValue<dimension0>(), increment2);
        }

        static SliceType GetSlice(ElementType* pData, TensorShape shape, size_t increment1, size_t increment2, size_t index)
        {
            DEBUG_THROW(index >= NumSlices(shape), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

            return SliceType(pData + index * increment1, shape.GetValue<dimension2>(), shape.GetValue<dimension0>(), increment2);
        }
    };

    //
    // TensorVectorSlicers
    //

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2, Dimension vectorDimension>
    struct TensorVectorSlicer;

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    struct TensorVectorSlicer<ElementType, dimension0, dimension1, dimension2, dimension0>
    {
        using SliceType = ColumnVectorReference<ElementType>;
        using ConstSliceType = ConstColumnVectorReference<ElementType>;

        static inline size_t NumSlices(TensorShape shape)
        {
            return shape.GetValue<dimension1>() * shape.GetValue<dimension2>();
        }

        static ConstSliceType GetConstSlice(const ElementType* pData, TensorShape shape, size_t increment1, size_t increment2, size_t index1, size_t index2)
        {
            constexpr bool shouldSwap = dimension1 > dimension2;
            if /*constexpr*/ (shouldSwap)
            {
                std::swap(index1, index2);
            }

            DEBUG_THROW(index1 >= shape.GetValue<dimension1>(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index1 exceeds tensor dimensions."));
            DEBUG_THROW(index2 >= shape.GetValue<dimension2>(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index2 exceeds tensor dimensions."));

            return ConstSliceType(pData + index1 * increment1 + index2 * increment2, shape.GetValue<dimension0>(), 1);
        }

        static SliceType GetSlice(ElementType* pData, TensorShape shape, size_t increment1, size_t increment2, size_t index1, size_t index2)
        {
            constexpr bool shouldSwap = dimension1 > dimension2;
            if /*constexpr*/ (shouldSwap)
            {
                std::swap(index1, index2);
            }

            DEBUG_THROW(index1 >= shape.GetValue<dimension1>(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index1 exceeds tensor dimensions."));
            DEBUG_THROW(index2 >= shape.GetValue<dimension2>(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index2 exceeds tensor dimensions."));

            return SliceType(pData + index1 * increment1 + index2 * increment2, shape.GetValue<dimension0>(), 1);
        }
    };

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    struct TensorVectorSlicer<ElementType, dimension0, dimension1, dimension2, dimension1>
    {
        using SliceType = ColumnVectorReference<ElementType>;
        using ConstSliceType = ConstColumnVectorReference<ElementType>;

        static inline size_t NumSlices(TensorShape shape)
        {
            return shape.GetValue<dimension0>() * shape.GetValue<dimension2>();
        }

        static ConstSliceType GetConstSlice(const ElementType* pData, TensorShape shape, size_t increment1, size_t increment2, size_t index1, size_t index2)
        {
            constexpr bool shouldSwap = dimension0 > dimension2;
            if /*constexpr*/ (shouldSwap)
            {
                std::swap(index1, index2);
            }

            DEBUG_THROW(index1 >= shape.GetValue<dimension0>(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index1 exceeds tensor dimensions."));
            DEBUG_THROW(index2 >= shape.GetValue<dimension2>(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index2 exceeds tensor dimensions."));

            return ConstSliceType(pData + index1 + index2 * increment2, shape.GetValue<dimension1>(), increment1);
        }

        static SliceType GetSlice(ElementType* pData, TensorShape shape, size_t increment1, size_t increment2, size_t index1, size_t index2)
        {
            constexpr bool shouldSwap = dimension0 > dimension2;
            if /*constexpr*/ (shouldSwap)
            {
                std::swap(index1, index2);
            }

            DEBUG_THROW(index1 >= shape.GetValue<dimension0>(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index1 exceeds tensor dimensions."));
            DEBUG_THROW(index2 >= shape.GetValue<dimension2>(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index2 exceeds tensor dimensions."));

            return SliceType(pData + index1 + index2 * increment2, shape.GetValue<dimension1>(), increment1);
        }
    };

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    struct TensorVectorSlicer<ElementType, dimension0, dimension1, dimension2, dimension2>
    {
        using SliceType = ColumnVectorReference<ElementType>;
        using ConstSliceType = ConstColumnVectorReference<ElementType>;

        static inline size_t NumSlices(TensorShape shape)
        {
            return shape.GetValue<dimension0>() * shape.GetValue<dimension1>();
        }

        static ConstSliceType GetConstSlice(const ElementType* pData, TensorShape shape, size_t increment1, size_t increment2, size_t index1, size_t index2)
        {
            constexpr bool shouldSwap = dimension0 > dimension1;
            if /*constexpr*/ (shouldSwap)
            {
                std::swap(index1, index2);
            }

            DEBUG_THROW(index1 >= shape.GetValue<dimension0>(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index1 exceeds tensor dimensions."));
            DEBUG_THROW(index2 >= shape.GetValue<dimension1>(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index2 exceeds tensor dimensions."));

            return ConstSliceType(pData + index1 + index2 * increment1, shape.GetValue<dimension2>(), increment2);
        }

        static SliceType GetSlice(ElementType* pData, TensorShape shape, size_t increment1, size_t increment2, size_t index1, size_t index2)
        {
            constexpr bool shouldSwap = dimension0 > dimension1;
            if /*constexpr*/ (shouldSwap)
            {
                std::swap(index1, index2);
            }

            DEBUG_THROW(index1 >= shape.GetValue<dimension0>(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index1 exceeds tensor dimensions."));
            DEBUG_THROW(index2 >= shape.GetValue<dimension1>(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index2 exceeds tensor dimensions."));

            return SliceType(pData + index1 + index2 * increment1, shape.GetValue<dimension2>(), increment2);
        }
    };

    //
    // ConstTensorReference
    //

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::ConstTensorReference(TensorShape shape) :
        ConstTensorReference<ElementType, dimension0, dimension1, dimension2>(nullptr, shape)
    {
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::ConstTensorReference(const ElementType* pData, TensorShape shape) :
        _pData(pData),
        _shape(shape)
    {
        _increment1 = shape.GetValue<dimension0>();
        _increment2 = _increment1 * shape.GetValue<dimension1>();
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <size_t dimensionIndex>
    size_t ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::GetSize() const
    {
        constexpr auto dimension = std::get<dimensionIndex>(std::make_tuple(dimension0, dimension1, dimension2));
        return _shape.GetValue<dimension>();
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <Dimension rowDimension, Dimension columnDimension>
    size_t ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::NumSlices() const
    {
        return TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, rowDimension, columnDimension>::NumSlices(_shape);
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <Dimension dimension>
    size_t ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::NumSlices() const
    {
        return TensorVectorSlicer<ElementType, dimension0, dimension1, dimension2, dimension>::NumSlices(_shape);
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::NumPrimarySlices() const
    {
        return GetSize2();
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ElementType ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::operator()(size_t row, size_t column, size_t channel) const
    {
        return operator()({ row, column, channel });
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ElementType ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::operator()(TensorCoordinate coordinate) const
    {
        return GetConstDataPointer()[this->GetOffset(coordinate)];
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    std::vector<ElementType> ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::ToArray() const
    {
        if (!IsContiguous())
        {
            auto resultIncrement0 = _shape.GetValue<dimension0>();
            auto resultIncrement1 = resultIncrement0 * _shape.GetValue<dimension1>();

            std::vector<ElementType> result(NumRows() * NumColumns() * NumChannels());
            for (size_t i = 0; i < NumRows(); ++i)
            {
                for (size_t j = 0; j < NumColumns(); ++j)
                {
                    for (size_t k = 0; k < NumChannels(); ++k)
                    {
                        auto value = (*this)(i, j, k);
                        auto coordinate = TensorCoordinate(i, j, k);
                        auto resultIndex = coordinate.GetValue<dimension0>() + coordinate.GetValue<dimension1>() * resultIncrement0 + coordinate.GetValue<dimension2>() * resultIncrement1;
                        result[resultIndex] = value;
                    }
                }
            }
            return result;
        }
        return { GetConstDataPointer(), GetConstDataPointer() + Size() };
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::Swap(ConstTensorReference<ElementType, dimension0, dimension1, dimension2>& other)
    {
        std::swap(_pData, other._pData);
        std::swap(_shape, other._shape);
        std::swap(_increment1, other._increment1);
        std::swap(_increment2, other._increment2);
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    bool ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::IsContiguous() const
    {
        return GetSize0() == GetIncrement1() && GetSize0() * GetSize1() == GetIncrement2();
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <typename VisitorType>
    void ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::Visit(VisitorType visitor) const
    {
        for (size_t i = 0; i < this->NumPrimarySlices(); ++i)
        {
            auto slice = GetPrimarySlice(i);
            slice.Visit(visitor);
        }
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
    bool ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::IsEqual(ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2> other, ElementType tolerance) const
    {
        if (NumRows() != other.NumRows() || NumColumns() != other.NumColumns() || NumChannels() != other.NumChannels())
        {
            return false;
        }

        for (size_t i = 0; i < NumRows(); ++i)
        {
            for (size_t j = 0; j < NumColumns(); ++j)
            {
                for (size_t k = 0; k < NumChannels(); ++k)
                {
                    auto diff = (*this)(i, j, k) - other(i, j, k);
                    if (diff > tolerance || -diff > tolerance)
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
    bool ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::operator==(const ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2>& other) const
    {
        return IsEqual(other);
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
    bool ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::operator!=(const ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2>& other) const
    {
        return !IsEqual(other);
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstTensorReference<ElementType, dimension0, dimension1, dimension2> ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::GetSubTensor(size_t firstRow, size_t firstColumn, size_t firstChannel, size_t numRows, size_t numColumns, size_t numChannels) const
    {
        return GetSubTensor({ firstRow, firstColumn, firstChannel }, { numRows, numColumns, numChannels });
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstTensorReference<ElementType, dimension0, dimension1, dimension2> ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::GetSubTensor(TensorCoordinate firstCoordinate, TensorShape shape) const
    {
        DEBUG_THROW(firstCoordinate.GetRowIndex() + shape.NumRows() > NumRows() || firstCoordinate.GetColumnIndex() + shape.NumColumns() > NumColumns() || firstCoordinate.GetChannelIndex() + shape.NumChannels() > NumChannels(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "subtensor exceeds tensor dimensions."));

        return ConstTensorReference(GetConstDataPointer() + GetOffset(firstCoordinate), shape, GetIncrement1(), GetIncrement2());
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <Dimension rowDimension, Dimension columnDimension>
    auto ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::GetSlice(size_t index) const -> typename TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, rowDimension, columnDimension>::ConstSliceType
    {
        return TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, rowDimension, columnDimension>::GetConstSlice(GetConstDataPointer(), GetShape(), GetIncrement1(), GetIncrement2(), index);
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <Dimension dimension>
    auto ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::GetSlice(size_t index1, size_t index2) const -> typename TensorVectorSlicer<ElementType, dimension0, dimension1, dimension2, dimension>::ConstSliceType
    {
        return TensorVectorSlicer<ElementType, dimension0, dimension1, dimension2, dimension>::GetConstSlice(GetConstDataPointer(), GetShape(), GetIncrement1(), GetIncrement2(), index1, index2);
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2> // pData -> GetDataPointer
    auto ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::GetPrimarySlice(size_t index) const -> typename TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, dimension0, dimension1>::ConstSliceType
    {
        return TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, dimension0, dimension1>::GetConstSlice(GetConstDataPointer(), GetShape(), GetIncrement1(), GetIncrement2(), index);
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstRowVectorReference<ElementType> ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::ReferenceAsVector() const
    {
        DEBUG_THROW(GetSize0() != GetIncrement1() || GetSize0() * GetSize1() != GetIncrement2(), utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Can only flatten a tensor when alll the dimensions are full"));

        return ConstRowVectorReference<ElementType>(GetConstDataPointer(), Size(), 1);
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstRowMatrixReference<ElementType> ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::ReferenceAsMatrix() const
    {
        DEBUG_THROW(GetSize0() != GetIncrement1(), utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Can only flatten a tensor when the first dimension is full"));

        return ConstRowMatrixReference<ElementType>(GetConstDataPointer(), GetSize2(), GetSize0() * GetSize1(), GetIncrement2());
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::GetOffset(TensorCoordinate coordinate) const
    {
        DEBUG_THROW(coordinate.GetRowIndex() >= NumRows() || coordinate.GetColumnIndex() >= NumColumns() || coordinate.GetChannelIndex() >= NumChannels(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, std::string("index exceeds tensor size in ConstTensorReference::GetOffset().") + " Tensor size: (" + std::to_string(NumRows()) + " x " + std::to_string(NumColumns()) + " x " + std::to_string(NumChannels()) + "), "
                                                                                                                                                                                                                                                                                                                                                                                                                                                       " index: (" +
                                                                                                                                                                                                                                           std::to_string(coordinate.GetRowIndex()) + ", " + std::to_string(coordinate.GetColumnIndex()) + ", " + std::to_string(coordinate.GetChannelIndex()) + ")"));

        return coordinate.GetValue<dimension0>() + coordinate.GetValue<dimension1>() * GetIncrement1() + coordinate.GetValue<dimension2>() * GetIncrement2();
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::ConstTensorReference(const ElementType* pData, TensorShape shape, size_t increment1, size_t increment2) :
        _pData(pData),
        _shape(shape),
        _increment1(increment1),
        _increment2(increment2)
    {}

    template <Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t NumSlices(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> tensor)
    {
        return tensor.template NumSlices<rowDimension, columnDimension>();
    }

    template <Dimension dimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t NumSlices(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> tensor)
    {
        return tensor.template NumSlices<dimension>();
    }

    template <Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    auto GetSlice(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> tensor, size_t index)
    {
        return tensor.template GetSlice<rowDimension, columnDimension>(index);
    }

    template <Dimension dimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    auto GetSlice(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> tensor, size_t index1, size_t index2)
    {
        return tensor.template GetSlice<dimension>(index1, index2);
    }

    //
    // TensorReference
    //

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    TensorReference<ElementType, dimension0, dimension1, dimension2>::TensorReference(TensorShape shape) :
        ConstTensorRef(shape)
    {}

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    TensorReference<ElementType, dimension0, dimension1, dimension2>::TensorReference(ElementType* pData, size_t numRows, size_t numColumns, size_t numChannels) :
        ConstTensorRef(pData, TensorShape{ numRows, numColumns, numChannels })
    {}

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ElementType& TensorReference<ElementType, dimension0, dimension1, dimension2>::operator()(size_t row, size_t column, size_t channel)
    {
        return operator()({ row, column, channel });
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ElementType& TensorReference<ElementType, dimension0, dimension1, dimension2>::operator()(TensorCoordinate coordinate)
    {
        return GetDataPointer()[this->GetOffset(coordinate)];
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void TensorReference<ElementType, dimension0, dimension1, dimension2>::Swap(TensorReference<ElementType, dimension0, dimension1, dimension2>& other)
    {
        ConstTensorRef::Swap(other);
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void TensorReference<ElementType, dimension0, dimension1, dimension2>::CopyFrom(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> other)
    {
        DEBUG_CHECK_SIZES(this->NumRows() != other.NumRows(), "Tensors must have the same number of rows");
        DEBUG_CHECK_SIZES(this->NumColumns() != other.NumColumns(), "Tensors must have the same number of columns");
        DEBUG_CHECK_SIZES(this->NumChannels() != other.NumChannels(), "Tensors must have the same number of channels");

        for (size_t i = 0; i < this->NumPrimarySlices(); ++i)
        {
            auto slice = other.GetPrimarySlice(i);
            GetPrimarySlice(i).CopyFrom(slice);
        }
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void TensorReference<ElementType, dimension0, dimension1, dimension2>::CopyFrom(ConstTensorReference<ElementType, dimension0, dimension2, dimension1> other)
    {
        DEBUG_CHECK_SIZES(this->NumRows() != other.NumRows(), "Tensors must have the same number of rows");
        DEBUG_CHECK_SIZES(this->NumColumns() != other.NumColumns(), "Tensors must have the same number of columns");
        DEBUG_CHECK_SIZES(this->NumChannels() != other.NumChannels(), "Tensors must have the same number of channels");

        for (size_t i = 0; i < NumSlices<dimension0, dimension1>(*this); ++i)
        {
            this->template GetSlice<dimension0, dimension1>(i).CopyFrom(GetSlice<dimension0, dimension1>(other, i));
        }
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
    void TensorReference<ElementType, dimension0, dimension1, dimension2>::CopyFrom(ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2> other)
    {
        DEBUG_CHECK_SIZES(this->NumRows() != other.NumRows(), "Tensors must have the same number of rows");
        DEBUG_CHECK_SIZES(this->NumColumns() != other.NumColumns(), "Tensors must have the same number of columns");
        DEBUG_CHECK_SIZES(this->NumChannels() != other.NumChannels(), "Tensors must have the same number of channels");

        for (size_t i = 0; i < math::NumSlices<dimension0, otherDimension0>(*this); ++i)
        {
            auto thisSlice = this->template GetSlice<dimension0, otherDimension0>(i);
            auto otherSlice = other.template GetSlice<dimension0, otherDimension0>(i);
            thisSlice.CopyFrom(otherSlice);
        }
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void TensorReference<ElementType, dimension0, dimension1, dimension2>::Fill(ElementType value)
    {
        for (size_t i = 0; i < this->NumPrimarySlices(); ++i)
        {
            auto slice = GetPrimarySlice(i);
            slice.Fill(value);
        }
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <typename GeneratorType>
    void TensorReference<ElementType, dimension0, dimension1, dimension2>::Generate(GeneratorType generator)
    {
        for (size_t i = 0; i < this->NumPrimarySlices(); ++i)
        {
            auto slice = GetPrimarySlice(i);
            slice.Generate(generator);
        }
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <typename TransformationType>
    void TensorReference<ElementType, dimension0, dimension1, dimension2>::Transform(TransformationType transformation)
    {
        for (size_t i = 0; i < this->NumPrimarySlices(); ++i)
        {
            auto slice = GetPrimarySlice(i);
            slice.Transform(transformation);
        }
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    TensorReference<ElementType, dimension0, dimension1, dimension2> TensorReference<ElementType, dimension0, dimension1, dimension2>::GetSubTensor(size_t firstRow, size_t firstColumn, size_t firstChannel, size_t numRows, size_t numColumns, size_t numChannels)
    {
        return GetSubTensor({ firstRow, firstColumn, firstChannel }, { numRows, numColumns, numChannels });
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    TensorReference<ElementType, dimension0, dimension1, dimension2> TensorReference<ElementType, dimension0, dimension1, dimension2>::GetSubTensor(TensorCoordinate firstCoordinate, TensorShape shape)
    {
        DEBUG_THROW(firstCoordinate.GetRowIndex() + shape.NumRows() > this->NumRows() || firstCoordinate.GetColumnIndex() + shape.NumColumns() > this->NumColumns() || firstCoordinate.GetChannelIndex() + shape.NumChannels() > this->NumChannels(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "subtensor exceeds tensor dimensions."));

        return TensorReference(GetDataPointer() + this->GetOffset(firstCoordinate), shape, this->GetIncrement1(), this->GetIncrement2());
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <Dimension rowDimension, Dimension columnDimension>
    auto TensorReference<ElementType, dimension0, dimension1, dimension2>::GetSlice(size_t index) -> typename TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, rowDimension, columnDimension>::SliceType
    {
        return TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, rowDimension, columnDimension>::GetSlice(GetDataPointer(), this->GetShape(), this->GetIncrement1(), this->GetIncrement2(), index);
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <Dimension dimension>
    auto TensorReference<ElementType, dimension0, dimension1, dimension2>::GetSlice(size_t index1, size_t index2) -> typename TensorVectorSlicer<ElementType, dimension0, dimension1, dimension2, dimension>::SliceType
    {
        return TensorVectorSlicer<ElementType, dimension0, dimension1, dimension2, dimension>::GetSlice(GetDataPointer(), this->GetShape(), this->GetIncrement1(), this->GetIncrement2(), index1, index2);
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    auto TensorReference<ElementType, dimension0, dimension1, dimension2>::GetPrimarySlice(size_t index) -> typename TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, dimension0, dimension1>::SliceType
    {
        return TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, dimension0, dimension1>::GetSlice(GetDataPointer(), this->GetShape(), this->GetIncrement1(), this->GetIncrement2(), index);
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    RowVectorReference<ElementType> TensorReference<ElementType, dimension0, dimension1, dimension2>::ReferenceAsVector()
    {
        DEBUG_THROW(this->GetSize0() != this->GetIncrement1() || this->GetSize0() * this->GetSize1() != this->GetIncrement2(), utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Can only flatten a tensor to vector when alll the dimensions are full"));

        return RowVectorReference<ElementType>(GetDataPointer(), this->Size(), 1);
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    RowMatrixReference<ElementType> TensorReference<ElementType, dimension0, dimension1, dimension2>::ReferenceAsMatrix()
    {
        DEBUG_THROW(this->GetSize0() != this->GetIncrement1(), utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Can only flatten a tensor when the first dimension is full"));

        return RowMatrixReference<ElementType>(GetDataPointer(), this->GetSize2(), this->GetSize0() * this->GetSize1(), this->GetIncrement2());
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    TensorReference<ElementType, dimension0, dimension1, dimension2>::TensorReference(ElementType* pData, TensorShape shape, size_t increment1, size_t increment2) :
        ConstTensorRef(pData, shape, increment1, increment2)
    {}

    template <Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t NumSlices(TensorReference<ElementType, dimension0, dimension1, dimension2> tensor)
    {
        return tensor.template NumSlices<rowDimension, columnDimension>();
    }

    template <Dimension dimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t NumSlices(TensorReference<ElementType, dimension0, dimension1, dimension2> tensor)
    {
        return tensor.template NumSlices<dimension>();
    }

    template <Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    auto GetSlice(TensorReference<ElementType, dimension0, dimension1, dimension2> tensor, size_t index)
    {
        return tensor.template GetSlice<rowDimension, columnDimension>(index);
    }

    template <Dimension dimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    auto GetSlice(TensorReference<ElementType, dimension0, dimension1, dimension2> tensor, size_t index1, size_t index2)
    {
        return tensor.template GetSlice<dimension>(index1, index2);
    }

    //
    // Tensor
    //

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor() :
        Tensor(TensorShape{ 0, 0, 0 })
    {
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(size_t numRows, size_t numColumns, size_t numChannels) :
        TensorRef(TensorShape(numRows, numColumns, numChannels)),
        _data(numRows * numColumns * numChannels)
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(size_t numRows, size_t numColumns, size_t numChannels, const std::vector<ElementType>& data) :
        TensorRef(TensorShape{ numRows, numColumns, numChannels }),
        _data(data)
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(size_t numRows, size_t numColumns, size_t numChannels, std::vector<ElementType>&& data) :
        TensorRef(TensorShape{ numRows, numColumns, numChannels }),
        _data(std::move(data))
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(TensorShape shape) :
        TensorRef(shape),
        _data(shape.Size())
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(const Tensor<ElementType, dimension0, dimension1, dimension2>& other) :
        TensorRef(other),
        _data(other._data)
    {
        this->_pData = _data.data();
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2> other) :
        TensorRef(TensorShape{ other.NumRows(), other.NumColumns(), other.NumChannels() }),
        _data(other.Size())
    {
        this->_pData = _data.data();
        this->CopyFrom(other);
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(std::initializer_list<std::initializer_list<std::initializer_list<ElementType>>> list) :
        TensorRef(TensorShape{ list.size(), list.begin()->size(), list.begin()->begin()->size() }),
        _data(list.size() * list.begin()->size() * list.begin()->begin()->size())
    {
        this->_pData = _data.data();
        auto numColumns = list.begin()->size();
        auto numChannels = list.begin()->begin()->size();
        DEBUG_USED(numColumns, numChannels);

        size_t i = 0;
        for (auto rowIter = list.begin(); rowIter < list.end(); ++rowIter)
        {
            DEBUG_CHECK_SIZES(rowIter->size() != numColumns, "incorrect number of elements in initializer list");

            size_t j = 0;
            for (auto columnIter = rowIter->begin(); columnIter < rowIter->end(); ++columnIter)
            {
                DEBUG_CHECK_SIZES(columnIter->size() != numChannels, "incorrect number of elements in initializer list");

                size_t k = 0;
                for (auto channelIter = columnIter->begin(); channelIter < columnIter->end(); ++channelIter)
                {
                    (*this)(i, j, k) = *channelIter;
                    ++k;
                }
                ++j;
            }
            ++i;
        }
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>& Tensor<ElementType, dimension0, dimension1, dimension2>::operator=(Tensor<ElementType, dimension0, dimension1, dimension2> other)
    {
        Swap(other);
        return *this;
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void Tensor<ElementType, dimension0, dimension1, dimension2>::Swap(Tensor<ElementType, dimension0, dimension1, dimension2>& other)
    {
        TensorRef::Swap(other);
        std::swap(_data, other._data);
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void TensorArchiver::Write(const Tensor<ElementType, dimension0, dimension1, dimension2>& tensor, const std::string& name, utilities::Archiver& archiver)
    {
        archiver[GetRowsName(name)] << tensor.NumRows();
        archiver[GetColumnsName(name)] << tensor.NumColumns();
        archiver[GetChannelsName(name)] << tensor.NumChannels();
        archiver[GetValuesName(name)] << tensor.ToArray();
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void TensorArchiver::Read(Tensor<ElementType, dimension0, dimension1, dimension2>& tensor, const std::string& name, utilities::Unarchiver& archiver)
    {
        size_t rows = 0;
        size_t columns = 0;
        size_t channels = 0;
        std::vector<ElementType> values;

        archiver[GetRowsName(name)] >> rows;
        archiver[GetColumnsName(name)] >> columns;
        archiver[GetChannelsName(name)] >> channels;
        archiver[GetValuesName(name)] >> values;

        Tensor<ElementType, dimension0, dimension1, dimension2> value(rows, columns, channels, std::move(values));

        tensor = std::move(value);
    }

} // namespace math
} // namespace ell

#pragma endregion implementation
