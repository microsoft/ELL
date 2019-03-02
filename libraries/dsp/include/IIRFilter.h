////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IIRFilter.h (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <utilities/include/Archiver.h>
#include <utilities/include/IArchivable.h>
#include <utilities/include/RingBuffer.h>
#include <utilities/include/TypeName.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <vector>

namespace ell
{
namespace dsp
{
    /// <summary> A class representing an infinite impulse response (IIR) filter. </summary>
    /// The output is computed according to the equation:
    ///
    ///     a0*y[t] + a1*y[t-1] + a2*y[t-2] + ... = b0*x[t] + b1*x[t-1] + b2*x[t-2]
    /// -->
    ///     y[t] = (b0*x[t] + b1*x[t-1] + b2*x[t-2] ... - a1*y[t-1] - a2*y[t-2] + ...) / a0
    ///
    /// But for the purpose of this class the "a0" coefficient is dropped and assumed to be
    /// equal to 1.
    template <typename ValueType>
    class IIRFilter : public utilities::IArchivable
    {
    public:
        /// <summary> Construct a filter given the feedforward and recursive filter coefficients. </summary>
        ///
        /// <param name="b"> The coefficients that operate on input values (feed forward). </param>
        /// <param name="a"> The coefficients that operate on past output values, not including a0 (feedback). </param>
        IIRFilter(std::vector<ValueType> b, std::vector<ValueType> a);

        /// <summary> Filter a new input sample. <summary>
        ///
        /// <param name="x"> The new input sample to process. <param>
        ///
        /// <returns> The next output sample from the filter </returns>
        ValueType FilterSample(ValueType x);

        /// <summary> Filter a sequence of input samples. <summary>
        ///
        /// <param name="x"> The new input samples to process. <param>
        ///
        /// <returns> The next output samples from the filter </returns>
        std::vector<ValueType> FilterSamples(const std::vector<ValueType>& x);

        /// <summary> Reset the internal state of the filter to zero. </summary>
        void Reset();

        /// <summary> Accessor for the recursive coefficients. </summary>
        ///
        /// <returns> The filter coefficients applied to the output of the filter. </returns>
        std::vector<ValueType> GetRecursiveCoefficients() const { return _a; }

        /// <summary> Accessor for the feedforward coefficients. </summary>
        ///
        /// <returns> The filter coefficients applied directly to the input signal. </returns>
        std::vector<ValueType> GetFeedforwardCoefficients() const { return _b; }

        /// <summary> Gets the name of this type. </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("IIRFilter"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        utilities::RingBuffer<ValueType> _previousInput; // x[n]
        utilities::RingBuffer<ValueType> _previousOutput; // y[n]
        std::vector<ValueType> _b; // _b = {b0, b1, b2, ... }, so _b[0] = b0 = the scaling on the current input
        std::vector<ValueType> _a; // _a = {a1, a2, ... }, so _a[0] == a1 (since we never use the scaling coeff a0)
    };
} // namespace dsp
} // namespace ell

#pragma region implementation

namespace ell
{
namespace dsp
{
    template <typename ValueType>
    IIRFilter<ValueType>::IIRFilter(std::vector<ValueType> b, std::vector<ValueType> a) :
        _previousInput(b.size()),
        _previousOutput(a.size()),
        _b(b),
        _a(a)
    {
        Reset();
    }

    template <typename ValueType>
    ValueType IIRFilter<ValueType>::FilterSample(ValueType x)
    {
        _previousInput.Append(x);
        ValueType output = 0;
        assert(_b.size() == _previousInput.Size());
        assert(_a.size() == _previousOutput.Size());
        for (size_t index = 0; index < _b.size(); index++)
        {
            output += _b[index] * _previousInput[index];
        }

        if (_a.size() > 0)
        {
            for (size_t index = 0; index < _a.size(); index++)
            {
                output -= _a[index] * _previousOutput[index];
            }
            _previousOutput.Append(output);
        }
        return output;
    }

    template <typename ValueType>
    std::vector<ValueType> IIRFilter<ValueType>::FilterSamples(const std::vector<ValueType>& x)
    {
        std::vector<ValueType> result(x.size());
        std::transform(x.begin(), x.end(), result.begin(), [this](ValueType elem) {
            return FilterSample(elem);
        });
        return result;
    }

    template <typename ValueType>
    void IIRFilter<ValueType>::Reset()
    {
        _previousInput.Fill(0);
        _previousOutput.Fill(0);
    }

    template <typename ValueType>
    void IIRFilter<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        archiver["b"] << _b;
        archiver["a"] << _a;
    }

    template <typename ValueType>
    void IIRFilter<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        archiver["b"] >> _b;
        archiver["a"] >> _a;
        _previousInput.Resize(_b.size());
        _previousOutput.Resize(_a.size());
    }
} // namespace dsp
} // namespace ell

#pragma endregion implementation
