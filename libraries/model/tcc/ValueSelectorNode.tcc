////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ValueSelectorNode.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    template <typename ValueType>
    ValueSelectorNode<ValueType>::ValueSelectorNode(const OutputPortElementList<bool>& condition, const OutputPortElementList<ValueType>& value1, const OutputPortElementList<ValueType>& value2) : Node({ &_condition, &_value1, &_value2 }, { &_output }), _condition(this, condition), _value1(this, value1), _value2(this, value2), _output(this, value1.Size())
    {
        if (condition.Size() != 1)
        {
            throw std::runtime_error("Error: Condition must be 1-D signal");
        }

        if (value1.Size() != value2.Size())
        {
            throw std::runtime_error("Error: input values must be same dimension");
        }
    };

    template <typename ValueType>
    void ValueSelectorNode<ValueType>::Compute() const
    {
        bool cond = _condition[0];
        _output.SetOutput(cond ? _value1.GetValue() : _value2.GetValue());
    };

    template <typename ValueType>
    void ValueSelectorNode<ValueType>::CopyImpl(ModelTransformer& transformer) const
    {
        auto newCondition = transformer.TransformInputPort(_condition);
        auto newValue1 = transformer.TransformInputPort(_value1);
        auto newValue2 = transformer.TransformInputPort(_value2);

        auto newNode = transformer.AddNode<ValueSelectorNode<ValueType>>(newCondition, newValue1, newValue2);

         MapOutputPorts(transformer, newNode);
    }

    template <typename ValueType>
    void ValueSelectorNode<ValueType>::RefineImpl(ModelTransformer& transformer) const
    {
        auto newCondition = transformer.TransformInputPort(_condition);
        auto newValue1 = transformer.TransformInputPort(_value1);
        auto newValue2 = transformer.TransformInputPort(_value2);

        auto newNode = transformer.AddNode<ValueSelectorNode<ValueType>>(newCondition, newValue1, newValue2);

        MapOutputPorts(transformer, newNode);
    }

    //
    // Cheese
    //
    template <typename ValueType>
    SelectIfLessNode<ValueType>::SelectIfLessNode(const OutputPortElementList<ValueType>& input1, const OutputPortElementList<ValueType>& input2, const OutputPortElementList<ValueType>& value1, const OutputPortElementList<ValueType>& value2) : Node({ &_input1, &_input2, &_value1, &_value2 }, { &_output }), _input1(this, input1), _input2(this, input2), _value1(this, value1), _value2(this, value2), _output(this, value1.Size())
    {
        if (input1.Size() != input2.Size())
        {
            throw std::runtime_error("Error: input values must be same dimension");
        }

        if (value1.Size() != value2.Size())
        {
            throw std::runtime_error("Error: input values must be same dimension");
        }
    };

    template <typename ValueType>
    void SelectIfLessNode<ValueType>::Compute() const
    {
        auto in1 = _input1[0];
        auto in2 = _input2[0];
        bool cond = in1 < in2;
        _output.SetOutput(cond ? _value1.GetValue() : _value2.GetValue());
    };

    template <typename ValueType>
    void SelectIfLessNode<ValueType>::CopyImpl(ModelTransformer& transformer) const
    {
        auto newInput1 = transformer.TransformInputPort(_input1);
        auto newInput2 = transformer.TransformInputPort(_input2);
        auto newValue1 = transformer.TransformInputPort(_value1);
        auto newValue2 = transformer.TransformInputPort(_value2);

        auto newNode = transformer.AddNode<SelectIfLessNode<ValueType>>(newInput1, newInput2, newValue1, newValue2);

        transformer.MapPort(_output, newNode->_output);
    }

    template <typename ValueType>
    void SelectIfLessNode<ValueType>::RefineImpl(ModelTransformer& transformer) const
    {
        auto newInput1 = transformer.TransformInputPort(_input1);
        auto newInput2 = transformer.TransformInputPort(_input2);
        auto newValue1 = transformer.TransformInputPort(_value1);
        auto newValue2 = transformer.TransformInputPort(_value2);

        // TODO: do something here
        auto ifLessNode = transformer.AddNode<IfLessNode<ValueType>>(newInput1, newInput2);
        auto selectNode = transformer.AddNode<ValueSelectorNode<ValueType>>(ifLessNode->output, newValue1, newValue2);

        MapOutputPorts(transformer, selectNode);
    }

    //
    // More cheese
    //
    template <typename ValueType>
    IfLessNode<ValueType>::IfLessNode(const OutputPortElementList<ValueType>& value, const OutputPortElementList<ValueType>& threshold) : Node({ &_value, &_threshold }, { &_output }), _value(this, value), _threshold(this, threshold), _output(this, 1)
    {
        if (value.Size() != threshold.Size())
        {
            throw std::runtime_error("Error: input values must be same dimension");
        }
    };

    template <typename ValueType>
    void IfLessNode<ValueType>::Compute() const
    {
        std::vector<bool> output;
        output.push_back(_value[0] < _threshold[0]);
        _output.SetOutput(output);
    };

    template <typename ValueType>
    void IfLessNode<ValueType>::CopyImpl(ModelTransformer& transformer) const
    {
        auto newValue = transformer.TransformInputPort(_value);
        auto newThreshold = transformer.TransformInputPort(_threshold);

        auto newNode = transformer.AddNode<IfLessNode<ValueType>>(newValue, newThreshold);

        MapOutputPorts(transformer, newNode);
//        transformer.MapPort(_output, newNode->_output);
    }

    template <typename ValueType>
    void IfLessNode<ValueType>::RefineImpl(ModelTransformer& transformer) const
    {
        auto newValue = transformer.TransformInputPort(_value);
        auto newThreshold = transformer.TransformInputPort(_threshold);

        auto newNode = transformer.AddNode<IfLessNode<ValueType>>(newValue, newThreshold);

        MapOutputPorts(transformer, newNode);
//        transformer.MapPort(_output, newNode->_output);
    }
}
