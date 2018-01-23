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

// stl
#include <cmath>
#include <vector>
#include <array>

// utilities
#include "Debug.h"
#include "Exception.h"
#include "IArchivable.h"

// stl
#include <array>
#include <initializer_list>
#include <tuple>
#include <vector>
#include <ostream>

namespace ell
{
namespace math
{
    /// <summary> Enum that represents the dimensions of a tensor. </summary>
    enum class Dimension { row, column, channel };

    // abbreviations
    using IntegerTriplet = std::array<size_t, 3>;

    /// <summary> Base class for TensorCoordinate and TensorShape. </summary>
    class TensorCoordinateBase
    {
    public:
        /// <summary> Constructs a TensorCoordinateBase from row, column, channel values. </summary>
        ///
        /// <param name="rowValue"> The row value. </param>
        /// <param name="columnValue"> The column value. </param>
        /// <param name="channelValue"> The channel value. </param>
        TensorCoordinateBase(size_t rowValue, size_t columnValue, size_t channelValue);

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
        operator IntegerTriplet() const { return {_rowValue, _columnValue, _channelValue}; }

        /// <summary> Casting operator to a vector equivalent to IntegerTriplet. </summary>
        ///
        /// <returns> The result of the cast. </returns>
        operator std::vector<size_t>() const { return{ _rowValue, _columnValue, _channelValue }; }

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
        template<Dimension dimension>
        size_t GetValue() const;

        friend std::ostream& operator<<(std::ostream& stream, const TensorCoordinateBase& tensor)
        {
            stream << "(" << tensor._rowValue << ", " << tensor._columnValue << ", " << tensor._channelValue << ")";
            return stream;
        }

    protected:
        size_t _rowValue;
        size_t _columnValue;
        size_t _channelValue;
    };

    /// <summary> A class that defines the shape of a tensor. </summary>
    class TensorShape : public TensorCoordinateBase
    {
    public:
        using TensorCoordinateBase::TensorCoordinateBase;

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
        using TensorCoordinateBase::TensorCoordinateBase;

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
        template<size_t dimensionIndex>
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
        template<Dimension rowDimension, Dimension columnDimension>
        size_t NumSlices() const;

        /// <summary>
        /// Gets the number of 1D slices that can be returned by GetSlice.
        /// </summary>
        ///
        /// <typeparam name="dimension"> Which tensor dimension to use for the slice. </typeparam>
        ///
        /// <returns> The number of 1D slices. </returns>
        template<Dimension dimension>
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

        /// @}

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
        template<Dimension rowDimension, Dimension columnDimension>
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
        size_t GetOffset(TensorCoordinate coordinate) const;
        ConstTensorReference(const ElementType* pData, TensorShape shape, size_t increment1, size_t increment2);

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
    template<Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
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
    template<Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
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
    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    class TensorReference : public ConstTensorReference<ElementType, dimension0, dimension1, dimension2>
    {
    public:
        using ConstTensorRef = ConstTensorReference<ElementType, dimension0, dimension1, dimension2>;
        using TensorElementType = typename ConstTensorRef::TensorElementType;

        using ConstTensorRef::ConstTensorReference;

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

        using ConstTensorRef::GetSubTensor;
        using ConstTensorRef::GetSlice;
        using ConstTensorRef::GetPrimarySlice;
        using ConstTensorRef::ReferenceAsVector;
        using ConstTensorRef::ReferenceAsMatrix;

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
        template<Dimension rowDimension, Dimension columnDimension>
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
    template<Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
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
    template<Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
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
    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    class Tensor : public TensorReference<ElementType, dimension0, dimension1, dimension2>
    {
    public:
        using TensorRef = TensorReference<ElementType, dimension0, dimension1, dimension2>;
        using TensorElementType = typename TensorRef::TensorElementType;

        using TensorRef::TensorReference;

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
        template<Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
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
    /// Specialization to column/row/channel memory layout. This means that channels are stored
    /// one after the other and each channel is stored as a row-major matrix.
    /// </summary>
    template <typename ElementType>
    using ColumnRowChannelTensor = Tensor<ElementType, Dimension::column, Dimension::row, Dimension::channel>;
}
}
#include "../tcc/Tensor.tcc"
