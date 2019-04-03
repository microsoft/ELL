////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Softmax.cpp (emittable_functions)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LogisticFunctions.h"

#include <value/include/ComputeContext.h>
#include <value/include/EmitterContext.h>
#include <value/include/Vector.h>
#include <value/include/Scalar.h>
#include <value/include/ScalarOperations.h>

namespace ell
{
namespace emittable_functions
{
    using namespace value;

    void Softmax(value::Vector input, value::Vector output)
    {
        // softmax is invariant under translation by the same constant value in each coordinate so we can subtract
        // the maxValue from each coordinate in order to keep the sum of the exponents under control.        
        Scalar max = Max(input);

        // now compute the Euler values and the sum of those values
        Scalar sum = Allocate(input.GetType(), ell::utilities::ScalarLayout);
        sum = Cast(0, input.GetType());
        For(input, [&](Scalar index) {
            Scalar e = Exp(input(index) - max);
            sum += e;
            output(index) = e;
        });

        // now squish the output by the sum
        output /= sum;
    }

    value::Scalar Sigmoid(value::Scalar x)
    {
        Scalar zero = Cast(0, x.GetType());
        Scalar one = Cast(1, x.GetType());
        Scalar result;
        If (x > zero, [&]{
            result = one / (Exp(-x) + one);
        }).Else([&]{
            auto expInput = Exp(x);
            result = expInput / (expInput + one);
        });
        return result;
    }

    value::Scalar HardSigmoid(value::Scalar x)
    {
        // y = clip (scale*x + bias) to [0,1]

        Scalar scale = Cast(0.2, x.GetType());
        Scalar bias = Cast(0.5, x.GetType());
        Scalar result = (scale * x) + bias;

        Scalar zero = Cast(0.0, x.GetType());
        Scalar one = Cast(1.0, x.GetType());

        If(result < zero, [&] {
            result = zero;
        }).ElseIf(result > one, [&] {
            result = one;
        });
        return result;
    }

} // namespace emittable_functions
} // namespace ell
