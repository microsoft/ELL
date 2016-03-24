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
    class SupervisedExample 
    {
    public:

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
        void Reset();

        /// <summary> Inherited via IDataVector. </summary>
        ///
        /// <param name="index"> Zero-based index of the. </param>
        /// <param name="value"> The value. </param>
        void PushBack(uint64 index, double value = 1.0);

        /// <summary> Inherited via IDataVector. </summary>
        ///
        /// <returns> An uint64. </returns>
        uint64 Size() const;

        /// <summary> \returns The number of non-zeros. </summary>
        ///
        /// <returns> The total number of nonzeros. </returns>
        uint64 NumNonzeros() const;

        /// <summary> Inherited via IDataVector. </summary>
        ///
        /// <returns> A double. </returns>
        double Norm2() const;

        /// <summary> Adds a weighted version of this vector into another vector. </summary>
        ///
        /// <param name="p_other"> [in,out] The other vector. </param>
        /// <param name="scalar"> The weight. </param>
        void AddTo(double * p_other, double scalar = 1.0) const;

        /// <summary> Adds a weighted version of this vector into another vector. </summary>
        ///
        /// <param name="other"> [in,out] The other vector. </param>
        /// <param name="scalar"> The weight. </param>
        void AddTo(std::vector<double>& other, double scalar = 1.0) const;

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="p_other"> The other vector. </param>
        ///
        /// <returns> The result. </returns>
        double Dot(const double * p_other) const;

        /// <summary> Computes the dot product with another vector. </summary>
        ///
        /// <param name="p_other"> The other vector. </param>
        ///
        /// <returns> The result. </returns>
        double Dot(const std::vector<double>& other) const;

        /// <summary> Prints the datavector to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        void Print(std::ostream& os) const;

    private:
        std::unique_ptr<IDataVector> _upInstance;
        double _weight;
        double _label;
    };

    /// <summary> Stream insertion operator. </summary>
    ///
    /// <param name="ostream"> [in,out] Stream to write data to. </param>
    /// <param name="example"> The example. </param>
    ///
    /// <returns> The shifted ostream. </returns>
    std::ostream& operator<<(std::ostream& ostream, const SupervisedExample& example);
}
