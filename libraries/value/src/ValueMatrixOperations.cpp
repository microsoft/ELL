////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ValueMatrixOperations.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ValueMatrixOperations.h"
#include "EmitterContext.h"
#include "ValueMatrix.h"
#include "ValueScalar.h"

namespace ell
{
using namespace utilities;

namespace value
{
   Scalar Accumulate(Matrix matrix, Scalar initialValue)
   {
       Scalar result = initialValue;

       For(matrix, [&](auto row, auto column)
       {
           result += matrix(row, column);
       });

       return result;
   }

    void For(Matrix matrix, std::function<void(Scalar, Scalar)> fn)
    {
        auto layout = matrix.GetValue().GetLayout();
        if (layout.NumDimensions() != 2)
        {
            throw InputException(InputExceptionErrors::invalidArgument,
                                 "Layout being looped over must be two-dimensional");
        }

        GetContext().For(layout, [fn = std::move(fn)](std::vector<Scalar> coordinates) {
            fn(coordinates[0], coordinates[1]);
        });
    }

    Matrix GEMM(Matrix m1, Matrix m2) { throw LogicException(LogicExceptionErrors::notImplemented); }

    Vector GEMV(Matrix m, Vector v) { throw LogicException(LogicExceptionErrors::notImplemented); }

} // namespace value
} // namespace ell
