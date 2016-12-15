////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Print.h (math)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Matrix.h"

// stl
#include <iostream>

namespace ell
{
namespace math
{
    /// <summary> Prints a vector in initializer list format. </summary>
    ///
    /// <typeparam name="ElementType"> Vector element type. </typeparam>
    /// <typeparam name="Orientation"> Vector orientation. </typeparam>
    /// <param name="v"> The vector. </param>
    /// <param name="os"> [in,out] The output stream. </param>
    template <typename ElementType, VectorOrientation Orientation>
    void Print(const ConstVectorReference<ElementType, Orientation>& v, std::ostream& os);

    /// <summary> Prints a matrix in initializer list format. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="Layout"> Matrix layout. </typeparam>
    /// <param name="M"> The matrix. </param>
    /// <param name="os"> [in,out] The output stream. </param>
    template <typename ElementType, MatrixLayout Layout>
    void Print(const ConstMatrixReference<ElementType, Layout>& M, std::ostream& os);
}
}

#include "../tcc/Print.tcc"