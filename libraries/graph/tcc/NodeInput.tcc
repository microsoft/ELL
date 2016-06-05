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
    assert(GetNode() != nullptr);

    // retrieve cached value from our connected output
    auto result = GetNode()->GetOutputValue<ValueType>(GetOutputIndex());
//    assert(result.size() == GetSize());
    return result;
}
