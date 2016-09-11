////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ModelTransformer.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
/// <summary> model namespace </summary>
namespace model
{
    template <typename ValueType>
    PortElements<ValueType> ModelTransformer::TransformPortElements(const PortElements<ValueType>& elements)
    {
        auto size = elements.Size();
        PortElements<ValueType> result;
        result.Reserve(size);
        for (size_t index = 0; index < size; ++index)
        {
            auto oldElement = elements.GetElement(index);
            assert(_elementToElementMap.find(oldElement) != _elementToElementMap.end());
            if (_elementToElementMap.find(oldElement) == _elementToElementMap.end())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Could not find element in new model.");
            }
            auto newElement = _elementToElementMap[oldElement];
            auto newPort = static_cast<const OutputPort<ValueType>*>(newElement.ReferencedPort());
            result.Append({ *newPort, newElement.GetIndex() });
        }
        result.Consolidate();
        return result;
    }

    template <typename ValueType>
    PortElements<ValueType> ModelTransformer::GetCorrespondingOutputs(const OutputPort<ValueType>& port)
    {
        PortElements<ValueType> elements(port);
        return GetCorrespondingOutputs(elements);
    }

    template <typename ValueType>
    PortElements<ValueType> ModelTransformer::GetCorrespondingOutputs(const PortElements<ValueType>& elements)
    {
        return TransformPortElements(elements);
    }

    template <typename ValueType>
    InputNode<ValueType>* ModelTransformer::GetCorrespondingInputNode(const InputNode<ValueType>* inputNode)
    {
        auto newNodeOutputs = GetCorrespondingOutputs(inputNode->output);
        auto newNodeConst = newNodeOutputs.GetElement(0).ReferencedPort()->GetNode();
        auto newInputNodeConst = dynamic_cast<const model::InputNode<ValueType>*>(newNodeConst);
        assert(newInputNodeConst != nullptr);
        auto newInputNode = const_cast<model::InputNode<ValueType>*>(newInputNodeConst);
        return newInputNode;
    }

    template <typename ValueType>
    void ModelTransformer::MapNodeOutput(const OutputPort<ValueType>& oldPort, const OutputPort<ValueType>& newPort)
    {
        auto size = oldPort.Size();
        assert(newPort.Size() == size);
        for (size_t index = 0; index < size; ++index)
        {
            _elementToElementMap[{ oldPort, index }] = { newPort, index };
        }
    }

    template <typename ValueType>
    void ModelTransformer::MapNodeOutput(const OutputPort<ValueType>& oldPort, const PortElements<ValueType>& newElements)
    {
        auto size = oldPort.Size();
        assert(newElements.Size() == size);
        for (size_t index = 0; index < size; ++index)
        {
            _elementToElementMap[{ oldPort, index }] = newElements.GetElement(index);
        }
    }

    template <typename ValueType>
    void ModelTransformer::MapNodeOutput(const PortElements<ValueType>& oldElements, const PortElements<ValueType>& newElements)
    {
        auto size = oldElements.Size();
        assert(oldElements.Size() == size);
        for (size_t index = 0; index < size; ++index)
        {
            _elementToElementMap[oldElements.GetElement(index)] = newElements.GetElement(index);
        }
    }

    template <typename NodeType, typename... Args>
    NodeType* ModelTransformer::AddNode(Args&&... args)
    {
        auto newNode = _model.AddNode<NodeType>(std::forward<Args>(args)...);
        _isModelCompilable &= _context.IsNodeCompilable(*newNode);
        return newNode;
    }
}
}
