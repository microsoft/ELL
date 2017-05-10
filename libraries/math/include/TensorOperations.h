////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TensorOperations.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Vector.h"
#include "Tensor.h"

// utilities
#include "Debug.h"
#include "Exception.h"

namespace ell
{
namespace math
{
    struct TensorOperations
    {
        /// <summary> Adds each vector element to the corresponding Tensor slice. </summary>
        ///
        /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
        /// <typeparam name="ElementType"> The element type. </typeparam>
        /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
        /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
        /// <param name="v"> The vector </param>
        /// <param name="T"> The Tensor. </param>
        template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
        static void Add(ConstVectorReference<ElementType, VectorOrientation::row> v, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T);

        /// <summary> Adds each vector element to the corresponding Tensor slice. </summary>
        ///
        /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
        /// <typeparam name="ElementType"> The element type. </typeparam>
        /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
        /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
        /// <param name="v"> The vector </param>
        /// <param name="T"> The Tensor. </param>
        template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
        static void Add(ConstVectorReference<ElementType, VectorOrientation::column> v, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T);

        /// <summary> Adds each vector element to the corresponding Tensor slice. </summary>
        ///
        /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
        /// <typeparam name="ElementType"> The element type. </typeparam>
        /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
        /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
        /// <param name="v"> The vector </param>
        /// <param name="T"> The Tensor. </param>
        template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
        static void Add(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> T);

        /// <summary> Adds each vector element to the corresponding Tensor slice. </summary>
        ///
        /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
        /// <typeparam name="ElementType"> The element type. </typeparam>
        /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
        /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
        /// <param name="v"> The vector </param>
        /// <param name="T"> The Tensor. </param>
        template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
        static void Add(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> T);

        /// <summary> Multiplies each slice in a Tensor with the corresponding vector element. </summary>
        ///
        /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
        /// <typeparam name="ElementType"> The element type. </typeparam>
        /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
        /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
        /// <param name="v"> The vector </param>
        /// <param name="T"> The Tensor. </param>
        template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
        static void Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T);

        /// <summary> Multiplies each slice in a Tensor with the corresponding vector element. </summary>
        ///
        /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
        /// <typeparam name="ElementType"> The element type. </typeparam>
        /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
        /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
        /// <param name="v"> The vector </param>
        /// <param name="T"> The Tensor. </param>
        template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
        static void Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> T);

        /// <summary> Multiplies each slice in a Tensor with the corresponding vector element. </summary>
        ///
        /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
        /// <typeparam name="ElementType"> The element type. </typeparam>
        /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
        /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
        /// <param name="v"> The vector </param>
        /// <param name="T"> The Tensor. </param>
        template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
        static void Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> T);

        /// <summary> Applies the transformation M = s[i] * M + b[i], where M is the i'th Tensor slice. </summary>
        ///
        /// <typeparam name="vectorOrientation"> The orientation in which to apply the vectors. </typeparam>
        /// <typeparam name="ElementType"> The element type. </typeparam>
        /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
        /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
        /// <param name="s"> The vector of elements that multiply the Tensor slices </param>
        /// <param name="b"> The vector of elements to add to the Tensor slices </param>
        /// <param name="T"> The Tensor. </param>
        template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
        static void MultiplyAdd(UnorientedConstVectorReference<ElementType> s, UnorientedConstVectorReference<ElementType> b, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T);

        /// <summary> Applies the transformation M = s[i] * M + b[i], where M is the i'th Tensor slice. </summary>
        ///
        /// <typeparam name="vectorOrientation"> The orientation in which to apply the vectors. </typeparam>
        /// <typeparam name="ElementType"> The element type. </typeparam>
        /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
        /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
        /// <param name="s"> The vector of elements that multiply the Tensor slices </param>
        /// <param name="b"> The vector of elements to add to the Tensor slices </param>
        /// <param name="T"> The Tensor. </param>
        template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
        static void MultiplyAdd(UnorientedConstVectorReference<ElementType> s, UnorientedConstVectorReference<ElementType> b, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> T);

        /// <summary> Applies the transformation M = s[i] * M + b[i], where M is the i'th Tensor slice. </summary>
        ///
        /// <typeparam name="vectorOrientation"> The orientation in which to apply the vectors. </typeparam>
        /// <typeparam name="ElementType"> The element type. </typeparam>
        /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
        /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
        /// <param name="s"> The vector of elements that multiply the Tensor slices </param>
        /// <param name="b"> The vector of elements to add to the Tensor slices </param>
        /// <param name="T"> The Tensor. </param>
        template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
        static void MultiplyAdd(UnorientedConstVectorReference<ElementType> s, UnorientedConstVectorReference<ElementType> b, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> T);
    };
}
}

#include "../tcc/TensorOperations.tcc"