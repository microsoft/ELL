// SupervisedExample.h

#pragma once

#include "IDataVector.h"
using linear::IDataVector;

#include "types.h"
using linear::uint;

#include <memory>
using std::unique_ptr;

namespace dataset
{
    class SupervisedExample : public IDataVector
    {
    public:

        using IVector::Dot;
        using IVector::AddTo;

        /// Constructs a supervised example
        ///
        SupervisedExample(unique_ptr<IDataVector> instance, double label, double weight = 1.0);

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
        virtual void PushBack(uint index, double value = 1.0) override;

        /// Inherited via IDataVector
        ///
        //virtual void foreach_nonzero(std::function<void(uint, double)> func, uint index_offset = 0) const override;

        /// Inherited via IDataVector
        ///
        virtual uint Size() const override;

        /// \returns The number of non-zeros
        ///
        virtual uint NumNonzeros() const override;

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
        virtual void Print(ostream& os) const override;

    private:
        unique_ptr<IDataVector> _up_instance;
        double _weight;
        double _label;

    };

}
