////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DCT.h (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <math/include/MathConstants.h>
#include <math/include/Matrix.h>
#include <math/include/MatrixOperations.h>
#include <math/include/Vector.h>

#include <utilities/include/Exception.h>

#include <cmath>
#include <vector>

namespace ell
{
namespace dsp
{
    /// <summary> Compute the discrete cosine transform (DCT-II) coefficient matrix for a given size DCT. </summary>
    ///
    /// <param name="numFilters"> The number of DCT filters to generate --- the output dimension of a signal processed by this filter matrix. </param>
    /// <param name="windowSize"> The size of the signal to be processed. </param>
    /// <param name="normalize"> A flag indicating if the resulting DCT matrix should be orthonormal. </param>
    ///
    /// <returns> The coefficient matrix to multiply by the signal vector. </returns>
    template <typename ValueType>
    math::RowMatrix<ValueType> GetDCTMatrix(size_t numFilters, size_t windowSize, bool normalize = false);

    /// <summary> Compute the discrete cosine transform (DCT-II) of a vector of values, with an existing coefficient matrix. </summary>
    ///
    /// <param name="dctMatrix"> The DCT coefficient matrix. </param>
    /// <param name="signal"> The vector to compute the DCT of. </param>
    /// <param name="normalize"> A flag indicating if the resulting DCT matrix should be orthonormal. </param>
    ///
    /// <returns> The DCT of the input signal. </returns>
    template <typename ValueType>
    math::ColumnVector<ValueType> DCT(math::ConstRowMatrixReference<ValueType> dctMatrix, math::ConstColumnVectorReference<ValueType> signal, bool normalize = false);

    /// <summary> Compute the discrete cosine transform (DCT-II) of a vector of values. </summary>
    ///
    /// <param name="signal"> The vector to compute the DCT of. </param>
    /// <param name="normalize"> A flag indicating if the resulting DCT matrix should be orthonormal. </param>
    ///
    /// <returns> The DCT of the input signal. </returns>
    template <typename ValueType>
    math::ColumnVector<ValueType> DCT(math::ConstColumnVectorReference<ValueType> signal, bool normalize = false);
} // namespace dsp
} // namespace ell

#pragma region implementation

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
} // namespace ell

#pragma endregion implementation
