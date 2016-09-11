////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Operations.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Exception.h"

namespace math
{
    template<typename ElementType, VectorOrientation Orientation>
    void Operations::AddTo(ElementType s, VectorReference<ElementType, Orientation>& v)
    {
        v += s;
    }

    template<typename ElementType, VectorOrientation Orientation>
    void Operations::AddTo(ElementType s, ConstVectorReference<ElementType, Orientation>& v, VectorReference<ElementType, Orientation>& u)
    {

        // TODO check inputs for equal size

#ifdef USE_BLAS
        return Blas::Axpy(static_cast<int>(u.Size()), s, vData, static_cast<int>(v._increment), uData, static_cast<int>(u._increment));
#else
        ElementType* uData = u._pData;
        size_t uIncrement = u._increment;
        const ElementType* vData = v._pData;
        size_t vIncrement = v._increment;
        const ElementType* end = uData + uIncrement * u.Size();

        while (uData < end)
        {
            (*uData) += s * (*vData);
            uData += uIncrement;
            vData += vIncrement;
        }
#endif
    }

    template<typename ElementType, VectorOrientation Orientation1, VectorOrientation Orientation2>
    ElementType Operations::Dot(ConstVectorReference<ElementType, Orientation1>& u, ConstVectorReference<ElementType, Orientation2>& v)
    {
        // TODO check inputs for equal size

        const ElementType* uData = u._pData;
        size_t uIncrement = u._increment;
        const ElementType* vData = v._pData;
        size_t vIncrement = v._increment;

#ifdef USE_BLAS
        return Blas::Dot(static_cast<int>(u.Size()), uData, static_cast<int>(uIncrement), vData, static_cast<int>(vIncrement));
#else
        ElementType result = 0;
        const ElementType* end = uData + uIncrement * u.Size();

        while (uData < end)
        {
            result += (*uData) * (*vData);
            uData += uIncrement;
            vData += vIncrement;
        }
        return result;
#endif
    }

    template<typename ElementType>
    void Operations::Multiply(ConstVectorReference<ElementType, VectorOrientation::row>& u, ConstVectorReference<ElementType, VectorOrientation::column>& v, ElementType& r)
    {
        r = Dot(u, v);
    }

    template<typename ElementType, MatrixLayout Layout>
    void Operations::Multiply(ElementType s, ConstMatrixReference<ElementType, Layout>& M, ConstVectorReference<ElementType, VectorOrientation::column>& v, ElementType t, VectorReference<ElementType, VectorOrientation::column>& u)
    {

        // TODO check inputs for equal size

#ifdef USE_BLAS

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
#else
        for (size_t i = 0; i < M.NumRows(); ++i)
        {
            auto row = M.GetRow(i);
            u[i] = s * Dot(row, v) + t * u[i];
        }
#endif
    }
}