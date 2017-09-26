////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     MathInterface.h (interfaces)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifndef SWIG

// stl
#include <array>
#include <vector>
#include <stddef.h>

#include "TensorShape.h"
#endif

namespace ell
{
namespace api
{
namespace math
{
    template<typename ElementType>
    class Tensor
    {
    public:
        Tensor(const std::vector<ElementType>& data, size_t rows, size_t columns, size_t channels) :
            shape{ rows, columns, channels }, data(data)
        {
        }

        const ell::math::TensorShape shape;
        std::vector<ElementType> data;
    };
}
}
}