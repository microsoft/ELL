////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ProtoNNInit.h (trainers)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstddef>
#include <map>
#include <memory>

// Matrix
#include <Matrix.h>

namespace ell
{
namespace trainers
{
    class ProtoNNInit
    {
    public:
        /// <summary> Returns the underlying projection matrix. </summary>
        ///
        /// <returns> The underlying projection matrix. </returns>
        ProtoNNInit(size_t dim, size_t numLabels, size_t numPrototypesPerLabel);

        /// <summary> Returns the underlying projection matrix. </summary>
        ///
        /// <returns> The underlying projection matrix. </returns>
        void Initialize(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> WX, math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> Y);

        /// <summary> Returns the underlying projection matrix. </summary>
        ///
        /// <returns> The underlying projection matrix. </returns>
        double InitializeGamma(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> D, double gammaInit);

        /// <summary> Returns the underlying projection matrix. </summary>
        ///
        /// <returns> The underlying projection matrix. </returns>
        math::ColumnMatrix<double>& GetPrototypeMatrix() { return _B; }

        /// <summary> Returns the underlying projection matrix. </summary>
        ///
        /// <returns> The underlying projection matrix. </returns>
        math::ColumnMatrix<double>& GetLabelMatrix() { return _Z; }

    private:
        size_t _dim;

        size_t _numPrototypesPerLabel;

        // Returns the underlying projection matrix.
        math::ColumnMatrix<double> _B;

        // Returns the underlying projection matrix.
        math::ColumnMatrix<double> _Z;
    };
}
}
