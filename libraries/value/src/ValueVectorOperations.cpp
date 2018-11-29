////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueVectorOperations.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ValueVectorOperations.h"
#include "EmitterContext.h"
#include "ValueScalar.h"
#include "ValueVector.h"

namespace ell
{
using namespace utilities;

namespace value
{
    Scalar Accumulate(Vector input, Scalar initalValue)
    {
        Scalar result = initalValue;

        For(input, [&](auto index) { result += input(index); });

        return result;
    }

    Scalar Dot(Vector v1, Vector v2)
    {
        if (v1.Size() != v2.Size())
        {
            throw InputException(InputExceptionErrors::sizeMismatch);
        }
        if (v1.GetType() != v2.GetType())
        {
            throw InputException(InputExceptionErrors::typeMismatch);
        }

        Scalar result;
        For(v1, [&](auto index) { result += v1[index] * v2[index]; });

        return result;
    }

    void For(Vector v, std::function<void(Scalar)> fn)
    {
        auto layout = v.GetValue().GetLayout();

        if (layout.NumDimensions() != 1)
        {
            throw InputException(InputExceptionErrors::invalidArgument,
                                 "Layout being looped over must be one-dimensional");
        }

        GetContext().For(layout, [fn = std::move(fn)](std::vector<Scalar> coordinates) { fn(coordinates[0]); });
    }

} // namespace value
} // namespace ell
