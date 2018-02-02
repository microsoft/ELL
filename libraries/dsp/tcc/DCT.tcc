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
    // From wikipedia
    // https://en.wikipedia.org/wiki/Discrete_cosine_transform#DCT-II

    //        N-1
    // X[k] = sum x_n*cos((pi/N) * (n + 1/2)*k)
    //        n=0
    //
    // If normalized, the x_0 term gets scaled by 1/sqrt(2), and then multiply the overall result by sqrt(2/N)
    template <typename ValueType>
    math::RowMatrix<ValueType> GetDCTMatrix(size_t windowSize, size_t numFilters, bool normalize)
    {
        const auto pi = math::Constants<ValueType>::pi;
        const auto one_sqrt2 = 1.0 / std::sqrt(2.0);
        const auto scale = std::sqrt(2.0 / windowSize);
        math::RowMatrix<ValueType> result(numFilters, windowSize);
        for (size_t k = 0; k < numFilters; ++k)
        {
            for (size_t n = 0; n < windowSize; ++n)
            {
                auto x = std::cos((pi * (n + 0.5) * k) / windowSize);
                if (normalize)
                {
                    if (k == 0)
                    {
                        x *= one_sqrt2;
                    }
                    x *= scale;
                }
                result(k, n) = static_cast<ValueType>(x);
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
    math::ColumnVector<ValueType> DCT(math::ConstColumnVectorReference<ValueType> signal, size_t numFilters, bool normalize)
    {
        auto windowSize = signal.Size();
        auto dctMatrix = GetDCTMatrix<ValueType>(windowSize, numFilters);
        math::ColumnVector<ValueType> result(numFilters);
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
