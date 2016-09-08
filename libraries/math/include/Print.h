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
    template <typename ElementType, MatrixLayout Layout>
    void Print(const ConstMatrixReference<ElementType, Layout>& matrix, std::ostream& os)
    {
        for(size_t i = 0; i < matrix.NumRows(); ++i)
        {
            os << matrix(i, 0);
            for(size_t j = 1; j < matrix.NumColumns(); ++j)
            {
                os << '\t' << matrix(i, j);
            }
            os << '\n';
        }
    }

}