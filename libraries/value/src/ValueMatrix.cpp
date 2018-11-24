////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueMatrix.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ValueMatrix.h"
#include "EmitterContext.h"

// utilities
#include "Exception.h"

// stl
#include <cassert>
#include <functional>

namespace ell
{
using namespace utilities;

namespace value
{

    Matrix::Matrix() = default;

    Matrix::Matrix(Value value) :
        _value(value)
    {
        if (!_value.IsDefined() || !_value.IsConstrained() || _value.GetLayout().NumDimensions() != 2)
        {
            throw InputException(InputExceptionErrors::invalidArgument, "Value passed in must be two-dimensional");
        }
    }

    Scalar Matrix::operator()(Scalar rowIndex, Scalar columnIndex)
    {
        Value indexedValue = GetContext().Offset(_value, { rowIndex, columnIndex });
        indexedValue.SetLayout(ScalarLayout);

        return indexedValue;
    }

    Value Matrix::GetValue() const { return _value; }

    Matrix Matrix::SubMatrix(Scalar row, Scalar column, int numRows, int numColumns) const
    {
        const MemoryLayout& currentLayout = _value.GetLayout();

        if (numRows > currentLayout.GetLogicalDimensionActiveSize(0) ||
            numColumns > currentLayout.GetLogicalDimensionActiveSize(1))
        {
            throw InputException(InputExceptionErrors::indexOutOfRange);
        }

        Value indexedValue = GetContext().Offset(_value, { row, column });
        const auto& dimensionOrder = currentLayout.GetLogicalDimensionOrder();
        const int logicalDimemnsions[] = { numRows, numColumns };
        std::vector<int> physicalDimensions(2);

        assert(dimensionOrder.NumDimensions() == 2);
        for (unsigned index = 0; index < 2; ++index)
        {
            physicalDimensions[index] = logicalDimemnsions[dimensionOrder[index]];
        }

        MemoryLayout newLayout(physicalDimensions, currentLayout.GetExtent(), { 0, 0 }, dimensionOrder);
        indexedValue.SetLayout(newLayout);
        return indexedValue;
    }

    Matrix Matrix::Copy() const
    {
        auto newValue = Allocate(_value.GetBaseType(), _value.GetLayout());
        newValue = _value;
        return newValue;
    }

    size_t Matrix::Size() const { return _value.GetLayout().NumElements(); }

    Vector Matrix::Row(Scalar index) const
    {
        Value indexedValue = GetContext().Offset(_value, { index, 0 });
        const MemoryLayout& currentLayout = _value.GetLayout();

        indexedValue.SetLayout(currentLayout.GetSliceLayout(currentLayout.GetPhysicalDimension(0)));
        return indexedValue;
    }

    Vector Matrix::Column(Scalar index) const
    {
        Value indexedValue = GetContext().Offset(_value, { 0, index });
        const MemoryLayout& currentLayout = _value.GetLayout();

        indexedValue.SetLayout(currentLayout.GetSliceLayout(currentLayout.GetPhysicalDimension(1)));
        return indexedValue;
    }

    size_t Matrix::Rows() const { return static_cast<size_t>(_value.GetLayout().GetLogicalDimensionActiveSize(0)); }

    size_t Matrix::Columns() const { return static_cast<size_t>(_value.GetLayout().GetLogicalDimensionActiveSize(1)); }

    ValueType Matrix::Type() const { return _value.GetBaseType(); }

} // namespace value
} // namespace ell