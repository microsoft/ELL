////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TensorOperations.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MatrixOperations.h"

namespace ell
{
namespace math
{
    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void Print(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> T, std::ostream& stream, size_t row, size_t column)
    {
        stream << "{" << T(row, column, 0);
        for (size_t k = 1; k < T.NumChannels(); ++k)
        {
            stream << ", " << T(row, column, k);
        }
        stream << "}";
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void Print(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> T, std::ostream& stream, size_t row)
    {
        stream << "{ ";
        Print(T, stream, row, 0);
        for (size_t j = 1; j < T.NumColumns(); ++j)
        {
            stream << ", ";
            Print(T, stream, row, j);
        }
        stream << " }";
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void Print(ConstTensorReference<ElementType, dimension0, dimension1, dimension2> T, std::ostream& stream)
    {
        stream << "{ ";
        Print(T, stream, 0);
        for (size_t i = 1; i < T.NumRows(); ++i)
        {
            stream << ",\n  ";
            Print(T, stream, i);
        }
        stream << " }\n";
    }

    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    std::ostream& operator<<(std::ostream& stream, ConstTensorReference<ElementType, dimension0, dimension1, dimension2> T)
    {
        Print(T, stream);
        return stream;
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void Add(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> T)
    {
        DEBUG_THROW(v.Size() != T.GetLayout()[2], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
        for (size_t i = 0; i < v.Size(); ++i)
        {
            T.template GetSlice<dimension0, dimension1>(i) += v[i];
        }
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void Add(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> T)
    {
        DEBUG_THROW(v.Size() != T.GetLayout()[1], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
        for (size_t i = 0; i < v.Size(); ++i)
        {
            T.template GetSlice<dimension0, dimension2>(i) += v[i];
        }
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void Add(ConstRowVectorReference<ElementType> v, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T)
    {
        Add<vectorOrientation>(v.Transpose(), T);
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void Add(ConstColumnVectorReference<ElementType> v, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T)
    {
        auto layout = T.GetLayout();
        DEBUG_THROW(v.Size() != layout[0], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));

        for (size_t i = 0; i < layout[2]; ++i)
        {
            auto M = T.GetPrimarySlice(i);
            for (size_t j = 0; j < layout[1]; ++j)
            {
                auto u = M.GetColumn(j);
                u += v;
            }
        }
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> T)
    {
        Internal::TensorOperations<ImplementationType::openBlas>::Multiply<vectorOrientation>(v, T);
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> T)
    {
        Internal::TensorOperations<ImplementationType::openBlas>::Multiply<vectorOrientation>(v, T);
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T)
    {
        Internal::TensorOperations<ImplementationType::openBlas>::Multiply<vectorOrientation>(v, T);
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void MultiplyAdd(UnorientedConstVectorReference<ElementType> s, UnorientedConstVectorReference<ElementType> b, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> T)
    {
        DEBUG_THROW(s.Size() != T.GetLayout()[2], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
        for (size_t i = 0; i < s.Size(); ++i)
        {
            math::MultiplyAdd(s[i], b[i], T.template GetSlice<dimension0, dimension1>(i));
        }
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void MultiplyAdd(UnorientedConstVectorReference<ElementType> s, UnorientedConstVectorReference<ElementType> b, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> T)
    {
        DEBUG_THROW(s.Size() != T.GetLayout()[1], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
        for (size_t i = 0; i < s.Size(); ++i)
        {
            math::MultiplyAdd(s[i], b[i], T.template GetSlice<dimension0, dimension2>(i));
        }
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void MultiplyAdd(UnorientedConstVectorReference<ElementType> s, UnorientedConstVectorReference<ElementType> b, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T)
    {
        auto layout = T.GetLayout();
        DEBUG_THROW(s.Size() != layout[0] || b.Size() != layout[0], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vectors and tensor dimensions must be the same"));

        for (size_t i = 0; i < layout[2]; ++i)
        {
            auto M = T.GetPrimarySlice(i);
            for (size_t j = 0; j < layout[1]; ++j)
            {
                auto u = M.GetColumn(j);
                for (size_t k = 0; k < layout[0]; ++k)
                {
                    u[k] = s[k] * u[k] + b[k];
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
        void TensorOperations<ImplementationType::native>::Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> T)
        {
            DEBUG_THROW(v.Size() != T.GetLayout()[2], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
            for (size_t i = 0; i < v.Size(); ++i)
            {
                MatrixOperations<ImplementationType::native>::Multiply(v[i], T.template GetSlice<dimension0, dimension1>(i));
            }
        }

        template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
        void TensorOperations<ImplementationType::native>::Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> T)
        {
            DEBUG_THROW(v.Size() != T.GetLayout()[1], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
            for (size_t i = 0; i < v.Size(); ++i)
            {
                MatrixOperations<ImplementationType::native>::Multiply(v[i], T.template GetSlice<dimension0, dimension2>(i));
            }
        }

        template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
        void TensorOperations<ImplementationType::native>::Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T)
        {
            auto layout = T.GetLayout();
            DEBUG_THROW(v.Size() != layout[0], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));

            for (size_t i = 0; i < layout[2]; ++i)
            {
                auto M = T.GetPrimarySlice(i);
                for (size_t j = 0; j < layout[0]; ++j)
                {
                    auto u = M.GetRow(j);
                    VectorOperations<ImplementationType::native>::Multiply(v[j], u);
                }
            }
        }

#ifdef USE_BLAS
        //
        // OpenBLAS implementations of operations
        //

        template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
        void TensorOperations<ImplementationType::openBlas>::Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> T)
        {
            DEBUG_THROW(v.Size() != T.GetLayout()[2], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
            for (size_t i = 0; i < v.Size(); ++i)
            {
                MatrixOperations<ImplementationType::openBlas>::Multiply(v[i], T.template GetSlice<dimension0, dimension1>(i));
            }
        }

        template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
        void TensorOperations<ImplementationType::openBlas>::Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> T)
        {
            DEBUG_THROW(v.Size() != T.GetLayout()[1], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
            for (size_t i = 0; i < v.Size(); ++i)
            {
                MatrixOperations<ImplementationType::openBlas>::Multiply(v[i], T.template GetSlice<dimension0, dimension2>(i));
            }
        }

        template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
        void TensorOperations<ImplementationType::openBlas>::Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T)
        {
            auto layout = T.GetLayout();
            DEBUG_THROW(v.Size() != layout[0], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));

            for (size_t i = 0; i < layout[2]; ++i)
            {
                auto M = T.GetPrimarySlice(i);
                for (size_t j = 0; j < layout[0]; ++j)
                {
                    auto u = M.GetRow(j);
                    VectorOperations<ImplementationType::openBlas>::Multiply(v[j], u);
                }
            }
        }
#endif
    }
}
}