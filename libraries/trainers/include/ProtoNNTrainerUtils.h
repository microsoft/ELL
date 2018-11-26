////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ProtoNNTrainerUtils.h (trainers)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <data/include/Dataset.h>
#include <data/include/Example.h>

#include <math/include/Matrix.h>

#include <cstddef>
#include <memory>

namespace ell
{
namespace trainers
{
    class ProtoNNTrainerUtils
    {
    public:
        /// <summary></summary>
        static void GetDatasetAsMatrix(const data::AutoSupervisedDataset& anyDataset, math::MatrixReference<double, math::MatrixLayout::columnMajor> X, math::MatrixReference<double, math::MatrixLayout::columnMajor> Y);

        /// <summary></summary>
        template <typename math::MatrixLayout Layout>
        static math::Matrix<double, Layout> MatrixExp(math::ConstMatrixReference<double, Layout> A);

        /// <summary></summary>
        template <typename math::MatrixLayout Layout>
        static double MatrixNorm(math::ConstMatrixReference<double, Layout> A);

        /// <summary></summary>
        template <typename math::MatrixLayout Layout>
        static double MaxAbsoluteElement(math::ConstMatrixReference<double, Layout> A);

        /// <summary></summary>
        static void HardThresholding(math::MatrixReference<double, math::MatrixLayout::columnMajor> M, double sparsity);
    };
} // namespace trainers
} // namespace ell

#include "../tcc/ProtoNNTrainerUtils.tcc"
