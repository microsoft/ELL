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
        size_t lhsIncrement = lhsVector.GetIncrement();
        const ElementType* pRhs = rhsVector.GetDataPointer();
        size_t rhsIncrement = rhsVector.GetIncrement();

#ifdef USE_BLAS
        return Blas::Axpy(lhsSize, rhsScalar, pRhs, rhsIncrement, pLhs, lhsIncrement);
#else
        const ElementType* pEnd = pLhs + lhsSize;

        while (pLhs < pEnd)
        {
            (*pLhs) += rhsScalar * (*pRhs);
            pLhs += lhsIncrement;
            pRhs += rhsIncrement;
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
        size_t Increment1 = vector1.GetIncrement();
        const ElementType* ptr2 = vector2.GetDataPointer();
        size_t Increment2 = vector2.GetIncrement();

#ifdef USE_BLAS
        return Blas::Dot(size1, ptr1, Increment1, ptr2, Increment2);
#else
        ElementType result = 0;
        const ElementType* end1 = ptr1 + size1;

        while (ptr1 < end1)
        {
            result += (*ptr1) * (*ptr2);
            ptr1 += Increment1;
            ptr2 += Increment2;
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