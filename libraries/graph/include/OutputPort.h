////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     OutputPort.h (graph)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Port.h"

#include <vector>
#include <memory>
#include <iostream>

class Node;

/// <summary> Base for classes that represent outputs from a function (essentially identical to Port) </summary>
class OutputPortBase : public Port
{
protected:
    OutputPortBase(const class Node* node, size_t outputIndex, PortType type, size_t size) : Port(node, outputIndex, type, size) 
    {
    };
};

/// <summary> Represents an output from a node </summary>
template <typename ValueType>
class OutputPort : public OutputPortBase
{
public:
    OutputPort(const class Node* node, size_t outputIndex, size_t size) : OutputPortBase(node, outputIndex, OutputPortBase::GetTypeCode<ValueType>(), size) {}

    /// <summary> Returns the cached output from this port </summary>
    std::vector<ValueType> GetOutput() const { return _cachedOutput; }

    /// <summary> Sets the cached output from this port </summary>
    void SetOutput(std::vector<ValueType> values) const { _cachedOutput = values; }

private:
    mutable std::vector<ValueType> _cachedOutput;
};
