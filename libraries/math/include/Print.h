////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Print.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Matrix.h"

// stl
#include <iostream>

namespace math
{
    template <typename ElementType, VectorOrientation Orientation>
    void Print(const ConstVectorReference<ElementType, Orientation>& v, std::ostream& os)
    {
        os << '{' << v[0];
        for (size_t i = 1; i < v.Size(); ++i)
        {
            os << '\t' << v[i];
        }
        os << '}';
    }

    template <typename ElementType, MatrixLayout Layout>
    void Print(const ConstMatrixReference<ElementType, Layout>& M, std::ostream& os)
    {
        os << '{'
        for(size_t i = 0; i < M.NumRows(); ++i)
        {
            os << "\t{" << M(i, 0);
            for(size_t j = 1; j < M.NumColumns(); ++j)
            {
                os << '\t' << M(i, j);
            }
            os << '}\n';
        }
        os << '}';
    }

}