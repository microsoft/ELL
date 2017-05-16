////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Tensor.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Operations.h"

// stl
#include <algorithm> // for std::generate

namespace ell
{
namespace math
{
    //
    // TensorSlicers
    //

    /// <summary> TensorSlicer is a helper class in lieu of the ability to specialize the GetSlice() function </summary>
    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2, Dimension rowDimension, Dimension columnDimension>
    struct TensorSlicer;

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    struct TensorSlicer<ElementType, dimension0, dimension1, dimension2, dimension0, dimension1>
    {
        using SliceType = MatrixReference<ElementType, MatrixLayout::columnMajor>;
        using ConstSliceType = ConstMatrixReference<ElementType, MatrixLayout::columnMajor>;
        
        static SliceType GetSlice(const TensorContents<ElementType>& contents, size_t index)
        {
            return GetSlice<SliceType>(contents, index);
        }

        static ConstSliceType GetConstSlice(const TensorContents<ElementType>& contents, size_t index)
        {
            return GetSlice<SliceType>(contents, index);
        }

    private:
        template <typename ReturnType>
        static ReturnType GetSlice(const TensorContents<ElementType>& contents, size_t index)
        {
            DEBUG_THROW(index >= contents.layout[2], utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

            return ReturnType(contents.layout[0], contents.layout[1], contents.increments[0], contents.pData + index * contents.increments[1]);
        }
    };

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    struct TensorSlicer<ElementType, dimension0, dimension1, dimension2, dimension0, dimension2>
    {
        using SliceType = MatrixReference<ElementType, MatrixLayout::columnMajor>;
        using ConstSliceType = ConstMatrixReference<ElementType, MatrixLayout::columnMajor>;

        static SliceType GetSlice(const TensorContents<ElementType>& contents, size_t index)
        {
            return GetSlice<SliceType>(contents, index);
        }

        static ConstSliceType GetConstSlice(const TensorContents<ElementType>& contents, size_t index)
        {
            return GetSlice<SliceType>(contents, index);
        }

    private:
        template <typename ReturnType>
        static ReturnType GetSlice(const TensorContents<ElementType>& contents, size_t index)
        {
            DEBUG_THROW(index >= contents.layout[1], utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

            return ReturnType(contents.layout[0], contents.layout[2], contents.increments[1], contents.pData + index * contents.increments[0]);
        }

    };

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    struct TensorSlicer<ElementType, dimension0, dimension1, dimension2, dimension1, dimension0>
    {
        using SliceType = MatrixReference<ElementType, MatrixLayout::rowMajor>;
        using ConstSliceType = ConstMatrixReference<ElementType, MatrixLayout::rowMajor>;

        static SliceType GetSlice(const TensorContents<ElementType>& contents, size_t index)
        {
            return GetSlice<SliceType>(contents, index);
        }

        static ConstSliceType GetConstSlice(const TensorContents<ElementType>& contents, size_t index)
        {
            return GetSlice<SliceType>(contents, index);
        }

    private:
        template <typename ReturnType>
        static ReturnType GetSlice(const TensorContents<ElementType>& contents, size_t index)
        {
            DEBUG_THROW(index >= contents.layout[2], utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

            return ReturnType(contents.layout[1], contents.layout[0], contents.increments[0], contents.pData + index * contents.increments[1]);
        }
    };

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    struct TensorSlicer<ElementType, dimension0, dimension1, dimension2, dimension2, dimension0>
    {
        using SliceType = MatrixReference<ElementType, MatrixLayout::rowMajor>;
        using ConstSliceType = MatrixReference<ElementType, MatrixLayout::rowMajor>;
        
        static SliceType GetSlice(const TensorContents<ElementType>& contents, size_t index)
        {
            return GetSlice<SliceType>(contents, index);
        }

        static ConstSliceType GetConstSlice(const TensorContents<ElementType>& contents, size_t index)
        {
            return GetSlice<SliceType>(contents, index);
        }

    private:
        template <typename ReturnType>
        static ReturnType GetSlice(const TensorContents<ElementType>& contents, size_t index)
        {
            DEBUG_THROW(index >= contents.layout[1], utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));

            return ReturnType(contents.layout[2], contents.layout[0], contents.increments[1], contents.pData + index * contents.increments[0]);
        }
    };
    
    //
    // ConstTensorReference
    // 
    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::ConstTensorReference(Triplet shape)
    {
        _contents.layout = TensorLayoutT::CanonicalToLayout(shape);
        _contents.increments[0] = _contents.layout[0];
        _contents.increments[1] = _contents.layout[0] * _contents.layout[1];
        _contents.pData = nullptr;
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::NumElements() const
    {
        return _contents.layout[0] * _contents.layout[1] * _contents.layout[2];
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ElementType ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::operator()(size_t row, size_t column, size_t channel) const
    {
        return operator()({ row, column, channel });
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ElementType ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::operator()(Triplet coordinate) const
    {
        return _contents.pData[this->GetOffset(coordinate)];
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

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template<Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
    bool ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::operator==(const ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2>& other) const
    {
        return IsEqual(other);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstVectorReference<ElementType, VectorOrientation::row> ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::ReferenceAsVector() const
    {
        DEBUG_THROW(_contents.layout[0] != _contents.increments[0] || _contents.layout[0]*_contents.layout[1] != _contents.increments[1], utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Can only flatten a tensor when alll the dimensions are full"));

        return ConstVectorReference<ElementType, VectorOrientation::row>(_contents.pData, NumElements(), 1);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstMatrixReference<ElementType, MatrixLayout::rowMajor> ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::ReferenceAsMatrix() const
    {
        DEBUG_THROW(_contents.layout[0] != _contents.increments[0], utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Can only flatten a tensor when the first dimension is full"));

        return ConstMatrixReference<ElementType, MatrixLayout::rowMajor>(_contents.layout[2], _contents.layout[0] * _contents.layout[1], _contents.increments[1], _contents.pData);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template<Dimension rowDimension, Dimension columnDimension>
    auto ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::GetSlice(size_t index) const
    {
        return TensorSlicer<ElementType, dimension0, dimension1, dimension2, rowDimension, columnDimension>::GetConstSlice(_contents, index);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    auto ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::GetPrimarySlice(size_t index) const
    {
        return TensorSlicer<ElementType, dimension0, dimension1, dimension2, dimension0, dimension1>::GetConstSlice(_contents, index);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstTensorReference<ElementType, dimension0, dimension1, dimension2> ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::GetSubTensor(size_t firstRow, size_t firstColumn, size_t firstChannel, size_t numRows, size_t numColumns, size_t numChannels) const
    {
        return GetSubTensor({ firstRow, firstColumn, firstChannel }, { numRows, numColumns, numChannels });
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstTensorReference<ElementType, dimension0, dimension1, dimension2> ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::GetSubTensor(Triplet firstCoordinate, Triplet shape) const
    {
        DEBUG_THROW(firstCoordinate[0]+shape[0] > NumRows() || firstCoordinate[1]+shape[1] > NumColumns() || firstCoordinate[2]+shape[2] > NumChannels(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "subtensor exceeds tensor dimensions."));

        return ConstTensorReference({ TensorLayoutT::CanonicalToLayout(shape), _contents.increments, _contents.pData + this->GetOffset(firstCoordinate) });
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::ConstTensorReference(TensorContents<ElementType> contents)
        : _contents(std::move(contents))
    {
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    size_t ConstTensorReference<ElementType, dimension0, dimension1, dimension2>::GetOffset(Triplet coordinate) const
    {
        auto layoutCoordinate = TensorLayoutT::CanonicalToLayout(coordinate);
        DEBUG_THROW(layoutCoordinate[0] >= _contents.layout[0] || layoutCoordinate[1] >= _contents.layout[1] || layoutCoordinate[2] >= _contents.layout[2], utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "index exceeds tensor dimensions."));
        return layoutCoordinate[0] + layoutCoordinate[1] * _contents.increments[0] + layoutCoordinate[2] * _contents.increments[1];
    }

    //
    // TensorReference
    // 

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ElementType& TensorReference<ElementType, dimension0, dimension1, dimension2>::operator()(size_t row, size_t column, size_t channel)
    {
        return operator()({ row, column, channel });
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    ElementType& TensorReference<ElementType, dimension0, dimension1, dimension2>::operator()(Triplet coordinate)
    {
        return _contents.pData[this->GetOffset(coordinate)];
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    VectorReference<ElementType, VectorOrientation::row> TensorReference<ElementType, dimension0, dimension1, dimension2>::ReferenceAsVector()
    {
        DEBUG_THROW(_contents.layout[0] != _contents.increments[0] || _contents.layout[0] * _contents.layout[1] != _contents.increments[1], utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Can only flatten a tensor to vector when alll the dimensions are full"));

        return VectorReference<ElementType, VectorOrientation::row>(_contents.pData, NumElements(), 1);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    MatrixReference<ElementType, MatrixLayout::rowMajor> TensorReference<ElementType, dimension0, dimension1, dimension2>::ReferenceAsMatrix()
    {
        DEBUG_THROW(_contents.layout[0] != _contents.increments[0], utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Can only flatten a tensor when the first dimension is full"));

        return MatrixReference<ElementType, MatrixLayout::rowMajor>(_contents.layout[2], _contents.layout[0] * _contents.layout[1], _contents.increments[1], _contents.pData);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template<Dimension rowDimension, Dimension columnDimension>
    auto TensorReference<ElementType, dimension0, dimension1, dimension2>::GetSlice(size_t index)
    {
        return TensorSlicer<ElementType, dimension0, dimension1, dimension2, rowDimension, columnDimension>::GetSlice(_contents, index);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    auto TensorReference<ElementType, dimension0, dimension1, dimension2>::GetPrimarySlice(size_t index)
    {
        return TensorSlicer<ElementType, dimension0, dimension1, dimension2, dimension0, dimension1>::GetSlice(_contents, index);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    TensorReference<ElementType, dimension0, dimension1, dimension2> TensorReference<ElementType, dimension0, dimension1, dimension2>::GetSubTensor(size_t firstRow, size_t firstColumn, size_t firstChannel, size_t numRows, size_t numColumns, size_t numChannels)
    {
        return GetSubTensor({ firstRow, firstColumn, firstChannel }, { numRows, numColumns, numChannels });
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    TensorReference<ElementType, dimension0, dimension1, dimension2> TensorReference<ElementType, dimension0, dimension1, dimension2>::GetSubTensor(Triplet firstCoordinate, Triplet shape)
    {
        DEBUG_THROW(firstCoordinate[0] + shape[0] > this->NumRows() || firstCoordinate[1] + shape[1] > this->NumColumns() || firstCoordinate[2] + shape[2] > this->NumChannels(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "subtensor exceeds tensor dimensions."));

        return TensorReference({ TensorLayoutT::CanonicalToLayout(shape), _contents.increments, _contents.pData + this->GetOffset(firstCoordinate) });
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void TensorReference<ElementType, dimension0, dimension1, dimension2>::Fill(ElementType value)
    {
        for (size_t i = 0; i < _contents.layout[2]; ++i)
        {
            auto slice = GetPrimarySlice(i);
            slice.Fill(value);
        }
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <typename GeneratorType>
    void TensorReference<ElementType, dimension0, dimension1, dimension2>::Generate(GeneratorType generator)
    {
        for (size_t i = 0; i < _contents.layout[2]; ++i)
        {
            auto slice = GetPrimarySlice(i);
            slice.Generate(generator);
        }
    }

    //
    // Tensor
    // 

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(size_t numRows, size_t numColumns, size_t numChannels) 
        : Tensor(Triplet{ numRows, numColumns, numChannels }) 
    {
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(Triplet shape)
        : TensorRef(shape), _data(shape[0] * shape[1] * shape[2])
    {
        _contents.pData = _data.data();
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(const Tensor<ElementType, dimension0, dimension1, dimension2>& other) 
        : TensorRef(other), _data(other._data)
    {
        _contents.pData = _data.data();
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template<Dimension otherDimension0, Dimension otherDimension1, Dimension otherDimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(ConstTensorReference<ElementType, otherDimension0, otherDimension1, otherDimension2> other)
        : TensorRef(Triplet{ other.NumRows(), other.NumColumns(), other.NumChannels() }), _data(other.NumElements())
    {
        _contents.pData = _data.data();
        for (size_t i = 0; i < this->NumRows(); ++i)
        {
            for (size_t j = 0; j < this->NumColumns(); ++j)
            {
                for (size_t k = 0; k < this->NumChannels(); ++k)
                {
                    (*this)(i, j, k) = other(i, j, k);
                }
            }
        }
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    Tensor<ElementType, dimension0, dimension1, dimension2>::Tensor(std::initializer_list<std::initializer_list<std::initializer_list<ElementType>>> list)
        : TensorRef(Triplet{ list.size(), list.begin()->size(), list.begin()->begin()->size() }), _data(list.size() * list.begin()->size() * list.begin()->begin()->size())
    {
        _contents.pData = _data.data();
        auto numColumns = list.begin()->size();
        auto numChannels = list.begin()->begin()->size();

        size_t i = 0;
        for (auto rowIter = list.begin(); rowIter < list.end(); ++rowIter)
        {
            DEBUG_THROW(rowIter->size() != numColumns, utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "incorrect number of elements in initializer list"));

            size_t j = 0;
            for (auto columnIter = rowIter->begin(); columnIter < rowIter->end(); ++columnIter)
            {
                DEBUG_THROW(columnIter->size() != numChannels, utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "incorrect number of elements in initializer list"));

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
    void Tensor<ElementType, dimension0, dimension1, dimension2>::Fill(ElementType value)
    {
        std::fill(_data.begin(), _data.end(), value);
    }

    template<typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    template <typename GeneratorType>
    void Tensor<ElementType, dimension0, dimension1, dimension2>::Generate(GeneratorType generator)
    {
        std::generate(_data.begin(), _data.end(), generator);;
    }

}
}