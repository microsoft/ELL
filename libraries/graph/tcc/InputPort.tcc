////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputPort.tcc (graph)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Node.h"

template <typename ValueType>
InputPort::InputPort(const OutputPort<ValueType>* output): Port(output->Node(), output->Index(), Port::GetTypeCode<ValueType>(), output->Size()) 
{
    assert(Port::GetTypeCode<ValueType>() == output->Type());
}

template <typename ValueType>
std::vector<ValueType> InputPort::GetValue() const
{
    assert(Node() != nullptr);

    // retrieve cached value from our connected output
    auto result = Node()->GetOutputValue<ValueType>(Index());
    assert(result.size() == Size());
    return result;
}
