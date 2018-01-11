////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DCT.tcc (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace dsp
{
    template <typename ValueType>
    math::RowMatrix<ValueType> GetDCTMatrix(size_t size)
    {
        const auto pi = math::Constants<ValueType>::pi;
        math::RowMatrix<ValueType> result(size, size);
        for (size_t k = 0; k < size; ++k)
        {
            for (size_t n = 0; n < size; ++n)
            {
                result(k, n) = std::cos(pi / size * (n + 0.5) * k);
            }
        }
        return result;
    }

    template <typename ValueType>
    math::ColumnVector<ValueType> DCT(math::ConstRowMatrixReference<ValueType> dctMatrix, math::ConstColumnVectorReference<ValueType> signal, bool normalize)
    {
        math::ColumnVector<ValueType> result(signal.Size());
        if (normalize)
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }
        else
        {
            math::MultiplyScaleAddUpdate(static_cast<ValueType>(1.0), dctMatrix, signal, static_cast<ValueType>(0.0), result);
        }
        return result;
    }

    template <typename ValueType>
    math::ColumnVector<ValueType> DCT(math::ConstColumnVectorReference<ValueType> signal, bool normalize)
    {
        auto size = signal.Size();
        auto dctMatrix = GetDCTMatrix<ValueType>(size);
        math::ColumnVector<ValueType> result(size);
        if (normalize)
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
        }
        else
        {
            math::MultiplyScaleAddUpdate(static_cast<ValueType>(1.0), dctMatrix, signal, static_cast<ValueType>(0.0), result);
        }
        return result;
    }

} // end namespace dsp
}
