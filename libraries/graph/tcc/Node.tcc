////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Node.tcc (graph)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InputPort.h"

template <typename ValueType>
std::vector<ValueType> Node::GetOutputValue(size_t outputIndex) const
{
    // runtime enforcement of type-matching
    if(GetOutputType(outputIndex) != Port::GetTypeCode<ValueType>())
    {
        throw std::runtime_error("Incompatible types for GetOutputValue");
    }
        
    auto typedOutput = static_cast<OutputPort<ValueType>*>(_outputs[outputIndex]);
    return typedOutput->GetOutput();
}
