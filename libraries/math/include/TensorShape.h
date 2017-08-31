////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TensorShape.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

namespace ell
{
    namespace math
    {
        ///<summary> A handy struct that describes the shape of a tensor</summary>
        struct TensorShape
        {
            size_t rows;
            size_t columns;
            size_t channels;
        };
    }
}