////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     DenseSupervisedExample.h (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DenseDataVector.h"
#include "SupervisedExample.h"

// stl
#include <cstdint>
#include <memory>

namespace dataset
{
    /// <summary> A supervised example. </summary>
    class DenseSupervisedExample 
    {
    public:

        /// <summary> Constructs a supervised example. </summary>
        ///
        /// <param name="instance"> The instance. </param>
        /// <param name="label"> The label. </param>
        /// <param name="weight"> The weight. </param>
        DenseSupervisedExample(DoubleDataVector instance, double label, double weight = 1.0);

        /// <summary> Constructs an instance of DenseSupervisedExample from a SupervisedExample. </summary>
        ///
        /// <param name="example"> The SupervisedExample. </param>
        /// <param name="size"> The size of the dense DataVector. </param>
        DenseSupervisedExample(const SupervisedExample& example, uint64_t size);

        /// <summary> Gets the weight. </summary>
        ///
        /// <returns> The weight. </returns>
        double GetWeight() const;

        /// <summary> Gets the label. </summary>
        ///
        /// <returns> The label. </returns>
        double GetLabel() const;

        /// <summary> Gets the data vector. </summary>
        ///
        /// <returns> The data vector. </returns>
        const DoubleDataVector& GetDataVector() const;

        /// <summary> Prints the datavector to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        void Print(std::ostream& os) const;

    private:
        DoubleDataVector _dataVector;
        double _weight;
        double _label;
    };

    /// <summary> Stream insertion operator. </summary>
    ///
    /// <param name="ostream"> [in,out] Stream to write data to. </param>
    /// <param name="example"> The example. </param>
    ///
    /// <returns> The shifted ostream. </returns>
    std::ostream& operator<<(std::ostream& ostream, const DenseSupervisedExample& example);
}
