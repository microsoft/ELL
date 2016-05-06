////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     VectorMath.h (features)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Vector.h"

#include <vector>


namespace features
{
    inline double sqr(double x) { return x*x; }

    double VectorLength(const std::vector<double>& vec);
    double Dot(const std::vector<double>& vec1, const std::vector<double>& vec2);

    double Mean(const std::vector<double>& vec);
    double Var(const std::vector<double>& vec, double mean);
    double VectorMean(const std::vector<std::vector<double>>& vecs);
    double VectorVar(const std::vector<std::vector<double>>& vecs, double mean);
    double StdDev(const std::vector<double>& vec, double mean);
    double Rms(const std::vector<double>& vec);
    double Median(std::vector<double> vec);
    double MeanAbsDev(const std::vector<double>& vec, double reference);

    std::vector<double> PrincipalDirection(const std::vector<std::vector<double>>& data);
}

namespace vectorOps
{
    // +
    template <typename T>
    features::Vector<T> operator +(const std::vector<T>& a, const std::vector<T>& b);

    template <typename T>
    features::Vector<T> operator +(std::vector<T>&& a, const std::vector<T>& b);

    template <typename T>
    features::Vector<T> operator +(const std::vector<T>& a, std::vector<T>&& b);

    // -
    template <typename T>
    features::Vector<T> operator -(const std::vector<T>& a, const std::vector<T>& b);

    template <typename T>
    features::Vector<T> operator -(std::vector<T>&& a, const std::vector<T>& b);

    template <typename T>
    features::Vector<T> operator -(const std::vector<T>& a, std::vector<T>&& b);

    // *
    template <typename T>
    features::Vector<T> operator *(const std::vector<T>& a, double scale);

    template <typename T>
    features::Vector<T> operator *(std::vector<T>&& a, double scale);

    template <typename T>
    features::Vector<T> operator *(double scale, const std::vector<T>& a);

    template <typename T>
    features::Vector<T> operator *(double scale, std::vector<T>&& a);

    // /
    template <typename T>
    features::Vector<T> operator /(const std::vector<T>& a, double scale);

    template <typename T>
    features::Vector<T> operator /(std::vector<T>&& a, double scale);
}

#include "../tcc/VectorMath.tcc"
