////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TensorOperations.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Operations.h"

namespace ell
{
namespace math
{
    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void TensorOperations::Add(ConstVectorReference<ElementType, VectorOrientation::row> v, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T)
    {
        Add<vectorOrientation>(v.Transpose(), T);
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void TensorOperations::Add(ConstVectorReference<ElementType, VectorOrientation::column> v, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T)
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
    void TensorOperations::Add(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> T)
    {
        DEBUG_THROW(v.Size() != T.GetLayout()[2], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
        for (size_t i = 0; i < v.Size(); ++i)
        {
            Operations::Add(v[i], T.template GetSlice<dimension0, dimension1 >(i));
        }
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void TensorOperations::Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> T)
    {
        DEBUG_THROW(v.Size() != T.GetLayout()[2], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
        for (size_t i = 0; i < v.Size(); ++i)
        {
            Operations::Multiply(v[i], T.template GetSlice<dimension0, dimension1>(i));
        }
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension1>
    void TensorOperations::MultiplyAdd(UnorientedConstVectorReference<ElementType> s, UnorientedConstVectorReference<ElementType> b, TensorReference<ElementType, dimension0, dimension1, vectorOrientation> T)
    {
        DEBUG_THROW(s.Size() != T.GetLayout()[2], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
        for (size_t i = 0; i < s.Size(); ++i)
        {
            Operations::MultiplyAdd(s[i], b[i], T.template GetSlice<dimension0, dimension1>(i));
        }
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void TensorOperations::MultiplyAdd(UnorientedConstVectorReference<ElementType> s, UnorientedConstVectorReference<ElementType> b, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> T)
    {
        DEBUG_THROW(s.Size() != T.GetLayout()[1], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
        for (size_t i = 0; i < s.Size(); ++i)
        {
            Operations::MultiplyAdd(s[i], b[i], T.template GetSlice<dimension0, dimension2>(i));
        }
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void TensorOperations::MultiplyAdd(UnorientedConstVectorReference<ElementType> s, UnorientedConstVectorReference<ElementType> b, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T)
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

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void TensorOperations::Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> T)
    {
        DEBUG_THROW(v.Size() != T.GetLayout()[1], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
        for (size_t i = 0; i < v.Size(); ++i)
        {
            Operations::Multiply(v[i], T.template GetSlice<dimension0, dimension2>(i));
        }
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension1, Dimension dimension2>
    void TensorOperations::Multiply(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, vectorOrientation, dimension1, dimension2> T)
    {
        auto layout = T.GetLayout();
        DEBUG_THROW(v.Size() != layout[0], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));

        for (size_t i = 0; i < layout[2]; ++i)
        {
            auto M = T.GetPrimarySlice(i);
            for (size_t j = 0; j < layout[1]; ++j)
            {
                auto u = M.GetColumn(j);
                for (size_t k = 0; k < layout[0]; ++k)
                {
                    u[k] *= v[k];
                }
            }
        }
    }

    template<Dimension vectorOrientation, typename ElementType, Dimension dimension0, Dimension dimension2>
    void TensorOperations::Add(UnorientedConstVectorReference<ElementType> v, TensorReference<ElementType, dimension0, vectorOrientation, dimension2> T)
    {
        DEBUG_THROW(v.Size() != T.GetLayout()[1], utilities::InputException(utilities::InputExceptionErrors::sizeMismatch, "vector and tensor dimensions must be the same"));
        for (size_t i = 0; i < v.Size(); ++i)
        {
            Operations::Add(v[i], T.template GetSlice<dimension0, dimension2>(i));
        }
    }
}
}