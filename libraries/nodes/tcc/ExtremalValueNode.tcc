////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ExtremalValueNode.tcc (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
/// <summary> nodes namespace </summary>
namespace nodes
{
    template <typename ValueType, bool max>
    ExtremalValueNode<ValueType, max>::ExtremalValueNode()
        : Node({ &_input }, { &_val, &_argVal }), _input(this, {}, inputPortName), _val(this, valPortName, 1), _argVal(this, argValPortName, 1)
    {
    }

    template <typename ValueType, bool max>
    ExtremalValueNode<ValueType, max>::ExtremalValueNode(const model::PortElements<ValueType>& input)
        : Node({ &_input }, { &_val, &_argVal }), _input(this, input, inputPortName), _val(this, valPortName, 1), _argVal(this, argValPortName, 1)
    {
    }

    template <typename ValueType, bool max>
    std::string ExtremalValueNode<ValueType, max>::GetTypeName()
    {
        if (max)
        {
            return utilities::GetCompositeTypeName<ValueType, std::true_type>("ExtremalValueNode");
        }
        else
        {
            return utilities::GetCompositeTypeName<ValueType, std::false_type>("ExtremalValueNode");
        }
    }

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::WriteToArchive(utilities::Archiver& archiver) const
    {
        Node::WriteToArchive(archiver);
        archiver[inputPortName] << _input;
        archiver[valPortName] << _val;
        archiver[argValPortName] << _argVal;
    }

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::ReadFromArchive(utilities::Unarchiver& archiver)
    {
        Node::ReadFromArchive(archiver);
        archiver[inputPortName] >> _input;
        archiver[valPortName] >> _val;
        archiver[argValPortName] >> _argVal;
    }

    template <typename ValueType, bool max>
    void ExtremalValueNode<ValueType, max>::Compute() const
    {
        auto inputValues = _input.GetValue();
        decltype(std::max_element(inputValues.begin(), inputValues.end())) result;
        if (max)
        {
            result = std::max_element(inputValues.begin(), inputValues.end());
        }
        else
        {
            result = std::min_element(inputValues.begin(), inputValues.end());
        }
        auto val = *result;
        auto index = result - inputValues.begin();
        _val.SetOutput({ val });
        _argVal.SetOutput({ (int)index });
    };

    //
    // Copy definitions for subclasses
    //
    template <typename ValueType>
    void ArgMinNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(this->_input.GetPortElements());
        auto newNode = transformer.AddNode<ArgMinNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(this->val, newNode->val);
        transformer.MapNodeOutput(this->argVal, newNode->argVal);
    }

    template <typename ValueType>
    void ArgMaxNode<ValueType>::Copy(model::ModelTransformer& transformer) const
    {
        auto newPortElements = transformer.TransformPortElements(this->_input.GetPortElements());
        auto newNode = transformer.AddNode<ArgMaxNode<ValueType>>(newPortElements);
        transformer.MapNodeOutput(this->val, newNode->val);
        transformer.MapNodeOutput(this->argVal, newNode->argVal);
    }
}
}
