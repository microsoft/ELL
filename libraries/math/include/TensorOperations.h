////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TensorOperations.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Tensor.h"

namespace math
{
    /// <summary> A struct that holds all of the binary tensor operations. </summary>
    struct TensorOperations
    {
        /// <summary> Calculates a vector dot product (between vectors in any orientation). </summary>
        ///
        /// <typeparam name="ElementType"> Element type. </typeparam>
        /// <param name="vector1"> The first vector, in any orientation. </param>
        /// <param name="vector2"> The second vector, in any orientation. </param>
        ///
        /// <returns> The dot product result. </returns>
        template<typename ElementType>
        static ElementType Dot(const VectorReferenceBase<ElementType>& vector1, const VectorReferenceBase<ElementType>& vector2);

        /// <summary> Calculates the product of a row vector with a column vector. </summary>
        ///
        /// <typeparam name="ElementType"> Element type. </typeparam>
        /// <param name="left"> The left vector, in row orientation. </param>
        /// <param name="right"> The right vector, in column orientation. </param>
        /// <param name="result"> [out] The result. </param>
        template<typename ElementType>
        static void Product(const VectorReference<ElementType, TensorOrientation::rowMajor>& left, const VectorReference<ElementType, TensorOrientation::columnMajor>& right, ElementType& result);
    };
}

#include "../tcc/TensorOperations.tcc"
