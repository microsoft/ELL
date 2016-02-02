// SupervisedExample.h

#pragma once

#include "IDataVector.h"

// types
#include "types.h"

// linear
#include "RowMatrix.h"

// stl
#include <memory>

namespace dataset
{
    class SupervisedExample : public IDataVector // TODO - consider making this class not implement IDataVector
    {
    public:

        using IVector::Dot;
        using IVector::AddTo;

        /// Constructs a supervised example
        ///
        SupervisedExample(std::unique_ptr<IDataVector> instance, double label, double weight = 1.0);

        /// \returns The weight
        ///
        double GetWeight() const;

        /// \returns The label
        ///
        double GetLabel() const;

        /// Inherited via IDataVector
        ///
        virtual type GetType() const override;

        /// Inherited via IDataVector
        ///
        virtual void Reset() override;

        /// Inherited via IDataVector
        ///
        virtual void PushBack(uint64 index, double value = 1.0) override;

        /// Inherited via IDataVector
        ///
        virtual uint64 Size() const override;

        /// \returns The number of non-zeros
        ///
        virtual uint64 NumNonzeros() const override;

        /// Inherited via IDataVector
        ///
        virtual double Norm2() const override;

        /// Inherited via IDataVector
        ///
        virtual void AddTo(double * p_other, double scalar = 1.0) const override;

        /// Inherited via IDataVector
        ///
        virtual double Dot(const double * p_other) const override;

        /// Prints the datavector to an output stream
        ///
        virtual void Print(std::ostream& os) const override;

    private:
        std::unique_ptr<IDataVector> _upInstance;
        double _weight;
        double _label;
    };

    using RowDataset = linear::RowMatrix<SupervisedExample>;
}
