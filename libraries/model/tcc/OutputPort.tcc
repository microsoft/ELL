////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputPort.tcc (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

/// <summary> model namespace </summary>
namespace model
{
    //
    // OutputPort
    //
    template <typename ValueType>
    OutputPort<ValueType>::OutputPort(const class Node* node, std::string name, size_t size) : OutputPortBase(node, name, OutputPortBase::GetPortType<ValueType>(), size)
    {}

    template <typename ValueType>
    ValueType OutputPort<ValueType>::GetOutput(size_t index) const
    { 
        return _cachedOutput[index]; 
    }

    template <typename ValueType>
    void OutputPort<ValueType>::SetOutput(std::vector<ValueType> values) const
    {
        _cachedOutput = values;
    }

    template <typename ValueType>
    utilities::ObjectDescription OutputPort<ValueType>::GetTypeDescription()
    {
        utilities::ObjectDescription description = utilities::MakeObjectDescription<OutputPortBase, OutputPort<ValueType>>("OutputPort");
        return description;
    }

    template <typename ValueType>
    utilities::ObjectDescription OutputPort<ValueType>::GetDescription() const
    {
        utilities::ObjectDescription description = GetParentDescription<OutputPortBase, OutputPort<ValueType>>();
        return description;
    }

    template <typename ValueType>
    void OutputPort<ValueType>::SetObjectState(const utilities::ObjectDescription& description, utilities::SerializationContext& context)
    {
        OutputPortBase::SetObjectState(description, context);
    }    
}
