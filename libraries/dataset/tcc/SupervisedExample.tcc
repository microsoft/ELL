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
    SupervisedExample<DataVectorType>::SupervisedExample(std::unique_ptr<DataVectorType> instance, double label, double weight) : _dataVector(std::move(instance)), _label(label), _weight(weight)
    {}

    template<typename DataVectorType>
    SupervisedExample<DataVectorType> SupervisedExample<DataVectorType>::ShallowCopy() const 
    {
        return SupervisedExample<DataVectorType>(_dataVector->Clone(), _label, _weight); // TODO make this shallow
    }

    template<typename DataVectorType>
    SupervisedExample<DataVectorType> SupervisedExample<DataVectorType>::DeepCopy() const 
    {
        return SupervisedExample<DataVectorType>(_dataVector->Clone(), _label, _weight);
    }

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
    SupervisedExample<DataVectorType>& SupervisedExample<DataVectorType>::operator=(SupervisedExample<DataVectorType> other)
    {
        Swap(*this, other);
        return *this;
    }

    template<typename DataVectorType>
    void SupervisedExample<DataVectorType>::Swap(SupervisedExample<DataVectorType>& a, SupervisedExample<DataVectorType> &b)
    {
        using std::swap;
        swap(a._dataVector, b._dataVector);
        swap(a._label, b._label);
        swap(a._weight, b._weight);
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
