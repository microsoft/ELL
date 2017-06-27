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
            rows(rows), columns(columns), channels(channels), data(data)
        {

        }

        const size_t rows;
        const size_t columns;
        const size_t channels;
        std::vector<ElementType> data;
    };
}
}
}