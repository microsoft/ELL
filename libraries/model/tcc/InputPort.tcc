////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InputPort.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    //
    // InputPortBase
    //
    template <typename ValueType>
    InputPortBase::InputPortBase(const Node* owningNode, const OutputPort<ValueType>& input, const std::string& name) :
        Port(owningNode, name, Port::GetPortType<ValueType>()),
        _referencedPort(&input)
    {
    }

    //
    // InputPort
    //
    template <typename ValueType>
    InputPort<ValueType>::InputPort() :
        InputPortBase(Port::GetPortType<ValueType>())
    {
    }

    template <typename ValueType>
    InputPort<ValueType>::InputPort(const Node* owningNode, const OutputPort<ValueType>& input, const std::string& name) :
        InputPortBase(owningNode, input, name)
    {
    }

    template <typename ValueType>
    InputPort<ValueType>& InputPort<ValueType>::operator=(const InputPort<ValueType>& other)
    {
        _referencedPort = other._referencedPort;
        return *this;
    }

    template <typename ValueType>
    std::vector<ValueType> InputPort<ValueType>::GetValue() const
    {
        if (!IsValid())
        {
            return {};
        }

        auto result = GetReferencedPort().GetOutput();

        if (Size() != result.size())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState);
        }
        return result;
    }

    template <typename ValueType>
    ValueType InputPort<ValueType>::GetValue(size_t index) const
    {
        return GetReferencedPort().GetOutput(index);
    }

    template <typename ValueType>
    ValueType InputPort<ValueType>::operator[](size_t index) const
    {
        return GetValue(index);
    }

    template <typename ValueType>
    PortElements<ValueType> InputPort<ValueType>::GetPortElements() const
    {
        if (!IsValid())
        {
            return {};
        }

        return PortElements<ValueType>{ GetReferencedPort() };
    }

    template <typename ValueType>
    const OutputPort<ValueType>& InputPort<ValueType>::GetReferencedPort() const
    {
        if (!IsValid())
        {
            throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Error: empty input port.");
        }

        return static_cast<const OutputPort<ValueType>&>(*_referencedPort);
    }

    template <typename ValueType>
    void InputPort<ValueType>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Port::WriteToArchive(archiver);
        auto portElements = PortElements<ValueType>{ GetReferencedPort() };
        archiver["input"] << portElements;
    }

    template <typename ValueType>
    void InputPort<ValueType>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Port::ReadFromArchive(archiver);
        PortElements<ValueType> input;
        archiver["input"] >> input;
        if (!input.IsFullPortOutput())
        {
            // Back-compat: if this port has a non-simple PortElements, add nodes to the model as needed to simplify.
            auto& context = archiver.GetContext();
            ModelSerializationContext& modelContext = dynamic_cast<ModelSerializationContext&>(context);
            const auto& newInput = modelContext.GetModel()->AddRoutingNodes(input);
            _referencedPort = &newInput;
        }
        else
        {
            _referencedPort = input.GetRanges()[0].ReferencedPort();
        }
    }
} // namespace model
} // namespace ell
