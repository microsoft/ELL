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
    template <typename ValueType>
    void DynamicMap::SetInput(const std::string& inputName, const std::vector<ValueType>& inputValues)
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
    void DynamicMap::SetInput(const std::string& inputName, const dataset::DenseDataVector<ValueType>& inputValues)
    {
        SetInput(inputName, inputValues.ToArray());
    }

    template <typename ValueType, utilities::IsFundamental<ValueType>>
    std::vector<ValueType> DynamicMap::ComputeOutput(const std::string& outputName)
    {
        auto iter = _outputElementsMap.find(outputName);
        if (iter == _outputElementsMap.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }

        return _model.ComputeOutput<ValueType>(iter->second);
    }

    template <typename VectorType, typename ValueType>
    VectorType DynamicMap::ComputeOutput(const std::string& outputName)
    {
        return VectorType{ ComputeOutput<ValueType>(outputName) };
    }
}
}
