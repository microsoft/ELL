////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     VectorMath.cpp (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "VectorMath.h"

#include <algorithm>
#include <cmath>
#include <cassert>
#include <vector>

const double TwoPi = 2 * acos(-1.0);

namespace features
{
    double VectorLength(const std::vector<double>& vec)
    {
        double val = 0;
        for (const auto& x : vec)
        {
            val += x*x;
        }
        return sqrt(val);
    }

    void accum(std::vector<double>& x, const std::vector<double>& inc)
    {
        assert(x.size() == inc.size());
        const auto n = x.size();
        for (unsigned int index = 0; index < n; index++)
        {
            x[index] += inc[index];
        }
    }

    std::vector<double> scale(const std::vector<double>&a, double b)
    {
        const auto n = a.size();
        std::vector<double> result(n);
        for (unsigned int index = 0; index < n; index++)
        {
            result[index] = a[index] * b;
        }
        return result;
    }

    double Dot(const std::vector<double>& v1, const std::vector<double>& v2)
    {
        assert(v1.size() == v2.size());
        size_t n = v1.size();
        double result = 0;
        for (size_t index = 0; index < n; index++)
        {
            result += v1[index] * v2[index];
        }
        return result;
    }

    double Mean(const std::vector<double>& vec)
    {
        double sum = 0;
        for (auto x : vec)
        {
            sum += x;
        }
        return sum / vec.size();
    }

    double Var(const std::vector<double>& vec, double mean)
    {
        double var = 0;
        for (auto x : vec)
        {
            double diff = x - mean;
            var += diff*diff;
        }
        return var / vec.size();
    }

    double VectorMean(const std::vector<std::vector<double>>& vecs)
    {
        double size = 0;
        double sum = 0;
        for (const auto& vec : vecs)
        {
            for (auto x : vec)
            {
                sum += x;
            }
            size += vec.size();
        }

        return sum / size;
    }

    double VectorVar(const std::vector<std::vector<double>>& vecs, double mean)
    {
        double size = 0;
        double var = 0;
        for (const auto& vec : vecs)
        {
            for (auto x : vec)
            {
                double diff = x - mean;
                var += diff*diff;
            }
            size += vec.size();
        }
        return var / size;
    }

    double StdDev(const std::vector<double>& vec, double mean)
    {
        return sqrt(Var(vec, mean));
    }

    double Rms(const std::vector<double>& vec)
    {
        double sum = 0;
        for (auto x : vec)
        {
            sum += x*x;
        }

        return sqrt(sum / vec.size());
    }

    double Median(std::vector<double> vec)
    {
        sort(vec.begin(), vec.end());
        if (vec.size() % 2 == 1)
        {
            return vec[vec.size() / 2];
        }
        else
        {
            return (vec[vec.size() / 2] + vec[vec.size() / 2 - 1]) / 2.0;
        }
    }

    double MeanAbsDev(const std::vector<double>& vec, double reference)
    {
        double sum = 0;
        for (auto x : vec)
        {
            sum += fabs(x - reference);
        }
        return sum / vec.size();
    }

    //
    // SVD / PCA
    //
    // http://scicomp.stackexchange.com/questions/7552/algorithm-for-principal-eigenvector-of-a-real-symmetric-3x3-matrix
    // https://en.wikipedia.org/wiki/Eigenvalue_algorithm#3.C3.973_matrices

    std::vector<std::vector<double>> covariance_matrix(const std::vector<std::vector<double>>& data)
    {
        // data is a std::vector of datapoints, each of dimension d
        // result is a dxd symmetric matrix
        const auto d = data[0].size();
        std::vector<std::vector<double>> result(d);
        for (unsigned int row = 0; row < d; row++)
        {
            result[row] = std::vector<double>(d);
            for (unsigned int col = row; col < d; col++)
            {
                double val = Dot(data[row], data[col]);
                result[row][col] = val;
                if (row != col)
                {
                    result[col][row] = 0;
                }
            }
        }

        return result;
    }

    // Iterative alg:
    // https://en.wikipedia.org/wiki/Principal_component_analysis#Iterative_computation

    // data is a std::vector of length d, each is a series of values for that coordinate
    std::vector<double> PrincipalDirection(const std::vector<std::vector<double>>& data)
    {
        const auto d = data.size();
        std::vector<double> result(d, 0.5);	// TODO: init r to some random value instead of zero
        const auto num_rows = data[0].size();
        unsigned int num_iter = 20;

        std::vector<double> mean(d);
        for (unsigned int c = 0; c < d; c++)
        {
            mean[c] = Mean(data[c]);
        }

        std::vector<double> x(d);
        for (unsigned int iter = 0; iter < num_iter; iter++)
        {
            std::vector<double> s(d);
            for (int row_index = 0; row_index < num_rows; row_index++)
            {
                for (int c = 0; c < d; c++)
                {
                    x[c] = data[c][row_index] - mean[c];
                }
                // s += x*(x dot r)
                accum(s, scale(x, Dot(x, result)));
            }

            // r = normalized(s)
            double len_s = VectorLength(s);
            const double eps = 0.000001;
            if (len_s < eps)
            {
                result = { 0, 0, 0 };
            }
            else
            {
                result = scale(s, 1.0 / len_s);
            }
        }

        if (Dot(result, { 1,1,1 }) < 0)
            result = scale(result, -1.0);
        return result;
    }
}
