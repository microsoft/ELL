////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TensorOperations.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace math
{
    template<typename ElementType>
    ElementType TensorOperations::Dot(const VectorReferenceBase<ElementType>& vector1, const VectorReferenceBase<ElementType>& vector2)
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
    void TensorOperations::Product(const VectorReference<ElementType, TensorOrientation::rowMajor>& left, const VectorReference<ElementType, TensorOrientation::columnMajor>& right, ElementType& result)
    {
        result = Dot(left, right);
    }
}