#pragma once
//
// Node
//

#include "Port.h"
#include "OutputPort.h"
#include "UniqueId.h"

#include <string>
#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>

class InputPort;

/// <summary> Superclass for all node types. </summary>
class Node
{
public:
    /// <summary> Type to use for our node id </summary>
    typedef UniqueId NodeId;

    /// <summary> Returns the unique ID for this node </summary>
    const NodeId Id() const { return _id; }

    /// <summary> Returns the input "ports" for this node </summary>
    const std::vector<InputPort*>& GetInputs() const { return _inputs; }

    // Convenience functions to get various properties of the outputs
    template <typename ValueType>
    std::vector<ValueType> GetOutputValue(size_t outputIndex) const;

    Port::PortType GetOutputType(size_t outputIndex) const;

    size_t GetOutputSize(size_t outputIndex) const;

    /// <summary> Gets the name of this type (for serialization). </summary>
    ///
    /// <returns> The name of this type. </returns>
    virtual std::string GetRuntimeTypeName() const = 0;

    // Get all nodes that depend (directly) on us
    const std::vector<const Node*>& GetDependentNodes() const { return _dependentNodes; }

protected:
    // The constructor for Node is kind of gross. The arguments (and the _inputs and _outputs members)
    // should perhaps be vectors of references instead of pointers.
    Node(const std::vector<InputPort*>& inputs, const std::vector<OutputPortBase*>& outputs);

    // Compute is implemented by subclasses to calculate their result and set them in their outputs
    virtual void Compute() const {};
    void RegisterOutputs() const; // TODO

private:
    friend class DirectedGraph;
    void AddDependent(const Node* dependent) const;
    void AddDependencies() const;

    NodeId _id;

    std::vector<InputPort*> _inputs;
    std::vector<OutputPortBase*> _outputs;
    std::unordered_map<Port::PortId, OutputPortBase*> _outputs2;

    mutable std::vector<const Node*> _dependentNodes;
};

#include "../tcc/Node.tcc"
