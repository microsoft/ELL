////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BlasOperations.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Exception.h"

namespace emll
{
namespace math
{
    template<typename ElementType, VectorOrientation Orientation>
    ElementType BlasOperations::Norm1(ConstVectorReference<ElementType, Orientation>& v) 
    {
        return Blas::Asum(static_cast<int>(v._size), v._pData, static_cast<int>(v._increment));
    }

    template<typename ElementType, VectorOrientation Orientation>
    ElementType BlasOperations::Norm2(ConstVectorReference<ElementType, Orientation>& v) 
    {
        return Blas::Nrm2(static_cast<int>(v._size), v._pData, static_cast<int>(v._increment));
    }

    template<typename ElementType, VectorOrientation Orientation>
    void BlasOperations::Add(ElementType s, ConstVectorReference<ElementType, Orientation>& v, VectorReference<ElementType, Orientation>& u)
    {
        if (v._size != u._size)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectors u and v are not the same size.");
        }

        return Blas::Axpy(static_cast<int>(u._size), s, v._pData, static_cast<int>(v._increment), u._pData, static_cast<int>(u._increment));
    }

    template<typename ElementType, VectorOrientation OrientationU, VectorOrientation OrientationV>
    ElementType BlasOperations::Dot(ConstVectorReference<ElementType, OrientationU>& u, ConstVectorReference<ElementType, OrientationV>& v)
    {
        if (v._size != u._size)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectors u and v are not the same size.");
        }

        return Blas::Dot(static_cast<int>(u._size), u._pData, static_cast<int>(u._increment), v._pData, static_cast<int>(v._increment));
    }

    template<typename ElementType, VectorOrientation Orientation>
    void BlasOperations::Multiply(ElementType s, VectorReference<ElementType, Orientation>& v)
    {
        Blas::Scal(static_cast<int>(v._size), s, v._pData, static_cast<int>(v._increment));
    }

    template<typename ElementType>
    void BlasOperations::Multiply(ConstVectorReference<ElementType, VectorOrientation::row>& u, ConstVectorReference<ElementType, VectorOrientation::column>& v, ElementType& r)
    {
        r = Dot(u, v);
    }

    template<typename ElementType, MatrixLayout Layout>
    void BlasOperations::Multiply(ElementType s, ConstMatrixReference<ElementType, Layout>& M, ConstVectorReference<ElementType, VectorOrientation::column>& v, ElementType t, VectorReference<ElementType, VectorOrientation::column>& u)
    {
        if (M._numRows != u._size || M._numColumns != v._size)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible matrix and vectors sizes.");
        }

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

        Blas::Gemv(order, CBLAS_TRANSPOSE::CblasNoTrans, static_cast<int>(M._numRows), static_cast<int>(M._numColumns), s, M._pData, static_cast<int>(M._increment), v._pData, static_cast<int>(v._increment), t, u._pData, static_cast<int>(u._increment));
    }

    template<typename ElementType, MatrixLayout Layout>
    void BlasOperations::Multiply(ElementType s, ConstVectorReference<ElementType, VectorOrientation::row>& v, ConstMatrixReference<ElementType, Layout>& M, ElementType t, VectorReference<ElementType, VectorOrientation::row>& u)
    {
        // TODO
    }
}
}