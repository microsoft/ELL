////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VectorReference.h (math_test)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ConstVectorReference.h"
#include "TransformedConstVectorReference.h"

namespace ell
{
namespace math
{
    /// <summary> A reference to a non-constant algebraic vector. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="orientation"> The orientation. </typeparam>
    template <typename ElementType, VectorOrientation orientation>
    class VectorReference : public ConstVectorReference<ElementType, orientation>
    {
    public:
        using ConstVectorReference<ElementType, orientation>::ConstVectorReference;

        /// <summary> Copies values from another vector into this vector. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        void Set(ConstVectorReference<ElementType, orientation> other);

        /// <summary> Copies values from a transformed vector into this vector. </summary>
        ///
        /// <param name="other"> The transformed vector. </param>
        template <typename TransformationType>
        void Set(TransformedConstVectorReference<ElementType, orientation, TransformationType> other);

        /// <summary> Gets a pointer to the underlying data storage. </summary>
        ///
        /// <returns> Pointer to the data. </returns>
        ElementType* GetDataPointer() const { return _pData; }

        /// <summary> Sets all vector elements to zero. </summary>
        void Reset();

        /// <summary> Sets all vector elements to a given value. </summary>
        ///
        /// <param name="value"> The value. </param>
        void Fill(ElementType value);

        /// <summary>
        /// Generates elements of the vector by repeatedly calling a generator function (such as a random
        /// number generator).
        /// </summary>
        ///
        /// <typeparam name="GeneratorType"> Type of lambda or functor to use as a generator. </typeparam>
        /// <param name="generator"> The generator function. </param>
        template <typename GeneratorType>
        void Generate(GeneratorType generator);

        /// <summary> Array indexer operator. </summary>
        ///
        /// <param name="index"> Zero-based index of the element. </param>
        ///
        /// <returns> Reference to the specified element. </returns>
        ElementType& operator[](size_t index);
        using ConstVectorReference<ElementType, orientation>::operator[];

        /// <summary> Gets a reference to this vector. </summary>
        ///
        /// <returns> A reference to this vector. </returns>
        VectorReference<ElementType, orientation> GetReference();
        using ConstVectorReference<ElementType, orientation>::GetReference;

        /// <summary> Gets a reference to a sub-vector. </summary>
        ///
        /// <param name="offset"> The index of the first element in the sub-vector. </param>
        /// <param name="size"> The size of the sub-vector. </param>
        ///
        /// <returns> The sub vector. </returns>
        VectorReference<ElementType, orientation> GetSubVector(size_t offset, size_t size);
        using ConstVectorReference<ElementType, orientation>::GetSubVector;

        /// <summary> Gets a reference to the transpose of this vector. </summary>
        ///
        /// <returns> A reference to the transpose of this vector. </returns>
        auto Transpose() -> VectorReference<ElementType, VectorBase<orientation>::transposeOrientation>
        {
            return VectorReference<ElementType, VectorBase<orientation>::transposeOrientation>(_pData, _size, _increment);
        }

        using ConstVectorReference<ElementType, orientation>::Transpose;

        /// <summary> Applies a transfromation to each element of the vector. </summary>
        ///
        /// <typeparam name="TransformationType"> Tranformation type. </typeparam>
        /// <param name="transformation"> The transfromation. </param>
        template <typename TransformationType>
        void Transform(TransformationType transformation);

        /// <summary> Adds another vector to this vector. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        void operator+=(ConstVectorReference<ElementType, orientation> other);

        /// <summary> Copies the values from another vector into this vector. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        void operator=(ConstVectorReference<ElementType, orientation> other);

        /// <summary> Adds a transformed vector to this vector. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        template <typename TransformationType>
        void operator+=(TransformedConstVectorReference<ElementType, orientation, TransformationType> other);

        /// <summary> Subtracts another vector from this vector. </summary>
        ///
        /// <param name="other"> The other vector. </param>
        void operator-=(ConstVectorReference<ElementType, orientation> other);

        /// <summary> Adds a constant value to this vector. </summary>
        ///
        /// <param name="other"> The constant value. </param>
        void operator+=(ElementType value);

        /// <summary> Subtracts a constant value from this vector. </summary>
        ///
        /// <param name="other"> The constant value. </param>
        void operator-=(ElementType value);

        /// <summary> Multiplies this vector by a constant value. </summary>
        ///
        /// <param name="other"> The constant value. </param>
        void operator*=(ElementType value);

        /// <summary> Divides each element of this vector by a constant value. </summary>
        ///
        /// <param name="other"> The constant value. </param>
        void operator/=(ElementType value);

    protected:
        using ConstVectorReference<ElementType, orientation>::_pData;
        using ConstVectorReference<ElementType, orientation>::_size;
        using ConstVectorReference<ElementType, orientation>::_increment;

    };
}
}

#include "../tcc/VectorReference.tcc"