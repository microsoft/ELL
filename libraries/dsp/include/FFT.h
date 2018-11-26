////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FFT.h (dsp)
//  Authors:  James Devine, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <math/include/MathConstants.h>
#include <math/include/Vector.h>

#include <utilities/include/Unused.h>

#include <complex>
#include <vector>

namespace ell
{
namespace dsp
{
    /// <summary> Perform an in-place discrete ("fast") fourier transform (FFT) of a complex-valued input signal. </summary>
    ///
    /// <param name="signal"> The signal vector to process. Must be a power of 2 in length. </param>
    /// <param name="inverse"> A flag indicating if the inverse FFT should be computed instead. </param>
    template <typename ValueType>
    void FFT(std::vector<std::complex<ValueType>>& signal, bool inverse = false);

    /// <summary>
    /// Perform an in-place discrete ("fast") fourier transform (FFT) of a real-valued input signal,
    /// returning the magnitudes of the frequency bands.
    /// </summary>
    ///
    /// <param name="signal"> The signal vector to process. Must be a power of 2 in length. </param>
    /// <param name="inverse"> A flag indicating if the inverse FFT should be computed instead. </param>
    ///
    /// <remarks> The output of a real-valued FFT is symmetric, so only the first (N/2)+1 entries of the signal input are necessary </remarks>
    template <typename ValueType>
    void FFT(math::RowVector<ValueType>& signal, bool inverse = false);

    /// <summary>
    /// Perform an in-place discrete ("fast") fourier transform (FFT) of a real-valued input signal,
    /// returning the magnitudes of the frequency bands.
    /// </summary>
    ///
    /// <param name="signal"> The signal vector to process. Must be a power of 2 in length. </param>
    /// <param name="inverse"> A flag indicating if the inverse FFT should be computed instead. </param>
    ///
    /// <remarks> The output of a real-valued FFT is symmetric, so only the first (N/2)+1 entries of the signal input are necessary </remarks>
    template <typename ValueType>
    void FFT(std::vector<ValueType>& signal, bool inverse = false);
} // namespace dsp
} // namespace ell

#pragma region implementation

namespace ell
{
namespace dsp
{
    namespace detail
    {
        template <typename Iterator>
        void Deinterleave(Iterator begin, Iterator end, Iterator scratch)
        {
            const auto size = end - begin;
            const auto halfN = size / 2;
            for (int index = 0; index < halfN; ++index)
            {
                scratch[index] = begin[2 * index + 1];
                begin[index] = begin[2 * index];
            }
            for (int index = 0; index < halfN; ++index)
            {
                begin[index + halfN] = scratch[index];
            }
        }

        template <typename Iterator>
        void FFT(Iterator begin, Iterator end, Iterator scratch, bool inverse)
        {
            if (inverse)
            {
                throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented);
            }
            using ValueType = typename Iterator::value_type::value_type;
            const ValueType pi = math::Constants<ValueType>::pi;

            auto halfN = (end - begin) / 2;
            if (halfN < 1)
            {
                return; // done
            }

            Deinterleave(begin, end, scratch);
            auto evens = begin;
            auto odds = begin + halfN;

            if (halfN > 1)
            {
                FFT(evens, evens + halfN, scratch, inverse);
                FFT(odds, odds + halfN, scratch, inverse);
            }

            for (int k = 0; k < halfN; k++)
            {
                // w = e^(2*pi*k/N)
                std::complex<ValueType> w = std::exp(std::complex<ValueType>(0, pi * k / halfN));
                auto e = evens[k];
                auto o = odds[k];
                auto wo = w * o;
                evens[k] = e + wo; // even
                odds[k] = e - wo; // odd
            }
        }

        template <typename Iterator, typename ComplexIterator>
        void FFTReal(Iterator begin, Iterator end, Iterator scratch, ComplexIterator outputBegin, ComplexIterator outputEnd, bool inverse)
        {
            UNUSED(outputEnd);
            if (inverse)
            {
                throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "inverse must be false");
            }

            using ValueType = typename Iterator::value_type;
            const ValueType pi = math::Constants<ValueType>::pi;

            auto halfN = (end - begin) / 2;
            if (halfN < 1)
            {
                return; // done
            }

            Deinterleave(begin, end, scratch);

            auto evens = begin;
            auto odds = begin + halfN;
            auto complexEvens = outputBegin;
            auto complexOdds = outputBegin + halfN;

            if (halfN > 1)
            {
                FFTReal(evens, evens + halfN, scratch, complexEvens, complexEvens + halfN, inverse);
                FFTReal(odds, odds + halfN, scratch, complexOdds, complexOdds + halfN, inverse);
            }
            else
            {
                // Base case: copy from real to complex
                for (int index = 0; index < halfN; ++index)
                {
                    complexEvens[index] = evens[index];
                    complexOdds[index] = odds[index];
                }
            }

            for (int k = 0; k < halfN; k++)
            {
                // w = e^(2*pi*k/N)
                std::complex<ValueType> w = std::exp(std::complex<ValueType>(0, pi * k / halfN));
                auto e = complexEvens[k];
                auto o = complexOdds[k];
                auto wo = w * o;
                complexEvens[k] = e + wo; // even
                complexOdds[k] = e - wo; // odd
            }
        }
    } // namespace detail

    template <typename ValueType>
    void FFT(std::vector<std::complex<ValueType>>& input, bool inverse)
    {
        std::vector<std::complex<ValueType>> scratch(input.size() / 2);
        detail::FFT(std::begin(input), std::end(input), std::begin(scratch), inverse);
    }

    template <typename ValueType>
    void FFT(std::vector<ValueType>& input, bool inverse)
    {
        auto size = input.size();
        std::vector<ValueType> scratch(size / 2);
        std::vector<std::complex<ValueType>> output(size);
        detail::FFTReal(std::begin(input), std::end(input), std::begin(scratch), std::begin(output), std::end(output), inverse);
        for (size_t index = 0; index < size; ++index)
        {
            input[index] = std::abs(output[index]);
        }
    }

    template <typename ValueType>
    void FFT(math::RowVector<ValueType>& input, bool inverse)
    {
        using std::begin;
        auto size = input.Size();
        math::RowVector<ValueType> scratch(size / 2);
        std::vector<std::complex<ValueType>> output(size);
        detail::FFTReal(begin(input), end(input), begin(scratch), begin(output), end(output), inverse);
        for (size_t index = 0; index < size; ++index)
        {
            input[index] = std::abs(output[index]);
        }
    }
} // namespace dsp
} // namespace ell

#pragma endregion implementation
