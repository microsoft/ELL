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