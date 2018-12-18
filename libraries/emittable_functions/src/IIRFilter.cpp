////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IIRFilterValue.cpp (emittable_functions)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IIRFilter.h"

#include <value/include/EmitterContext.h>
#include <value/include/Vector.h>

namespace ell
{
namespace emittable_functions
{
    using namespace value;

    void ShiftRight(Vector buffer, int shift)
    {
        // TODO: turn this into a real For() loop
        auto bufferSize = static_cast<int>(buffer.Size());
        for (int i = bufferSize - 1; i >= shift; --i)
        {
            buffer[i] = buffer[i - shift];
        }
    }

    void AppendToRingBuffer(Scalar value, Vector buffer)
    {
        ShiftRight(buffer, 1);
        buffer[0] = value;
    }

    Scalar FilterSample(Scalar sample, IIRFilterCoefficients filterCoeffs, Vector previousInput, Vector previousOutput)
    {
        Scalar output = 0.0;
        AppendToRingBuffer(sample, previousInput);

        output += Dot(filterCoeffs.b, previousInput);
        output -= Dot(filterCoeffs.a, previousOutput);

        AppendToRingBuffer(output, previousOutput);

        return output;
    }

    Vector FilterSamples(Vector samples, IIRFilterCoefficients filterCoeffs)
    {
        /// The output is computed according to the equation:
        ///
        ///     a0*y[t] + a1*y[t-1] + a2*y[t-2] + ... = b0*x[t] + b1*x[t-1] + b2*x[t-2]
        /// -->
        ///     y[t] = (b0*x[t] + b1*x[t-1] + b2*x[t-2] ... - a1*y[t-1] - a2*y[t-2] + ...) / a0
        ///
        /// The algorithm assumes that the filter coefficients passed in have been scaled by a0 before
        /// being passed in

        auto bSize = static_cast<int>(filterCoeffs.b.Size());
        auto aSize = static_cast<int>(filterCoeffs.a.Size());
        auto previousInput = GlobalAllocate("previousInput", ValueType::Double, utilities::MemoryShape{ bSize });
        auto previousOutput = GlobalAllocate("previousOutput", ValueType::Double, utilities::MemoryShape{ aSize });

        Vector output = MakeVector<double>(samples.Size());
        For(output, [&](Scalar index) {
            output[index] = FilterSample(samples[index], filterCoeffs, previousInput, previousOutput);
        });

        return output;
    }

} // namespace emittable_functions
} // namespace ell
