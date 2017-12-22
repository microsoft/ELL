////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TensorOperations.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MatrixOperations.h"

// utilities
#include "Logger.h"

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

    template<Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void ScaleUpdate(UnorientedConstVectorBase<ElementType> vector, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor)
    {
        for (size_t i = 0; i < vector.Size(); ++i)
        {
            math::ScaleUpdate<implementation>(vector[i], tensor.template GetSlice<dimension0, dimension1>(i));
        }
    }

    template<Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void ScaleUpdate(UnorientedConstVectorBase<ElementType> vector, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor)
    {
        for (size_t i = 0; i < vector.Size(); ++i)
        {
            math::ScaleUpdate<implementation>(vector[i], tensor.template GetSlice<dimension0, dimension2>(i));
        }
    }

    template<Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension1, Dimension dimension2>
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

    template<Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void AddUpdate(UnorientedConstVectorBase<ElementType> vector, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor)
    {
        DEBUG_CHECK_SIZES(vector.Size() != tensor.GetSize2(), "vector and tensor dimensions must be the same");

        for (size_t i = 0; i < vector.Size(); ++i)
        {
            AddUpdate<implementation>(vector[i], tensor.template GetSlice<dimension0, dimension1>(i));
        }
    }

    template<Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void AddUpdate(UnorientedConstVectorBase<ElementType> vector, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor)
    {
        DEBUG_CHECK_SIZES(vector.Size() != tensor.GetSize1(), "vector and tensor dimensions must be the same");
        for (size_t i = 0; i < vector.Size(); ++i)
        {
            AddUpdate<implementation>(vector[i], tensor.template GetSlice<dimension0, dimension2>(i));
        }
    }

    template<Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void AddUpdate(ConstRowVectorReference<ElementType> vector, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor)
    {
        AddUpdate<vectorOrientation, implementation>(vector.Transpose(), tensor);
    }

    template<Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension1, Dimension dimension2>
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

    template<Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void ScaleAddUpdate(UnorientedConstVectorBase<ElementType> scale, UnorientedConstVectorBase<ElementType> bias, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor)
    {
        DEBUG_CHECK_SIZES(scale.Size() != tensor.GetSize2(), "vector and tensor dimensions must be the same");
        for (size_t i = 0; i < scale.Size(); ++i)
        {
            ScaleAddUpdate<implementation>(scale[i], OnesMatrix(), bias[i], tensor.template GetSlice<dimension0, dimension1>(i));
        }
    }

    template<Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void ScaleAddUpdate(UnorientedConstVectorBase<ElementType> scale, UnorientedConstVectorBase<ElementType> bias, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor)
    {
        DEBUG_CHECK_SIZES(scale.Size() != tensor.GetSize1(), "vector and tensor dimensions must be the same");
        for (size_t i = 0; i < scale.Size(); ++i)
        {
            ScaleAddUpdate<implementation>(scale[i], OnesMatrix(), bias[i], tensor.template GetSlice<dimension0, dimension2>(i));
        }
    }

    template<Dimension vectorOrientation, ImplementationType implementation, typename ElementType, Dimension dimension1, Dimension dimension2>
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
}
}
