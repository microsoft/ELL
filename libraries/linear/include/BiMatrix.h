// BiMatrix.h

#pragma once

#include <vector>

namespace linear
{
    /// A matrix type that stores two identical copies of the entries, one in row major orientation and one in column major
    ///
    template<typename RowMatrixType>
    class BiMatrix : public RowMatrixType
    {
    public:
        
        /// The type of each row
        ///
        using RowType = typename RowMatrixType::RowType;

        /// Constructs a bioriented matrix
        ///
        BiMatrix() = default;

        /// Move constructor
        ///
        BiMatrix(BiMatrix<RowMatrixType>&&) = default;

        /// Deleted copy constructor
        ///
        BiMatrix(const BiMatrix<RowMatrixType>&) = delete;

        /// \returns A reference to a column
        ///
        const RowType& GetColumn(uint64 index) const;

        /// Performs a general std::vector matrix product: y = alpha * x * M + beta * y
        ///
        virtual void Gevm(const double* p_x, double* p_y, double alpha = 1.0, double beta = 0.0) const override;

    private:
        RowMatrixType _transp;
    };
}

#include "../tcc/BiMatrix.tcc"
