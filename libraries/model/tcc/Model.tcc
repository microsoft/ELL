////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Model.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace model
{
    // Hiding some stuff in this namespace that's unlikely to confict with anything
    namespace ModelImpl
    {
        template <typename ContainerType>
        class ReverseRange
        {
        public:
            ReverseRange(const ContainerType& container)
                : _begin(container.crbegin()), _end(container.crend()) {}

            typename ContainerType::const_reverse_iterator begin() const { return _begin; }

            typename ContainerType::const_reverse_iterator end() const { return _end; }

        private:
            typename ContainerType::const_reverse_iterator _begin;
            typename ContainerType::const_reverse_iterator _end;
        };

        template <typename ContainerType>
        ReverseRange<ContainerType> Reverse(const ContainerType& container)
        {
            return ReverseRange<ContainerType>(container);
        }
    }

    //
    // Factory method for creating nodes
    //
    template <typename NodeType, typename... Args>
    NodeType* Model::AddNode(Args&&... args)
    {
        auto node = std::make_shared<NodeType>(std::forward<Args>(args)...);
        node->RegisterDependencies();
        _idToNodeMap[node->GetId()] = node;
        return node.get();
    }

    //
    // Compute output value
    //
    template <typename ValueType>
    std::vector<ValueType> Model::ComputeOutput(const OutputPort<ValueType>& outputPort) const
    {
        auto compute = [](const Node& node) { node.Compute(); };
        VisitSubset({ outputPort.GetNode() }, compute);
        return outputPort.GetOutput();
    }

    template <typename ValueType>
    std::vector<ValueType> Model::ComputeOutput(const PortElements<ValueType>& elements) const
    {
        // get set of nodes to make sure we visit
        std::unordered_set<const Node*> usedNodes;
        for (const auto& range : elements.GetRanges())
        {
            usedNodes.insert(range.ReferencedPort()->GetNode());
        }

        auto compute = [](const Node& node) { node.Compute(); };
        auto nodes = std::vector<const Node*>(usedNodes.begin(), usedNodes.end());
        VisitSubset(nodes, compute);

        // Now construct the output
        auto numElements = elements.Size();
        std::vector<ValueType> result(numElements);
        for (size_t index = 0; index < numElements; ++index)
        {
            auto element = elements.GetElement(index);
            auto port = element.ReferencedPort();
            auto portOutput = port->GetOutput()[element.GetIndex()];
            result[index] = portOutput;
        }
        return result;
    }

    template <typename ValueType>
    std::vector<ValueType> Model::ComputeOutput(const PortElementsBase& elements) const
    {
        auto typedElements = PortElements<ValueType>(elements);
        return ComputeOutput(typedElements);
    }

    //
    // Get nodes by type
    //
    template <typename NodeType>
    std::vector<const NodeType*> Model::GetNodesByType() const
    {
        std::vector<const NodeType*> result;
        auto findNodes = [&result](const Node& node) {
            auto nodePtr = dynamic_cast<const NodeType*>(&node);
            if (nodePtr != nullptr)
            {
                result.push_back(nodePtr);
            }
        };
        Visit(findNodes);
        return result;
    }

    template <typename NodeType>
    std::vector<NodeType*> Model::GetNodesByType()
    {
        std::vector<NodeType*> result;
        auto findNodes = [&result](const Node& node) {
            auto nodePtr = dynamic_cast<const NodeType*>(&node);
            if (nodePtr != nullptr)
            {
                result.push_back(const_cast<NodeType*>(nodePtr));
            }
        };
        Visit(findNodes);
        return result;
    }

    //
    // Visitors
    //

    // Visits the entire model
    template <typename Visitor>
    void Model::Visit(Visitor&& visitor) const
    {
        std::vector<const Node*> emptyVec;
        VisitSubset(emptyVec, visitor);
    }

    // Visits just the parts necessary to compute output node
    template <typename Visitor>
    void Model::VisitSubset(const Node* outputNode, Visitor&& visitor) const
    {
        VisitSubset(std::vector<const Node*>{ outputNode }, visitor);
    }

    // Real implementation function for `VisitSubset()`
    template <typename Visitor>
    void Model::VisitSubset(const std::vector<const Node*>& outputNodes, Visitor&& visitor) const
    {
        auto iter = GetNodeIterator(outputNodes);
        while (iter.IsValid())
        {
            visitor(*iter.Get());
            iter.Next();
        }
    }
}
}
