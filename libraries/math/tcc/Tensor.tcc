////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Tensor.tcc (math)
//  Authors:  Ofer Dekel, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// stl
#include <algorithm> // for std::generate

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

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::ConstTensorReference(TensorShape shape) : ConstTensorReference<ElementType, dimension0, dimension1, dimension2>(nullptr, shape)
    {
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::ConstTensorReference(const ElementType* pData, TensorShape shape) : _pData(pData), _shape(shape)
    {
        _increment1 = shape.GetValue<dimension0>();
        _increment2 = _increment1 * shape.GetValue<dimension1>();
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template<size_t dimensionIndex>
    size_t ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::GetSize() const
    {
        constexpr auto dimension = std::get<dimensionIndex>(std::make_tuple(dimension0, dimension1, dimension2));
        return _shape.GetValue<dimension>();
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template<Dimension rowDimension, Dimension columnDimension>
    size_t ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::NumSlices() const
    {
        return TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, rowDimension, columnDimension>::NumSlices(_shape);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template<Dimension dimension>
    size_t ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::NumSlices() const
    {
        return TensorVectorSlicer<ElementType, dimension0, dimension1, dimension2, dimension>::NumSlices(_shape);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
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
        return{ GetConstDataPointer(), GetConstDataPointer() + Size() };
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
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

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template<Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
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

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template<Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
    bool ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::operator!=(const ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2>& other) const
    {
        return !IsEqual(other);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstTensorReference<ElementType, dimension0, dimension1, dimension2> ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::GetSubTensor(size_t firstRow, size_t firstColumn, size_t firstChannel, size_t numRows, size_t numColumns, size_t numChannels) const
    {
        return GetSubTensor({ firstRow, firstColumn, firstChannel }, { numRows, numColumns, numChannels });
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
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

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstRowVectorReference<ElementType> ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::ReferenceAsVector() const
    {
        DEBUG_THROW(GetSize0() != GetIncrement1() || GetSize0() * GetSize1() != GetIncrement2(), utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Can only flatten a tensor when alll the dimensions are full"));

        return ConstRowVectorReference<ElementType>(GetConstDataPointer(), Size(), 1);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstRowMatrixReference<ElementType> ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::ReferenceAsMatrix() const
    {
        DEBUG_THROW(GetSize0() != GetIncrement1(), utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Can only flatten a tensor when the first dimension is full"));

        return ConstRowMatrixReference<ElementType>(GetConstDataPointer(), GetSize2(), GetSize0() * GetSize1(), GetIncrement2());
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::GetOffset(TensorCoordinate coordinate) const
    {
        DEBUG_THROW(coordinate.GetRowIndex() >= NumRows() || coordinate.GetColumnIndex() >= NumColumns() || coordinate.GetChannelIndex() >= NumChannels(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange,
            std::string("index exceeds tensor size in ConstTensorReference::GetOffset().") +
            " Tensor size: (" + std::to_string(NumRows()) + " x " + std::to_string(NumColumns()) + " x " + std::to_string(NumChannels()) + "), "
            " index: (" + std::to_string(coordinate.GetRowIndex()) + ", " + std::to_string(coordinate.GetColumnIndex()) + ", " + std::to_string(coordinate.GetChannelIndex()) + ")" ));

        return coordinate.GetValue<dimension0>() + coordinate.GetValue<dimension1>() * GetIncrement1() + coordinate.GetValue<dimension2>() * GetIncrement2();
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::ConstTensorReference(const ElementType* pData, TensorShape shape, size_t increment1, size_t increment2)
        : _pData(pData), _shape(shape), _increment1(increment1), _increment2(increment2)
    {}

    template<Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t NumSlices(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> tensor)
    {
        return tensor.template NumSlices<rowDimension, columnDimension>();
    }

    template <Dimension dimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t NumSlices(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> tensor)
    {
        return tensor.template NumSlices<dimension>();
    }

    template<Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
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

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    TensorReference<ElementType, dimension0, dimension1, dimension2>::TensorReference(ElementType* pData, size_t numRows, size_t numColumns, size_t numChannels)
        : ConstTensorRef(pData, TensorShape{ numRows, numColumns, numChannels })
    {}

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ElementType& TensorReference<ElementType, dimension0, dimension1, dimension2>::operator()(size_t row, size_t column, size_t channel)
    {
        return operator()({ row, column, channel });
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ElementType& TensorReference<ElementType, dimension0, dimension1, dimension2>::operator()(TensorCoordinate coordinate)
    {
        return GetDataPointer()[this->GetOffset(coordinate)];
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void TensorReference<ElementType, dimension0, dimension1, dimension2>::Swap(TensorReference<ElementType, dimension0, dimension1, dimension2>& other)
    {
        ConstTensorRef::Swap(other);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
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

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void TensorReference<ElementType, dimension0, dimension1, dimension2>::CopyFrom(ConstTensorReference<ElementType, dimension0, dimension2, dimension1> other)
    {
        DEBUG_CHECK_SIZES(this->NumRows() != other.NumRows(),"Tensors must have the same number of rows");
        DEBUG_CHECK_SIZES(this->NumColumns() != other.NumColumns(),"Tensors must have the same number of columns");
        DEBUG_CHECK_SIZES(this->NumChannels() != other.NumChannels(),"Tensors must have the same number of channels");

        for (size_t i = 0; i < NumSlices<dimension0, dimension1>(*this); ++i)
        {
            GetSlice<dimension0, dimension1>(i).CopyFrom(GetSlice<dimension0, dimension1>(other, i));
        }
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template<Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
    void TensorReference<ElementType, dimension0, dimension1, dimension2>::CopyFrom(ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2> other)
    {
        DEBUG_CHECK_SIZES(this->NumRows() != other.NumRows(),"Tensors must have the same number of rows");
        DEBUG_CHECK_SIZES(this->NumColumns() != other.NumColumns(),"Tensors must have the same number of columns");
        DEBUG_CHECK_SIZES(this->NumChannels() != other.NumChannels(),"Tensors must have the same number of channels");

        for (size_t i = 0; i < math::NumSlices<dimension0, otherDimension0>(*this); ++i)
        {
            auto thisSlice = GetSlice<dimension0, otherDimension0>(i);
            auto otherSlice = other.template GetSlice<dimension0, otherDimension0>(i);
            thisSlice.CopyFrom(otherSlice);
        }
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void TensorReference<ElementType, dimension0, dimension1, dimension2>::Fill(ElementType value)
    {
        for (size_t i = 0; i < this->NumPrimarySlices(); ++i)
        {
            auto slice = GetPrimarySlice(i);
            slice.Fill(value);
        }
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <typename GeneratorType>
    void TensorReference<ElementType, dimension0, dimension1, dimension2>::Generate(GeneratorType generator)
    {
        for (size_t i = 0; i < this->NumPrimarySlices(); ++i)
        {
            auto slice = GetPrimarySlice(i);
            slice.Generate(generator);
        }
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
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

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template<Dimension rowDimension, Dimension columnDimension>
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

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    auto TensorReference<ElementType, dimension0, dimension1, dimension2>::GetPrimarySlice(size_t index) -> typename TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, dimension0, dimension1>::SliceType
    {
        return TensorMatrixSlicer<ElementType, dimension0, dimension1, dimension2, dimension0, dimension1>::GetSlice(GetDataPointer(), this->GetShape(), this->GetIncrement1(), this->GetIncrement2(), index);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    RowVectorReference<ElementType> TensorReference<ElementType, dimension0, dimension1, dimension2>::ReferenceAsVector()
    {
        DEBUG_THROW(this->GetSize0() != this->GetIncrement1() || this->GetSize0() * this->GetSize1() != this->GetIncrement2(), utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Can only flatten a tensor to vector when alll the dimensions are full"));

        return RowVectorReference<ElementType>(GetDataPointer(), this->Size(), 1);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    RowMatrixReference<ElementType> TensorReference<ElementType, dimension0, dimension1, dimension2>::ReferenceAsMatrix()
    {
        DEBUG_THROW(this->GetSize0() != this->GetIncrement1(), utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Can only flatten a tensor when the first dimension is full"));

        return RowMatrixReference<ElementType>(GetDataPointer(), this->GetSize2(), this->GetSize0() * this->GetSize1(), this->GetIncrement2());
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    TensorReference<ElementType, dimension0, dimension1, dimension2>::TensorReference(ElementType* pData, TensorShape shape, size_t increment1, size_t increment2)
        : ConstTensorRef(pData, shape, increment1, increment2)
    {}

    template<Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t NumSlices(TensorReference<ElementType, dimension0, dimension1, dimension2> tensor)
    {
        return tensor.template NumSlices<rowDimension, columnDimension>();
    }

    template <Dimension dimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t NumSlices(TensorReference<ElementType, dimension0, dimension1, dimension2> tensor)
    {
        return tensor.template NumSlices<dimension>();
    }

    template<Dimension rowDimension, Dimension columnDimension, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
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
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor()
        : Tensor(TensorShape{ 0, 0, 0 })
    {
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(size_t numRows, size_t numColumns, size_t numChannels)
        : TensorRef(TensorShape(numRows, numColumns, numChannels)), _data(numRows * numColumns * numChannels)
    {
         this->_pData = _data.data();
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(size_t numRows, size_t numColumns, size_t numChannels, const std::vector<ElementType>& data)
        : TensorRef(TensorShape{ numRows, numColumns, numChannels }), _data(data)
    {
         this->_pData = _data.data();
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(size_t numRows, size_t numColumns, size_t numChannels, std::vector<ElementType>&& data)
        : TensorRef(TensorShape{ numRows, numColumns, numChannels }), _data(std::move(data))
    {
         this->_pData = _data.data();
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(TensorShape shape)
        : TensorRef(shape), _data(shape.Size())
    {
         this->_pData = _data.data();
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(const Tensor<ElementType, dimension0, dimension1, dimension2>& other)
        : TensorRef(other), _data(other._data)
    {
         this->_pData = _data.data();
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2> other)
        : TensorRef(TensorShape{ other.NumRows(), other.NumColumns(), other.NumChannels() }), _data(other.Size())
    {
        this->_pData = _data.data();
        this->CopyFrom(other);
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(std::initializer_list<std::initializer_list<std::initializer_list<ElementType>>> list)
        : TensorRef(TensorShape{ list.size(), list.begin()->size(), list.begin()->begin()->size() }), _data(list.size() * list.begin()->size() * list.begin()->begin()->size())
    {
         this->_pData = _data.data();
        auto numColumns = list.begin()->size();
        auto numChannels = list.begin()->begin()->size();
        DEBUG_USED(numColumns, numChannels);

        size_t i = 0;
        for (auto rowIter = list.begin(); rowIter < list.end(); ++rowIter)
        {
            DEBUG_CHECK_SIZES(rowIter->size() != numColumns,"incorrect number of elements in initializer list");

            size_t j = 0;
            for (auto columnIter = rowIter->begin(); columnIter < rowIter->end(); ++columnIter)
            {
                DEBUG_CHECK_SIZES(columnIter->size() != numChannels,"incorrect number of elements in initializer list");

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

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>& Tensor<ElementType, dimension0, dimension1, dimension2>::operator=(Tensor<ElementType, dimension0, dimension1, dimension2> other)
    {
        Swap(other);
        return *this;
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void Tensor<ElementType, dimension0, dimension1, dimension2>::Swap(Tensor<ElementType, dimension0, dimension1, dimension2>& other)
    {
        TensorRef::Swap(other);
        std::swap(_data, other._data);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void TensorArchiver::Write(const Tensor<ElementType, dimension0, dimension1, dimension2>& tensor, const std::string& name, utilities::Archiver& archiver)
    {
        archiver[GetRowsName(name)] << tensor.NumRows();
        archiver[GetColumnsName(name)] << tensor.NumColumns();
        archiver[GetChannelsName(name)] << tensor.NumChannels();
        archiver[GetValuesName(name)] << tensor.ToArray();
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
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

}
}
