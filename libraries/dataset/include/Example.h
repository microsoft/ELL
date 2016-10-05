////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Example.h (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DenseDataVector.h"
#include "AutoDataVector.h"

// utilities
#include "TypeTraits.h"

// stl
#include <algorithm> // for std::swap
#include <cstdint>
#include <memory>

namespace emll
{
namespace dataset
{
    /// <summary> A supervised example. </summary>
    template <typename DataVectorT, typename MetadataT>
    class Example
    {
    public:
        using DataVectorType = DataVectorT;
        using MetadataType = MetadataT;

        Example() = default;

        Example(const Example<DataVectorType, MetadataType>& other) = default;

        Example(Example<DataVectorType, MetadataType>&& other) = default;

        /// <summary> Constructs a supervised example. </summary>
        ///
        /// <param name="dataVector"> The data vector. </param>
        /// <param name="metadataType"> The metadata. </param>
        Example(const std::shared_ptr<const DataVectorType>& dataVector, const MetadataType& metadata); // TODO replace this with a converting ctor, or make it private

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

        /// <summary>
        /// Creates a new example that contains the same data as this example, in a specified data vector
        /// type and meta data type. This overload creates a shallow copy of the data vector, because the
        /// requested data vector type matches the one already stored in this example.
        /// </summary>
        ///
        /// <typeparam name="NewDataVectorType"> Requested data vector type. </typeparam>
        /// <typeparam name="NewMetadataType"> Requested metadata type (must have a ctor whose argument is the old
        /// MetadataType). </typeparam>
        /// <returns> An example. </returns>
        template<typename NewDataVectorType, typename NewMetadataType, utilities::IsSame<NewDataVectorType, DataVectorType> Concept = true>
        Example<NewDataVectorType, NewMetadataType> ToExample() const;

        /// <summary>
        /// Creates a new example that contains the same data as this example, in a specified data vector
        /// type and meta data type. This overload creates a deep copy of the data vector, because the
        /// requested data vector is different from the one stored in this example.
        /// </summary>
        ///
        /// <typeparam name="NewDataVectorType"> Requested data vector type. </typeparam>
        /// <typeparam name="NewMetadataType"> Requested metadata type (must have a ctor whose argument is the old
        /// MetadataType). </typeparam>
        /// <returns> An example. </returns>
        template<typename NewDataVectorType, typename NewMetadataType, utilities::IsDifferent<NewDataVectorType, DataVectorType> Concept = true>
        Example<NewDataVectorType, NewMetadataType> ToExample() const;

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

    typedef Example<AutoDataVector, WeightLabel> AutoSupervisedExample;
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
