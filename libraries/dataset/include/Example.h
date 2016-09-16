////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Example.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DenseDataVector.h"
#include "IDataVector.h"

// stl
#include <algorithm> // for std::swap
#include <cstdint>
#include <memory>

namespace emll
{
namespace dataset
{
    /// <summary> A supervised example. </summary>
    template <typename DataVectorType, typename MetadataType>
    class Example
    {
    public:
        Example() = default;

        Example(const Example<DataVectorType, MetadataType>& other) = default;

        Example(Example<DataVectorType, MetadataType>&& other) = default;

        /// <summary> Constructs a supervised example. </summary>
        ///
        /// <param name="dataVector"> The data vector. </param>
        /// <param name="metadataType"> The metadata. </param>
        Example(const std::shared_ptr<DataVectorType>& dataVector, const MetadataType& metadata);
        
        Example(const DataVectorType& dataVector, const MetadataType& metadata);

        template <typename OtherDataVectorType>
        Example(const Example<OtherDataVectorType, MetadataType>& other);

        /// <summary> Assignment operator. </summary>
        ///
        /// <param name="other"> The other. </param>
        ///
        /// <returns> A reference to this Example. </returns>
        Example& operator=(const Example<DataVectorType, MetadataType>& other) = delete;

        /// <summary> Move assignment operator. </summary>
        ///
        /// <param name="other"> [in,out] The other. </param>
        ///
        /// <returns> A reference to this Example. </returns>
        Example& operator=(Example<DataVectorType, MetadataType>&& other) = default;

        /// <summary> Gets the data vector. </summary>
        ///
        /// <returns> The data vector. </returns>
        const DataVectorType& GetDataVector() const { return *_dataVector.get(); }

        /// <summary> Gets the metadata. </summary>
        ///
        /// <returns> The metadata. </returns>
        MetadataType& GetMetadata() { return _metadata; }

        /// <summary> Gets the metadata. </summary>
        ///
        /// <returns> The metadata. </returns>
        const MetadataType& GetMetadata() const { return _metadata; }

        /// <summary> Prints the datavector to an output stream. </summary>
        ///
        /// <param name="os"> [in,out] Stream to write data to. </param>
        void Print(std::ostream& os) const;

    private:
        std::shared_ptr<const DataVectorType> _dataVector;
        MetadataType _metadata;
    };

    struct WeightLabel
    {
        /// <summary> Prints the weight label pair. </summary>
        ///
        /// <param name="os"> [in,out] The output stream. </param>
        void Print(std::ostream& os) const;

        double weight;
        double label;
    };

    typedef Example<IDataVector, WeightLabel> GenericSupervisedExample;
    typedef Example<DoubleDataVector, WeightLabel> DenseSupervisedExample; 

    /// <summary> Stream insertion operator. </summary>
    ///
    /// <param name="ostream"> [in,out] Stream to write data to. </param>
    /// <param name="example"> The example. </param>
    ///
    /// <returns> The shifted ostream. </returns>
    template <typename DataVectorType, typename MetadataType>
    std::ostream& operator<<(std::ostream& ostream, const Example<DataVectorType, MetadataType>& example);
}
}

#include "../tcc/Example.tcc"
