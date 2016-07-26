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
    OutputPortElements<ValueType> ModelTransformer::TransformOutputPortElements(const OutputPortElements<ValueType>& elements)
    {
        std::vector<OutputPortElements<ValueType>> newRanges;
        for (const auto& range : elements)
        {
            auto oldPort = range.ReferencedPort();
            assert(_portToPortMap.find(oldPort) != _portToPortMap.end());
            if(_portToPortMap.find(oldPort) == _portToPortMap.end())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
            }
            auto newPort = _portToPortMap[oldPort];
            auto outputPort = dynamic_cast<const OutputPort<ValueType>*>(newPort);
            assert(outputPort != nullptr);

            auto start = range.GetStartIndex();
            auto size = range.Size();
            newRanges.emplace_back(*outputPort, start, size);
        }
        return OutputPortElements<ValueType>(newRanges);
    }

    template <typename ValueType>
    const OutputPort<ValueType>* ModelTransformer::GetCorrespondingOutputPort(const OutputPort<ValueType>& port)
    {
        auto result = dynamic_cast<const model::OutputPort<double>*>(GetCorrespondingPort(port));
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
        return _model.AddNode<NodeType>(std::forward<Args>(args)...);
    }
}
