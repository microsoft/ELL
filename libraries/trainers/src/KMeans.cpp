////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     KMeans.cpp (trainers)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "KMeans.h"

// stl
#include <cassert>
#include <cmath>

namespace ell
{
namespace trainers
{
    KMeans::KMeans(size_t dim, size_t K, size_t iters) : _mu(dim, K), _numClusters(K), _nIters(iters), _isInitialized(false) {	}

    KMeans::KMeans(size_t K, size_t iters, math::ColumnMatrix<double> mu) : _numClusters(K), _nIters(iters), _mu(mu), _isInitialized(true) {	}

    void KMeans::RunKMeans(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> X)
    {
        if (false == _isInitialized)
            initializeMeans(X);

        math::ColumnVector<size_t> clusterAssignment(X.NumColumns());
        double prevDistance = 0.0;
        for (int i = 0; i < _nIters; ++i)
        {
            auto totalDistance = assignClosestCenter(X, clusterAssignment);
            if (totalDistance == prevDistance)
                break;
            recomputeMeans(X, clusterAssignment);
            prevDistance = totalDistance;
        }
    }

    void KMeans::initializeMeans(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> X)
    {
        size_t N = X.NumColumns();
        size_t K = _numClusters;
        size_t choice = rand() % N;

        math::Operations::Copy(X.GetColumn(choice), _mu.GetColumn(0));

        math::ColumnVector<double> minimumDistance(X.NumColumns());
        for (int k = 1; k < _numClusters; ++k)
        {
            // distance to previously selected mean
            auto D = pairwiseDistance(X, _mu.GetSubMatrix(0, k - 1, _mu.NumRows(), 1));
            auto distanceToPreviousMean = D.GetColumn(0);

            if (k == 1)
            {
                math::Operations::Copy(distanceToPreviousMean, minimumDistance);
            }
            else
            {
                // distance to closest center
                for (int i = 0; i < minimumDistance.Size(); ++i)
                    minimumDistance[i] = std::min(minimumDistance[i], distanceToPreviousMean[i]);
            }

            choice = weightedSample(minimumDistance);
            math::Operations::Copy(X.GetColumn(choice), _mu.GetColumn(k));
        }
    }

    /// D_ij = || X_i - mu_j || ^ 2   (Distance of ith point to jth cluster)
    /// distance = ||X||^2 + ||mu||^2 - 2 *  mu * X'
    math::RowMatrix<double> KMeans::pairwiseDistance(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> X, math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> means)
    {
        auto n = X.NumColumns();
        auto k = means.NumColumns();

        math::ColumnMatrix<double> xSq(X.NumRows(), n);
        math::Operations::ElementWiseMultiply(X, X, xSq);

        math::ColumnMatrix<double> muSq(means.NumRows(), k);
        math::Operations::ElementWiseMultiply(means, means, muSq);

        auto xSqNorm = ColumnwiseSum(xSq);
        auto muSqNorm = ColumnwiseSum(muSq);

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

    double KMeans::assignClosestCenter(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> X, math::VectorReference<size_t, math::VectorOrientation::column> clusterAssignment)
    {
        auto D = pairwiseDistance(X, _mu);

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

    void KMeans::recomputeMeans(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> X, const math::ColumnVector<size_t> clusterAssignment)
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

        math::Operations::Copy(clusterSum, _mu);
    }

    math::RowMatrix<double> KMeans::ColumnwiseSum(math::ConstMatrixReference<double, math::MatrixLayout::columnMajor> A)
    {
        math::ColumnMatrix<double> R(1, A.NumColumns());
        math::ColumnMatrix<double> ones(1, A.NumRows());
        ones.Fill(1.0);

        math::Operations::Multiply(1.0, ones, A, 0.0, R);

        return std::move(R);
    }

    size_t KMeans::weightedSample(math::ColumnVector<double> weights)
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
