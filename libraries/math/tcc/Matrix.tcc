////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Matrix.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Exception.h"

// utilities
#include "Debug.h"
#include "Exception.h"

// stl
#include <algorithm> // for std::generate

namespace ell
{
namespace math
{
    //
    // CommonMatrixBase
    //

    template <typename ElementType>
    CommonMatrixBase<ElementType>::CommonMatrixBase(size_t numRows, size_t numColumns, size_t increment, ElementType* pData)
        : _pData(pData), _numRows(numRows), _numColumns(numColumns), _increment(increment)
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
    MatrixBase<ElementType, MatrixLayout::rowMajor>::MatrixBase(size_t numRows, size_t numColumns, ElementType* pData)
        : CommonMatrixBase<ElementType>(numRows, numColumns, numColumns, pData)
    {
    }

    template <typename ElementType>
    void MatrixBase<ElementType, MatrixLayout::rowMajor>::Swap(MatrixBase<ElementType, MatrixLayout::rowMajor>& other)
    {
        CommonMatrixBase<ElementType>::Swap(other);
    }

    // Column-major
    template <typename ElementType>
    MatrixBase<ElementType, MatrixLayout::columnMajor>::MatrixBase(size_t numRows, size_t numColumns, ElementType* pData)
        : CommonMatrixBase<ElementType>(numRows, numColumns, numRows, pData)
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
    ElementType ConstMatrixReference<ElementType, layout>::operator()(size_t rowIndex, size_t columnIndex) const
    {
        DEBUG_THROW(rowIndex >= NumRows() || columnIndex >= NumColumns(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "(rowIndex, columnIndex) exceeds matrix dimensions."));

        return _pData[rowIndex * GetRowIncrement() + columnIndex * GetColumnIncrement()];
    }

    template <typename ElementType, MatrixLayout layout>
    std::vector<ElementType> ConstMatrixReference<ElementType, layout>::ToArray() const
    {
        std::vector<ElementType> v(Size());
        auto vIterator = v.begin();
        for (size_t i = 0; i < NumIntervals(); ++i)
        {
            auto pIntervalData = GetMajorVectorBegin(i);
            std::copy(pIntervalData, pIntervalData + GetIntervalSize(), vIterator);
            vIterator += GetIntervalSize();
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
        if (NumRows() != other.NumRows() || NumColumns() != other.NumColumns())
        {
            return false;
        }

        for (size_t i = 0; i < NumIntervals(); ++i)
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
        if (NumRows() != other.NumRows() || NumColumns() != other.NumColumns())
        {
            return false;
        }

        for (size_t i = 0; i < NumRows(); ++i)
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
        DEBUG_THROW(firstRow + numRows > NumRows() || firstColumn + numColumns > NumColumns(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "block exceeds matrix dimensions."));

        return ConstMatrixReference<ElementType, layout>(numRows, numColumns, GetIncrement(), _pData + firstRow * GetRowIncrement() + firstColumn * GetColumnIncrement());
    }

    template <typename ElementType, MatrixLayout layout>
    ConstVectorReference<ElementType, VectorOrientation::column> ConstMatrixReference<ElementType, layout>::GetColumn(size_t index) const
    {
        DEBUG_THROW(index >= NumColumns(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "column index exceeds matrix dimensions."));

        return ConstVectorReference<ElementType, VectorOrientation::column>(_pData + index * GetColumnIncrement(), NumRows(), GetRowIncrement());
    }

    template <typename ElementType, MatrixLayout layout>
    ConstVectorReference<ElementType, VectorOrientation::row> ConstMatrixReference<ElementType, layout>::GetRow(size_t index) const
    {
        DEBUG_THROW(index >= NumRows(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "row index exceeds matrix dimensions."));

        return ConstVectorReference<ElementType, VectorOrientation::row>(_pData + index * GetRowIncrement(), NumColumns(), GetColumnIncrement());
    }

    template <typename ElementType, MatrixLayout layout>
    ConstVectorReference<ElementType, VectorOrientation::column> ConstMatrixReference<ElementType, layout>::GetDiagonal() const
    {
        auto size = std::min(NumColumns(), NumRows());
        return ConstVectorReference<ElementType, VectorOrientation::column>(_pData, size, GetIncrement() + 1);
    }

    template <typename ElementType, MatrixLayout layout>
    ConstVectorReference<ElementType, VectorOrientation::column> ConstMatrixReference<ElementType, layout>::ReferenceAsVector() const
    {
        DEBUG_THROW(!IsContiguous(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Can only flatten a matrix when its memory is contiguous"));
        return ConstVectorReference<ElementType, VectorOrientation::column>(_pData, NumRows() * NumColumns(), 1);
    }

    template <typename ElementType, MatrixLayout layout>
    auto ConstMatrixReference<ElementType, layout>::Transpose() const -> ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value>
    {
        return ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value>(NumColumns(), NumRows(), GetIncrement(), _pData);
    }

    template <typename ElementType, MatrixLayout layout>
    ElementType* ConstMatrixReference<ElementType, layout>::GetMajorVectorBegin(size_t index) const
    {
        return _pData + index * GetIncrement();
    }

    //
    // MatrixReference
    //

    template <typename ElementType, MatrixLayout layout>
    ElementType& MatrixReference<ElementType, layout>::operator()(size_t rowIndex, size_t columnIndex)
    {
        DEBUG_THROW(rowIndex >= NumRows() || columnIndex >= NumColumns(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "(rowIndex, columnIndex) exceeds matrix dimensions."));

        return _pData[rowIndex * GetRowIncrement() + columnIndex * GetColumnIncrement()];
    }

    template <typename ElementType, MatrixLayout layout>
    void MatrixReference<ElementType, layout>::CopyFrom(ConstMatrixReference<ElementType, layout> other)
    {
        if (NumRows() != other.NumRows() || NumColumns() != other.NumColumns())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Matrix dimensions are not the same.");
        }

        for (size_t i = 0; i < other.NumIntervals(); ++i)
        {
            GetMajorVector(i).CopyFrom(other.GetMajorVector(i));
        }
    }

    template <typename ElementType, MatrixLayout layout>
    void MatrixReference<ElementType, layout>::CopyFrom(ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value> other)
    {
        if (NumRows() != other.NumRows() || NumColumns() != other.NumColumns())
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
        for (size_t i = 0; i < NumIntervals(); ++i)
        {
            auto begin = GetMajorVectorBegin(i);
            std::fill(begin, begin + GetIntervalSize(), value);
        }
    }

    template <typename ElementType, MatrixLayout layout>
    template <typename GeneratorType>
    void MatrixReference<ElementType, layout>::Generate(GeneratorType generator)
    {
        for (size_t i = 0; i < NumIntervals(); ++i)
        {
            GetMajorVector(i).Generate(generator);
        }
    }

    template <typename ElementType, MatrixLayout layout>
    template <typename TransformationType>
    void MatrixReference<ElementType, layout>::Transform(TransformationType transformation)
    {
        for (size_t i = 0; i < NumIntervals(); ++i)
        {
            GetMajorVector(i).Transform(transformation);
        }
    }

    template <typename ElementType, MatrixLayout layout>
    auto MatrixReference<ElementType, layout>::Transpose() const -> MatrixReference<ElementType, TransposeMatrixLayout<layout>::value>
    {
        return MatrixReference<ElementType, TransposeMatrixLayout<layout>::value>(NumColumns(), NumRows(), GetIncrement(), _pData);
    }

    template <typename ElementType, MatrixLayout layout>
    MatrixReference<ElementType, layout> MatrixReference<ElementType, layout>::GetSubMatrix(size_t firstRow, size_t firstColumn, size_t numRows, size_t numColumns)
    {
        DEBUG_THROW(firstRow + numRows > NumRows() || firstColumn + numColumns > NumColumns(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "block exceeds matrix dimensions."));

        return MatrixReference<ElementType, layout>(numRows, numColumns, GetIncrement(), _pData + firstRow * GetRowIncrement() + firstColumn * GetColumnIncrement());
    }

    template <typename ElementType, MatrixLayout layout>
    VectorReference<ElementType, VectorOrientation::column> MatrixReference<ElementType, layout>::GetColumn(size_t index)
    {
        DEBUG_THROW(index >= NumColumns(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "column index exceeds matrix dimensions."));

        return VectorReference<ElementType, VectorOrientation::column>(_pData + index * GetColumnIncrement(), NumRows(), GetRowIncrement());
    }

    template <typename ElementType, MatrixLayout layout>
    VectorReference<ElementType, VectorOrientation::row> MatrixReference<ElementType, layout>::GetRow(size_t index)
    {
        DEBUG_THROW(index >= NumRows(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "row index exceeds matrix dimensions."));

        return VectorReference<ElementType, VectorOrientation::row>(_pData + index * GetRowIncrement(), NumColumns(), GetColumnIncrement());
    }

    template <typename ElementType, MatrixLayout layout>
    VectorReference<ElementType, VectorOrientation::column> MatrixReference<ElementType, layout>::GetDiagonal()
    {
        auto size = std::min(NumColumns(), NumRows());
        return VectorReference<ElementType, VectorOrientation::column>(_pData, size, GetIncrement() + 1);
    }

    template <typename ElementType, MatrixLayout layout>
    VectorReference<ElementType, VectorOrientation::column> MatrixReference<ElementType, layout>::ReferenceAsVector()
    {
        DEBUG_THROW(!IsContiguous(), utilities::InputException(utilities::InputExceptionErrors::indexOutOfRange, "Can only flatten a matrix when its memory is contiguous"));
        return VectorReference<ElementType, VectorOrientation::column>(_pData, NumRows() * NumColumns(), 1);
    }

    //
    // Matrix
    //

    template <typename ElementType, MatrixLayout layout>
    Matrix<ElementType, layout>::Matrix(size_t numRows, size_t numColumns)
        : MatrixReference<ElementType, layout>(numRows, numColumns, nullptr), _data(numRows * numColumns)
    {
        _pData = _data.data();
    }

    template <typename ElementType, MatrixLayout layout>
    Matrix<ElementType, layout>::Matrix(std::initializer_list<std::initializer_list<ElementType>> list)
        : MatrixReference<ElementType, layout>(list.size(), list.begin()->size(), nullptr), _data(list.size() * list.begin()->size())
    {
        _pData = _data.data();
        auto numColumns = list.begin()->size();

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
    Matrix<ElementType, layout>::Matrix(size_t numRows, size_t numColumns, const std::vector<ElementType>& data)
        : MatrixReference<ElementType, layout>(numRows, numColumns, nullptr), _data(data)
    {
        _pData = _data.data();
    }

    template <typename ElementType, MatrixLayout layout>
    Matrix<ElementType, layout>::Matrix(size_t numRows, size_t numColumns, std::vector<ElementType>&& data)
        : MatrixReference<ElementType, layout>(numRows, numColumns, nullptr), _data(std::move(data))
    {
        _pData = _data.data();
    }

    template <typename ElementType, MatrixLayout layout>
    Matrix<ElementType, layout>::Matrix(Matrix<ElementType, layout>&& other)
        : MatrixReference<ElementType, layout>(other.NumRows(), other.NumColumns(), nullptr), _data(std::move(other._data))
    {
        _pData = _data.data();
    }

    template <typename ElementType, MatrixLayout layout>
    Matrix<ElementType, layout>::Matrix(const Matrix<ElementType, layout>& other)
        : MatrixReference<ElementType, layout>(other.NumRows(), other.NumColumns(), nullptr), _data(other._data)
    {
        _pData = _data.data();
    }

    template <typename ElementType, MatrixLayout layout>
    Matrix<ElementType, layout>::Matrix(ConstMatrixReference<ElementType, layout>& other)
        : MatrixReference<ElementType, layout>(other.NumRows(), other.NumColumns(), nullptr), _data(other.NumRows() * other.NumColumns())
    {
        _pData = _data.data();
        for (size_t i = 0; i < NumRows(); ++i)
        {
            for (size_t j = 0; j < NumColumns(); ++j)
            {
                (*this)(i, j) = other(i, j);
            }
        }
    }

    template <typename ElementType, MatrixLayout layout>
    Matrix<ElementType, layout>::Matrix(ConstMatrixReference<ElementType, TransposeMatrixLayout<layout>::value> other)
        : MatrixReference<ElementType, layout>(other.NumRows(), other.NumColumns(), nullptr), _data(other.NumRows() * other.NumColumns())
    {
        _pData = _data.data();
        for (size_t i = 0; i < NumRows(); ++i)
        {
            for (size_t j = 0; j < NumColumns(); ++j)
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
}
}