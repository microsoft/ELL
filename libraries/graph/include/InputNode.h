#pragma once
//
// InputNode
// 

#include "Node.h"
#include "OutputPort.h"

#include <vector>
#include <memory>
#include <string>

template <typename ValueType>
class InputNode: public Node
{
public:
    InputNode(size_t dimension);
    void SetInput(std::vector<ValueType> inputValues);
    
    virtual std::string GetTypeName() const override { return "Input"; }

    const OutputPort<ValueType>& output = _output;

protected:
    virtual void Compute() const override;

private:
    std::vector<ValueType> _inputValues;
    OutputPort<ValueType> _output;
};

#include "../tcc/InputNode.tcc"
