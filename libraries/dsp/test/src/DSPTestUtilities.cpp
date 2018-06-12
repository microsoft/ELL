////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DSPTestUtilities.cpp (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DSPTestUtilities.h"

// math
#include "MathConstants.h"

using namespace ell;

//
// Helper functions
//
template <typename ValueType>
std::ostream& operator<<(std::ostream& os, const std::vector<ValueType>& vec)
{
    for (auto x : vec)
        os << x << "  ";
    return os;
}

template <typename ValueType>
std::string GetSizeString(math::ConstRowMatrixReference<ValueType> input)
{
    return std::to_string(input.NumRows()) + " x " + std::to_string(input.NumColumns());
}

template <typename ValueType>
std::string GetSizeString(math::ConstChannelColumnRowTensorReference<ValueType> input)
{
    return std::to_string(input.NumRows()) + " x " + std::to_string(input.NumColumns()) + " x " + std::to_string(input.NumChannels());
}

template <typename ValueType>
std::string GetFilterSizeString(math::ConstRowMatrixReference<ValueType> filter)
{
    auto filterSize = filter.NumColumns();
    auto numFilters = filter.NumRows() / filterSize;
    return std::to_string(numFilters) + " " + std::to_string(filterSize) + " x " + std::to_string(filter.NumColumns());
}

template <typename ValueType>
std::string GetFilterSizeString(math::ConstChannelColumnRowTensorReference<ValueType> filters)
{
    auto filterSize = filters.NumColumns();
    auto numFilters = filters.NumRows() / filterSize;
    return std::to_string(numFilters) + " " + std::to_string(filterSize) + " x " + std::to_string(filterSize) + " x " + std::to_string(filters.NumChannels());
}

std::string GetConvAlgName(dsp::ConvolutionMethodOption alg)
{
    switch (alg)
    {
    case dsp::ConvolutionMethodOption::automatic:
        return "automatic";
    case dsp::ConvolutionMethodOption::simple:
        return "simple";
    case dsp::ConvolutionMethodOption::unrolled:
        return "unrolled";
    case dsp::ConvolutionMethodOption::diagonal:
        return "diagonal";
    case dsp::ConvolutionMethodOption::winograd:
        return "winograd";
    }
    return "";
}

// Helper function to avoid annoying double-to-float errors
template <typename ValueType, typename ValueType2>
math::RowVector<ValueType> MakeVector(std::initializer_list<ValueType2> list)
{
    auto numElements = list.size();
    std::vector<ValueType> data;
    data.reserve(numElements);
    std::transform(list.begin(), list.end(), std::back_inserter(data), [](ValueType2 x) { return static_cast<ValueType>(x); });
    return ell::math::RowVector<ValueType>(data);
}

// Helper function to avoid annoying double-to-float errors
template <typename ValueType, typename ValueType2>
ell::math::ChannelColumnRowTensor<ValueType> MakeTensor(std::initializer_list<std::initializer_list<ValueType2>> list)
{
    auto numRows = list.size();
    auto numColumns = list.begin()->size();
    auto numChannels = 1;
    std::vector<ValueType> data;
    data.reserve(numRows * numColumns * numChannels);
    for (const auto& row : list)
    {
        DEBUG_THROW(row.size() != numColumns, ell::utilities::InputException(ell::utilities::InputExceptionErrors::sizeMismatch, "incorrect number of elements in initializer list"));
        std::transform(row.begin(), row.end(), std::back_inserter(data), [](ValueType2 x) { return static_cast<ValueType>(x); });
    }
    return ell::math::ChannelColumnRowTensor<ValueType>(numRows, numColumns, numChannels, data);
}

// Helper function to avoid annoying double-to-float errors
template <typename ValueType, typename ValueType2>
ell::math::ChannelColumnRowTensor<ValueType> MakeTensor(std::initializer_list<std::initializer_list<std::initializer_list<ValueType2>>> list)
{
    auto numRows = list.size();
    auto numColumns = list.begin()->size();
    auto numChannels = list.begin()->begin()->size();
    std::vector<ValueType> data;
    data.reserve(numRows * numColumns * numChannels);
    for (const auto& row : list)
    {
        DEBUG_THROW(row.size() != numColumns, ell::utilities::InputException(ell::utilities::InputExceptionErrors::sizeMismatch, "incorrect number of elements in initializer list"));
        for (const auto& column: row)
        {
            DEBUG_THROW(column.size() != numChannels, ell::utilities::InputException(ell::utilities::InputExceptionErrors::sizeMismatch, "incorrect number of elements in initializer list"));
            std::transform(column.begin(), column.end(), std::back_inserter(data), [](ValueType2 x) { return static_cast<ValueType>(x); });
        }
    }
    return ell::math::ChannelColumnRowTensor<ValueType>(numRows, numColumns, numChannels, data);
}

//
// Fill a vector with some "interesting" input signal data. The particular values aren't that important, 
// but using something other than uniform noise is probably a good idea.
//
// The `std::trunc` part is there just so that the numbers have relatively few significant digits after the decimal,
// so that printing them out for debugging purposes is easier on the eyes.
//
template <typename ValueType>
void FillInputVector(math::RowVectorReference<ValueType> input)
{
    const auto pi = math::Constants<ValueType>::pi;
    const auto size = static_cast<int>(input.Size());
    for (int index = 0; index < size; ++index)
    {
        double value = std::sin(2 * 2 * pi * index / size) + std::sin(2 * 3 * pi * index / size) + std::sin(2 * 9 * pi * index / size);
        value = std::trunc(value * 16) / 16.0;
        input[index] = static_cast<ValueType>(value);
    }
}

//
// Fill a vector with some "interesting" filter weights. The particular values aren't that important, 
// but using something other than uniform noise is probably a good idea.
//
// The `std::trunc` part is there just so that the numbers have relatively few significant digits after the decimal,
// so that printing them out for debugging purposes is easier on the eyes.
//
template <typename ValueType>
void FillFilterVector(math::RowVectorReference<ValueType> filter)
{
    const auto pi = math::Constants<ValueType>::pi;
    const auto size = static_cast<int>(filter.Size());
    for (int index = 0; index < size; ++index)
    {
        double value = 2 * std::sin(3.7 * pi * index / size) * std::cos(2 * 5 * pi * index / size) + std::cos(2 * 15 * pi * index / size);
        value = std::trunc(value * 16) / 16.0;
        filter[index] = static_cast<ValueType>(value);
    }
}

//
// Fill a matrix with some "interesting" input signal data. The particular values aren't that important, 
// but using something other than uniform noise is probably a good idea.
//
// The `std::trunc` part is there just so that the numbers have relatively few significant digits after the decimal,
// so that printing them out for debugging purposes is easier on the eyes.
//
template <typename ValueType>
void FillInputMatrix(math::RowMatrixReference<ValueType> input)
{
    const auto pi = math::Constants<ValueType>::pi;
    const auto numRows = input.NumRows();
    const auto numColumns = input.NumColumns();
    for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex)
    {
        for (size_t columnIndex = 0; columnIndex < numColumns; ++columnIndex)
        {
            double value = std::sin(5.3 * 2 * pi * rowIndex / numRows) + std::cos(1.6 * 2 * pi * columnIndex / numColumns);
            value = std::trunc(value * 4) / 4.0;
            input(rowIndex, columnIndex) = static_cast<ValueType>(value);
        }
    }
}

//
// Fill a matrix with some "interesting" filter weights. The particular values aren't that important, 
// but using something other than uniform noise is probably a good idea.
//
// The `std::trunc` part is there just so that the numbers have relatively few significant digits after the decimal,
// so that printing them out for debugging purposes is easier on the eyes.
//
template <typename ValueType>
void FillFilterMatrix(math::RowMatrixReference<ValueType> filter)
{
    const auto pi = math::Constants<ValueType>::pi;
    const auto numRows = filter.NumRows();
    const auto numColumns = filter.NumColumns();
    for (size_t rowIndex = 0; rowIndex < numRows; ++rowIndex)
    {
        for (size_t columnIndex = 0; columnIndex < numColumns; ++columnIndex)
        {
            double value = 2 * std::sin((2.1 * pi * rowIndex + 0.25) / numRows) * std::cos(3.7 * 2 * pi * columnIndex / numColumns);
            value = std::trunc(value * 8) / 8.0;
            filter(rowIndex, columnIndex) = static_cast<ValueType>(value);
        }
    }
}

//
// Fill a tensor with some "interesting" input signal data. The particular values aren't that important, 
// but using something other than uniform noise is probably a good idea.
//
// The `std::trunc` part is there just so that the numbers have relatively few significant digits after the decimal,
// so that printing them out for debugging purposes is easier on the eyes.
//
template <typename ValueType>
void FillInputTensor(math::ChannelColumnRowTensorReference<ValueType> input)
{
    FillInputMatrix(input.ReferenceAsMatrix());
}

//
// Fill a tensor with some "interesting" filter weights. The particular values aren't that important, 
// but using something other than uniform noise is probably a good idea.
//
// The `std::trunc` part is there just so that the numbers have relatively few significant digits after the decimal,
// so that printing them out for debugging purposes is easier on the eyes.
//
template <typename ValueType>
void FillFiltersTensor(math::ChannelColumnRowTensorReference<ValueType> filters, size_t numFilters)
{
    UNUSED(numFilters);
    FillFilterMatrix(filters.ReferenceAsMatrix());
}

//
// Explicit instantiations
//
template std::ostream& operator<<(std::ostream& os, const std::vector<float>& vec);
template std::ostream& operator<<(std::ostream& os, const std::vector<double>& vec);

template std::string GetSizeString(math::ConstRowMatrixReference<float> input);
template std::string GetSizeString(math::ConstRowMatrixReference<double> input);

template std::string GetSizeString(math::ConstChannelColumnRowTensorReference<float> input);
template std::string GetSizeString(math::ConstChannelColumnRowTensorReference<double> input);

template std::string GetFilterSizeString(math::ConstRowMatrixReference<float> filter);
template std::string GetFilterSizeString(math::ConstRowMatrixReference<double> filter);

template std::string GetFilterSizeString(math::ConstChannelColumnRowTensorReference<float> filters);
template std::string GetFilterSizeString(math::ConstChannelColumnRowTensorReference<double> filters);

// Helper function to avoid annoying double-to-float errors
template ell::math::RowVector<float> MakeVector(std::initializer_list<float> list);
template ell::math::RowVector<float> MakeVector(std::initializer_list<double> list);
template ell::math::RowVector<double> MakeVector(std::initializer_list<float> list);
template ell::math::RowVector<double> MakeVector(std::initializer_list<double> list);

template ell::math::ChannelColumnRowTensor<float> MakeTensor(std::initializer_list<std::initializer_list<float>> list);
template ell::math::ChannelColumnRowTensor<float> MakeTensor(std::initializer_list<std::initializer_list<double>> list);
template ell::math::ChannelColumnRowTensor<double> MakeTensor(std::initializer_list<std::initializer_list<float>> list);
template ell::math::ChannelColumnRowTensor<double> MakeTensor(std::initializer_list<std::initializer_list<double>> list);

template ell::math::ChannelColumnRowTensor<float> MakeTensor(std::initializer_list<std::initializer_list<std::initializer_list<float>>> list);
template ell::math::ChannelColumnRowTensor<float> MakeTensor(std::initializer_list<std::initializer_list<std::initializer_list<double>>> list);
template ell::math::ChannelColumnRowTensor<double> MakeTensor(std::initializer_list<std::initializer_list<std::initializer_list<float>>> list);
template ell::math::ChannelColumnRowTensor<double> MakeTensor(std::initializer_list<std::initializer_list<std::initializer_list<double>>> list);

//
// Get some "interesting" input signal data.
//
template void FillInputVector(math::RowVectorReference<float> input);
template void FillInputVector(math::RowVectorReference<double> input);

template void FillInputMatrix(math::RowMatrixReference<float> input);
template void FillInputMatrix(math::RowMatrixReference<double> input);

template void FillInputTensor(math::ChannelColumnRowTensorReference<float> input);
template void FillInputTensor(math::ChannelColumnRowTensorReference<double> input);

//
// Get some "interesting" filter weights. 
//
template void FillFilterVector(math::RowVectorReference<float> filter);
template void FillFilterVector(math::RowVectorReference<double> filter);

template void FillFilterMatrix(math::RowMatrixReference<float> filter);
template void FillFilterMatrix(math::RowMatrixReference<double> filter);

template void FillFiltersTensor(math::ChannelColumnRowTensorReference<float> filters, size_t numFilters);
template void FillFiltersTensor(math::ChannelColumnRowTensorReference<double> filters, size_t numFilters);
