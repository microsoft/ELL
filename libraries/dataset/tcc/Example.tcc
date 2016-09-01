////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Example.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace dataset
{
    template<typename DataVectorType, typename MetadataType>
    Example<DataVectorType, MetadataType>::Example(const std::shared_ptr<DataVectorType>& dataVector, const MetadataType& metadata) : _dataVector(dataVector), _metadata(metadata)
    {}

    template<typename DataVectorType, typename MetadataType>
    void Example<DataVectorType, MetadataType>::Print(std::ostream & os) const
    {
        _metadata.Print(os);
        os << "\t";
        _dataVector->Print(os);
    }

    template<typename DataVectorType, typename MetadataType>
    std::ostream & operator<<(std::ostream & ostream, const Example<DataVectorType, MetadataType>& example)
    {
        example.Print(ostream);
        return ostream;
    }
}
