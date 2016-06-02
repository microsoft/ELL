#pragma once
//
// InputNode
// 

#include "Node.h"
#include "NodeOutput.h"

#include <vector>
#include <memory>

template <typename ValueType>
class InputNode: public Node
{
public:
    InputNode(size_t dimension);

    NodeOutput<ValueType> output;
};
