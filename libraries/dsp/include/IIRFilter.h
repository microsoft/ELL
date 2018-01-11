////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IIRFilter.h (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "Archiver.h"
#include "IArchivable.h"
#include "RingBuffer.h"
#include "TypeName.h"

// stl
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
    template <typename ValueType>
    class IIRFilter : public utilities::IArchivable
    {
    public:
        /// <summary> Construct a filter given the feedforward and recursive filter coefficients. </summary>
        ///
        /// <param name="b"> The feedforward coefficients for the filter. </param>
        /// <param name="a"> The recursive coefficients for the filter. </param>
        IIRFilter(std::vector<ValueType> b, std::vector<ValueType> a); // a are the coeffs on past output values (feedback), and b are the coeffs on input values (feedforward / FIR part)

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
}
}

#include "../tcc/IIRFilter.tcc"
