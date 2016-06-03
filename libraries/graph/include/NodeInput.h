#pragma once

//
// NodeInput
// 
#include "NodeOutput.h"

#include <vector>
#include <memory>

template <typename ValueType>
class NodeInput
{
public:
    NodeInput(const NodeOutput<ValueType>& output);

    NodeInput(const std::vector<NodeOutput<ValueType>>& outputs);
private:  
};

