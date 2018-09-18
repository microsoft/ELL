////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelTransformer.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    //
    // ModelTransformer
    //
    template <typename ValueType>
    const OutputPort<ValueType>& ModelTransformer::GetCorrespondingInputs(const InputPort<ValueType>& port) const
    {
        return static_cast<const OutputPort<ValueType>&>(_elementsMap.GetCorrespondingPort(port.GetReferencedPort()));
    }

    template <typename ValueType>
    const OutputPort<ValueType>& ModelTransformer::GetCorrespondingOutputs(const OutputPort<ValueType>& port) const
    {
        const auto& result = _elementsMap.GetCorrespondingPort(port);
        return static_cast<const OutputPort<ValueType>&>(result);
    }

    template <typename ValueType>
    const OutputPort<ValueType>& ModelTransformer::GetCorrespondingOutputs(const PortElements<ValueType>& elements) const
    {
        if (!elements.IsFullPortOutput())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "ModelTransformer::GetCorrespondingOutputs(): Invalid PortElements");
        }
        return static_cast<const OutputPort<ValueType>&>(GetCorrespondingOutputs(*elements.GetRanges()[0].ReferencedPort()));
    }

    template <typename NodeType>
    NodeType* ModelTransformer::GetCorrespondingInputNodeAs(const NodeType* inputNode) const
    {
        const auto& newNodeOutputs = GetCorrespondingOutputs(inputNode->GetOutputPort());
        auto newNodeConst = newNodeOutputs.GetNode();
        auto newInputNodeConst = dynamic_cast<const NodeType*>(newNodeConst);
        assert(newInputNodeConst != nullptr);
        auto newInputNode = const_cast<NodeType*>(newInputNodeConst);
        return newInputNode;
    }

    template <typename ValueType>
    InputNode<ValueType>* ModelTransformer::GetCorrespondingInputNode(const InputNode<ValueType>* inputNode) const
    {
        return GetCorrespondingInputNodeAs(inputNode);
    }

    template <typename NodeType, typename... Args>
    NodeType* ModelTransformer::AddNode(Args&&... args)
    {
        auto newNode = _model.AddNode<NodeType>(std::forward<Args>(args)...);
        _isModelCompilable &= _context.IsNodeCompilable(*newNode);
        return newNode;
    }

    template <typename ValueType>
    void ModelTransformer::MapNodeOutput(const OutputPort<ValueType>& oldPort, const OutputPortBase& newPort)
    {
        _elementsMap.MapNodeOutput(&oldPort, newPort);
    }

    template <typename ValueType>
    void ModelTransformer::MapNodeOutput(const OutputPort<ValueType>& oldPort, const OutputPort<ValueType>& newPort)
    {
        _elementsMap.MapNodeOutput(&oldPort, newPort);
    }

    template <typename ValueType>
    void ModelTransformer::MapNodeOutput(const OutputPort<ValueType>& oldPort, const PortElements<ValueType>& newElements)
    {
        const auto& newPort = _model.AddRoutingNodes(newElements);
        _elementsMap.MapNodeOutput(&oldPort, newPort);
    }
}
}
