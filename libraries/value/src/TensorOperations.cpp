////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TensorOperations.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TensorOperations.h"
#include "EmitterContext.h"
#include "Scalar.h"
#include "Tensor.h"

namespace ell
{
using namespace utilities;

namespace value
{
    Scalar Accumulate(Tensor tensor, Scalar initialValue)
    {
        Scalar result = initialValue;

        For(tensor, [&](auto row, auto column, auto channel) {
            result += tensor(row, column, channel);
        });

        return result;
    }

    void For(Tensor tensor, std::function<void(Scalar, Scalar, Scalar)> fn)
    {
        auto layout = tensor.GetValue().GetLayout();
        if (layout.NumDimensions() != 3)
        {
            throw InputException(InputExceptionErrors::invalidArgument,
                                 "Layout being looped over must be three-dimensional");
        }

        GetContext().For(layout, [fn = std::move(fn)](std::vector<Scalar> coordinates) {
            fn(coordinates[0], coordinates[1], coordinates[2]);
        });
    }

    Tensor operator+(Tensor t, Scalar s)
    {
        Tensor copy = t.Copy();
        return copy += s;
    }

    Tensor operator-(Tensor t, Scalar s)
    {
        Tensor copy = t.Copy();
        return copy -= s;
    }

    Tensor operator*(Tensor t, Scalar s)
    {
        Tensor copy = t.Copy();
        return copy *= s;
    }

    Tensor operator/(Tensor t, Scalar s)
    {
        Tensor copy = t.Copy();
        return copy /= s;
    }

} // namespace value
} // namespace ell
