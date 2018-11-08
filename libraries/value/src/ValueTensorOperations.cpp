////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueTensorOperations.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ValueTensorOperations.h"
#include "EmitterContext.h"
#include "ValueScalar.h"
#include "ValueTensor.h"

namespace ell
{
using namespace utilities;

namespace value
{

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

} // namespace value
} // namespace ell