////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Model.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    // Hiding some stuff in this namespace that's unlikely to confict with anything
    // TODO: put this in some utility place
    namespace ModelImpl
    {
        template <typename ContainerType>
        class ReverseRange
        {
        public:
            ReverseRange(const ContainerType& container) : _begin(container.crbegin()), _end(container.crend()) {}

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
        auto node = std::make_shared<NodeType>(args...);
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

        Visit(compute, { outputPort.GetNode() });

        return outputPort.GetOutput();
    }

    //
    // Get nodes by type
    //
    template <typename NodeType>
    std::vector<const NodeType*> Model::GetNodesByType()
    {
        std::vector<const NodeType*> result;
        auto findNodes = [&result](const Node& node) 
        {
            if(typeid(node) == typeid(NodeType))
            {
                result.push_back(dynamic_cast<const NodeType*>(&node));
            }
        };
        Visit(findNodes);
        return result;
    }

    //
    // Visitors
    //

    // Visits the entire graph
    template <typename Visitor>
    void Model::Visit(Visitor&& visitor) const
    {
        std::vector<const Node*> emptyVec;
        Visit(visitor, emptyVec);
    }

    // Visits just the parts necessary to compute output node
    template <typename Visitor>
    void Model::Visit(Visitor&& visitor, const Node* outputNode) const
    {
        Visit(visitor, std::vector<const Node*>{ outputNode });
    }

    // Real implementation function for `Visit()`
    template <typename Visitor>
    void Model::Visit(Visitor&& visitor, const std::vector<const Node*>& outputNodes) const
    {
        auto iter = GetNodeIterator(outputNodes);
        while(iter.IsValid())
        {
            visitor(*iter.Get());
            iter.Next();
        }
    }
}
