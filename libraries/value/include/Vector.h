////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Vector.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Value.h"
#include "Scalar.h"
#include "VectorOperations.h"

// utilities
#include "MemoryLayout.h"

#include <functional>

namespace ell
{
namespace value
{

/// <summary> Wraps a Value instance and enforces a memory layout that represents a vector </summary>
class Vector
{
public:
    Vector();

    /// <summary> Constructor that wraps the provided instance of Value </summary>
    /// <param name="value"> The Value instance to wrap </param>
    Vector(Value value);

    /// <summary> Constructs an instance from a vector of fundamental types </summary>
    /// <typeparam name="T"> Any fundamental type accepted by Value </typeparam>
    /// <param name="t"> The data to wrap </param>
    template <typename T, typename A = std::allocator<T>>
    Vector(std::vector<T, A> data) :
        _value(std::move(data))
    {}

    /// <summary> Constructs an instance from an initializer list of fundamental types </summary>
    /// <typeparam name="T"> Any fundamental type accepted by Value </typeparam>
    /// <param name="t"> The data to wrap </param>
    template <typename T>
    Vector(std::initializer_list<T> data) :
        _value(std::vector<T>(data))
    {}

    /// <summary> Returns a Scalar value that represents the data at the specified index within the vector </summary>
    /// <param name="index"> The value by which to offset into the vector and return the specified value </param>
    /// <returns> The Scalar value wrapping the value that is at the specified index within the vector </return>
    Scalar operator[](Scalar index);

    /// <summary> Returns a Scalar value that represents the data at the specified index within the vector </summary>
    /// <param name="index"> The value by which to offset into the vector and return the specified value </param>
    /// <returns> The Scalar value wrapping the value that is at the specified index within the vector </return>
    Scalar operator()(Scalar index);

    /// <summary> Conversion function that returns the underlying wrapped Value instance </summary>
    explicit operator Value() const;

    /// <summary> Returns a subvector that starts at the specified offset and is of the specified size </summary>
    /// <param name="offset"> The starting index of the subvector </param>
    /// <param name="size"> The size of the subvector </param>
    /// <returns> A Vector instance that starts at the data specified by the offset and is of the specified size </returns>
    Vector SubVector(Scalar offset, int size) const;

    /// <summary> Creates a new Vector instance that contains the same data as this instance </summary>
    /// <returns> A new Vector instance that points to a new, distinct memory that contains the same data as this instance </returns>
    Vector Copy() const;

    /// <summary> Returns the number of active elements within the Vector instance </summary>
    /// <returns> The size of the vector </returns>
    size_t Size() const;

    /// <summary> Retrieves the type of data stored in the wrapped Value instance </summary>
    /// <returns> The type </returns>
    ValueType GetType() const;

private:
    Value _value;
};

} // value
} // ell
