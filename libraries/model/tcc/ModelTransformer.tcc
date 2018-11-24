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
    const OutputPort<ValueType>& ModelTransformer::TransformSubmodelOnto(const Model& sourceModel, const std::vector<const InputPortBase*>& sourceInputs, const OutputPort<ValueType>& sourceOutput, Model& destModel, const std::vector<const OutputPortBase*>& destInputs, const TransformContext& context, const NodeTransformFunction& transformFunction)
    {
        const auto& result = TransformSubmodelOnto(sourceModel, sourceInputs, static_cast<const OutputPortBase&>(sourceOutput), destModel, destInputs, context, transformFunction);
        return static_cast<const OutputPort<ValueType>&>(result);
    }

    template <typename ValueType>
    const OutputPort<ValueType>& ModelTransformer::CopySubmodelOnto(const Model& sourceModel, const std::vector<const InputPortBase*>& sourceInputs, const OutputPort<ValueType>& sourceOutput, Model& destModel, const std::vector<const OutputPortBase*>& destInputs, const TransformContext& context)
    {
        const auto& result = CopySubmodelOnto(sourceModel, sourceInputs, static_cast<const OutputPortBase&>(sourceOutput), destModel, destInputs, context);
        return static_cast<const OutputPort<ValueType>&>(result);
    }

    template <typename ValueType>
    const OutputPort<ValueType>& ModelTransformer::GetCorrespondingInputs(const InputPort<ValueType>& port) const
    {
        const auto& result = GetCorrespondingInputs(static_cast<const InputPortBase&>(port));
        return static_cast<const OutputPort<ValueType>&>(result);
    }

    template <typename ValueType>
    const OutputPort<ValueType>& ModelTransformer::GetCorrespondingOutputs(const OutputPort<ValueType>& port) const
    {
        const auto& result = GetCorrespondingOutputs(static_cast<const OutputPortBase&>(port));
        return static_cast<const OutputPort<ValueType>&>(result);
    }

    template <typename ValueType>
    const OutputPort<ValueType>& ModelTransformer::GetCorrespondingOutputs(const PortElements<ValueType>& elements) const
    {
        if (!elements.IsFullPortOutput())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "ModelTransformer::GetCorrespondingOutputs(): Invalid PortElements");
        }
        const auto& result = GetCorrespondingOutputs(*elements.GetRanges()[0].ReferencedPort());
        return static_cast<const OutputPort<ValueType>&>(result);
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
        _elementsMap.MapNodeOutput(&oldPort, &newPort);
    }

    template <typename ValueType>
    void ModelTransformer::MapNodeOutput(const OutputPort<ValueType>& oldPort, const OutputPort<ValueType>& newPort)
    {
        _elementsMap.MapNodeOutput(&oldPort, &newPort);
    }

    template <typename ValueType>
    void ModelTransformer::MapNodeOutput(const OutputPort<ValueType>& oldPort, const PortElements<ValueType>& newElements)
    {
        const auto& newPort = _model.AddRoutingNodes(newElements);
        _elementsMap.MapNodeOutput(&oldPort, &newPort);
    }
} // namespace model
} // namespace ell
