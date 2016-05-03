////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     SupervisedExample.h (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IDataVector.h"

// stl
#include <cstdint>
#include <memory>
#include <algorithm> // for std::swap

namespace dataset
{
    /// <summary> A supervised example. </summary>
    template<typename DataVectorType = IDataVector>
    class SupervisedExample 
    {
    public:

        SupervisedExample() = default;

        SupervisedExample(const SupervisedExample<DataVectorType>& other);

        SupervisedExample(SupervisedExample<DataVectorType>&&) = default;

        /// <summary> Constructs a supervised example. </summary>
        ///
        /// <param name="instance"> The instance. </param>
        /// <param name="label"> The label. </param>
        /// <param name="weight"> The weight. </param>
        SupervisedExample(std::unique_ptr<DataVectorType> instance, double label, double weight = 1.0);

        /// <summary> Gets the data vector. </summary>
        ///
        /// <returns> The data vector. </returns>
        const DataVectorType& GetDataVector() const;

        /// <summary> Gets the weight. </summary>
        ///
        /// <returns> The weight. </returns>
        double GetWeight() const;

        /// <summary> Gets the label. </summary>
        ///
        /// <returns> The label. </returns>
        double GetLabel() const;

        SupervisedExample<DataVectorType>& operator=(SupervisedExample<DataVectorType> other);

        /// <summary> Prints the datavector to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        void Print(std::ostream& os) const;

    private:
        static void Swap(SupervisedExample<DataVectorType>& a, SupervisedExample<DataVectorType>& b);

        std::unique_ptr<DataVectorType> _dataVector;
        double _label;
        double _weight;
    };

    /// <summary> Stream insertion operator. </summary>
    ///
    /// <param name="ostream"> [in,out] Stream to write data to. </param>
    /// <param name="example"> The example. </param>
    ///
    /// <returns> The shifted ostream. </returns>
    template<typename DataVectorType>
    std::ostream& operator<<(std::ostream& ostream, const SupervisedExample<DataVectorType>& example);
}

#include "../tcc/SupervisedExample.tcc"
