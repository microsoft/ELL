//
// NodeInput.tcc
// 

#include "Node.h"

template <typename ValueType>
NodeInput::NodeInput(const NodeOutput<ValueType>& output): NodeEdge(output.GetNode(), output.GetOutputIndex(), NodeEdge::GetTypeCode<ValueType>(), output.Size()) 
{
    assert(NodeEdge::GetTypeCode<ValueType>() == output.GetType());
}

template <typename ValueType>
std::vector<ValueType> NodeInput::GetValue() const
{
    assert(GetNode() != nullptr);

    // retrieve cached value from our connected output
    auto result = GetNode()->GetOutputValue<ValueType>(GetOutputIndex());
    assert(result.size() == Size());
    return result;
}
