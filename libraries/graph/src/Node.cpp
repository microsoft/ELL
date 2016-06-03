//
// Node.cpp
//

#include "Node.h"

Node::Node(const std::vector<NodeOutputBase*>& inputs, const std::vector<NodeOutputBase*>& outputs) : _inputs(inputs), _outputs(outputs)
{
    for(const auto& input: inputs)
    {
        input->GetNode()->AddDependent(this);
    }
};
