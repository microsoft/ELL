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
#include "TensorShape.h"

// stl
#include <vector>
#include <array>
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