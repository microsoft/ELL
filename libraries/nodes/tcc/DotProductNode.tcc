////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DotProductNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace nodes
{
    template <typename ValueType>
    DotProductNode<ValueType>::DotProductNode() : Node({&_input1, &_input2}, {&_output}), _input1(this, {}, input1PortName), _input2(this, {}, input2PortName), _output(this, outputPortName, 1)
    {
    }

    template <typename ValueType>
    DotProductNode<ValueType>::DotProductNode(const model::PortElements<ValueType>& input1, const model::PortElements<ValueType>& input2) : Node({&_input1, &_input2}, {&_output}), _input1(this, input1, input1PortName), _input2(this, input2, input2PortName), _output(this, outputPortName, 1)
    {
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::Compute() const
    {
        ValueType result = 0;
        for(size_t index = 0; index < _input1.Size(); ++index)
        {
            result += _input1[index] * _input2[index];
        }
        _output.SetOutput({ result });
    };

    template <typename ValueType>
    void DotProductNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newInput1 = transformer.TransformPortElements(_input1.GetPortElements());
        auto newInput2 = transformer.TransformPortElements(_input2.GetPortElements());
        auto newNode = transformer.AddNode<DotProductNode<ValueType>>(newInput1, newInput2);
        transformer.MapNodeOutput(output, newNode->output);
    }

    template <typename ValueType>
    bool DotProductNode<ValueType>::Refine(model::ModelTransformer& transformer) const
    {
        // Maybe... in reality, dot product will likely want to be computed as in Compute() above
        auto newInput1 = transformer.TransformPortElements(_input1.GetPortElements());
        auto newInput2 = transformer.TransformPortElements(_input2.GetPortElements());
        auto multNode = transformer.AddNode<BinaryOperationNode<ValueType>>(newInput1, newInput2, BinaryOperationNode<ValueType>::OperationType::coordinatewiseMultiply);
        auto sumNode = transformer.AddNode<SumNode<ValueType>>(multNode->output);

        transformer.MapNodeOutput(output, sumNode->output);
        return true;
    }

    template <typename ValueType>
    utilities::ObjectDescription DotProductNode<ValueType>::GetTypeDescription()
    {
        auto description = utilities::MakeObjectDescription<Node, DotProductNode<ValueType>>("Binary operation node");
        description.template AddProperty<decltype(_input1)>(input1PortName, "Input port 1");
        description.template AddProperty<decltype(_input2)>(input2PortName, "Input port 2");
        description.template AddProperty<decltype(_output)>(outputPortName, "Output port");
        return description;
    }

    template <typename ValueType>
    utilities::ObjectDescription DotProductNode<ValueType>::GetDescription() const
    {
        utilities::ObjectDescription description = GetParentDescription<Node, DotProductNode<ValueType>>();
        description[input1PortName] = _input1;
        description[input2PortName] = _input2;
        description[outputPortName] = _output;
        return description;
    }

    template <typename ValueType>
    void DotProductNode<ValueType>::SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context)
    {
        Node::SetObjectState(description, context);
        description[input1PortName] >> _input1;
        description[input2PortName] >> _input2;
        description[outputPortName] >> _output;
    }
}
