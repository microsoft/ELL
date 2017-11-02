////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MathInterface.h (interfaces)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef SWIG
// math
#include <Tensor.h>

// stl
#include <array>
#include <vector>
#include <stddef.h>

#endif

namespace ell
{
namespace api
{
namespace math
{
    struct TensorShape
    {
        size_t rows;
        size_t columns;
        size_t channels;

        /// <summary> Gets the number of elements. </summary>
        ///
        /// <returns> The number of elements in the `TensorShape` </returns>
        size_t Size() const { return rows * columns * channels; }

        TensorShape(size_t rows, size_t columns, size_t channels) :
            rows(rows), columns(columns), channels(channels) 
        {
        }

#ifndef SWIG
        ell::math::TensorShape ToMathTensorShape() const
        {
            return ell::math::TensorShape(rows, columns, channels);
        }

        static TensorShape FromMathTensorShape(const ell::math::TensorShape& shape)
        {
            return TensorShape(shape.NumRows(), shape.NumColumns(), shape.NumChannels());
        }
#endif
    };

    template<typename ElementType>
    class Tensor
    {
    public:
        Tensor(const std::vector<ElementType>& data, size_t rows, size_t columns, size_t channels) :
            shape{ rows, columns, channels }, data(data)
        {
        }

        const ell::api::math::TensorShape shape;
        std::vector<ElementType> data;
    };
}
}
}