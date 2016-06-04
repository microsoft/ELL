#pragma once
//
// InputNode
// 

#include "Node.h"
#include "NodeOutput.h"

#include <vector>
#include <memory>
#include <string>

template <typename ValueType>
class InputNode: public Node
{
public:
    InputNode(size_t dimension) : Node({}, {&_output}), _output(this, 0, dimension) {};

    virtual std::string Type() const override { return "Input"; }

    NodeOutput<ValueType> _output;

private:
};
