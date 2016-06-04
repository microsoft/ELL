//
// NodeInput.tcc
// 

#include <cassert>

template <typename ValueType>
NodeInput::NodeInput(const NodeOutput<ValueType>& output): NodeEdge(output.GetNode(), output.GetOutputIndex(), NodeEdge::GetTypeCode<ValueType>(), output.GetSize()) 
{
    assert(NodeEdge::GetTypeCode<ValueType>() == output.GetType());        
}
