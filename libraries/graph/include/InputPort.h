#pragma once

//
// InputPort
// 
#include "Port.h"
#include "OutputPort.h"

#include <vector>
#include <memory>
#include <cassert>

class Node;

//
// InputPort: each node/function stores a list of InputPort objects that represent the inputs to the computation
//
class InputPort : public Port
{
public:
    // constructor --- takes the output that we take our value from
    template <typename ValueType>
    InputPort(const OutputPort<ValueType>* output);

    // "concat" version
    // InputPort(const std::vector<OutputPort<ValueType>>& outputs);

    // returns the (already-computed) output value corresponding to this input    
    template <typename ValueType>
    std::vector<ValueType> GetValue() const;
    
private:
    const Port* _inputLocation; // TODO: rename this variable
    // optional offset, size
    // maybe a union?
    // Later, maybe keep a list of     
};

// TODO: if we want to allow functions/nodes to gather values from arbitrary collections of output elements, then
//       we'll probablay want to make InputPort not be a subclass of Port, but for it to contain a vector of
//       (Node*, outputIndex, start, len) items.
//       We'll have to replace `GetNode()` with `GetNodes()` and maybe just remove `OutputIndex()`.

#include "../tcc/InputPort.tcc"
