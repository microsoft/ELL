////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Map.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    namespace MapImpl
    {
        template <typename T>
        T FromDouble(double x)
        {
            return static_cast<T>(x);
        }

        template <>
        inline bool FromDouble<bool>(double x)
        {
            return x != 0;
        }
    }

    template <typename OutputType, typename InputType, utilities::IsFundamental<OutputType>, utilities::IsFundamental<InputType>>
    std::vector<OutputType> Map::Compute(const std::vector<InputType>& inputValues) const
    {
        SetInputValue(0, inputValues);
        return ComputeOutput<OutputType>(GetOutput(0));
    }

    template <typename OutputVectorType, typename InputVectorType, data::IsDataVector<OutputVectorType>, data::IsDataVector<InputVectorType>>
    OutputVectorType Map::Compute(const InputVectorType& inputValues) const
    {
        SetInputValue(GetInput(0), inputValues);
        return ComputeOutput<OutputVectorType>(GetOutput(0));
    }

    //
    // SetInput
    //
    template <typename DataVectorType, typename ElementsType, data::IsDataVector<DataVectorType>>
    void Map::SetInputValue(InputNodeBase* node, const DataVectorType& inputValues) const
    {
        auto inputSize = node->GetOutputPort().Size();
        auto inputArray = inputValues.ToArray(inputSize);
        std::vector<ElementsType> array(inputSize);
        std::transform(inputArray.begin(), inputArray.end(), array.begin(), [](auto x) { return MapImpl::FromDouble<ElementsType>(x); });
        auto typedNode = static_cast<InputNode<ElementsType>*>(node);

        SetNodeInput(typedNode, array);
    }

    template <typename DataVectorType, data::IsDataVector<DataVectorType>>
    void Map::SetInputValue(InputNodeBase* inputNode, const DataVectorType& inputValues) const
    {
        switch (inputNode->GetOutputPort().GetType())
        {
            case Port::PortType::none:
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
                break;
            case Port::PortType::smallReal:
                SetInputValue<DataVectorType, float>(inputNode, inputValues);
                break;
            case Port::PortType::real:
                SetInputValue<DataVectorType, double>(inputNode, inputValues);
                break;
            case Port::PortType::integer:
                SetInputValue<DataVectorType, int>(inputNode, inputValues);
                break;
            case Port::PortType::bigInt:
                SetInputValue<DataVectorType, int64_t>(inputNode, inputValues);
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
    void Map::SetInputValue(const std::string& inputName, const std::vector<ValueType>& inputValues) const
    {
        auto node = dynamic_cast<InputNode<ValueType>*>(GetInput(inputName));
        if (node == nullptr)
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch);
        }

        SetNodeInput(node, inputValues);
    }

    template <typename DataVectorType, data::IsDataVector<DataVectorType>>
    void Map::SetInputValue(const std::string& inputName, const DataVectorType& inputValues) const
    {
        auto node = GetInput(inputName);
        SetInputValue(node, inputValues);
    }

    // By index
    template <typename ValueType>
    void Map::SetInputValue(int index, const std::vector<ValueType>& inputValues) const
    {
        auto node = dynamic_cast<InputNode<ValueType>*>(GetInput(index));
        if (node == nullptr)
        {
            std::string nodeType = "missing InputNode<";
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, nodeType + utilities::TypeName<ValueType>::GetName() + ">");
        }

        SetNodeInput(node, inputValues);
    }

    template <typename DataVectorType, data::IsDataVector<DataVectorType>>
    void Map::SetInputValue(int index, const DataVectorType& inputValues) const
    {
        auto node = GetInput(index);
        SetInputValue(node, inputValues);
    }

    //
    // ComputeOutput
    //

    template <typename OutputDataVectorType, typename ElementsValueType, data::IsDataVector<OutputDataVectorType>>
    OutputDataVectorType Map::ComputeOutput(const PortElementsBase& elements) const
    {
        auto resultVector = ComputeOutput<ElementsValueType>(elements);
        auto resultVectorIterator = data::MakeVectorIndexValueIterator<data::IterationPolicy::skipZeros>(resultVector);
        return { resultVectorIterator };
    }

    template <typename DataVectorType, data::IsDataVector<DataVectorType>>
    DataVectorType Map::ComputeOutput(const PortElementsBase& elements) const
    {
        switch (elements.GetPortType())
        {
            case Port::PortType::none:
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
                break;
            case Port::PortType::smallReal:
                return ComputeOutput<DataVectorType, float>(elements);
                break;
            case Port::PortType::real:
                return ComputeOutput<DataVectorType, double>(elements);
                break;
            case Port::PortType::integer:
                return ComputeOutput<DataVectorType, int>(elements);
                break;
            case Port::PortType::bigInt:
                return ComputeOutput<DataVectorType, int64_t>(elements);
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
    std::vector<ValueType> Map::ComputeOutput(int index) const
    {
        return ComputeOutput<ValueType>(GetOutput(index));
    }

    template <typename DataVectorType, data::IsDataVector<DataVectorType>>
    DataVectorType Map::ComputeOutput(int index) const
    {
        return ComputeOutput<DataVectorType>(GetOutput(index));
    }

    // By name
    template <typename ValueType, utilities::IsFundamental<ValueType>>
    std::vector<ValueType> Map::ComputeOutput(const std::string& outputName) const
    {
        return ComputeOutput<ValueType>(GetOutput(outputName));
    }

    template <typename DataVectorType, data::IsDataVector<DataVectorType>>
    DataVectorType Map::ComputeOutput(const std::string& outputName) const
    {
        return ComputeOutput<DataVectorType>(GetOutput(outputName));
    }

    template <typename ValueType>
    PortElements<ValueType> Map::GetOutputElements(size_t outputIndex) const
    {
        return PortElements<ValueType>(GetOutput(outputIndex));
    }

    template <typename ValueType>
    PortElements<ValueType> Map::GetOutputElements(std::string outputName) const
    {
        return PortElements<ValueType>(GetOutput(outputName));
    }
}
}
