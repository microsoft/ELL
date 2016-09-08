////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Operations.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace math
{
    template<typename ElementType, VectorOrientation Orientation>
    void Operations::AddTo(ElementType rhsScalar, VectorReference<ElementType, Orientation>& lhsVector)
    {
        lhsVector += rhsScalar;
    }

    template<typename ElementType, VectorOrientation Orientation>
    void Operations::AddTo(ElementType rhsScalar, ConstVectorReference<ElementType, Orientation>& rhsVector, VectorReference<ElementType, Orientation>& lhsVector)
    {
        size_t lhsSize = lhsVector.Size();
        size_t rhsSize = rhsVector.Size();

        // TODO check inputs for equal size

        ElementType* pLhs = lhsVector.GetDataPointer();
        size_t lhsStride = lhsVector.GetStride();
        const ElementType* pRhs = rhsVector.GetDataPointer();
        size_t rhsStride = rhsVector.GetStride();

#ifdef USE_BLAS
        return Blas::Axpy(lhsSize, rhsScalar, pRhs, rhsStride, pLhs, lhsStride);
#else
        const ElementType* pEnd = pLhs + lhsSize;

        while (pLhs < pEnd)
        {
            (*pLhs) += rhsScalar * (*pRhs);
            pLhs += lhsStride;
            pRhs += rhsStride;
        }
#endif
    }

    template<typename ElementType, VectorOrientation Orientation1, VectorOrientation Orientation2>
    ElementType Operations::Dot(ConstVectorReference<ElementType, Orientation1>& vector1, ConstVectorReference<ElementType, Orientation2>& vector2)
    {
        size_t size1 = vector1.Size();
        size_t size2 = vector2.Size();

        // TODO check inputs for equal size

        const ElementType* ptr1 = vector1.GetDataPointer();
        size_t stride1 = vector1.GetStride();
        const ElementType* ptr2 = vector2.GetDataPointer();
        size_t stride2 = vector2.GetStride();

#ifdef USE_BLAS
        return Blas::Dot(size1, ptr1, stride1, ptr2, stride2);
#else
        ElementType result = 0;
        const ElementType* end1 = ptr1 + size1;

        while (ptr1 < end1)
        {
            result += (*ptr1) * (*ptr2);
            ptr1 += stride1;
            ptr2 += stride2;
        }
        return result;
#endif
    }

    template<typename ElementType>
    void Operations::Product(ConstVectorReference<ElementType, VectorOrientation::row>& rhsVector1, ConstVectorReference<ElementType, VectorOrientation::column>& rhsVector2, ElementType& lhsScalar)
    {
        lhsScalar = Dot(rhsVector1, rhsVector2);
    }
}