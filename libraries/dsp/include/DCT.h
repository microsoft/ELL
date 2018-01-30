////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DCT.h (dsp)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// math
#include "MathConstants.h"
#include "Matrix.h"
#include "MatrixOperations.h"
#include "Vector.h"

// utilities
#include "Exception.h"

// stl
#include <cmath>
#include <vector>

namespace ell
{
namespace dsp
{
    /// <summary> Compute the discrete cosine transform (DCT-II) coefficient matrix for a given size DCT. </summary>
    ///
    /// <param name="numFilters"> The number of DCT filters to generate --- the output dimension of a signal processed by this filter matrix. </param>
    /// <param name="windowSize"> The size of the signal to be processed. </param>
    /// <param name="normalize"> A flag indicating if the resulting DCT matrix should be orthonormal. </param>
    ///
    /// <returns> The coefficient matrix to multiply by the signal vector. </returns>
    template <typename ValueType>
    math::RowMatrix<ValueType> GetDCTMatrix(size_t numFilters, size_t windowSize, bool normalize=false);

    /// <summary> Compute the discrete cosine transform (DCT-II) of a vector of values, with an existing coefficient matrix. </summary>
    ///
    /// <param name="dctMatrix"> The DCT coefficient matrix. </param>
    /// <param name="signal"> The vector to compute the DCT of. </param>
    /// <param name="normalize"> A flag indicating if the resulting DCT matrix should be orthonormal. </param>
    ///
    /// <returns> The DCT of the input signal. </returns>
    template <typename ValueType>
    math::ColumnVector<ValueType> DCT(math::ConstRowMatrixReference<ValueType> dctMatrix, math::ConstColumnVectorReference<ValueType> signal, bool normalize = false);

    /// <summary> Compute the discrete cosine transform (DCT-II) of a vector of values. </summary>
    ///
    /// <param name="signal"> The vector to compute the DCT of. </param>
    /// <param name="normalize"> A flag indicating if the resulting DCT matrix should be orthonormal. </param>
    ///
    /// <returns> The DCT of the input signal. </returns>
    template <typename ValueType>
    math::ColumnVector<ValueType> DCT(math::ConstColumnVectorReference<ValueType> signal, bool normalize = false);
}
}

#include "../tcc/DCT.tcc"
