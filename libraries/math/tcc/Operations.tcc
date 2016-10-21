////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CommonOperations.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Exception.h"

namespace emll
{
namespace math
{
    //
    // CommonOperations
    //

    template <typename ElementType>
    ElementType CommonOperations::Norm0(UnorientedConstVectorReference<ElementType> v)
    {
        return v.Aggregate([](ElementType x) { return x != 0 ? 1 : 0; });
    }

    template <typename ElementType, VectorOrientation Orientation>
    void CommonOperations::Add(ElementType s, VectorReference<ElementType, Orientation> v)
    {
        v.Transform([s](ElementType x) { return x + s; });
    }

    template <typename ElementType, MatrixLayout Layout>
    void CommonOperations::Add(ElementType s, MatrixReference<ElementType, Layout> M)
    {
        for (size_t i = 0; i < M.NumIntervals(); ++i)
        {
            auto interval = M.GetMajorVector(i);
            Add(s, interval);
        }
    }

    //
    // DerivedOperations
    //

    template <class DerivedClass>
    template <typename ElementType, MatrixLayout Layout>
    void DerivedOperations<DerivedClass>::Copy(ConstMatrixReference<ElementType, Layout> B, MatrixReference<ElementType, Layout> A)
    {
        if (A.NumRows() != B.NumRows() || A.NumColumns() != B.NumColumns())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Matrix dimensions are not the same size.");
        }

        for (size_t i = 0; i < B.NumIntervals(); ++i)
        {
            auto intervalA = A.GetMajorVector(i);
            auto intervalB = B.GetMajorVector(i);
            DerivedClass::Copy(intervalB, intervalA);
        }
    }

    template <class DerivedClass>
    template <typename ElementType, MatrixLayout Layout>
    void DerivedOperations<DerivedClass>::Multiply(ElementType s, MatrixReference<ElementType, Layout> M)
    {
        for (size_t i = 0; i < M.NumIntervals(); ++i)
        {
            auto interval = M.GetMajorVector(i);
            DerivedClass::Multiply(s, interval);
        }
    }

    template <class DerivedClass>
    template <typename ElementType, MatrixLayout Layout>
    void DerivedOperations<DerivedClass>::Multiply(ElementType s, ConstVectorReference<ElementType, VectorOrientation::row> v, ConstMatrixReference<ElementType, Layout> M, ElementType t, VectorReference<ElementType, VectorOrientation::row> u)
    {
        DerivedClass::Multiply(s, M.Transpose(), v.Transpose(), t, u.Transpose());
    }

    //
    // Native implementations of operations
    //

    template <typename ElementType, VectorOrientation Orientation>
    void OperationsImplementation<ImplementationType::native>::Copy(ConstVectorReference<ElementType, Orientation> v, VectorReference<ElementType, Orientation> u)
    {
        if (v.Size() != u.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectors u and v are not the same size.");
        }

        ElementType* uData = u.GetDataPointer();
        const ElementType* vData = v.GetDataPointer();

        if (u.GetIncrement() == 1 && v.GetIncrement() == 1)
        {
            // optimized implementation for vectors with trivial increments
            const ElementType* vEnd = v.GetDataPointer() + v.Size();
            std::copy(vData, vEnd, uData);
        }
        else
        {
            const ElementType* vEnd = v.GetDataPointer() + v.GetIncrement() * v.Size();
            while (vData < vEnd)
            {
                (*uData) = (*vData);
                uData += u.GetIncrement();
                vData += v.GetIncrement();
            }
        }
    }

    template <typename ElementType>
    ElementType OperationsImplementation<ImplementationType::native>::Norm1(UnorientedConstVectorReference<ElementType> v)
    {
        return v.Aggregate([](ElementType x) { return std::abs(x); });
    }

    template <typename ElementType>
    ElementType OperationsImplementation<ImplementationType::native>::Norm2(UnorientedConstVectorReference<ElementType> v)
    {
        return std::sqrt(v.Aggregate([](ElementType x) { return x * x; }));
    }

    template <typename ElementType, VectorOrientation Orientation>
    void OperationsImplementation<ImplementationType::native>::Add(ElementType s, ConstVectorReference<ElementType, Orientation> v, VectorReference<ElementType, Orientation> u)
    {
        if (v.Size() != u.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectors u and v are not the same size.");
        }

        ElementType* uData = u.GetDataPointer();
        const ElementType* vData = v.GetDataPointer();
        const ElementType* end = u.GetDataPointer() + u.GetIncrement() * u.Size();

        while (uData < end)
        {
            (*uData) += s * (*vData);
            uData += u.GetIncrement();
            vData += v.GetIncrement();
        }
    }

    template <typename ElementType>
    ElementType OperationsImplementation<ImplementationType::native>::Dot(UnorientedConstVectorReference<ElementType> u, UnorientedConstVectorReference<ElementType> v)
    {
        if (v.Size() != u.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectors u and v are not the same size.");
        }

        const ElementType* uData = u.GetDataPointer();
        const ElementType* vData = v.GetDataPointer();

        ElementType result = 0;
        const ElementType* end = u.GetDataPointer() + u.GetIncrement() * u.Size();

        while (uData < end)
        {
            result += (*uData) * (*vData);
            uData += u.GetIncrement();
            vData += v.GetIncrement();
        }
        return result;
    }

    template <typename ElementType, VectorOrientation Orientation>
    void OperationsImplementation<ImplementationType::native>::Multiply(ElementType s, VectorReference<ElementType, Orientation> v)
    {
        v.Transform([s](ElementType x) { return x * s; });
    }

    template <typename ElementType>
    void OperationsImplementation<ImplementationType::native>::Multiply(ConstVectorReference<ElementType, VectorOrientation::row> u, ConstVectorReference<ElementType, VectorOrientation::column> v, ElementType& r)
    {
        r = Dot(u, v);
    }

    template <typename ElementType, MatrixLayout Layout>
    void OperationsImplementation<ImplementationType::native>::Multiply(ElementType s, ConstMatrixReference<ElementType, Layout> M, ConstVectorReference<ElementType, VectorOrientation::column> v, ElementType t, VectorReference<ElementType, VectorOrientation::column> u)
    {
        if (M.NumRows() != u.Size() || M.NumColumns() != v.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible matrix and vectors sizes.");
        }

        for (size_t i = 0; i < M.NumRows(); ++i)
        {
            auto row = M.GetRow(i);
            u[i] = s * Dot(row, v) + t * u[i];
        }
    }

#ifdef USE_BLAS
    //
    // OpenBLAS wrappers
    //

    template <typename ElementType, VectorOrientation Orientation>
    void OperationsImplementation<ImplementationType::openBlas>::Copy(ConstVectorReference<ElementType, Orientation> v, VectorReference<ElementType, Orientation> u)
    {
        Blas::Copy(static_cast<int>(u.Size()), v.GetDataPointer(), static_cast<int>(v.GetIncrement()), u.GetDataPointer(), static_cast<int>(u.GetIncrement()));
    }

    template <typename ElementType>
    ElementType OperationsImplementation<ImplementationType::openBlas>::Norm1(UnorientedConstVectorReference<ElementType> v)
    {
        return Blas::Asum(static_cast<int>(v.Size()), v.GetDataPointer(), static_cast<int>(v.GetIncrement()));
    }

    template <typename ElementType>
    ElementType OperationsImplementation<ImplementationType::openBlas>::Norm2(UnorientedConstVectorReference<ElementType> v)
    {
        return Blas::Nrm2(static_cast<int>(v.Size()), v.GetDataPointer(), static_cast<int>(v.GetIncrement()));
    }

    template <typename ElementType, VectorOrientation Orientation>
    void OperationsImplementation<ImplementationType::openBlas>::Add(ElementType s, ConstVectorReference<ElementType, Orientation> v, VectorReference<ElementType, Orientation> u)
    {
        if (v.Size() != u.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectors u and v are not the same size.");
        }

        return Blas::Axpy(static_cast<int>(u.Size()), s, v.GetDataPointer(), static_cast<int>(v.GetIncrement()), u.GetDataPointer(), static_cast<int>(u.GetIncrement()));
    }

    template <typename ElementType>
    ElementType OperationsImplementation<ImplementationType::openBlas>::Dot(UnorientedConstVectorReference<ElementType> u, UnorientedConstVectorReference<ElementType> v)
    {
        if (v.Size() != u.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "vectors u and v are not the same size.");
        }

        return Blas::Dot(static_cast<int>(u.Size()), u.GetDataPointer(), static_cast<int>(u.GetIncrement()), v.GetDataPointer(), static_cast<int>(v.GetIncrement()));
    }

    template <typename ElementType, VectorOrientation Orientation>
    void OperationsImplementation<ImplementationType::openBlas>::Multiply(ElementType s, VectorReference<ElementType, Orientation> v)
    {
        Blas::Scal(static_cast<int>(v.Size()), s, v.GetDataPointer(), static_cast<int>(v.GetIncrement()));
    }

    template <typename ElementType>
    void OperationsImplementation<ImplementationType::openBlas>::Multiply(ConstVectorReference<ElementType, VectorOrientation::row> u, ConstVectorReference<ElementType, VectorOrientation::column> v, ElementType& r)
    {
        r = Dot(u, v);
    }

    template <typename ElementType, MatrixLayout Layout>
    void OperationsImplementation<ImplementationType::openBlas>::Multiply(ElementType s, ConstMatrixReference<ElementType, Layout> M, ConstVectorReference<ElementType, VectorOrientation::column> v, ElementType t, VectorReference<ElementType, VectorOrientation::column> u)
    {
        if (M.NumRows() != u.Size() || M.NumColumns() != v.Size())
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

        Blas::Gemv(order, CBLAS_TRANSPOSE::CblasNoTrans, static_cast<int>(M.NumRows()), static_cast<int>(M.NumColumns()), s, M.GetDataPointer(), static_cast<int>(M.GetIncrement()), v.GetDataPointer(), static_cast<int>(v.GetIncrement()), t, u.GetDataPointer(), static_cast<int>(u.GetIncrement()));
    }

    template <typename ElementType, MatrixLayout Layout>
    void OperationsImplementation<ImplementationType::openBlas>::Multiply(ElementType s, ConstVectorReference<ElementType, VectorOrientation::row> v, ConstMatrixReference<ElementType, Layout> M, ElementType t, VectorReference<ElementType, VectorOrientation::row> u)
    {
        Multiply(s, M.Transpose(), v.Transpose(), t, u.Transpose());
    }
#endif
}
}