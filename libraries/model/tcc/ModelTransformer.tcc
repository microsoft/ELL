////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ModelTransformer.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    template <typename ValueType>
    PortElements<ValueType> ModelTransformer::TransformPortElements(const PortElements<ValueType>& elements)
    {
        std::vector<PortElements<ValueType>> newRanges;
        for (const auto& range : elements)
        {
            auto oldPort = range.ReferencedPort();
            auto oldPortRange = PortRange{*oldPort};
            assert(_elementToElementMap.find(oldPortRange) != _elementToElementMap.end());
            if(_elementToElementMap.find(oldPortRange) == _elementToElementMap.end())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
            }
            auto newPortRange = _elementToElementMap[oldPortRange];
            auto outputPort = dynamic_cast<const OutputPort<ValueType>*>(newPortRange.ReferencedPort());
            assert(outputPort != nullptr);

            auto start = range.GetStartIndex();
            auto size = range.Size();
            newRanges.emplace_back(*outputPort, start, size);
        }
        return PortElements<ValueType>(newRanges);
    }

    template <typename ValueType>
    const OutputPort<ValueType>* ModelTransformer::GetCorrespondingOutputPort(const OutputPort<ValueType>& port)
    {
        auto correspondingPort = GetCorrespondingPort(port);
        auto result = dynamic_cast<const model::OutputPort<ValueType>*>(correspondingPort);
        assert(result != nullptr);
        return result;
    }

    template <typename ValueType>
    InputNode<ValueType>* ModelTransformer::GetCorrespondingInputNode(const InputNode<ValueType>* inputNode)
    {
        // This sucks:
        const auto inputNodeOutputPort = inputNode->GetOutputPorts()[0];
        auto newInputNodeOutputPort = GetCorrespondingPort(*inputNodeOutputPort);
        auto newInputNodeConst = dynamic_cast<const model::InputNode<ValueType>*>(newInputNodeOutputPort->GetNode());
        assert(newInputNodeConst != nullptr);
        auto newInputNode = const_cast<model::InputNode<ValueType>*>(newInputNodeConst);
        return newInputNode;
    }

    template <typename ValueType>
    void ModelTransformer::MapOutputPort(const OutputPort<ValueType>& oldPort, const OutputPort<ValueType>& newPort)
    {
        MapPort(oldPort, newPort);
    }

    template <typename NodeType, typename... Args>
    NodeType* ModelTransformer::AddNode(Args&&... args)
    {
        auto newNode = _model.AddNode<NodeType>(std::forward<Args>(args)...);
        _isModelCompilable &= _context.IsNodeCompilable(*newNode);
        return newNode;
    }
}
