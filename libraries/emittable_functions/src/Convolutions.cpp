////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Convolutions.cpp (emittable_functions)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Convolutions.h"

#include <value/include/ComputeContext.h>
#include <value/include/EmitterContext.h>
#include <value/include/ValueTensor.h>

#include <iostream>

namespace ell
{
namespace emittable_functions
{
    using namespace value;

    void SimpleConvolve1D(Vector signal, Vector filter, Vector output)
    {
        For(output, [&](Scalar index) {
            Scalar accum;
            For(filter, [&](Scalar filterIndex) { accum += filter(filterIndex) * signal(index + filterIndex); });

            output(index) = accum;
        });
    }

    // Depthwise Separable using For loop where input has been explicitly padded
    void SimpleDepthwiseSeparableConvolve2D(Tensor signal,
                                            Tensor filter,
                                            Scalar rowStride,
                                            Scalar columnStride,
                                            value::Tensor output)
    {
        // Change to For once we have emittable For for 0 to Value
        For(signal.Slice(0, 0, Slice::All), [&](auto depth) {
            auto slicedInputMatrix = signal.Slice(Slice::All, Slice::All, depth);
            auto slicedOutputMatrix = output.Slice(Slice::All, Slice::All, depth);
            auto slicedFilterMatrix = filter.Slice(Slice::All, Slice::All, depth);

            For(output.Slice(Slice::All, 0, 0), [&](auto outRow) {
                For(output.Slice(0, Slice::All, 0), [&](auto outColumn) {
                    const auto inputRow = outRow * rowStride;
                    const auto inputColumn = outColumn * columnStride;

                    // Set the initial value to 0
                    slicedOutputMatrix(outRow, outColumn) = Cast(0, output.Type());
                    // Accumulate along values
                    For(filter.Slice(Slice::All, 0, 0), [&](auto filterRow) {
                        auto filterVec = slicedFilterMatrix.Row(filterRow);
                        auto inputVec = slicedInputMatrix.Row(inputRow + filterRow).SubVector(inputColumn, filter.Columns());

                        slicedOutputMatrix(outRow, outColumn) += Dot(inputVec, filterVec);
                    });
                });
            });
        });
    }
} // namespace emittable_functions
} // namespace ell
