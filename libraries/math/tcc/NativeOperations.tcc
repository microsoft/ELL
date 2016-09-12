////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     NativeOperations.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Exception.h"
#include "..\include\Operations.h"

namespace math
{
    //
    // NativeOperations
    // 

    template<typename ElementType, typename MapperType>
    ElementType Aggregate(const ElementType *pData, size_t size, size_t increment, MapperType mapper)
    {
        ElementType result = 0;
        const ElementType* current = pData;
        const ElementType* end = pData + size * increment;
        while (current < end)
        {
            result += mapper(*current);
            current += increment;
        }
        return result;
    }

    template<typename ElementType, typename MapperType>
    void ForEach(ElementType *pData, size_t size, size_t increment, MapperType mapper)
    {
        ElementType* current = pData;
        const ElementType* end = pData + size * increment;
        while (current < end)
        {
            *current = mapper(*current);
            current += increment;
        }
    }

    template<typename ElementType, VectorOrientation Orientation>
    ElementType NativeOperations::Norm0(ConstVectorReference<ElementType, Orientation>& v)
    {
        return Aggregate(v._pData, v._size, v._increment, [](ElementType x) { return x != 0 ? 1 : 0; });
    }

    template<typename ElementType, VectorOrientation Orientation>
    ElementType NativeOperations::Norm1(ConstVectorReference<ElementType, Orientation>& v)
    {
        return Aggregate(v._pData, v._size, v._increment, [](ElementType x) { return std::abs(x); });
    }

    template<typename ElementType, VectorOrientation Orientation>
    ElementType NativeOperations::Norm2(ConstVectorReference<ElementType, Orientation>& v)
    {
        return std::sqrt(Aggregate(v._pData, v._size, v._increment, [](ElementType x) { return x*x; }));
    }

    template<typename ElementType, VectorOrientation Orientation>
    static void NativeOperations::Add(ElementType s, VectorReference<ElementType, Orientation>& v)
    {
        ForEach(v._pData, v._size, v._increment, [s](ElementType x) { return x + s; });
    }

    template<typename ElementType, VectorOrientation Orientation>
    void NativeOperations::Add(ElementType s, ConstVectorReference<ElementType, Orientation>& v, VectorReference<ElementType, Orientation>& u)
    {
        // TODO check inputs for equal size

        ElementType* uData = u._pData;
        const ElementType* vData = v._pData;
        const ElementType* end = u._pData + u._increment * u._size;

        while (uData < end)
        {
            (*uData) += s * (*vData);
            uData += u._increment;
            vData += v._increment;
        }
    }

    template<typename ElementType, VectorOrientation Orientation1, VectorOrientation Orientation2>
    ElementType NativeOperations::Dot(ConstVectorReference<ElementType, Orientation1>& u, ConstVectorReference<ElementType, Orientation2>& v)
    {
        // TODO check inputs for equal size

        const ElementType* uData = u._pData;
        const ElementType* vData = v._pData;

        ElementType result = 0;
        const ElementType* end = u._pData + u._increment * u._size;

        while (uData < end)
        {
            result += (*uData) * (*vData);
            uData += u._increment;
            vData += v._increment;
        }
        return result;
    }

    template<typename ElementType, VectorOrientation Orientation>
    void NativeOperations::Multiply(ElementType s, VectorReference<ElementType, Orientation>& v)
    {
        ForEach(v._pData, v._size, v._increment, [s](ElementType x) { return x * s; });
    }

    template<typename ElementType>
    void NativeOperations::Multiply(ConstVectorReference<ElementType, VectorOrientation::row>& u, ConstVectorReference<ElementType, VectorOrientation::column>& v, ElementType& r)
    {
        r = Dot(u, v);
    }

    template<typename ElementType, MatrixLayout Layout>
    void NativeOperations::Multiply(ElementType s, ConstMatrixReference<ElementType, Layout>& M, ConstVectorReference<ElementType, VectorOrientation::column>& v, ElementType t, VectorReference<ElementType, VectorOrientation::column>& u)
    {
        // TODO check inputs for equal size

        for (size_t i = 0; i < M.NumRows(); ++i)
        {
            auto row = M.GetRow(i);
            u[i] = s * Dot(row, v) + t * u[i];
        }
    }
}