////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TensorOperations.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Common.h"
#include "Vector.h"
#include "Tensor.h"

// utilities
#include "Debug.h"
#include "Exception.h"

// stl
#include <string>
#include <ostream>

namespace ell
{
namespace math
{
    /// \name Utility Functions
    /// @{

    /// <summary> Prints a tensor in initializer list format. </summary>
    ///
    /// <typeparam name="ElementType"> Tensor element type. </typeparam>
    /// <typeparam name="dimension0"> Tensor first dimension. </typeparam>
    /// <typeparam name="dimension1"> Tensor second dimension. </typeparam>
    /// <typeparam name="dimension2"> Tensor third dimension. </typeparam>
    /// <param name="T"> The tensor. </param>
    /// <param name="stream"> [in,out] The output stream. </param>
    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void Print(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> T, std::ostream& stream);

    /// <summary> Prints a tensor in initializer list format. </summary>
    ///
    /// <param name="stream"> [in,out] The output stream. </param>
    /// <param name="v"> The const tensor reference to print. </param>
    ///
    /// <returns> Reference to the output stream. </returns>
    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    std::ostream& operator<<(std::ostream& stream, ConstTensorReference<ElementType, dimension0, dimension1, dimension2> T);

    /// @}

    /// \name Vector Tensor Addition
    /// @{

    /// <summary> Adds each vector element to the corresponding Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="v"> The vector </param>
    /// <param name="T"> The Tensor. </param>
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void Add(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> T);

    /// <summary> Adds each vector element to the corresponding Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <param name="v"> The vector </param>
    /// <param name="T"> The Tensor. </param>
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void Add(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> T);

    /// <summary> Adds each vector element to the corresponding Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="v"> The vector </param>
    /// <param name="T"> The Tensor. </param>
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void Add(ConstRowVectorReference<ElementType> v, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T);

    /// <summary> Adds each vector element to the corresponding Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="v"> The vector </param>
    /// <param name="T"> The Tensor. </param>
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void Add(ConstColumnVectorReference<ElementType> v, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T);

    /// @}

    /// \name Vector Tensor Multiplication
    /// @{

    /// <summary> Multiplies each slice in a Tensor with the corresponding vector element. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="v"> The vector </param>
    /// <param name="T"> The Tensor. </param>
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T);

    /// <summary> Multiplies each slice in a Tensor with the corresponding vector element. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="v"> The vector </param>
    /// <param name="T"> The Tensor. </param>
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> T);

    /// <summary> Multiplies each slice in a Tensor with the corresponding vector element. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <param name="v"> The vector </param>
    /// <param name="T"> The Tensor. </param>
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> T);

    /// @}

    /// \name Vector Tensor Multiply Add
    /// @{

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
    void MultiplyAdd(UnorientedConstVectorReference<ElementType> s, UnorientedConstVectorReference<ElementType> b, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T);

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
    void MultiplyAdd(UnorientedConstVectorReference<ElementType> s, UnorientedConstVectorReference<ElementType> b, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> T);

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
    void MultiplyAdd(UnorientedConstVectorReference<ElementType> s, UnorientedConstVectorReference<ElementType> b, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> T);

    /// @}
    
    namespace Internal
    {
        template <ImplementationType type>
        struct TensorOperations
        {};

        template <>
        struct TensorOperations<ImplementationType::native>
        {
            static std::string GetImplementationName() { return "Native"; }

            template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
            static void Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> T);

            template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
            static void Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> T);

            template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
            static void Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T);
        };

#ifdef USE_BLAS
        template <>
        struct TensorOperations<ImplementationType::openBlas>
        {
            static std::string GetImplementationName() { return "OpenBlas"; }

            template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
            static void Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> T);

            template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
            static void Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> T);

            template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
            static void Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T);
        };

#else
        template <>
        struct TensorOperations<ImplementationType::openBlas> : public TensorOperations<ImplementationType::native>
        {
        };

#endif // USE_BLAS
    }
}
}

#include "../tcc/TensorOperations.tcc"