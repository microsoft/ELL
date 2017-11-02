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
    /// <param name="tensor"> The tensor. </param>
    /// <param name="stream"> [in,out] The output stream. </param>
    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void Print(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> tensor, std::ostream& stream);

    /// <summary> Prints a tensor in initializer list format. </summary>
    ///
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="stream"> [in,out] The output stream. </param>
    /// <param name="tensor"> The const tensor reference to print. </param>
    ///
    /// <returns> Reference to the output stream. </returns>
    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    std::ostream& operator<<(std::ostream& stream, ConstTensorReference<ElementType, dimension0, dimension1, dimension2> tensor);

    /// @}

    /// \name Tensor Scalar Addition
    /// @{

    /// <summary> Adds a scalar to a tensor, tensor += scalar. </summary>
    ///
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="scalar"> The scalar being added. </param>
    /// <param name="tensor"> The tensor to which the scalar is added. </param>
    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void Add(ElementType scalar, TensorReference<ElementType, dimension0, dimension1, dimension2> tensor);

    /// <summary> Adds a scalar to a tensor, tensor += scalar. </summary>
    ///
    /// <typeparam name="TensorElementType"> The tensor element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <typeparam name="ScalarType"> The scalar type. </typeparam>
    /// <param name="tensor"> The tensor to which the scalar is added. </param>
    /// <param name="scalar"> The scalar being added. </param>
    template <typename TensorElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2, typename ScalarType, utilities::IsFundamental<ScalarType> concept = true>
    void operator+=(TensorReference<TensorElementType, dimension0, dimension1, dimension2> tensor, ScalarType scalar);

    /// <summary> Subtracts a scalar to a tensor, tensor -= scalar. </summary>
    ///
    /// <typeparam name="TensorElementType"> The tensor element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <typeparam name="ScalarType"> The scalar type. </typeparam>
    /// <param name="tensor"> The tensor from which the scalar is subtracted. </param>
    /// <param name="scalar"> The scalar being subtracted. </param>
    template <typename TensorElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2, typename ScalarType, utilities::IsFundamental<ScalarType> concept = true>
    void operator-=(TensorReference<TensorElementType, dimension0, dimension1, dimension2> tensor, ScalarType scalar);

    /// @}

    /// \name Vector Tensor Addition
    /// @{

    /// <summary> Adds each vector element to the corresponding Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="vector"> The vector </param>
    /// <param name="tensor"> The tensor. </param>
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void Add(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor);

    /// <summary> Adds each vector element to the corresponding Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <param name="vector"> The vector </param>
    /// <param name="tensor"> The tensor. </param>
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void Add(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor);

    /// <summary> Adds each vector element to the corresponding Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="vector"> The vector </param>
    /// <param name="tensor"> The tensor. </param>
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void Add(ConstRowVectorReference<ElementType> vector, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor);

    /// <summary> Adds each vector element to the corresponding Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="vector"> The vector </param>
    /// <param name="tensor"> The tensor. </param>
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void Add(ConstColumnVectorReference<ElementType> vector, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor);

    /// @}

    /// \name Tensor Scalar Multiplication
    /// @{

    /// <summary> Multiplies a tensor by a scalar, tensor *= scalar. </summary>
    ///
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="scalar"> The scalar that multiplies the tensor. </param>
    /// <param name="tensor"> The tensor which the scalar multiplies. </param>
    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void Multiply(ElementType scalar, TensorReference<ElementType, dimension0, dimension1, dimension2> tensor);

    /// <summary> Multiplies a tensor by a scalar, tensor *= scalar. </summary>
    ///
    /// <typeparam name="TensorElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <typeparam name="ScalarType"> The scalar type. </typeparam>
    /// <param name="tensor"> The tensor which the scalar multiplies. </param>
    /// <param name="scalar"> The scalar that multiplies the tensor. </param>
    template <typename TensorElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2, typename ScalarType, utilities::IsFundamental<ScalarType> concept = true>
    void operator*=(TensorReference<TensorElementType, dimension0, dimension1, dimension2> tensor, ScalarType scalar);

    /// <summary> Divides a tensor by a scalar, tensor /= scalar. </summary>
    ///
    /// <typeparam name="TensorElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <typeparam name="ScalarType"> The scalar type. </typeparam>
    /// <param name="tensor"> The tensor which the scalar divides. </param>
    /// <param name="scalar"> The scalar that divides the tensor. </param>
    template <typename TensorElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2, typename ScalarType, utilities::IsFundamental<ScalarType> concept = true>
    void operator/=(TensorReference<TensorElementType, dimension0, dimension1, dimension2> tensor, ScalarType scalar);

    /// @}

    /// \name Vector Tensor Multiplication
    /// @{

    /// <summary> Multiplies each slice in a Tensor with the corresponding vector element. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="vector"> The vector </param>
    /// <param name="tensor"> The tensor. </param>
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor);

    /// <summary> Multiplies each slice in a Tensor with the corresponding vector element. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="vector"> The vector </param>
    /// <param name="tensor"> The tensor. </param>
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor);

    /// <summary> Multiplies each slice in a Tensor with the corresponding vector element. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <param name="vector"> The vector </param>
    /// <param name="tensor"> The tensor. </param>
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor);

    /// @}

    /// \name Vector Tensor Multiply Add
    /// @{

    /// <summary> Applies the transformation M = scale[i] * M + bias[i], where M is the i'th Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vectors. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="scale"> The vector of elements that multiply the Tensor slices </param>
    /// <param name="bias"> The vector of elements to add to the Tensor slices </param>
    /// <param name="tensor"> The tensor. </param>
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void MultiplyAdd(UnorientedConstVectorReference<ElementType> scale, UnorientedConstVectorReference<ElementType> bias, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor);

    /// <summary> Applies the transformation M = scale[i] * M + bias[i], where M is the i'th Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vectors. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="scale"> The vector of elements that multiply the Tensor slices </param>
    /// <param name="bias"> The vector of elements to add to the Tensor slices </param>
    /// <param name="tensor"> The tensor. </param>
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void MultiplyAdd(UnorientedConstVectorReference<ElementType> scale, UnorientedConstVectorReference<ElementType> bias, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor);

    /// <summary> Applies the transformation M = scale[i] * M + bias[i], where M is the i'th Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vectors. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <param name="scale"> The vector of elements that multiply the Tensor slices </param>
    /// <param name="bias"> The vector of elements to add to the Tensor slices </param>
    /// <param name="tensor"> The tensor. </param>
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void MultiplyAdd(UnorientedConstVectorReference<ElementType> scale, UnorientedConstVectorReference<ElementType> bias, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor);

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
            static void Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor);

            template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
            static void Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor);

            template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
            static void Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor);
        };

#ifdef USE_BLAS
        template <>
        struct TensorOperations<ImplementationType::openBlas>
        {
            static std::string GetImplementationName() { return "OpenBlas"; }

            template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
            static void Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor);

            template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
            static void Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor);

            template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
            static void Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor);
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
