//
// Node.tcc
//

#include "NodeInput.h"

template <typename ValueType>
std::vector<ValueType> Node::GetOutputValue(size_t outputIndex) const
{
    // Ensure inputs are computed
    for(const auto& input: _inputs)
    {
        input->Node()->ComputeOutput();
    }
    
    auto typedOutput = static_cast<NodeOutput<ValueType>*>(_outputs[outputIndex]);
    return typedOutput->GetOutput();
}
