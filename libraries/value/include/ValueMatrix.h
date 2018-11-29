////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueMatrix.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterContext.h"
#include "ValueMatrixOperations.h"
#include "ValueVector.h"

#include <utilities/include/MemoryLayout.h>

#include <functional>

namespace ell
{
namespace value
{

    /// <summary> Wraps a Value instance and enforces a memory layout that represents a matrix </summary>
    class Matrix
    {
    public:
        Matrix();

        /// <summary> Constructor that wraps the provided instance of Value </summary>
        /// <param name="value"> The Value instance to wrap </param>
        Matrix(Value value);

        /// <summary> Constructs an instance from a std::vector based representation of the matrix </summary>
        /// <typeparam name="T"> Any fundamental type accepted by Value </typeparam>
        /// <param name="data"> The data represented as a std::vector of std::vectors, in canonical row-major layout </param>
        template <typename T>
        Matrix(const std::vector<std::vector<T>>& data);

        /// <summary> Matrix element access operator. </summary>
        /// <returns> The Scalar value wrapping the value that is at the specified index within the matrix </return>
        Scalar operator()(Scalar rowIndex, Scalar columnIndex);

        /// <summary> Gets the underlying wrapped Value instance </summary>
        Value GetValue() const;

        /// <summary> Get a submatrix view of the data </summary>
        /// <param name="row"> The row index of the first coordinate of the block </param>
        /// <param name="column"> The column index of the first coordinate of the block </param>
        /// <param name="numRows"> The extent of the submatrix in the row dimension </param>
        /// <param name="numColumns"> The extent of the submatrix in the column dimension </param>
        /// <returns> The resulting submatrix block </returns>
        Matrix SubMatrix(Scalar row, Scalar column, int numRows, int numColumns) const;

        /// <summary> Creates a new Matrix instance that contains the same data as this instance </summary>
        /// <returns> A new Matrix instance that points to a new, distinct memory that contains the same data as this instance </returns>
        Matrix Copy() const;

        /// <summary> Returns the number of active elements </summary>
        /// <returns> The size of the matrix </returns>
        size_t Size() const;

        /// <summary> Get a row of the matrix </summary>
        /// <param name="index"> The row index along which to get the vector </summary>
        /// <returns> The vector instance along the specified row </returns>
        Vector Row(Scalar index) const;

        /// <summary> Get a column of the matrix </summary>
        /// <param name="index"> The column index along which to get the vector </summary>
        /// <returns> The vector instance along the specified column </returns>
        Vector Column(Scalar index) const;

        /// <summary> Gets the number of rows within the active area </summary>
        size_t Rows() const;

        /// <summary> Gets the number of columns within the active area </summary>
        size_t Columns() const;

        /// <summary> Retrieves the type of data stored in the wrapped Value instance </summary>
        /// <returns> The type </returns>
        ValueType Type() const;

    private:
        Value _value;
    };

    /// <summary> Constructs an allocated instance with the specified dimensions </summary>
    /// <typeparam name="T"> Any fundamental type accepted by Value </typeparam>
    /// <param name="rows"> The number of rows of the allocated matrix </param>
    /// <param name="columns"> The number of columns of the allocated matrix </param>
    template <typename T>
    Matrix MakeMatrix(int rows, int columns)
    {
        return Matrix(Allocate<T>(utilities::MemoryLayout({ rows, columns })));
    }

} // namespace value
} // namespace ell

#pragma region implementation

namespace ell
{
namespace value
{

    template <typename T>
    Matrix::Matrix(const std::vector<std::vector<T>>& data)
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

        std::vector<T> coalesced(numRows * numColumns);
        auto it = coalesced.begin();
        for (const auto& row : data)
        {
            if (static_cast<int>(row.size()) != numColumns)
            {
                throw InputException(InputExceptionErrors::invalidSize);
            }

            it = std::copy(row.begin(), row.end(), it);
        }

        _value = Value(coalesced, MemoryLayout({ numRows, numColumns }));
    }

} // namespace value
} // namespace ell

#pragma endregion implementation
