////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TensorOperations.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Common.h"
#include "Tensor.h"
#include "Vector.h"

#include <utilities/include/Debug.h>
#include <utilities/include/Exception.h>

#include <ostream>
#include <string>

namespace ell
{
namespace math
{
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

    /// <summary> Multiplies a tensor by a scalar, tensor *= scalar. </summary>
    ///
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="scalar"> The scalar that multiplies the tensor. </param>
    /// <param name="tensor"> The tensor which the scalar multiplies. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void ScaleUpdate(ElementType scalar, TensorReference<ElementType, dimension0, dimension1, dimension2> tensor);

    /// <summary> Multiplies each slice in a Tensor with the corresponding vector element. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="vector"> The vector </param>
    /// <param name="tensor"> The tensor. </param>
    template <Dimension vectorOrientation, ImplementationType implementation = ImplementationType::openBlas, typename ElementType, Dimension dimension1, Dimension dimension2>
    void ScaleUpdate(UnorientedConstVectorBase<ElementType> vector, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor);

    /// <summary> Multiplies each slice in a Tensor with the corresponding vector element. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="vector"> The vector </param>
    /// <param name="tensor"> The tensor. </param>
    template <Dimension vectorOrientation, ImplementationType implementation = ImplementationType::openBlas, typename ElementType, Dimension dimension0, Dimension dimension2>
    void ScaleUpdate(UnorientedConstVectorBase<ElementType> vector, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor);

    /// <summary> Multiplies each slice in a Tensor with the corresponding vector element. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <param name="vector"> The vector </param>
    /// <param name="tensor"> The tensor. </param>
    template <Dimension vectorOrientation, ImplementationType implementation = ImplementationType::openBlas, typename ElementType, Dimension dimension0, Dimension dimension1>
    void ScaleUpdate(UnorientedConstVectorBase<ElementType> vector, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor);

    /// <summary> Adds a scalar to a tensor, tensor += scalar. </summary>
    ///
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="scalar"> The scalar being added. </param>
    /// <param name="tensor"> The tensor to which the scalar is added. </param>
    template <ImplementationType implementation = ImplementationType::openBlas, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void AddUpdate(ElementType scalar, TensorReference<ElementType, dimension0, dimension1, dimension2> tensor);

    /// <summary> Adds each vector element to the corresponding Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="vector"> The vector </param>
    /// <param name="tensor"> The tensor. </param>
    template <Dimension vectorOrientation, ImplementationType implementation = ImplementationType::openBlas, typename ElementType, Dimension dimension0, Dimension dimension2>
    void AddUpdate(UnorientedConstVectorBase<ElementType> vector, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor);

    /// <summary> Adds each vector element to the corresponding Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <param name="vector"> The vector </param>
    /// <param name="tensor"> The tensor. </param>
    template <Dimension vectorOrientation, ImplementationType implementation = ImplementationType::openBlas, typename ElementType, Dimension dimension0, Dimension dimension1>
    void AddUpdate(UnorientedConstVectorBase<ElementType> vector, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor);

    /// <summary> Adds each vector element to the corresponding Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="vector"> The vector </param>
    /// <param name="tensor"> The tensor. </param>
    template <Dimension vectorOrientation, ImplementationType implementation = ImplementationType::openBlas, typename ElementType, Dimension dimension1, Dimension dimension2>
    void AddUpdate(ConstRowVectorReference<ElementType> vector, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor);

    /// <summary> Adds each vector element to the corresponding Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vector. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="vector"> The vector </param>
    /// <param name="tensor"> The tensor. </param>
    template <Dimension vectorOrientation, ImplementationType implementation = ImplementationType::openBlas, typename ElementType, Dimension dimension1, Dimension dimension2>
    void AddUpdate(ConstColumnVectorReference<ElementType> vector, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor);

    /// <summary> Applies the transformation M = scale[i] * M + bias[i], where M is the i'th Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vectors. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="scale"> The vector of elements that multiply the Tensor slices </param>
    /// <param name="bias"> The vector of elements to add to the Tensor slices </param>
    /// <param name="tensor"> The tensor. </param>
    template <Dimension vectorOrientation, ImplementationType implementation = ImplementationType::openBlas, typename ElementType, Dimension dimension1, Dimension dimension2>
    void ScaleAddUpdate(UnorientedConstVectorBase<ElementType> scale, UnorientedConstVectorBase<ElementType> bias, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor);

    /// <summary> Applies the transformation M = scale[i] * M + bias[i], where M is the i'th Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vectors. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension2"> The third dimension in the Tensor layout. </typeparam>
    /// <param name="scale"> The vector of elements that multiply the Tensor slices </param>
    /// <param name="bias"> The vector of elements to add to the Tensor slices </param>
    /// <param name="tensor"> The tensor. </param>
    template <Dimension vectorOrientation, ImplementationType implementation = ImplementationType::openBlas, typename ElementType, Dimension dimension0, Dimension dimension2>
    void ScaleAddUpdate(UnorientedConstVectorBase<ElementType> scale, UnorientedConstVectorBase<ElementType> bias, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor);

    /// <summary> Applies the transformation M = scale[i] * M + bias[i], where M is the i'th Tensor slice. </summary>
    ///
    /// <typeparam name="vectorOrientation"> The orientation in which to apply the vectors. </typeparam>
    /// <typeparam name="ElementType"> The element type. </typeparam>
    /// <typeparam name="dimension0"> The first dimension in the Tensor layout. </typeparam>
    /// <typeparam name="dimension1"> The second dimension in the Tensor layout. </typeparam>
    /// <param name="scale"> The vector of elements that multiply the Tensor slices </param>
    /// <param name="bias"> The vector of elements to add to the Tensor slices </param>
    /// <param name="tensor"> The tensor. </param>
    template <Dimension vectorOrientation, ImplementationType implementation = ImplementationType::openBlas, typename ElementType, Dimension dimension0, Dimension dimension1>
    void ScaleAddUpdate(UnorientedConstVectorBase<ElementType> scale, UnorientedConstVectorBase<ElementType> bias, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor);
} // namespace math
} // namespace ell

#pragma region implementation

#include "../include/MatrixOperations.h"

#include <utilities/include/Logger.h>

namespace ell
{
namespace math
{
    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void Print(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> tensor, std::ostream& stream, size_t row, size_t column)
    {
        stream << "{" << tensor(row, column, 0);
        for (size_t k = 1; k < tensor.NumChannels(); ++k)
        {
            stream << ", " << tensor(row, column, k);
        }
        stream << "}";
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void Print(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> tensor, std::ostream& stream, size_t row)
    {
        stream << "{ ";
        Print(tensor, stream, row, 0);
        for (size_t j = 1; j < tensor.NumColumns(); ++j)
        {
            stream << ", ";
            Print(tensor, stream, row, j);
        }
        stream << " }";
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void Print(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> tensor, std::ostream& stream)
    {
        using namespace logging;

        stream << "{ ";
        Print(tensor, stream, 0);
        for (size_t i = 1; i < tensor.NumRows(); ++i)
        {
            stream << "," << EOL << "  ";
            Print(tensor, stream, i);
        }
        stream << " }" << EOL;
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    std::ostream& operator<<(std::ostream& stream, ConstTensorReference<ElementType, dimension0, dimension1, dimension2> tensor)
    {
        Print(tensor, stream);
        return stream;
    }

    template <typename TensorElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2, typename ScalarType, utilities::IsFundamental<ScalarType>>
    void operator+=(TensorReference<TensorElementType, dimension0, dimension1, dimension2> tensor, ScalarType scalar)
    {
        AddUpdate(static_cast<TensorElementType>(scalar), tensor);
    }

    template <typename TensorElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2, typename ScalarType, utilities::IsFundamental<ScalarType>>
    void operator-=(TensorReference<TensorElementType, dimension0, dimension1, dimension2> tensor, ScalarType scalar)
    {
        AddUpdate(-static_cast<TensorElementType>(scalar), tensor);
    }

    template <typename TensorElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2, typename ScalarType, utilities::IsFundamental<ScalarType>>
    void operator*=(TensorReference<TensorElementType, dimension0, dimension1, dimension2> tensor, ScalarType scalar)
    {
        ScaleUpdate(static_cast<TensorElementType>(scalar), tensor);
    }

    template <typename TensorElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2, typename ScalarType, utilities::IsFundamental<ScalarType>>
    void operator/=(TensorReference<TensorElementType, dimension0, dimension1, dimension2> tensor, ScalarType scalar)
    {
        DEBUG_THROW(scalar == 0, utilities::NumericException(utilities::NumericExceptionErrors::divideByZero, "Divide by zero."));

        ScaleUpdate(static_cast<TensorElementType>(1.0 / scalar), tensor);
    }

    template <ImplementationType implementation, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void ScaleUpdate(ElementType scalar, TensorReference<ElementType, dimension0, dimension1, dimension2> tensor)
    {
        for (size_t i = 0; i < tensor.NumPrimarySlices(); ++i)
        {
            ScaleUpdate<implementation>(scalar, tensor.GetPrimarySlice(i));
        }
    }

    template <Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void ScaleUpdate(UnorientedConstVectorBase<ElementType> vector, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor)
    {
        for (size_t i = 0; i < vector.Size(); ++i)
        {
            math::ScaleUpdate<implementation>(vector[i], tensor.template GetSlice<dimension0, dimension1>(i));
        }
    }

    template <Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void ScaleUpdate(UnorientedConstVectorBase<ElementType> vector, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor)
    {
        for (size_t i = 0; i < vector.Size(); ++i)
        {
            math::ScaleUpdate<implementation>(vector[i], tensor.template GetSlice<dimension0, dimension2>(i));
        }
    }

    template <Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void ScaleUpdate(UnorientedConstVectorBase<ElementType> vector, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor)
    {
        for (size_t i = 0; i < tensor.GetSize2(); ++i)
        {
            auto M = tensor.GetPrimarySlice(i);
            for (size_t j = 0; j < tensor.GetSize0(); ++j)
            {
                auto u = M.GetRow(j);
                math::ScaleUpdate<implementation>(vector[j], u);
            }
        }
    }

    template <ImplementationType implementation, typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void AddUpdate(ElementType scalar, TensorReference<ElementType, dimension0, dimension1, dimension2> tensor)
    {
        for (size_t i = 0; i < tensor.NumPrimarySlices(); ++i)
        {
            AddUpdate<implementation>(scalar, tensor.GetPrimarySlice(i));
        }
    }

    template <Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void AddUpdate(UnorientedConstVectorBase<ElementType> vector, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor)
    {
        DEBUG_CHECK_SIZES(vector.Size() != tensor.GetSize2(), "vector and tensor dimensions must be the same");

        for (size_t i = 0; i < vector.Size(); ++i)
        {
            AddUpdate<implementation>(vector[i], tensor.template GetSlice<dimension0, dimension1>(i));
        }
    }

    template <Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void AddUpdate(UnorientedConstVectorBase<ElementType> vector, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor)
    {
        DEBUG_CHECK_SIZES(vector.Size() != tensor.GetSize1(), "vector and tensor dimensions must be the same");
        for (size_t i = 0; i < vector.Size(); ++i)
        {
            AddUpdate<implementation>(vector[i], tensor.template GetSlice<dimension0, dimension2>(i));
        }
    }

    template <Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void AddUpdate(ConstRowVectorReference<ElementType> vector, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor)
    {
        AddUpdate<vectorOrientation, implementation>(vector.Transpose(), tensor);
    }

    template <Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void AddUpdate(ConstColumnVectorReference<ElementType> vector, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor)
    {
        DEBUG_CHECK_SIZES(vector.Size() != tensor.GetSize0(), "vector and tensor dimensions must be the same");

        for (size_t i = 0; i < tensor.GetSize2(); ++i)
        {
            auto M = tensor.GetPrimarySlice(i);
            for (size_t j = 0; j < tensor.GetSize1(); ++j)
            {
                auto u = M.GetColumn(j);
                AddUpdate<implementation>(vector, u);
            }
        }
    }

    template <Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void ScaleAddUpdate(UnorientedConstVectorBase<ElementType> scale, UnorientedConstVectorBase<ElementType> bias, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor)
    {
        DEBUG_CHECK_SIZES(scale.Size() != tensor.GetSize2(), "vector and tensor dimensions must be the same");
        for (size_t i = 0; i < scale.Size(); ++i)
        {
            ScaleAddUpdate<implementation>(scale[i], OnesMatrix(), bias[i], tensor.template GetSlice<dimension0, dimension1>(i));
        }
    }

    template <Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void ScaleAddUpdate(UnorientedConstVectorBase<ElementType> scale, UnorientedConstVectorBase<ElementType> bias, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor)
    {
        DEBUG_CHECK_SIZES(scale.Size() != tensor.GetSize1(), "vector and tensor dimensions must be the same");
        for (size_t i = 0; i < scale.Size(); ++i)
        {
            ScaleAddUpdate<implementation>(scale[i], OnesMatrix(), bias[i], tensor.template GetSlice<dimension0, dimension2>(i));
        }
    }

    template <Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void ScaleAddUpdate(UnorientedConstVectorBase<ElementType> scale, UnorientedConstVectorBase<ElementType> bias, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor)
    {
        DEBUG_CHECK_SIZES(scale.Size() != tensor.GetSize0() || bias.Size() != tensor.GetSize0(), "vectors and tensor dimensions must be the same");

        for (size_t i = 0; i < tensor.GetSize2(); ++i)
        {
            auto M = tensor.GetPrimarySlice(i);
            for (size_t j = 0; j < tensor.GetSize1(); ++j)
            {
                auto u = M.GetColumn(j);
                for (size_t k = 0; k < tensor.GetSize0(); ++k)
                {
                    u[k] = scale[k] * u[k] + bias[k];
                }
            }
        }
    }
} // namespace math
} // namespace ell

#pragma endregion implementation
