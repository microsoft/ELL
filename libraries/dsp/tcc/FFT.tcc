////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FFT.tcc (dsp)
//  Authors:  James Devine, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

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
            assert(!inverse);
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
            assert(!inverse);
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
    }

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
}
}
