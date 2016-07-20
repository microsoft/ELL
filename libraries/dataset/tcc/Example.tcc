////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Example.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace dataset
{
    template<typename DataVectorType, typename MetaDataType>
    Example<DataVectorType, MetaDataType>::Example(const std::shared_ptr<DataVectorType>& dataVector, const MetaDataType& metaData) : _dataVector(dataVector), _metaData(metaData)
    {}

    template<typename DataVectorType, typename MetaDataType>
    void Example<DataVectorType, MetaDataType>::Print(std::ostream & os) const
    {
        _metaData.Print(os);
        os << "\t";
        _dataVector->Print(os);
    }

    template<typename DataVectorType, typename MetaDataType>
    std::ostream & operator<<(std::ostream & ostream, const Example<DataVectorType, MetaDataType>& example)
    {
        example.Print(ostream);
        return ostream;
    }
}
