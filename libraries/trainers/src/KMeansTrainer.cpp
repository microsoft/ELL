////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     KMeansTrainer.cpp (trainers)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "KMeansTrainer.h"

// math
#include "Operations.h"

// stl
#include <cassert>
#include <cmath>

namespace ell
{
namespace trainers
{
    KMeansTrainer::KMeansTrainer(size_t dim, size_t numClusters, size_t iterations)
        : _means(dim, numClusters), _numClusters(numClusters), _iterations(iterations), _isInitialized(false) {}

    KMeansTrainer::KMeansTrainer(size_t numClusters, size_t iters, math::ColumnMatrix<double> means)
        : _numClusters(numClusters), _iterations(iters), _means(means), _isInitialized(true) {}

    void KMeansTrainer::RunKMeans(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> X)
    {
        if (false == _isInitialized)
            initializeMeans(X);

        math::ColumnVector<size_t> clusterAssignment(X.NumColumns());
        double prevDistance = 0.0;
        for (int i = 0; i < _iterations; ++i)
        {
            auto totalDistance = assignClosestCenter(X, clusterAssignment);
            if (totalDistance == prevDistance)
                break;
            recomputeMeans(X, clusterAssignment);
            prevDistance = totalDistance;
        }
    }

    void KMeansTrainer::initializeMeans(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> X)
    {
        size_t N = X.NumColumns();
        size_t K = _numClusters;
        size_t choice = rand() % N;

        _means.GetColumn(0).CopyFrom(X.GetColumn(choice));

        math::ColumnVector<double> minimumDistance(X.NumColumns());
        for (int k = 1; k < _numClusters; ++k)
        {
            // distance to previously selected mean
            auto D = pairwiseDistance(X, _means.GetSubMatrix(0, k - 1, _means.NumRows(), 1));
            auto distanceToPreviousMean = D.GetColumn(0);

            if (k == 1)
            {
                minimumDistance.CopyFrom(distanceToPreviousMean);
            }
            else
            {
                // distance to closest center
                for (int i = 0; i < minimumDistance.Size(); ++i)
                    minimumDistance[i] = std::min(minimumDistance[i], distanceToPreviousMean[i]);
            }

            choice = weightedSample(minimumDistance);
            _means.GetColumn(k).CopyFrom(X.GetColumn(choice));
        }
    }

    /// D_ij = || X_i - mu_j || ^ 2   (Distance of ith point to jth cluster)
    /// distance = ||X||^2 + ||means||^2 - 2 *  means * X'
    math::RowMatrix<double> KMeansTrainer::pairwiseDistance(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> X, math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> means)
    {
        auto n = X.NumColumns();
        auto k = means.NumColumns();

        math::ColumnMatrix<double> xSq(X.NumRows(), n);
        math::Operations::ElementWiseMultiply(X, X, xSq);

        math::ColumnMatrix<double> muSq(means.NumRows(), k);
        math::Operations::ElementWiseMultiply(means, means, muSq);

        math::ColumnMatrix<double> xSqNorm(1, xSq.NumColumns());
        math::Operations::ColumnWiseSum(xSq, xSqNorm.GetRow(0));

        math::ColumnMatrix<double> muSqNorm(1, muSq.NumColumns());
        math::Operations::ColumnWiseSum(muSq, muSqNorm.GetRow(0));

        math::RowMatrix<double> onesMultiplier(k, 1);
        onesMultiplier.Fill(1.0);
        math::RowMatrix<double> distFactor1(k, n);
        math::Operations::Multiply(1.0, onesMultiplier, xSqNorm, 0.0, distFactor1);

        math::ColumnMatrix<double> onesMultiplier1 = math::ColumnMatrix<double>(n, 1);
        onesMultiplier1.Fill(1.0);
        math::ColumnMatrix<double> distFactor2(n, k);
        math::Operations::Multiply(1.0, onesMultiplier1, muSqNorm, 0.0, distFactor2);

        math::RowMatrix<double> muX(n, k);
        math::Operations::Multiply(1.0, X.Transpose(), means, 1.0, muX);

        math::ColumnMatrix<double> tempD(n, k);
        math::Operations::Add(1.0, distFactor1.Transpose(), -2.0, muX, tempD);

        math::ColumnMatrix<double> distance(n, k);
        math::Operations::Add(1.0, tempD, 1.0, distFactor2, distance);

        return distance;
    }

    double KMeansTrainer::assignClosestCenter(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> X, math::VectorReference<size_t, math::VectorOrientation::column> clusterAssignment)
    {
        auto D = pairwiseDistance(X, _means);

        double totalDist = 0;
        for (int i = 0; i < D.NumRows(); ++i)
        {
            auto dist = D.GetRow(i);
            auto minElement = std::min_element(dist.GetDataPointer(), dist.GetDataPointer() + dist.Size());
            clusterAssignment[i] = minElement - dist.GetDataPointer();
            totalDist += *minElement;
        }

        return totalDist;
    }

    void KMeansTrainer::recomputeMeans(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> X, const math::ColumnVector<size_t> clusterAssignment)
    {
        math::ColumnMatrix<double> clusterSum(X.NumRows(), _numClusters);
        math::ColumnVector<double> numPointsPerCluster(_numClusters);
        for (int i = 0; i < X.NumColumns(); ++i)
        {
            auto idx = clusterAssignment[i];
            clusterSum.GetColumn(idx) += X.GetColumn(i);
            numPointsPerCluster[idx] += 1;
        }

        for (int i = 0; i < _numClusters; i++)
        {
            clusterSum.GetColumn(i) /= numPointsPerCluster[i];
        }

        _means.CopyFrom(clusterSum);
    }

    size_t KMeansTrainer::weightedSample(math::ColumnVector<double> weights)
    {
        size_t n = weights.Size();
        double sum = weights.Aggregate([](double x) { return x; });

        auto threshold = ((double)rand() / (RAND_MAX));
        threshold = sum * threshold;
        double cummulativeSum = 0;

        int choice = -1;

        // Select choice to be the smallest index i such that ( sum_{ j <= i } weights[j] ) >= threshold
        while (cummulativeSum < threshold)
        {
            choice += 1;
            cummulativeSum += weights[choice];
        }

        // Select an index uniformly at random if all the weights are 0
        if (-1 == choice)
            choice = rand() % weights.Size();

        return choice;
    }
}
}
