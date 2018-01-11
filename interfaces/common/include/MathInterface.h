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

// Note: this has to be outside the above #ifndef SWIG otherwise SWIG doesn't know what std::max_element is doing
// and wrongly reports "swig/python detected a memory leak of type '__int64 *', no destructor found."
#include <algorithm>

namespace ell
{
namespace api
{
namespace math
{
    // this is handy in Javascript where there is no numpy and even in Python
    // because the conversion from DoubleVector to numpy array is slow.
    template<typename ElementType>
    long Argmax(std::vector<ElementType>& buffer)
    {
        auto result = std::max_element(buffer.begin(), buffer.end());
        auto index = result - buffer.begin();
        return static_cast<long>(index);
    }

    struct TensorShape
    {
        int rows;
        int columns;
        int channels;

        /// <summary> Gets the number of elements. </summary>
        ///
        /// <returns> The number of elements in the `TensorShape` </returns>
        int Size() const { return rows * columns * channels; }

        TensorShape() : TensorShape(0, 0, 0)
        {
            // this is needed by SWIG wrappers.
        }

        TensorShape(int rows, int columns, int channels) :
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
        Tensor(const std::vector<ElementType>& data, int rows, int columns, int channels) :
            shape{ rows, columns, channels }, data(data)
        {
        }

        const ell::api::math::TensorShape shape;
        std::vector<ElementType> data;
    };
}
}
}