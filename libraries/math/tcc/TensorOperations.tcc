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

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void Add(ElementType scalar, TensorReference<ElementType, dimension0, dimension1, dimension2> tensor)
    {
        for (size_t i = 0; i < tensor.NumPrimarySlices(); ++i)
        {
            Add(scalar, tensor.GetPrimarySlice(i));
        }
    }

    template <typename TensorElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2, typename ScalarType, utilities::IsFundamental<ScalarType>>
    void operator+=(TensorReference<TensorElementType, dimension0, dimension1, dimension2> tensor, ScalarType scalar)
    {
        Add(static_cast<TensorElementType>(scalar), tensor);
    }

    template <typename TensorElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2, typename ScalarType, utilities::IsFundamental<ScalarType>>
    void operator-=(TensorReference<TensorElementType, dimension0, dimension1, dimension2> tensor, ScalarType scalar)
    {
        Add(-static_cast<TensorElementType>(scalar), tensor);
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void Add(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor)
    {
        DEBUG_THROW(vector.Size() != tensor.GetSize2(), utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
        for (size_t i = 0; i < vector.Size(); ++i)
        {
            tensor.template GetSlice<dimension0, dimension1>(i) += vector[i];
        }
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void Add(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor)
    {
        DEBUG_THROW(vector.Size() != tensor.GetSize1(), utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
        for (size_t i = 0; i < vector.Size(); ++i)
        {
            tensor.template GetSlice<dimension0, dimension2>(i) += vector[i];
        }
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void Add(ConstRowVectorReference<ElementType> vector, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor)
    {
        Add<vectorOrientation>(vector.Transpose(), tensor);
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void Add(ConstColumnVectorReference<ElementType> vector, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor)
    {
        DEBUG_THROW(vector.Size() != tensor.GetSize0(), utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));

        for (size_t i = 0; i < tensor.GetSize2(); ++i)
        {
            auto M = tensor.GetPrimarySlice(i);
            for (size_t j = 0; j < tensor.GetSize1(); ++j)
            {
                auto u = M.GetColumn(j);
                u += vector;
            }
        }
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void Multiply(ElementType scalar, TensorReference<ElementType, dimension0, dimension1, dimension2> tensor)
    {
        for (size_t i = 0; i < tensor.NumPrimarySlices(); ++i)
        {
            Multiply(scalar, tensor.GetPrimarySlice(i));
        }
    }

    template <typename TensorElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2, typename ScalarType, utilities::IsFundamental<ScalarType>>
    void operator*=(TensorReference<TensorElementType, dimension0, dimension1, dimension2> tensor, ScalarType scalar)
    {
        Multiply(static_cast<TensorElementType>(scalar), tensor);
    }

    template <typename TensorElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2, typename ScalarType, utilities::IsFundamental<ScalarType>>
    void operator/=(TensorReference<TensorElementType, dimension0, dimension1, dimension2> tensor, ScalarType scalar)
    {
        if (scalar == 0)
        {
            throw utilities::NumericException(utilities::NumericExceptionErrors::divideByZero, "divide by zero");
        }
        Multiply(static_cast<TensorElementType>(1.0/scalar), tensor);
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor)
    {
        Internal::TensorOperations<ImplementationType::openBlas>::Multiply<vectorOrientation>(vector, tensor);
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor)
    {
        Internal::TensorOperations<ImplementationType::openBlas>::Multiply<vectorOrientation>(vector, tensor);
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor)
    {
        Internal::TensorOperations<ImplementationType::openBlas>::Multiply<vectorOrientation>(vector, tensor);
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void MultiplyAdd(UnorientedConstVectorReference<ElementType> scale, UnorientedConstVectorReference<ElementType> bias, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor)
    {
        DEBUG_THROW(scale.Size() != tensor.GetSize2(), utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
        for (size_t i = 0; i < scale.Size(); ++i)
        {
            math::MultiplyAdd(scale[i], bias[i], tensor.template GetSlice<dimension0, dimension1>(i));
        }
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void MultiplyAdd(UnorientedConstVectorReference<ElementType> scale, UnorientedConstVectorReference<ElementType> bias, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor)
    {
        DEBUG_THROW(scale.Size() != tensor.GetSize1(), utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
        for (size_t i = 0; i < scale.Size(); ++i)
        {
            math::MultiplyAdd(scale[i], bias[i], tensor.template GetSlice<dimension0, dimension2>(i));
        }
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void MultiplyAdd(UnorientedConstVectorReference<ElementType> scale, UnorientedConstVectorReference<ElementType> bias, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor)
    {
        DEBUG_THROW(scale.Size() != tensor.GetSize0() || bias.Size() != tensor.GetSize0(), utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vectors and tensor dimensions must be the same"));

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

    namespace Internal
    {
        //
        // native implementations of operations
        //

        template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
        void TensorOperations<ImplementationType::native>::Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor)
        {
            DEBUG_THROW(vector.Size() != tensor.GetSize2(), utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
            for (size_t i = 0; i < vector.Size(); ++i)
            {
                MatrixOperations<ImplementationType::native>::Multiply(vector[i], tensor.template GetSlice<dimension0, dimension1>(i));
            }
        }

        template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
        void TensorOperations<ImplementationType::native>::Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor)
        {
            DEBUG_THROW(vector.Size() != tensor.GetSize1(), utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
            for (size_t i = 0; i < vector.Size(); ++i)
            {
                MatrixOperations<ImplementationType::native>::Multiply(vector[i], tensor.template GetSlice<dimension0, dimension2>(i));
            }
        }

        template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
        void TensorOperations<ImplementationType::native>::Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor)
        {
            DEBUG_THROW(vector.Size() != tensor.GetSize0(), utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));

            for (size_t i = 0; i < tensor.GetSize2(); ++i)
            {
                auto M = tensor.GetPrimarySlice(i);
                for (size_t j = 0; j < tensor.GetSize0(); ++j)
                {
                    auto u = M.GetRow(j);
                    ScaleUpdate(vector[j], u);
                }
            }
        }

#ifdef USE_BLAS
        //
        // OpenBLAS implementations of operations
        //

        template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
        void TensorOperations<ImplementationType::openBlas>::Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> tensor)
        {
            DEBUG_THROW(vector.Size() != tensor.GetSize2(), utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
            for (size_t i = 0; i < vector.Size(); ++i)
            {
                MatrixOperations<ImplementationType::openBlas>::Multiply(vector[i], tensor.template GetSlice<dimension0, dimension1>(i));
            }
        }

        template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
        void TensorOperations<ImplementationType::openBlas>::Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> tensor)
        {
            DEBUG_THROW(vector.Size() != tensor.GetSize1(), utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
            for (size_t i = 0; i < vector.Size(); ++i)
            {
                MatrixOperations<ImplementationType::openBlas>::Multiply(vector[i], tensor.template GetSlice<dimension0, dimension2>(i));
            }
        }

        template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
        void TensorOperations<ImplementationType::openBlas>::Multiply(UnorientedConstVectorReference<ElementType> vector, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> tensor)
        {
            DEBUG_THROW(vector.Size() != tensor.GetSize0(), utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));

            for (size_t i = 0; i < tensor.GetSize2(); ++i)
            {
                auto M = tensor.GetPrimarySlice(i);
                for (size_t j = 0; j < tensor.GetSize0(); ++j)
                {
                    auto u = M.GetRow(j);
                    ScaleUpdate(vector[j], u);
                }
            }
        }
#endif
    }
}
}
