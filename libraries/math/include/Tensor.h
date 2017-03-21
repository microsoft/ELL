////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Tensor.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Matrix.h"

// stl
#include <cmath>
#include <iostream>
#include <vector>

// utilities
#include "Debug.h"
#include "Exception.h"

namespace ell
{
namespace math
{
    /// <summary> Enum that represents the dimensions of a tensor. </summary>
    enum class Dimension{ row, column, channel };

    /// <summary> Conventient struct used to pass around the layout of a 3d tensor. </summary>
    struct TensorLayout
    {
        size_t size0;
        size_t size1;
        size_t size2;
        size_t increment1;
        size_t increment2;
    };

    /// <summary> Base class for three dimensional tensors. </summary>
    ///
    /// <typeparam name="ElementType"> Tensor element type. </typeparam>
    template <typename ElementType>
    class TensorBase
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

        /// <summary> Gets the number of channels. </summary>
        ///
        /// <returns> The number of channels. </returns>
        size_t NumChannels() const { return _numChannels; }

    protected:
        // protected ctor accessible only through derived classes
        TensorBase(size_t numRows, size_t numColumns, size_t numChannels, TensorLayout layout, ElementType* pData = nullptr);

        TensorLayout _layout;
        size_t _numRows;
        size_t _numColumns;
        size_t _numChannels;
        ElementType* _pData;
    };

    //
    // TensorLayoutBase class specializations
    // 

    /// <summary>
    /// Forward declaration of TensorLayoutBase. This class adds memory layout information, e.g.,
    /// which dimension is stored contiguously. This class is specialized to specific memory layouts
    /// below.
    /// </summary>
    ///
    /// <typeparam name="ElementType"> Tensor element type. </typeparam>
    /// <typeparam name="dimension0"> Identity of the tensor dimension that occupies contiguous memory
    /// (increment of 1). </typeparam>
    /// <typeparam name="dimension1"> Identity of the tensor dimension with a minor memory increment. </typeparam>
    /// <typeparam name="dimension2"> Identity of the tensor dimension with a major memory increment. </typeparam>
    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    class TensorLayoutBase;

    /// <summary>
    /// Specialization to column/row/channel memory layout. This means that channels are stored
    /// separately, one after the other, and each channel is stored as a row-major matrix.
    /// </summary>
    ///
    /// <typeparam name="ElementType"> Tensor element type. </typeparam>
    template<typename ElementType>
    class TensorLayoutBase<ElementType, Dimension::column, Dimension::row, Dimension::channel> : public TensorBase<ElementType>
    {
    protected:
        using TensorBase = TensorBase<ElementType>;
        using TensorBase::TensorBase;
        TensorLayoutBase(size_t numRows, size_t numColumns, size_t numChannels);

        using TensorBase::_layout;

        const size_t _rowIncrement = _layout.increment1;
        static constexpr size_t _columnIncrement = 1;
        const size_t _channelIncrement = _layout.increment2;
    };

    /// <summary>
    /// Specialization to channel/column/row memory layout. For example, if the tensor represents an
    /// RGB image, the entries will be stored as R_00, G_00, B_00, R_01, G_01, B_01, ...
    /// </summary>
    ///
    /// <typeparam name="ElementType"> Tensor element type. </typeparam>
    template<typename ElementType>
    class TensorLayoutBase<ElementType, Dimension::channel, Dimension::column, Dimension::row> : public TensorBase<ElementType>
    {
    protected:
        using TensorBase = TensorBase<ElementType>;
        using TensorBase::TensorBase;
        TensorLayoutBase(size_t numRows, size_t numColumns, size_t numChannels);

        using TensorBase::_layout;

        const size_t _rowIncrement = _layout.increment2;
        const size_t _columnIncrement = _layout.increment1;
        static constexpr size_t _channelIncrement = 1;
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
    class ConstTensorReference : public TensorLayoutBase<ElementType, dimension0, dimension1, dimension2>
    {
    public:
        /// <summary> Gets a constant reference to this tensor. </summary>
        ///
        /// <returns> A constant tensor reference. </returns>
        ConstTensorReference GetConstTensorReference() const { return *this; }

        /// <summary> Element access operator. </summary>
        ///
        /// <param name="row"> The row. </param>
        /// <param name="column"> The column. </param>
        /// <param name="channel"> The channel. </param>
        ///
        /// <returns> A copy of a tensor element. </returns>
        ElementType operator()(size_t row, size_t column, size_t channel) const;

        /// <summary>
        /// Returns a const matrix reference that includes all of the tensor elements, by flattening the first
        /// tensor dimension.
        /// </summary>
        ///
        /// <returns>
        /// A const reference to a row major matrix, which includes all of the tensor elements.
        /// </returns>
        ConstMatrixReference<ElementType, MatrixLayout::rowMajor> FlattenFirstDimension() const;

        /// <summary> ConstTensorSlicer is a helper class in lieu of the ability to specialize the GetSlice() function </summary>
        template<Dimension rowDimension, Dimension columnDimension>
        struct ConstTensorSlicer;

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

        /// <summary> Gets a const reference to a subtensor (a block). </summary>
        ///
        /// <param name="firstRow"> The first row of the subtensor. </param>
        /// <param name="firstColumn"> The first column of the subtensor. </param>
        /// <param name="firstChannel"> The first channel of the subtensor. </param>
        /// <param name="numRows"> Number of rows in the subtensor. </param>
        /// <param name="numColumns"> Number of columns in the subtensor. </param>
        /// <param name="numChannels"> Number of channels in the subtensor. </param>
        ///
        /// <returns> The resulting TensorReference. </returns>
        ConstTensorReference<ElementType, dimension0, dimension1, dimension2> GetSubTensor(size_t firstRow, size_t firstColumn, size_t firstChannel, size_t numRows, size_t numColumns, size_t numChannels) const;

    protected:
        // abbreviations
        using TensorLayoutBase = TensorLayoutBase<ElementType, dimension0, dimension1, dimension2>;

        // protected ctors can only be accessed by derived classes
        using TensorLayoutBase::TensorLayoutBase;

        //
        // ConstTensorSlicers
        //

        template<>
        struct ConstTensorSlicer<dimension0, dimension1>
        {
            using ReturnType = ConstMatrixReference<ElementType, MatrixLayout::columnMajor>;
            static ReturnType GetSlice(TensorLayout layout, size_t index, ElementType* pData)
            {
                DEBUG_THROW(index >= layout.size2, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

                return ReturnType(layout.size0, layout.size1, layout.increment1, pData + index * layout.increment2);
            }
        };

        template<>
        struct ConstTensorSlicer<dimension0, dimension2>
        {
            using ReturnType = ConstMatrixReference<ElementType, MatrixLayout::columnMajor>;
            static ReturnType GetSlice(TensorLayout layout, size_t index, ElementType* pData)
            {
                DEBUG_THROW(index >= layout.size1, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

                return ReturnType(layout.size0, layout.size2, layout.increment2, pData + index * layout.increment1);
            }
        };

        template<>
        struct ConstTensorSlicer<dimension1, dimension0>
        {
            using ReturnType = ConstMatrixReference<ElementType, MatrixLayout::rowMajor>;
            static ReturnType GetSlice(TensorLayout layout, size_t index, ElementType* pData)
            {
                DEBUG_THROW(index >= layout.size2, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

                return ReturnType(layout.size1, layout.size0, layout.increment1, pData + index * layout.increment2);
            }
        };

        template<>
        struct ConstTensorSlicer<dimension2, dimension0>
        {
            using ReturnType = ConstMatrixReference<ElementType, MatrixLayout::rowMajor>;
            static ReturnType GetSlice(TensorLayout layout, size_t index, ElementType* pData)
            {
                DEBUG_THROW(index >= layout.size1, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

                return ReturnType(layout.size2, layout.size0, layout.increment2, pData + index * layout.increment1);
            }
        };

        // variables
        using TensorBase::_layout;
        using TensorBase::_numRows;
        using TensorBase::_numColumns;
        using TensorBase::_numChannels;
        using TensorBase::_pData;
        using TensorLayoutBase::_rowIncrement;
        using TensorLayoutBase::_columnIncrement;
        using TensorLayoutBase::_channelIncrement;
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

        /// <summary>
        /// Returns a matrix reference that includes all of the tensor elements, by flattening the first
        /// tensor dimension.
        /// </summary>
        ///
        /// <returns>
        /// A reference to a row major matrix, which includes all of the tensor elements.
        /// </returns>
        MatrixReference<ElementType, MatrixLayout::rowMajor> FlattenFirstDimension();

        /// <summary> TensorSlicer is a helper class in lieu of the ability to specialize the GetSlice() function </summary>
        template<Dimension rowDimension, Dimension columnDimension>
        struct TensorSlicer;

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

        /// <summary> Gets a reference to a subtensor (a block). </summary>
        ///
        /// <param name="firstRow"> The first row of the subtensor. </param>
        /// <param name="firstColumn"> The first column of the subtensor. </param>
        /// <param name="firstChannel"> The first channel of the subtensor. </param>
        /// <param name="numRows"> Number of rows in the subtensor. </param>
        /// <param name="numColumns"> Number of columns in the subtensor. </param>
        /// <param name="numChannels"> Number of channels in the subtensor. </param>
        ///
        /// <returns> The resulting TensorReference. </returns>
        TensorReference<ElementType, dimension0, dimension1, dimension2> GetSubTensor(size_t firstRow, size_t firstColumn, size_t firstChannel, size_t numRows, size_t numColumns, size_t numChannels);

    protected:
        // abbreviations
        using ConstTensorRef = ConstTensorReference<ElementType, dimension0, dimension1, dimension2>;

        // protected ctors can only be accessed by derived classes
        using ConstTensorRef::ConstTensorReference;
        TensorReference(size_t numRows, size_t numColumns, size_t numChannels);

        //
        // TensorSlicers
        //

        template<>
        struct TensorSlicer<dimension0, dimension1>
        {
            using ReturnType = MatrixReference<ElementType, MatrixLayout::columnMajor>;
            static ReturnType GetSlice(TensorLayout layout, size_t index, ElementType* pData)
            {
                DEBUG_THROW(index >= layout.size2, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

                return ReturnType(layout.size0, layout.size1, layout.increment1, pData + index * layout.increment2);
            }
        };

        template<>
        struct TensorSlicer<dimension0, dimension2>
        {
            using ReturnType = MatrixReference<ElementType, MatrixLayout::columnMajor>;
            static ReturnType GetSlice(TensorLayout layout, size_t index, ElementType* pData)
            {
                DEBUG_THROW(index >= layout.size1, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

                return ReturnType(layout.size0, layout.size2, layout.increment2, pData + index * layout.increment1);
            }
        };

        template<>
        struct TensorSlicer<dimension1, dimension0>
        {
            using ReturnType = MatrixReference<ElementType, MatrixLayout::rowMajor>;
            static ReturnType GetSlice(TensorLayout layout, size_t index, ElementType* pData)
            {
                DEBUG_THROW(index >= layout.size2, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

                return ReturnType(layout.size1, layout.size0, layout.increment1, pData + index * layout.increment2);
            }
        };

        template<>
        struct TensorSlicer<dimension2, dimension0>
        {
            using ReturnType = MatrixReference<ElementType, MatrixLayout::rowMajor>;
            static ReturnType GetSlice(TensorLayout layout, size_t index, ElementType* pData)
            {
                DEBUG_THROW(index >= layout.size1, utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

                return ReturnType(layout.size2, layout.size0, layout.increment2, pData + index * layout.increment1);
            }
        };

        // variables
        using TensorBase::_layout;
        using TensorBase::_numRows;
        using TensorBase::_numColumns;
        using TensorBase::_numChannels;
        using TensorBase::_pData;
        using TensorLayoutBase::_rowIncrement;
        using TensorLayoutBase::_columnIncrement;
        using TensorLayoutBase::_channelIncrement;
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
        /// <summary> Constructs a the zero tensor of given dimensions. </summary>
        ///
        /// <param name="numRows"> Number of rows. </param>
        /// <param name="numColumns"> Number of columns. </param>
        /// <param name="numChannels"> Number of channels. </param>
        Tensor(size_t numRows, size_t numColumns, size_t numChannels);
    
        /// <summary> Copy Constructor. </summary>
        ///
        /// <param name="other"> The other tensor. </param>
        Tensor(const Tensor<ElementType, dimension0, dimension1, dimension2>& other);

        /// <summary> Copies a tensor of a different layout. </summary>
        ///
        /// <param name="other"> The other tensor. </param>
        template<Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
        Tensor(ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2> other);

    private:
        // abbreviation
        using TensorRef = TensorReference<ElementType, dimension0, dimension1, dimension2>;
        
        // the array used to store the tensor
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
