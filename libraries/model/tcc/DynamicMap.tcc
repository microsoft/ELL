////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DynamicMap.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
namespace model
{
    //
    // SetInput
    //

    // By name
    template <typename ValueType>
    void DynamicMap::SetInputValue(const std::string& inputName, const std::vector<ValueType>& inputValues) const
    {
        auto iter = _inputNodeMap.find(inputName);
        if (iter == _inputNodeMap.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }

        auto node = dynamic_cast<InputNode<ValueType>*>(iter->second);
        if (node != nullptr)
        {
            node->SetInput(inputValues);
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }
    }

    template <typename ValueType>
    void DynamicMap::SetInputValue(const std::string& inputName, const data::DenseDataVector<ValueType>& inputValues) const
    {
        auto arr = inputValues.template ToArrayT<ValueType>();
        arr.resize(GetInputSize(inputName));
        SetInputValue(inputName, arr);
    }

    // By index
    template <typename ValueType>
    void DynamicMap::SetInputValue(size_t index, const std::vector<ValueType>& inputValues) const
    {        
        if (index >= _inputNodes.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }

        auto node = dynamic_cast<InputNode<ValueType>*>(_inputNodes[index]);
        if (node != nullptr)
        {
            node->SetInput(inputValues);
        }
        else
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }
    }

    template <typename ValueType>
    void DynamicMap::SetInputValue(size_t index, const data::DenseDataVector<ValueType>& inputValues) const
    {
        auto arr = inputValues.template ToArrayT<ValueType>();
        arr.resize(GetInputSize(index));
        SetInputValue(index, arr);
    }

    //
    // ComputeOutput
    //

    // By name
    template <typename ValueType, utilities::IsFundamental<ValueType>>
    std::vector<ValueType> DynamicMap::ComputeOutput(const std::string& outputName) const
    {
        auto iter = _outputElementsMap.find(outputName);
        if (iter == _outputElementsMap.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }

        return _model.ComputeOutput<ValueType>(iter->second);
    }

    template <typename VectorType, typename ValueType>
    VectorType DynamicMap::ComputeOutput(const std::string& outputName) const
    {
        return VectorType{ ComputeOutput<ValueType>(outputName) };
    }

    // By index
    template <typename ValueType, utilities::IsFundamental<ValueType>>
    std::vector<ValueType> DynamicMap::ComputeOutput(size_t index) const
    {
        if (index >= _outputElements.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }

        return _model.ComputeOutput<ValueType>(_outputElements[index]);
    }

    template <typename VectorType, typename ValueType>
    VectorType DynamicMap::ComputeOutput(size_t index) const
    {
        return VectorType{ ComputeOutput<ValueType>(index) };
    }
}
}
