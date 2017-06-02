////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     KMeans.h (trainers)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// stl
#include <cstddef>
#include <memory>
#include <map>

// Matrix
#include <Matrix.h>

namespace ell
{
namespace trainers
{
    /// <summary> Impements KMeans++ algorithm </summary>
    ///
    class KMeans
    {
    public:
        /// <summary> Default constructor </summary>
        ///
        KMeans() = default;

        /// <summary> Constructs an instance of KMeans trainer </summary>
        ///
        /// <param name="dim"> The input dimension. </param>
        /// <param name="K"> The number of clusters. </param>
        /// <param name="iters"> The number of iterations. </param>
        ///
        KMeans(size_t dim, size_t K, size_t iters);

        /// <summary> Constructs an instance of KMeans trainer </summary>
        ///
        /// <param name="K"> The number of clusters. </param>
        /// <param name="iters"> The number of iterations. </param>
        /// <param name="mu"> The cluster means. </param>
        ///
        KMeans(size_t K, size_t iters, math::ColumnMatrix<double> mu);

        /// <summary> Runs the KMeans algorithm. </summary>
        ///
        /// <param name="X"> The input matrix. </param>
        ///
        void RunKMeans(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> X);

        /// <summary> Returns the underlying cluster means. </summary>
        ///
        /// <returns> The underlying cluster means matrix. </returns>
        const math::ColumnMatrix<double>& GetClusterMeans() const { return _mu; }

        /// <summary> Returns the underlying cluster assignment. </summary>
        ///
        /// <returns> The underlying cluster assignment matrix. </returns>
        const math::ColumnVector<double>& GetClusterAssignment() const { return _clusterAssignment; }

    private:
        /// <summary> Initializes the cluster means using the KMeans++ strategy. </summary>
        void initializeMeans(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> X);

        /// <summary> Distance of points to all the cluster means. </summary>
        math::RowMatrix<double> pairwiseDistance(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> X, math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> mu);

        /// <summary> Assign each point to the closest mean. </summary>
        double assignClosestCenter(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> X, math::VectorReference<size_t, math::VectorOrientation::column> clusterAssignment);

        /// <summary> Recompute the cluster means. </summary>
        void recomputeMeans(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> X, const math::ColumnVector<size_t> clusterAssignment);

        /// <summary> Helper function for column sum of a matrix. </summary>
        /// This will get removed when this function gets included in operations
        math::RowMatrix<double> ColumnwiseSum(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> A);

        /// <summary> Weighted sampling. </summary>
        size_t weightedSample(math::ColumnVector<double> weights);

        /// <summary> Cluster means. </summary>
        math::ColumnMatrix<double> _mu;

        /// <summary> Are the means initialized? </summary>
        bool _isInitialized;

        /// <summary> Cluster assignment for each data point. </summary>
        math::ColumnVector<double> _clusterAssignment;

        /// <summary> Number of iterations of KMeans algorithm. </summary>
        size_t _nIters;

        /// <summary> Number of clusters. </summary>
        size_t _numClusters;
    };
}
}
