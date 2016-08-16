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
        auto size = elements.Size();
        PortElements<ValueType> result;
        // result.Reserve(size);
        for (size_t index = 0; index < size; ++index)
        {
            auto oldElement = elements.GetElement(index);
            assert(_elementToElementMap.find(oldElement) != _elementToElementMap.end());
            auto newElement = _elementToElementMap[oldElement];
            auto newPort = static_cast<const OutputPort<ValueType>*>(newElement.ReferencedPort());
            result.Append({ *newPort, newElement.GetIndex() });
        }
        // result.Consolidate();
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
        // TODO: fix this --- it only returns a port
        // TODO: iterate over each element
        assert(elements.NumRanges() == 1);




        auto firstElement = elements.GetElement(0);
        if (_elementToElementMap.find(firstElement) == _elementToElementMap.end())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument, "Could not find port in new model.");
        }

        auto range = _elementToElementMap[firstElement];
        auto untypedPort = range.ReferencedPort();
        auto port = dynamic_cast<const OutputPort<ValueType>*>(untypedPort);
        assert(port != nullptr);
        return *port;
    }

    template <typename ValueType>
    InputNode<ValueType>* ModelTransformer::GetCorrespondingInputNode(const InputNode<ValueType>* inputNode)
    {
        // #### FIX THIS --- maybe we should just keep track of nodes separately somehow
        // This sucks:
        auto inputNodeOutputPortElements = PortElements<ValueType>(inputNode->output);
        auto newInputNodeOutputs = GetCorrespondingOutputs(inputNodeOutputPortElements);
        auto newInputNodeOutputPort = newInputNodeOutputs.GetElement(0).ReferencedPort();
        auto newInputNodeConst = dynamic_cast<const model::InputNode<ValueType>*>(newInputNodeOutputPort->GetNode());
        assert(newInputNodeConst != nullptr);
        auto newInputNode = const_cast<model::InputNode<ValueType>*>(newInputNodeConst);
        return newInputNode;
    }

    template <typename ValueType>
    void ModelTransformer::MapNodeOutput(const OutputPort<ValueType>& oldPort, const OutputPort<ValueType>& newPort)
    {
        auto portSize = oldPort.Size(); 
        assert(newPort.Size() == portSize);
        for(size_t index = 0; index < portSize; ++index)
        {
            _elementToElementMap[{oldPort, index}] = {newPort, index};
        }
    }

    template <typename ValueType>
    void ModelTransformer::MapNodeOutput(const PortElements<ValueType>& oldElements, const PortElements<ValueType>& newElements)
    {
        // auto portSize = oldPort.Size(); 
        // assert(newPort.Size() == portSize);
        // for(size_t index = 0; index < portSize; ++index)
        // {
        //     _elementToElementMap[{oldPort, index}] = {newPort, index};
        // }

    }

    template <typename NodeType, typename... Args>
    NodeType* ModelTransformer::AddNode(Args&&... args)
    {
        auto newNode = _model.AddNode<NodeType>(std::forward<Args>(args)...);
        _isModelCompilable &= _context.IsNodeCompilable(*newNode);
        return newNode;
    }
}
