////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Example.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IDataVector.h"
#include "DenseDataVector.h"
#include "WeightLabel.h"

// stl
#include <cstdint>
#include <memory>
#include <algorithm> // for std::swap

namespace dataset
{
    /// <summary> A supervised example. </summary>
    template<typename DataVectorType, typename MetaDataType>
    class Example 
    {
    public:

        Example() = default;

        explicit Example(const Example<DataVectorType, MetaDataType>& other) = default;

        Example(Example<DataVectorType, MetaDataType>&& other) = default;

        /// <summary> Constructs a supervised example. </summary>
        ///
        /// <param name="dataVector"> The data vector. </param>
        /// <param name="metaDataType"> The metadata. </param>
        Example(const std::shared_ptr<DataVectorType>& dataVector, const MetaDataType& metaData);

        /// <summary> Assignment operator. </summary>
        ///
        /// <param name="other"> The other. </param>
        ///
        /// <returns> A reference to this Example. </returns>
        Example& operator=(const Example<DataVectorType, MetaDataType>& other) = delete;

        /// <summary> Move assignment operator. </summary>
        ///
        /// <param name="other"> [in,out] The other. </param>
        ///
        /// <returns> A reference to this Example. </returns>
        Example& operator=(Example<DataVectorType, MetaDataType>&& other) = default;

        /// <summary> Gets the data vector. </summary>
        ///
        /// <returns> The data vector. </returns>
        const DataVectorType& GetDataVector() const { return *_dataVector.get(); }

        /// <summary> Gets the metadata. </summary>
        ///
        /// <returns> The metadata. </returns>
        const MetaDataType& GetMetaData() const { return _metaData; }

        /// <summary> Prints the datavector to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        void Print(std::ostream& os) const;

    private:
        std::shared_ptr<const DataVectorType> _dataVector;
        MetaDataType _metaData;
    };

    typedef Example<IDataVector, WeightLabel> GenericSupervisedExample;
    typedef Example<DoubleDataVector, WeightLabel> DenseSupervisedExample;

    /// <summary> Stream insertion operator. </summary>
    ///
    /// <param name="ostream"> [in,out] Stream to write data to. </param>
    /// <param name="example"> The example. </param>
    ///
    /// <returns> The shifted ostream. </returns>
    template<typename DataVectorType, typename MetaDataType>
    std::ostream& operator<<(std::ostream& ostream, const Example<DataVectorType, MetaDataType>& example);
}

#include "../tcc/Example.tcc"
