////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ConstVectorReference.h (math_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "VectorBase.h"

// stl
#include <cstddef>
#include <iostream>
#include <vector>

namespace ell
{
namespace math
{

    /// <summary> Represents a constant reference to a vector, without a specified row or column orientation. </summary>
    ///
    /// <typeparam name="ElementType"> ElementType. </typeparam>
    template <typename ElementType>
    class UnorientedConstVectorReference
    {
    public:
        /// <summary> Constructs an instance of UnorientedConstVectorReference. </summary>
        ///
        /// <param name="pData"> [in,out] Pointer to the data. </param>
        /// <param name="size"> The size of the vector. </param>
        /// <param name="increment"> The vector increment. </param>
        UnorientedConstVectorReference(ElementType* pData, size_t size, size_t increment);

        /// <summary> Gets a const pointer to the underlying data storage. </summary>
        ///
        /// <returns> Const pointer to the data. </returns>
        const ElementType* GetDataPointer() const { return _pData; }

        /// <summary> Gets the increment used in the underlying data storage. </summary>
        ///
        /// <returns> The increment. </returns>
        size_t GetIncrement() const { return _increment; }

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        ///
        /// <returns> A copy of the specified element. </returns>
        ElementType operator[](size_t index) const;

        /// <summary> Gets the vector size. </summary>
        ///
        /// <returns> The vector size. </returns>
        size_t Size() const { return _size; }

        /// <summary> Computes the 0-norm of the vector </summary>
        ///
        /// <returns> The norm. </returns>
        ElementType Norm0() const;

        /// <summary> Computes the 1-norm of the vector </summary>
        ///
        /// <returns> The norm. </returns>
        ElementType Norm1() const;

        /// <summary> Computes the 2-norm of the vector </summary>
        ///
        /// <returns> The norm. </returns>
        ElementType Norm2() const;

        /// <summary> Convert this vector into an array. </summary>
        ///
        /// <returns> An array that represents the data in this object. </returns>
        std::vector<ElementType> ToArray() const;

        /// <summary> Applies a map to each vector element and sums the result. </summary>
        ///
        /// <typeparam name="MapperType"> A functor type of the mapper. </typeparam>
        /// <param name="mapper"> The mapper. </param>
        ///
        /// <returns> The result of the operation. </returns>
        template <typename MapperType>
        ElementType Aggregate(MapperType mapper) const;

        /// <summary> Prints a tab separated representation of this vector to an output stream, independent of orientation. </summary>
        ///
        /// <param name="ostream"> [in,out] The output stream. </param>
        void Print(std::ostream& ostream) const;

    protected:
        void Swap(UnorientedConstVectorReference<ElementType>& other);

        ElementType* _pData;
        size_t _size;
        size_t _increment;
    };

    /// <summary>
    /// Streaming operator. Prints vectors to ostreams
    /// </summary>
    ///
    /// <param name="ostream"> [in,out] Stream to write data to. </param>
    /// <param name="vector"> The vector. </param>
    ///
    /// <returns> Reference to the stream. </returns>
    template <typename ElementType>
    std::ostream& operator<<(std::ostream& ostream, UnorientedConstVectorReference<ElementType> vector);

    /// <summary> A reference to a constant algebraic vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    template <typename ElementType, VectorOrientation orientation>
    class ConstVectorReference : public VectorBase<orientation>, public UnorientedConstVectorReference<ElementType>
    {
    public:
        using UnorientedConstVectorReference<ElementType>::UnorientedConstVectorReference;

        /// <summary> Gets a reference to this vector. </summary>
        ///
        /// <returns> A constant reference to this vector. </returns>
        ConstVectorReference<ElementType, orientation> GetReference() const;

        /// <summary> Gets a reference to this vector. </summary>
        ///
        /// <returns> A constant reference to this vector. </returns>
        ConstVectorReference<ElementType, orientation> GetConstantReference() const { return GetReference(); }

        /// <summary> Gets a reference to a sub-vector. </summary>
        ///
        /// <param name="offset"> The index of the first element in the sub-vector. </param>
        /// <param name="size"> The size of the sub-vector. </param>
        ///
        /// <returns> The sub vector. </returns>
        ConstVectorReference<ElementType, orientation> GetSubVector(size_t offset, size_t size) const;

        /// <summary> Gets a reference to the transpose of this vector. </summary>
        ///
        /// <returns> A reference to the transpose of this vector. </returns>
        auto Transpose() const -> ConstVectorReference<ElementType, VectorBase<orientation>::transposeOrientation>
        {
            return ConstVectorReference<ElementType, VectorBase<orientation>::transposeOrientation>(_pData, _size, _increment);
        }

        /// <summary> Check vector equality. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        /// <param name="tolerance"> The element comparison tolerance. </param>
        ///
        /// <returns> true if the vectors are considered equivalent. </returns>
        bool IsEqual(ConstVectorReference<ElementType, orientation> other, ElementType tolerance = 1.0e-8) const;

        /// <summary> Equality operator. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        ///
        /// <returns> true if the vectors are considered equivalent. </returns>
        bool operator==(const ConstVectorReference<ElementType, orientation>& other) const;

        /// <summary> Inequality operator. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        ///
        /// <returns> true if the vectors are not considered equivalent. </returns>
        bool operator!=(const ConstVectorReference<ElementType, orientation>& other) const;

    protected:
        using UnorientedConstVectorReference<ElementType>::_pData;
        using UnorientedConstVectorReference<ElementType>::_size;
        using UnorientedConstVectorReference<ElementType>::_increment;
    };
}
}

#include "../tcc/ConstVectorReference.tcc"