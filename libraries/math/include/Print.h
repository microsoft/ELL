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
    /// <param name="indent"> (Optional) How many indent to print before the tensor. </param>
    template <typename ElementType, VectorOrientation Orientation>
    void Print(const ConstVectorReference<ElementType, Orientation>& v, std::ostream& os, size_t indent=0);

    /// <summary> Prints a matrix in initializer list format. </summary>
    ///
    /// <typeparam name="ElementType"> Matrix element type. </typeparam>
    /// <typeparam name="Layout"> Matrix layout. </typeparam>
    /// <param name="M"> The matrix. </param>
    /// <param name="os"> [in,out] The output stream. </param>
    /// <param name="indent"> (Optional) How many indent to print before the tensor. </param>
    template <typename ElementType, MatrixLayout Layout>
    void Print(const ConstMatrixReference<ElementType, Layout>& M, std::ostream& os, size_t indent = 0);

    /// <summary> Prints a tensor in initializer list format. </summary>
    ///
    /// <typeparam name="ElementType"> Tensor element type. </typeparam>
    /// <typeparam name="dimension0"> Tensor first dimension. </typeparam>
    /// <typeparam name="dimension1"> Tensor second dimension. </typeparam>
    /// <typeparam name="dimension2"> Tensor third dimension. </typeparam>
    /// <param name="T"> The tensor. </param>
    /// <param name="os"> [in,out] The output stream. </param>
    /// <param name="indent"> (Optional) How many indent to print before the tensor. </param>
    template <typename ElementType, Dimension dimension0, Dimension dimension1, Dimension dimension2>
    void Print(const ConstTensorReference<ElementType, dimension0, dimension1, dimension2>& T, std::ostream& os, size_t indent = 0);
}
}

#include "../tcc/Print.tcc"