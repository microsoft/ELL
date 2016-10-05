////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Example.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
namespace dataset
{
    template <typename DataVectorType, typename MetadataType>
    Example<DataVectorType, MetadataType>::Example(const std::shared_ptr<const DataVectorType>& dataVector, const MetadataType& metadata)
        : _dataVector(dataVector), _metadata(metadata)
    {
    }

    template<typename DataVectorType, typename MetadataType>
    template<typename NewDataVectorType, typename NewMetadataType, utilities::IsSame<NewDataVectorType, DataVectorType> Concept>
    Example<NewDataVectorType, NewMetadataType> Example<DataVectorType, MetadataType>::ToExample() const
    {
        // shallow copy of data vector
        return Example<NewDataVectorType, NewMetadataType>(_dataVector, NewMetadataType(_metadata));
    }

    template<typename DataVectorType, typename MetadataType>
    template<typename NewDataVectorType, typename NewMetadataType, utilities::IsDifferent<NewDataVectorType, DataVectorType> Concept>
    Example<NewDataVectorType, NewMetadataType> Example<DataVectorType, MetadataType>::ToExample() const
    {
        // deep copy of data vector
        return Example<NewDataVectorType, NewMetadataType>(std::make_shared<NewDataVectorType>(_dataVector->Duplicate<NewDataVectorType>()), NewMetadataType(_metadata));
    }

    template <typename DataVectorType, typename MetadataType>
    void Example<DataVectorType, MetadataType>::Print(std::ostream& os) const
    {
        _metadata.Print(os);
        os << "\t";
        _dataVector->Print(os);
    }

    template <typename DataVectorType, typename MetadataType>
    std::ostream& operator<<(std::ostream& ostream, const Example<DataVectorType, MetadataType>& example)
    {
        example.Print(ostream);
        return ostream;
    }
}
}
