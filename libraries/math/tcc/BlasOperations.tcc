////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BlasOperations.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Exception.h"

namespace math
{
    template<typename ElementType, VectorOrientation Orientation>
    void BlasOperations::AddTo(ElementType s, VectorReference<ElementType, Orientation>& v)
    {
        v += s;
    }

    template<typename ElementType, VectorOrientation Orientation>
    void BlasOperations::AddTo(ElementType s, ConstVectorReference<ElementType, Orientation>& v, VectorReference<ElementType, Orientation>& u)
    {

        // TODO check inputs for equal size

        return Blas::Axpy(static_cast<int>(u._size), s, v._pData, static_cast<int>(v._increment), u._pData, static_cast<int>(u._increment));
    }

    template<typename ElementType, VectorOrientation Orientation1, VectorOrientation Orientation2>
    ElementType BlasOperations::Dot(ConstVectorReference<ElementType, Orientation1>& u, ConstVectorReference<ElementType, Orientation2>& v)
    {
        // TODO check inputs for equal size

        return Blas::Dot(static_cast<int>(u._size), u._pData, static_cast<int>(u._increment), v._pData, static_cast<int>(v._increment));
    }

    template<typename ElementType>
    void BlasOperations::Multiply(ConstVectorReference<ElementType, VectorOrientation::row>& u, ConstVectorReference<ElementType, VectorOrientation::column>& v, ElementType& r)
    {
        r = Dot(u, v);
    }

    template<typename ElementType, MatrixLayout Layout>
    void BlasOperations::Multiply(ElementType s, ConstMatrixReference<ElementType, Layout>& M, ConstVectorReference<ElementType, VectorOrientation::column>& v, ElementType t, VectorReference<ElementType, VectorOrientation::column>& u)
    {
        // TODO check inputs for equal size

        // map Layout to CBLAS_ORDER
        CBLAS_ORDER order;
        switch (M.GetLayout())
        {
        case MatrixLayout::rowMajor:
            order = CBLAS_ORDER::CblasRowMajor;
            break;
        case MatrixLayout::columnMajor:
            order = CBLAS_ORDER::CblasColMajor;
            break;
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Layout not supported");
        }

        Blas::Gemv(order, CBLAS_TRANSPOSE::CblasNoTrans, static_cast<int>(M.NumRows()), static_cast<int>(M.NumColumns()), s, M._pData, static_cast<int>(M._increment), v._pData, static_cast<int>(v._increment), t, u._pData, static_cast<int>(u._increment));
    }
}