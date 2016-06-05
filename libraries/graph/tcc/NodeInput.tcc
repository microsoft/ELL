//
// NodeInput.tcc
// 

#include "Node.h"

#include <cassert>

template <typename ValueType>
NodeInput::NodeInput(const NodeOutput<ValueType>& output): NodeEdge(output.GetNode(), output.GetOutputIndex(), NodeEdge::GetTypeCode<ValueType>(), output.GetSize()) 
{
    assert(NodeEdge::GetTypeCode<ValueType>() == output.GetType());        
}

template <typename ValueType>
std::vector<ValueType> NodeInput::GetValue() const
{
    return _node->GetOutputValue<ValueType>(_outputIndex);
    // retrieve cached value from our connected output
}
