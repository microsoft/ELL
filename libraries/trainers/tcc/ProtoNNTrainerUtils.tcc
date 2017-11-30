////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ProtoNNTrainerUtils.h (trainers)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ProtoNNTrainerUtils.h"

// data
#include "Dataset.h"
#include "Example.h"

// stl
#include <cstddef>
#include <memory>

// file
#include <fstream>
#include <sstream>


namespace ell
{
namespace trainers
{
    void ProtoNNTrainerUtils::GetDatasetAsMatrix(const data::AutoSupervisedDataset& anyDataset, math::MatrixReference<double, math::MatrixLayout::columnMajor> X, math::MatrixReference<double, math::MatrixLayout::columnMajor> Y)
    {
        auto exampleIterator = anyDataset.GetExampleIterator();
        int colIdx = 0;
        while (exampleIterator.IsValid())
        {
            // get the Next example
            const auto& example = exampleIterator.Get();
            double label = example.GetMetadata().label;
            const auto& dataVector = example.GetDataVector().ToArray();

            for (size_t j = 0; j < dataVector.size(); j++)
            {
                X(j, colIdx) = dataVector[j];
            }

            for (size_t i = 0; i < Y.NumRows(); i++)
            {
                if (i == label)
                    Y((size_t)i, colIdx) = 1;
                else
                    Y((size_t)i, colIdx) = 0;
            }

            colIdx += 1;
            exampleIterator.Next();
        }
    }

    template <typename math::MatrixLayout Layout>
    math::Matrix<double, Layout> ProtoNNTrainerUtils::MatrixExp(math::ConstMatrixReference<double, Layout> A)
    {
        auto m = A.NumRows();
        auto n = A.NumColumns();
        math::Matrix<double, Layout> R(m, n);
        for (size_t i = 0; i < m; i++)
        {
            for (size_t j = 0; j < n; j++)
            {
                R(i, j) = std::exp(A(i, j));
            }
        }

        return R;
    }

    template <typename math::MatrixLayout Layout>
    double ProtoNNTrainerUtils::MatrixNorm(math::ConstMatrixReference<double, Layout> A)
    {
        double norm = 0.0;
        for (size_t i = 0; i < A.NumColumns(); i++)
        {
            for (size_t j = 0; j < A.NumRows(); j++)
            {
                norm += A(j, i) * A(j, i);
            }

        }

        norm = sqrt(norm);
        return norm;
    }

    template <typename math::MatrixLayout Layout>
    double ProtoNNTrainerUtils::MaxAbsoluteElement(math::ConstMatrixReference<double, Layout> A)
    {
        double max = A(0, 0);
        auto m = A.NumRows();
        auto n = A.NumColumns();
        for (size_t i = 0; i < m; i++)
        {
            for (size_t j = 0; j < n; j++)
            {
                max = std::max(max, std::abs(A(i, j)));
            }
        }

        return max;
    }

    void ProtoNNTrainerUtils::HardThresholding(math::MatrixReference<double, math::MatrixLayout::columnMajor> M, double sparsity)
    {
        assert(sparsity >= 0.0 && sparsity <= 1.0);
        if (sparsity >= 0.999)
            return;

        const double eps = 1e-8;

        std::vector<double> data;
        data.assign(M.GetDataPointer(), M.GetDataPointer() + (size_t)(M.NumRows() * M.NumColumns()));
        std::sort(data.begin(), data.end(),
            [](double i, double j) {return std::abs(i) > std::abs(j); });

        size_t mat_size = M.NumRows() * M.NumColumns();

        double thresh = std::abs(data[(size_t)((sparsity * mat_size) - 1)]);
        if (thresh <= eps)
            thresh = eps;

        for (size_t i = 0; i < M.NumColumns(); i++)
        {
            M.GetColumn(i).Transform([thresh](double x) { return  (std::abs(x) < thresh ? 0.0 : x); });
        }
    }
}
}
