////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Tensor.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterContext.h"
#include "Matrix.h"
#include "TensorOperations.h"

namespace ell
{
namespace value
{

    /// <summary> Wraps a Value instance and enforces a memory layout that represents a tensor </summary>
    class Tensor
    {
    public:
        Tensor();

        /// <summary> Constructor that wraps the provided instance of Value </summary>
        /// <param name="value"> The Value instance to wrap </param>
        Tensor(Value value);

        /// <summary> Constructs an instance from a std::vector based representation of the tensor </summary>
        /// <typeparam name="T"> Any fundamental type accepted by Value </typeparam>
        /// <param name="data"> The data represented in canonical row-major layout </param>
        template <typename T>
        Tensor(const std::vector<std::vector<std::vector<T>>>& data);

        Tensor(const Tensor&);
        Tensor(Tensor&&) noexcept;
        Tensor& operator=(const Tensor&);
        Tensor& operator=(Tensor&&);
        ~Tensor();

        /// <summary> Tensor element access operator. </summary>
        /// <returns> The Scalar value wrapping the value that is at the specified index within the tensor </return>
        Scalar operator()(Scalar rowIndex, Scalar columnIndex, Scalar channelIndex);

        /// <summary> Gets the underlying wrapped Value instance </summary>
        Value GetValue() const;

        /// <summary> Get a subtensor view of the data </summary>
        /// <param name="row"> The row index of the first coordinate of the block </param>
        /// <param name="column"> The column index of the first coordinate of the block </param>
        /// <param name="channel"> The channel index of the first coordinate of the block </param>
        /// <param name="numRows"> The extent of the subtensor in the row dimension </param>
        /// <param name="numColumns"> The extent of the subtensor in the column dimension </param>
        /// <param name="numChannels"> The extent of the subtensor in the channel dimension </param>
        /// <returns> The resulting subtensor block </returns>
        Tensor SubTensor(Scalar row, Scalar column, Scalar channel, int numRows, int numColumns, int numChannels) const;

        /// <summary> Creates a new Tensor instance that contains the same data as this instance </summary>
        /// <returns> A new Tensor instance that points to a new, distinct memory that contains the same data as this instance </returns>
        Tensor Copy() const;

        /// <summary> Returns the number of active elements within the Tensor instance </summary>
        /// <returns> The size of the tensor </returns>
        size_t Size() const;

        /// <summary> Returns a Matrix slice of the tensor </summary>
        /// <param name="row"> The slice index </param>
        /// <param name="mode1"> The mode of slicing the column dimension </param>
        /// <param name="mode2"> The mode of slicing the channel dimension </param>
        /// <returns> A Matrix instance pointing to the values at the column-channel matrix at the specified index </returns>
        Matrix Slice(Scalar row, value::Slice mode1, value::Slice mode2) const;

        /// <summary> Returns a Matrix slice of the tensor </summary>
        /// <param name="mode1"> The mode of slicing the row dimension </param>
        /// <param name="row"> The slice index </param>
        /// <param name="mode2"> The mode of slicing the channel dimension </param>
        /// <returns> A Matrix instance pointing to the values at the row-channel matrix at the specified index </returns>
        Matrix Slice(value::Slice mode1, Scalar column, value::Slice mode2) const;

        /// <summary> Returns a Matrix slice of the tensor </summary>
        /// <param name="mode1"> The mode of slicing the channel dimension </param>
        /// <param name="mode2"> The mode of slicing the column dimension </param>
        /// <param name="column"> The slice index </param>
        /// <returns> A Matrix instance pointing to the values at the row-column matrix at the specified index </returns>
        Matrix Slice(value::Slice mode1, value::Slice mode2, Scalar channel) const;

        /// <summary> Returns a vector slice of the tensor </summary>
        /// <param name="row"> The slice index in the row dimension </param>
        /// <param name="column"> The slice index in the column dimension </param>
        /// <param name="mode"> The mode of slicing the channel dimension </param>
        /// <returns> A Vector instance pointing to the values along the channel dimension at the specified indices </returns>
        Vector Slice(Scalar row, Scalar column, value::Slice mode) const;

        /// <summary> Returns a vector slice of the tensor </summary>
        /// <param name="row"> The slice index in the row dimension </param>
        /// <param name="mode"> The mode of slicing the column dimension </param>
        /// <param name="channel"> The slice index in the channel dimension </param>
        /// <returns> A Vector instance pointing to the values along the column dimension at the specified indices </returns>
        Vector Slice(Scalar row, value::Slice mode, Scalar channel) const;

        /// <summary> Returns a vector slice of the tensor </summary>
        /// <param name="mode"> The mode of slicing the row dimension </param>
        /// <param name="column"> The slice index in the column dimension </param>
        /// <param name="channel"> The slice index in the channel dimension </param>
        /// <returns> A Vector instance pointing to the values along the row dimension at the specified indices </returns>
        Vector Slice(value::Slice mode, Scalar column, Scalar channel) const;

        /// <summary> Returns the number of rows within the active area </summary>
        size_t Rows() const;

        /// <summary> Returns the number of columns within the active area </summary>
        size_t Columns() const;

        /// <summary> Returns the number of channels within the active area </summary>
        size_t Channels() const;

        /// <summary> Retrieves the type of data stored in the wrapped Value instance </summary>
        /// <returns> The type </returns>
        ValueType Type() const;

        Tensor& operator+=(Scalar);
        Tensor& operator-=(Scalar);
        Tensor& operator*=(Scalar);
        Tensor& operator/=(Scalar);

    private:
        Value _value;
    };

    /// <summary> Constructs an allocated instance with the specified dimensions </summary>
    /// <typeparam name="T"> Any fundamental type accepted by Value </typeparam>
    /// <param name="rows"> The number of rows of the allocated tensor </param>
    /// <param name="columns"> The number of columns of the allocated tensor </param>
    /// <param name="channels"> The number of columns of the allocated tensor </param>
    template <typename T>
    Tensor MakeTensor(int rows, int columns, int channels)
    {
        return Tensor(Allocate<T>(utilities::MemoryLayout({ rows, columns, channels })));
    }

} // namespace value
} // namespace ell

#pragma region implementation

namespace ell
{
namespace value
{

    template <typename T>
    Tensor::Tensor(const std::vector<std::vector<std::vector<T>>>& data)
    {
        using namespace utilities;

        int numRows = static_cast<int>(data.size());
        if (numRows <= 0)
        {
            throw InputException(InputExceptionErrors::invalidSize);
        }

        int numColumns = static_cast<int>(data[0].size());
        if (numColumns <= 0)
        {
            throw InputException(InputExceptionErrors::invalidSize);
        }

        int numChannels = static_cast<int>(data[0][0].size());
        if (numChannels <= 0)
        {
            throw InputException(InputExceptionErrors::invalidSize);
        }

        std::vector<T> coalesced(numRows * numColumns * numChannels);
        auto it = coalesced.begin();
        for (const auto& row : data)
        {
            if (static_cast<int>(row.size()) != numColumns)
            {
                throw InputException(InputExceptionErrors::invalidSize);
            }

            for (const auto& column : row)
            {
                if (static_cast<int>(column.size()) != numChannels)
                {
                    throw InputException(InputExceptionErrors::invalidSize);
                }

                it = std::copy(column.begin(), column.end(), it);
            }
        }

        _value = Value(coalesced, MemoryLayout({ numRows, numColumns, numChannels }));
    }

} // namespace value
} // namespace ell

#pragma endregion implementation
