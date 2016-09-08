////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputPort.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    //
    // InputPortBase
    //
    template <typename ValueType>
    InputPortBase::InputPortBase(const class Node* owningNode, const PortElements<ValueType>& inputs, std::string name) : Port(owningNode, name, Port::GetPortType<ValueType>()), _inputElements(inputs)
    {
    }

    inline void InputPortBase::ComputeParents()
    {
        for (const auto& range : _inputElements.GetRanges())
        {
            auto port = range.ReferencedPort();
            auto node = port->GetNode();
            _parentNodes.push_back(node);
        }
    }

    //
    // InputPort
    //
    template <typename ValueType>
    InputPort<ValueType>::InputPort() : InputPortBase(nullptr, _input, "")
    {
        ComputeParents();
    }

    template <typename ValueType>
    InputPort<ValueType>::InputPort(const class Node* owningNode, const PortElements<ValueType>& input, std::string name) : InputPortBase(owningNode, _input, name), _input(input)
    {
        ComputeParents();
    }

    template <typename ValueType>
    InputPort<ValueType>& InputPort<ValueType>::operator=(const InputPort<ValueType>& other)
    {
        _input = other._input;
        ComputeParents();
        return *this;
    }

    template <typename ValueType>
    std::vector<ValueType> InputPort<ValueType>::GetValue() const
    {
        std::vector<ValueType> result;
        size_t size = Size();
        result.reserve(size);
        for(size_t index = 0; index < size; ++index)
        {
            auto element = _input.GetElement(index);
            auto typedOutput = static_cast<const OutputPort<ValueType>*>(element.ReferencedPort());
            auto temp = typedOutput->GetOutput(element.GetIndex());
            result.push_back(temp);
        }

        if (Size() != result.size())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
        }
        return result;
    }

    template <typename ValueType>
    ValueType InputPort<ValueType>::GetValue(size_t index) const
    {
        const auto& element = _input.GetElement(index);
        auto typedOutput = static_cast<const OutputPort<ValueType>*>(element.ReferencedPort());
        return typedOutput->GetOutput(element.GetIndex());
    }

    template <typename ValueType>
    ValueType InputPort<ValueType>::operator[](size_t index) const
    {
        const auto& element = _input.GetElement(index);
        auto typedOutput = static_cast<const OutputPort<ValueType>*>(element.ReferencedPort());
        return typedOutput->GetOutput(element.GetIndex());
    }

    template <typename ValueType>
    PortElements<ValueType> InputPort<ValueType>::GetPortElements() const
    {
        return _input;
    }

    template <typename ValueType>
    void InputPort<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Port::WriteToArchive(archiver);
        archiver["input"] << _input;
    }

    template <typename ValueType>
    void InputPort<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Port::ReadFromArchive(archiver);
        archiver["input"] >> _input;
        ComputeParents();
    }  
}
