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
    Example<DataVectorType, MetadataType>::Example(const DataVectorType& dataVector, const MetadataType& metadata)
        : _dataVector(std::make_shared<DataVectorType>(dataVector)), _metadata(metadata)
    {
    }

    template <typename DataVectorType, typename MetadataType>
    Example<DataVectorType, MetadataType>::Example(const std::shared_ptr<DataVectorType>& dataVector, const MetadataType& metadata)
        : _dataVector(dataVector), _metadata(metadata)
    {
    }

    template <typename DataVectorType, typename MetadataType>
    template <typename OtherDataVectorType>
    Example<DataVectorType, MetadataType>::Example(const Example<OtherDataVectorType, MetadataType>& other) : _dataVector(std::make_shared<DataVectorType>(other.GetDataVector())), _metadata(other.GetMetadata())
    {
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
