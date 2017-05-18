////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Example.tcc (data)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace data
{
    template <typename DataVectorType, typename MetadataType>
    Example<DataVectorType, MetadataType>::Example(DataVectorType dataVector, const MetadataType& metadata)
        : _dataVector(std::make_shared<const DataVectorType>(std::move(dataVector))), _metadata(metadata)
    {
    }

    template <typename DataVectorType, typename MetadataType>
    template <typename InputDataVectorType, typename InputMetadataType, utilities::IsDifferent<InputDataVectorType, DataVectorType>, IsDataVector<InputDataVectorType>>
    Example<DataVectorType, MetadataType>::Example(const InputDataVectorType& dataVector, const InputMetadataType& metadata)
        : _dataVector(std::make_shared<const DataVectorType>(dataVector.GetIterator<IterationPolicy::skipZeros>())), _metadata(MetadataType(metadata))
    {
    }

    template <typename DataVectorType, typename MetadataType>
    Example<DataVectorType, MetadataType>::Example(const std::shared_ptr<const DataVectorType>& dataVector, const MetadataType& metadata)
        : _dataVector(dataVector), _metadata(metadata)
    {
    }

    template <typename DataVectorType, typename MetadataType>
    template <typename TargetExampleType, utilities::IsSame<typename TargetExampleType::DataVectorType, DataVectorType> Concept>
    TargetExampleType Example<DataVectorType, MetadataType>::CopyAs() const
    {
        // shallow copy of data vector
        return TargetExampleType(_dataVector, typename TargetExampleType::MetadataType(_metadata));
    }

    template <typename DataVectorType, typename MetadataType>
    template <typename TargetExampleType, utilities::IsDifferent<typename TargetExampleType::DataVectorType, DataVectorType>>
    TargetExampleType Example<DataVectorType, MetadataType>::CopyAs() const
    {
        // deep copy of data vector
        using DataType = typename TargetExampleType::DataVectorType;
        using MetadataType = typename TargetExampleType::MetadataType;
        return TargetExampleType(std::make_shared<DataType>(_dataVector->template CopyAs<DataType>()), MetadataType(_metadata));
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
