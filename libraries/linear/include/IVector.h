// IVector.h

#pragma once

#include "types.h"

#include "Base.h"

// stl
#include <vector>

namespace linear
{
    /// Base class for infinite-dimensional vectors of double numbers. Each implementation of this class has a mathematical dimension of
    /// infinity and includes an explicitly specified prefix followed by an implicit suffix of zeros. 
    class IVector : public Base
    {
    public:

        /// \returns The Size of the std::vector
        ///
        virtual uint64 Size() const =0;

        /// Computes the std::vector squared 2-norm
        ///
        virtual double Norm2() const =0;

        /// Performs other += scalar * (*this), where other is a dense std::vector
        ///
        void AddTo(std::vector<double>& other, double scalar = 1.0) const;

        /// Performs (*p_other) += scalar * (*this), where other is a dense std::vector
        ///
        virtual void AddTo(double* p_other, double scalar = 1.0) const = 0;

        /// Computes the std::vector Dot product
        ///
        double Dot(const std::vector<double>& other) const;

        /// Computes the std::vector Dot product
        ///
        virtual double Dot(const double* p_other) const = 0;
    };
}
