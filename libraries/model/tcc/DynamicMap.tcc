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
    template <typename DataVectorType, typename ElementsType, data::IsDataVector<DataVectorType>>
    void DynamicMap::SetInputValue(InputNodeBase* node, const DataVectorType& inputValues) const
    {
        auto inputSize = node->GetOutputPort().Size();
        std::vector<ElementsType> arr(inputSize);
        auto iter = inputValues.GetIterator();
        while (iter.IsValid())
        {
            auto indexValue = iter.Get();
            if (indexValue.index >= inputSize)
            {
                break;
            }
            arr[indexValue.index] = static_cast<ElementsType>(indexValue.value);
            iter.Next();
        }

        auto typedNode = static_cast<InputNode<ElementsType>*>(node);
        typedNode->SetInput(arr);
    }

    template <typename DataVectorType, data::IsDataVector<DataVectorType>>
    void DynamicMap::SetInputValue(InputNodeBase* inputNode, const DataVectorType& inputValues) const
    {
        switch (inputNode->GetOutputPort().GetType())
        {
            case Port::PortType::none:
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
                break;
            case Port::PortType::real:
                SetInputValue<DataVectorType, double>(inputNode, inputValues);
                break;
            case Port::PortType::integer:
                SetInputValue<DataVectorType, int>(inputNode, inputValues);
                break;
            case Port::PortType::categorical:
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
                break;
            case Port::PortType::boolean:
                SetInputValue<DataVectorType, bool>(inputNode, inputValues);
                break;
            default:
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }
    }

    // By name
    template <typename ValueType>
    void DynamicMap::SetInputValue(const std::string& inputName, const std::vector<ValueType>& inputValues) const
    {
        auto node = dynamic_cast<InputNode<ValueType>*>(GetInput(inputName));
        if (node == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        node->SetInput(inputValues);
    }

    template <typename DataVectorType, data::IsDataVector<DataVectorType>>
    void DynamicMap::SetInputValue(const std::string& inputName, const DataVectorType& inputValues) const
    {
        auto node = GetInput(inputName);
        SetInputValue(node, inputValues);
    }

    // By index
    template <typename ValueType>
    void DynamicMap::SetInputValue(size_t index, const std::vector<ValueType>& inputValues) const
    {
        auto node = dynamic_cast<InputNode<ValueType>*>(GetInput(index));
        if (node == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        node->SetInput(inputValues);
    }

    template <typename ValueType>
    void DynamicMap::SetInputValue(int index, const std::vector<ValueType>& inputValues) const
    {
        SetInputValue(static_cast<size_t>(index), inputValues);
    }

    template <typename DataVectorType, data::IsDataVector<DataVectorType>>
    void DynamicMap::SetInputValue(size_t index, const DataVectorType& inputValues) const
    {
        auto node = GetInput(index);
        SetInputValue(node, inputValues);
    }

    template <typename DataVectorType, data::IsDataVector<DataVectorType>>
    void DynamicMap::SetInputValue(int index, const DataVectorType& inputValues) const
    {
        SetInputValue(static_cast<size_t>(index), inputValues);
    }

    //
    // ComputeOutput
    //

    template <typename OutputDataVectorType, typename ElementsValueType, data::IsDataVector<OutputDataVectorType>>
    OutputDataVectorType DynamicMap::ComputeOutput(const PortElementsBase& elements) const
    {
        auto resultVector = ComputeOutput<ElementsValueType>(elements);
        auto resultVectorIterator = linear::MakeVectorIndexValueIterator(resultVector);
        return { resultVectorIterator };
    }

    template <typename ValueType, utilities::IsFundamental<ValueType>>
    std::vector<ValueType> DynamicMap::ComputeOutput(const PortElementsBase& elements) const
    {
        return _model.ComputeOutput<ValueType>(elements);
    }

    template <typename DataVectorType, data::IsDataVector<DataVectorType>>
    DataVectorType DynamicMap::ComputeOutput(const PortElementsBase& elements) const
    {
        switch (elements.GetType())
        {
            case Port::PortType::none:
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
                break;
            case Port::PortType::real:
                return ComputeOutput<DataVectorType, double>(elements);
                break;
            case Port::PortType::integer:
                return ComputeOutput<DataVectorType, int>(elements);
                break;
            case Port::PortType::categorical:
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
                break;
            case Port::PortType::boolean:
                return ComputeOutput<DataVectorType, bool>(elements);
                break;
            default:
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
        }
    }

    // By index
    template <typename ValueType, utilities::IsFundamental<ValueType>>
    std::vector<ValueType> DynamicMap::ComputeOutput(size_t index) const
    {
        return ComputeOutput<ValueType>(GetOutput(index));
    }

    template <typename ValueType, utilities::IsFundamental<ValueType>>
    std::vector<ValueType> DynamicMap::ComputeOutput(int index) const
    {
        return ComputeOutput<ValueType>(static_cast<size_t>(index));
    }

    template <typename DataVectorType, data::IsDataVector<DataVectorType>>
    DataVectorType DynamicMap::ComputeOutput(size_t index) const
    {
        return ComputeOutput<DataVectorType>(GetOutput(index));
    }

    template <typename DataVectorType, data::IsDataVector<DataVectorType>>
    DataVectorType DynamicMap::ComputeOutput(int index) const
    {
        return ComputeOutput<DataVectorType>(static_cast<size_t>(index));
    }

    // By name
    template <typename ValueType, utilities::IsFundamental<ValueType>>
    std::vector<ValueType> DynamicMap::ComputeOutput(const std::string& outputName) const
    {
        return ComputeOutput<ValueType>(GetOutput(outputName));
    }

    template <typename DataVectorType, data::IsDataVector<DataVectorType>>
    DataVectorType DynamicMap::ComputeOutput(const std::string& outputName) const
    {
        return ComputeOutput<DataVectorType>(GetOutput(outputName));
    }
}
}
