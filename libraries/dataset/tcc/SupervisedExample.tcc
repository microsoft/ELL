////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SupervisedExample.tcc (dataset)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace dataset
{
    template<typename DataVectorType>
    SupervisedExample<DataVectorType>::SupervisedExample(const std::shared_ptr<DataVectorType>& dataVector, double label, double weight) : _dataVector(dataVector), _label(label), _weight(weight)
    {}

    template<typename DataVectorType>
    void SupervisedExample<DataVectorType>::Print(std::ostream & os) const
    {
        os << _label << '\t';
        GetDataVector().Print(os);
    }

    template<typename DataVectorType>
    std::ostream & operator<<(std::ostream & ostream, const SupervisedExample<DataVectorType>& example)
    {
        example.Print(ostream);
        return ostream;
    }
}
