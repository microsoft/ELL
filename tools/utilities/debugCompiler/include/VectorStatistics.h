////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     VectorStatistics.h (debugCompiler)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// math
#include "Tensor.h"

// stl
#include <cmath>
#include <cstring> // for size_t
#include <vector>

namespace ell
{
/// <summary> This class provides some handy statistics over vectors. </summary>
class VectorStatistics
{
public:
    VectorStatistics() = default;

    /// <summary> Create VectorStatistics from the given vector. </summary>
    template <typename ValueType>
    VectorStatistics(const std::vector<ValueType>& vec);
    
    /// <summary> Create VectorStatistics from the given tensor. </summary>
    template <typename TensorType>
    VectorStatistics(const TensorType& tensor);

    /// <summary> Return the number of elements in the vector. </summary>
    size_t NumElements() const { return _size; }

    /// <summary> Return the minimum numeric value found in the vector. </summary>
    double Min() const { return _min; }

    /// <summary> Return the maximum numeric value found in the vector. </summary>
    double Max() const { return _max; }

    /// <summary> Return the mean of the values in the vector. </summary>
    double Mean() const { return _mean; }

    /// <summary> Return the variance of the values in the vector. </summary>
    double Variance() const { return _variance; }

    /// <summary> Return the standard deviation of the values in the vector. </summary>
    double StdDev() const { return _stdDev; }

    /// <summary> Returns true if the vector is valid (has values). </summary>
    bool IsValid() const { return _valid; }

    /// <summary> Returns the sum of the difference between all the values in the given vectors. </summary>
    template <typename ValueType>
    static double Diff(const std::vector<ValueType>& vec1, const std::vector<ValueType>& vec2);

    /// <summary> Returns the sum of the difference between all the values in the given Tensors. </summary>
    template <typename TensorType>
    static double Diff(const TensorType& tensor1, const TensorType& tensor2);

private:
    template <typename ValueType>
    void Initialize(const std::vector<ValueType>& vec);

    bool _valid = false;
    size_t _size = 0;
    double _min = 0;
    double _max = 0;
    double _mean = 0;
    double _variance = 0;
    double _stdDev = 0;
};

/// <summary> Returns vec1 with element-wise subtraction of values in vec2.</summary>
template <typename ValueType>
std::vector<ValueType> Subtract(const std::vector<ValueType>& vec1, const std::vector<ValueType>& vec2);

/// <summary> Returns tensor1 with element-wise subtraction of values in tensor2.</summary>
template <typename TensorType>
ell::math::ChannelColumnRowTensor<typename TensorType::TensorElementType> Subtract(const TensorType& tensor1, const TensorType& tensor2);

/// <summary> Returns vec with each element converted to the absolute value of the original.</summary>
template <typename ValueType>
std::vector<ValueType> Abs(const std::vector<ValueType>& vec);

/// <summary> Returns tensor with each element converted to the absolute value of the original.</summary>
template <typename TensorType>
ell::math::ChannelColumnRowTensor<typename TensorType::TensorElementType> Abs(const TensorType& tensor);
}

#include "../tcc/VectorStatistics.tcc"