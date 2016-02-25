////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     SupervisedExample.h (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

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
    /// <summary> A supervised example. </summary>
    class SupervisedExample : public IDataVector // TODO - consider making this class not implement IDataVector
    {
    public:

        using IVector::Dot;
        using IVector::AddTo;

        /// <summary> Constructs a supervised example. </summary>
        ///
        /// <param name="instance"> The instance. </param>
        /// <param name="label"> The label. </param>
        /// <param name="weight"> The weight. </param>
        SupervisedExample(std::unique_ptr<IDataVector> instance, double label, double weight = 1.0);

        /// <summary> \returns The weight. </summary>
        ///
        /// <returns> The weight. </returns>
        double GetWeight() const;

        /// <summary> \returns The label. </summary>
        ///
        /// <returns> The label. </returns>
        double GetLabel() const;

        /// <summary> Inherited via IDataVector. </summary>
        ///
        /// <returns> The type. </returns>
        virtual type GetType() const override;

        /// <summary> Inherited via IDataVector. </summary>
        virtual void Reset() override;

        /// <summary> Inherited via IDataVector. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        virtual void PushBack(uint64 index, double value = 1.0) override;

        /// <summary> Inherited via IDataVector. </summary>
        ///
        /// <returns> An uint64. </returns>
        virtual uint64 Size() const override;

        /// <summary> \returns The number of non-zeros. </summary>
        ///
        /// <returns> The total number of nonzeros. </returns>
        virtual uint64 NumNonzeros() const override;

        /// <summary> Inherited via IDataVector. </summary>
        ///
        /// <returns> A double. </returns>
        virtual double Norm2() const override;

        /// <summary> Inherited via IDataVector. </summary>
        ///
        /// <param name="p_other"> [in,out] If non-null, the other. </param>
        /// <param name="scalar"> The scalar. </param>
        virtual void AddTo(double * p_other, double scalar = 1.0) const override;

        /// <summary> Inherited via IDataVector. </summary>
        ///
        /// <param name="p_other"> The other. </param>
        ///
        /// <returns> A double. </returns>
        virtual double Dot(const double * p_other) const override;

        /// <summary> Prints the datavector to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        virtual void Print(std::ostream& os) const override;

    private:
        std::unique_ptr<IDataVector> _upInstance;
        double _weight;
        double _label;
    };

    using RowDataset = linear::RowMatrix<SupervisedExample>;
}
