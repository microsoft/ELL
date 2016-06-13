////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     InputPort.h (graph)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Port.h"
#include "OutputPort.h"

#include <vector>
#include <cassert>

class Node;

/// <summary> Class representing an input to a node </summary>
class InputPort : public Port
{
public:
    /// <summary> Constructor </summary>
    ///
    /// <param name="output"> The output port this port receives values from </param>
    template <typename ValueType>
    InputPort(const OutputPort<ValueType>* output);

    /// <summary> Returns the (already-computed) output value corresponding to this input </summary>
    template <typename ValueType>
    std::vector<ValueType> GetValue() const;
    
private:
    const Port* _referencedPort;
};

// TODO: if we want to allow functions/nodes to gather values from arbitrary collections of output elements, then
//       we'll probablay want to make InputPort not be a subclass of Port, but for it to contain a vector of
//       (Node*, outputIndex, start, len) items.
//       We'll have to replace `GetNode()` with `GetNodes()` and maybe just remove `OutputIndex()`.

#include "../tcc/InputPort.tcc"
