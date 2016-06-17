////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Model.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include <iostream>

/// <summary> model namespace </summary>
namespace model
{
    // Just hiding some stuff in this namespace that's unlikely to confict with anything
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

    //template <typename NodeType, typename ArgType>
    //ArgType ConvertInput(Model* model, ArgType&& arg)
    //{
    //    static_assert(!std::is_same<ArgType, OutputRange<NodeType>>::value, "blah");
    //    std::cout << typeid(NodeType).name() << "\t" << typeid(ArgType).name() << std::endl;
    //    return arg;
    //}
    /*
    template <typename InputType>
    class ConvertInput
    {
    public:
        static InputType Convert(Model* model, const InputType& input)
        {
            return input;
        }
    };

    template <typename InputType>
    class ConvertInput <OutputRange<InputType>>
    {
    public:
        static OutputPort<InputType> Convert(Model* model, const OutputRange<InputType>& input)
        {
            auto combiner = model->AddNode<CombinerNode<InputType>>(input);
            return combiner->output;
        }
    };

    template <typename InputType>
    class ConvertInput <OutputRange<InputType>&>
    {
    public:
        static OutputPort<InputType> Convert(Model* model, const OutputRange<InputType>& input)
        {
            auto combiner = model->AddNode<CombinerNode<InputType>>(input);
            return combiner->output;
        }
    };

    template <typename InputType>
    class ConvertInput <OutputRangeList<InputType>>
    {
    public:
        static OutputPort<InputType> Convert(Model* model, const OutputRangeList<InputType>& inputs)
        {
            auto combiner = model->AddNode<CombinerNode<InputType>>(inputs);
            return combiner->output;
        }
    };

    template <typename InputType>
    class ConvertInput <OutputRangeList<InputType>&>
    {
    public:
        static OutputPort<InputType> Convert(Model* model, const OutputRangeList<InputType>& inputs)
        {
            auto combiner = model->AddNode<CombinerNode<InputType>>(inputs);
            return combiner->output;
        }
    };*/

    template <typename NodeType, typename... Args>
    std::shared_ptr<NodeType> Model::AddNode(Args&&... args)
    {
        auto node = std::make_shared<NodeType>(args...);
        //auto node = std::make_shared<NodeType>(ConvertInput<std::decay<Args>::type>::Convert(this, args)...);
//        auto node = std::make_shared<NodeType>(ConvertInput<Args>::Convert(this, std::forward<Args>(args))...);
        node->RegisterDependencies();
        _nodeMap[node->GetId()] = node;
        return node;
    }

    //
    // Compute output value
    //
    template <typename ValueType>
    std::vector<ValueType> Model::GetNodeOutput(const OutputPort<ValueType>& outputPort) const
    {
        auto compute = [](const Node& node) { node.Compute(); };

        Visit(compute, { outputPort.Node() });

        return outputPort.GetOutput();
    }

    template <typename ValueType>
    std::vector<ValueType> Model::GetNodeOutput(const std::shared_ptr<Node>& outputNode, size_t outputIndex) const
    {
        auto compute = [](const Node& node) { node.Compute(); };

        if (Port::GetTypeCode<ValueType>() != outputNode->GetOutputType(outputIndex))
        {
            throw std::runtime_error("output types don't match");
        }

        Visit(compute, outputNode);

        return outputNode->GetOutputValue<ValueType>(outputIndex);
    }

    //
    // Visitors
    //

    // Visits the entire graph
    template <typename Visitor>
    void Model::Visit(Visitor& visitor) const
    {
        std::vector<const Node*> emptyVec;
        Visit(visitor, emptyVec);
    }

    // Visits just the parts necessary to compute output node
    template <typename Visitor>
    void Model::Visit(Visitor& visitor, const std::shared_ptr<Node>& outputNode) const
    {
        std::vector<std::shared_ptr<Node>> x = { outputNode };
        Visit(visitor, { outputNode.get() });
    }

    // Visits just the parts necessary to compute output nodes
    template <typename Visitor>
    void Model::Visit(Visitor& visitor, const std::vector<std::shared_ptr<Node>>& outputNodes) const
    {
        // start with output nodes in the stack
        std::vector<const Node*> outputNodePtrs;
        for (auto outputNode : outputNodes)
        {
            outputNodePtrs.push_back(outputNode.get());
        }
        Visit(visitor, outputNodePtrs);
    }

    // Real implementation function for `Visit()`
    template <typename Visitor>
    void Model::Visit(Visitor& visitor, const std::vector<const Node*>& outputNodePtrs) const
    {
        if (_nodeMap.size() == 0)
        {
            return;
        }

        // start with output nodes in the stack
        std::unordered_set<const Node*> visitedNodes;
        std::vector<const Node*> stack = outputNodePtrs;

        const Node* sentinelNode = nullptr;
        if (stack.size() == 0) // Visit full graph
        {
            // helper function to find a terminal node
            auto IsLeaf = [](const Node* node) { return node->GetDependentNodes().size() == 0; };
            // start with some arbitrary node
            const Node* anOutputNode = _nodeMap.begin()->second.get();
            // follow dependency chain until we get an output node
            while (!IsLeaf(anOutputNode))
            {
                anOutputNode = anOutputNode->GetDependentNodes()[0];
            }
            stack.push_back(anOutputNode);
            sentinelNode = anOutputNode;
        }

        while (stack.size() > 0)
        {
            const Node* node = stack.back();

            // check if we've already visited this node
            if (visitedNodes.find(node) != visitedNodes.end())
            {
                stack.pop_back();
                continue;
            }

            // we can visit this node only if all its inputs have been visited already
            bool canVisit = true;
            for (auto input : node->_inputs)
            {
                // Note: If InputPorts can point to multiple nodes, we'll have to iterate over them here
                for (const auto& inputRange : input->GetInputRanges())
                {
                    auto inputNode = inputRange.referencedPort->Node();
                    canVisit = canVisit && visitedNodes.find(inputNode) != visitedNodes.end();
                }
            }

            if (canVisit)
            {
                stack.pop_back();
                visitedNodes.insert(node);

                // In "visit whole graph" mode, we want to defer visiting the chosen output node until the end
                // In "visit active graph" mode, this test should never fail, and we'll always visit the node
                if (node != sentinelNode)
                {
                    visitor(*node);
                }

                if (sentinelNode != nullptr) // sentinelNode is non-null only if we're in visit-whole-graph mode
                {
                    // now add all our children (Note: this part is the only difference between visit-all and visit-active-graph
                    for (const auto& child : ModelImpl::Reverse(node->_dependentNodes)) // Visiting the children in reverse order more closely retains the order the features were originally created
                    {
                        // note: this is kind of inefficient --- we're going to push multiple copies of child on the stack. But we'll check if we've visited it already when we pop it off.
                        // TODO: optimize this if it's a problem
                        stack.push_back(child);
                    }
                }
            }
            else // visit node's inputs
            {
                for (auto input : ModelImpl::Reverse(node->_inputs)) // Visiting the inputs in reverse order more closely retains the order the features were originally created
                {
                    for (const auto& inputRange : input->GetInputRanges())
                    {
                        stack.push_back(inputRange.referencedPort->Node()); // Again, if `InputPort`s point to multiple nodes, need to iterate here
                    }
                }
            }
        }
        if (sentinelNode != nullptr)
        {
            visitor(*sentinelNode);
        }
    }
}
