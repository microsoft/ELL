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
    OutputPortElementList<ValueType> ModelTransformer::TransformInputPort(const InputPort<ValueType>& input)
    {
        const auto& ranges = input.GetInputRanges();
        std::vector<OutputPortElementList<ValueType>> newRanges;
        for (const auto& range : ranges)
        {
            auto oldPort = range.ReferencedPort();
            auto newPort = _portMap[oldPort];
            auto outputPort = dynamic_cast<const OutputPort<ValueType>*>(newPort);
            // TODO: ensure dynamic cast worked

            auto start = range.GetStartIndex();
            auto size = range.Size();
            newRanges.emplace_back(*outputPort, start, size);
        }
        return OutputPortElementList<ValueType>(newRanges);
    }

    template <typename NodeType, typename... Args>
    std::shared_ptr<NodeType> ModelTransformer::AddNode(Args&&... args)
    {
        return _model.AddNode<NodeType>(std::forward<Args>(args)...);
    }
}
