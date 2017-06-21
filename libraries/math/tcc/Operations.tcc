////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Operations.tcc (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Debug.h"
#include "Exception.h"

namespace ell
{
namespace math
{
    //
    // CommonOperations
    //

    template <typename ElementType, VectorOrientation orientation>
    void CommonOperations::Add(ElementType s, VectorReference<ElementType, orientation> v)
    {
        v += s;
    }

    template <typename ElementType, MatrixLayout layout>
    void CommonOperations::Add(ElementType s, MatrixReference<ElementType, layout> M)
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
    template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void DerivedOperations<DerivedClass>::Add(ElementType s, ConstMatrixReference<ElementType, layoutA> A, ElementType t, ConstMatrixReference<ElementType, layoutB> B, MatrixReference<ElementType, layoutA> C)
    {
        DEBUG_THROW(A.NumRows() != B.NumRows() || A.NumColumns() != B.NumColumns() || B.NumRows() != C.NumRows() || B.NumColumns() != C.NumColumns(), utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible matrix sizes."));

        for (size_t i = 0; i < A.NumRows(); ++i)
        {
            DerivedClass::Add(s, A.GetRow(i), C.GetRow(i));
            DerivedClass::Add(t, B.GetRow(i), C.GetRow(i));
        }
    }

    template <class DerivedClass>
    template <typename ElementType, MatrixLayout layout>
    void DerivedOperations<DerivedClass>::Multiply(ElementType s, MatrixReference<ElementType, layout> M)
    {
        for (size_t i = 0; i < M.NumIntervals(); ++i)
        {
            DerivedClass::Multiply(s, M.GetMajorVector(i));
        }
    }

    template <class DerivedClass>
    template <typename ElementType, MatrixLayout layout>
    void DerivedOperations<DerivedClass>::Multiply(ElementType s, ConstVectorReference<ElementType, VectorOrientation::row> v, ConstMatrixReference<ElementType, layout> M, ElementType t, VectorReference<ElementType, VectorOrientation::row> u)
    {
        DerivedClass::Multiply(s, M.Transpose(), v.Transpose(), t, u.Transpose());
    }

    template <class DerivedClass>
    template <typename ElementType, VectorOrientation orientation>
    void DerivedOperations<DerivedClass>::MultiplyAdd(ElementType s, ElementType b, VectorReference<ElementType, orientation> v)
    {
        if (b == 0)
        {
            DerivedClass::Multiply(s, v);
        }
        else
        {
            v.Transform([s, b](ElementType x) { return (s*x) + b; });
        }
    }

    template <class DerivedClass>
    template <typename ElementType, MatrixLayout layout>
    void DerivedOperations<DerivedClass>::MultiplyAdd(ElementType s, ElementType b, MatrixReference<ElementType, layout> M)
    {
        if (b == 0)
        {
            DerivedClass::Multiply(s, M);
        }
        else
        {
            for (size_t i = 0; i < M.NumIntervals(); ++i)
            {
                auto interval = M.GetMajorVector(i);
                MultiplyAdd(s, b, interval);
            }
        }
    }

    template <class DerivedClass>
    template <typename ElementType, VectorOrientation orientation>
    void DerivedOperations<DerivedClass>::ElementWiseMultiply(UnorientedConstVectorReference<ElementType> u, UnorientedConstVectorReference<ElementType> v, VectorReference<ElementType, orientation> t)
    {
        DEBUG_THROW(u.Size() != v.Size() || u.Size() != t.Size(), utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible vector sizes."));

        const ElementType* uData = u.GetDataPointer();
        const ElementType* vData = v.GetDataPointer();

        size_t i = 0;
        const ElementType* end = u.GetDataPointer() + u.GetIncrement() * u.Size();

        while (uData < end)
        {
            t[i++] = (*uData) * (*vData);
            uData += u.GetIncrement();
            vData += v.GetIncrement();
        }
    }

    template <class DerivedClass>
    template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void DerivedOperations<DerivedClass>::ElementWiseMultiply(ConstMatrixReference<ElementType, layoutA> A, ConstMatrixReference<ElementType, layoutB> B, MatrixReference<ElementType, layoutA> C)
    {
        DEBUG_THROW(A.NumRows() != B.NumRows() || A.NumColumns() != B.NumColumns() || B.NumRows() != C.NumRows() || B.NumColumns() != C.NumColumns(), utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible matrix sizes."));

        for (size_t i = 0; i < A.NumRows(); ++i)
        {
            ElementWiseMultiply(A.GetRow(i), B.GetRow(i), C.GetRow(i));
        }
    }

    //
    // Native implementations of operations
    //

    template <typename ElementType, MatrixLayout layout>
    void OperationsImplementation<ImplementationType::native>::ColumnWiseSum(ConstMatrixReference<ElementType, layout> M, VectorReference<ElementType, VectorOrientation::row> u)
    {
        if (u.Size() != M.NumColumns())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible result size.");
        }

        math::RowVector<ElementType> ones(M.NumRows());
        ones.Fill(1.0);

        DerivedOperations::Multiply(static_cast<ElementType>(1), ones, M, static_cast<ElementType>(0), u);
    }

    template <typename ElementType, VectorOrientation orientation>
    void OperationsImplementation<ImplementationType::native>::Add(ElementType s, ConstVectorReference<ElementType, orientation> v, VectorReference<ElementType, orientation> u)
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

    template <typename ElementType, VectorOrientation orientation>
    void OperationsImplementation<ImplementationType::native>::Multiply(ElementType s, VectorReference<ElementType, orientation> v)
    {
        v *= s;
    }

    template <typename ElementType>
    void OperationsImplementation<ImplementationType::native>::Multiply(ConstVectorReference<ElementType, VectorOrientation::row> u, ConstVectorReference<ElementType, VectorOrientation::column> v, ElementType& r)
    {
        r = Dot(u, v);
    }

    template <typename ElementType, MatrixLayout layout>
    void OperationsImplementation<ImplementationType::native>::Multiply(ElementType s, ConstMatrixReference<ElementType, layout> M, ConstVectorReference<ElementType, VectorOrientation::column> v, ElementType t, VectorReference<ElementType, VectorOrientation::column> u)
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

    template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void OperationsImplementation<ImplementationType::native>::Multiply(ElementType s, ConstMatrixReference<ElementType, layoutA> A, ConstMatrixReference<ElementType, layoutB> B, ElementType t, MatrixReference<ElementType, layoutA> C)
    {
        if (A.NumColumns() != B.NumRows() || A.NumRows() != C.NumRows() || B.NumColumns() != C.NumColumns())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible matrix sizes.");
        }

        for (size_t i = 0; i < A.NumRows(); ++i)
        {
            for (size_t j = 0; j < B.NumColumns(); ++j)
            {
                auto row = A.GetRow(i);
                auto column = B.GetColumn(j);
                C(i, j) = s * Dot(row, column) + t * C(i, j);
            }
        }
    }

#ifdef USE_BLAS
    //
    // OpenBLAS wrappers
    //

    template <typename ElementType, MatrixLayout layout>
    void OperationsImplementation<ImplementationType::openBlas>::ColumnWiseSum(ConstMatrixReference<ElementType, layout> M, VectorReference<ElementType, VectorOrientation::row> u)
    {
        if (u.Size() != M.NumColumns())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible result size.");
        }

        math::RowVector<ElementType> ones(M.NumRows());
        ones.Fill(1.0);

        DerivedOperations::Multiply(static_cast<ElementType>(1), ones, M, static_cast<ElementType>(0), u);
    }

    template <typename ElementType, VectorOrientation orientation>
    void OperationsImplementation<ImplementationType::openBlas>::Add(ElementType s, ConstVectorReference<ElementType, orientation> v, VectorReference<ElementType, orientation> u)
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

    template <typename ElementType, VectorOrientation orientation>
    void OperationsImplementation<ImplementationType::openBlas>::Multiply(ElementType s, VectorReference<ElementType, orientation> v)
    {
        Blas::Scal(static_cast<int>(v.Size()), s, v.GetDataPointer(), static_cast<int>(v.GetIncrement()));
    }

    template <typename ElementType>
    void OperationsImplementation<ImplementationType::openBlas>::Multiply(ConstVectorReference<ElementType, VectorOrientation::row> u, ConstVectorReference<ElementType, VectorOrientation::column> v, ElementType& r)
    {
        r = Dot(u, v);
    }

    template <typename ElementType, MatrixLayout layout>
    void OperationsImplementation<ImplementationType::openBlas>::Multiply(ElementType s, ConstMatrixReference<ElementType, layout> M, ConstVectorReference<ElementType, VectorOrientation::column> v, ElementType t, VectorReference<ElementType, VectorOrientation::column> u)
    {
        if (M.NumRows() != u.Size() || M.NumColumns() != v.Size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible matrix and vectors sizes.");
        }

        // map layout to CBLAS_ORDER
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
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "layout not supported");
        }

        Blas::Gemv(order, CBLAS_TRANSPOSE::CblasNoTrans, static_cast<int>(M.NumRows()), static_cast<int>(M.NumColumns()), s, M.GetDataPointer(), static_cast<int>(M.GetIncrement()), v.GetDataPointer(), static_cast<int>(v.GetIncrement()), t, u.GetDataPointer(), static_cast<int>(u.GetIncrement()));
    }

    template <typename ElementType, MatrixLayout layout>
    void OperationsImplementation<ImplementationType::openBlas>::Multiply(ElementType s, ConstVectorReference<ElementType, VectorOrientation::row> v, ConstMatrixReference<ElementType, layout> M, ElementType t, VectorReference<ElementType, VectorOrientation::row> u)
    {
        Multiply(s, M.Transpose(), v.Transpose(), t, u.Transpose());
    }

    template <typename ElementType, MatrixLayout layoutA, MatrixLayout layoutB>
    void OperationsImplementation<ImplementationType::openBlas>::Multiply(ElementType s, ConstMatrixReference<ElementType, layoutA> A, ConstMatrixReference<ElementType, layoutB> B, ElementType t, MatrixReference<ElementType, layoutA> C)
    {
        if (A.NumColumns() != B.NumRows() || A.NumRows() != C.NumRows() || B.NumColumns() != C.NumColumns())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Incompatible matrix sizes.");
        }

        // map layout to CBLAS_ORDER
        CBLAS_ORDER order;
        switch (A.GetLayout())
        {
        case MatrixLayout::rowMajor:
            order = CBLAS_ORDER::CblasRowMajor;
            break;
        case MatrixLayout::columnMajor:
            order = CBLAS_ORDER::CblasColMajor;
            break;
        default:
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "layout not supported");
        }

        CBLAS_TRANSPOSE transposeB = CBLAS_TRANSPOSE::CblasNoTrans;
        if (A.GetLayout() != B.GetLayout())
        {
            transposeB = CBLAS_TRANSPOSE::CblasTrans;
        }

        Blas::Gemm(order, CBLAS_TRANSPOSE::CblasNoTrans, transposeB, static_cast<int>(A.NumRows()), static_cast<int>(B.NumColumns()), static_cast<int>(A.NumColumns()), s,
            A.GetDataPointer(), static_cast<int>(A.GetIncrement()), B.GetDataPointer(), static_cast<int>(B.GetIncrement()), t,
            C.GetDataPointer(), static_cast<int>(C.GetIncrement()));
    }
#endif
}
}