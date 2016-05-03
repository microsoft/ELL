////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  EMLL
//  File:     SupervisedExample.tcc (dataset)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace dataset
{
    template<typename DataVectorType>
    SupervisedExample<DataVectorType>::SupervisedExample(const SupervisedExample<DataVectorType>& other) : _dataVector(other._dataVector->Clone()), _label(other._label), _weight(other._weight) {}
    
    template<typename DataVectorType>
    SupervisedExample<DataVectorType>::SupervisedExample(std::unique_ptr<DataVectorType> instance, double label, double weight) : _dataVector(std::move(instance)), _label(label), _weight(weight)
    {}

    template<typename DataVectorType>
    const DataVectorType& SupervisedExample<DataVectorType>::GetDataVector() const
    {
        return *_dataVector.get();
    }

    template<typename DataVectorType>
    double SupervisedExample<DataVectorType>::GetWeight() const
    {
        return _weight;
    }

    template<typename DataVectorType>
    double SupervisedExample<DataVectorType>::GetLabel() const
    {
        return _label;
    }

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
