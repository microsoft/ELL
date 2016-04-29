////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     SupervisedExample.h (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IDataVector.h"

// linear
#include "RowMatrix.h"

// stl
#include <cstdint>
#include <memory>
#include <algorithm> // for std::swap

namespace dataset
{
    /// <summary> A supervised example. </summary>
    class SupervisedExample // : public DataRow<void>
    {
    public:
        /// <summary> Default constructors and assignment operators </summary>
        SupervisedExample() = default;
        SupervisedExample(const SupervisedExample& other); 
        SupervisedExample(SupervisedExample&& other) = default;
        
        SupervisedExample& operator=(SupervisedExample other)
        {
            swap(*this, other);
            return *this;
        }
        
        /// <summary> Constructs a supervised example. </summary>
        ///
        /// <param name="instance"> The instance. </param>
        /// <param name="label"> The label. </param>
        /// <param name="weight"> The weight. </param>
        SupervisedExample(std::unique_ptr<IDataVector> instance, double label, double weight = 1.0);

        /// <summary> Gets the data vector. </summary>
        ///
        /// <returns> The data vector. </returns>
        const IDataVector& GetDataVector() const;

        /// <summary> Gets the weight. </summary>
        ///
        /// <returns> The weight. </returns>
        double GetWeight() const;

        /// <summary> Gets the label. </summary>
        ///
        /// <returns> The label. </returns>
        double GetLabel() const;

        /// <summary> Prints the datavector to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        void Print(std::ostream& os) const;
        
        friend void swap(SupervisedExample& a, SupervisedExample &b)
        {
            using std::swap;
            swap(a._dataVector, b._dataVector);
            swap(a._label, b._label);
            swap(a._weight, b._weight);
        }
        
    private:
        std::unique_ptr<IDataVector> _dataVector; 
        double _label;
        double _weight;
    };

    /// <summary> Stream insertion operator. </summary>
    ///
    /// <param name="ostream"> [in,out] Stream to write data to. </param>
    /// <param name="example"> The example. </param>
    ///
    /// <returns> The shifted ostream. </returns>
    std::ostream& operator<<(std::ostream& ostream, const SupervisedExample& example);
}
