//
// NodeInput.tcc
// 

#include "Node.h"

template <typename ValueType>
NodeInput::NodeInput(const NodeOutput<ValueType>& output): NodeEdge(output.Node(), output.OutputIndex(), NodeEdge::GetTypeCode<ValueType>(), output.Size()) 
{
    assert(NodeEdge::GetTypeCode<ValueType>() == output.Type());
}

template <typename ValueType>
std::vector<ValueType> NodeInput::GetValue() const
{
    assert(Node() != nullptr);

    // retrieve cached value from our connected output
    auto result = Node()->GetOutputValue<ValueType>(OutputIndex());
    assert(result.size() == Size());
    return result;
}
