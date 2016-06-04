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
    InputNode(size_t dimension) : Node({}, {&_output}), _output(this, 0, dimension) {};

    NodeOutput<ValueType> _output;

private:
};
