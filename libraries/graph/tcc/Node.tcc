//
// Node.tcc
//

template <typename ValueType>
std::vector<ValueType> Node::GetOutputValue(size_t outputIndex) const
{
    auto typedOutput = static_cast<NodeOutput<ValueType>*>(_outputs[outputIndex]);
    return typedOutput->GetOutput();
}
