////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Node.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Port.h"
#include "OutputPort.h"
#include "UniqueId.h"

#include <string>
#include <memory>
#include <vector>
#include <unordered_set>
#include <unordered_map>

/// <summary> model namespace </summary>
namespace model
{
    class InputPort;

    /// <summary> Superclass for all node types. </summary>
    class Node
    {
    public:
        /// <summary> Type to use for our node id </summary>
        typedef UniqueId NodeId;

        /// <summary> Returns the unique ID for this node </summary>
        const NodeId GetId() const { return _id; }

        /// <summary> Returns the input "ports" for this node </summary>
        const std::vector<InputPort*>& GetInputs() const { return _inputs; }

        /// <summary> Convenience function to get the output value from a port </summary>
        ///
        /// <param name="outputIndex"> The index of the output port </param>
        /// <returns> The computed output value for the port </param>
        template <typename ValueType>
        std::vector<ValueType> GetOutputValue(size_t outputIndex) const;

        /// <summary> Convenience function to get the output type from a port </summary>
        ///
        /// <param name="outputIndex"> The index of the output port </param>
        /// <returns> The output type for the port </param>
        Port::PortType GetOutputType(size_t outputIndex) const;

        /// <summary> Convenience function to get the dimensionality of a port </summary>
        ///
        /// <param name="outputIndex"> The index of the output port </param>
        /// <returns> The dimensionality of the port </param>
        size_t GetOutputSize(size_t outputIndex) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const = 0;

        /// <summary> Get all nodes that depend (directly) on us </summary>
        ///
        /// <returns> a vector of all the nodes that depend on this node </summary>
        const std::vector<const Node*>& GetDependentNodes() const { return _dependentNodes; }

    protected:
        // The constructor for Node is kind of gross. The arguments (and the _inputs and _outputs members)
        // should perhaps be vectors of references instead of pointers.
        Node(const std::vector<InputPort*>& inputs, const std::vector<OutputPortBase*>& outputs);

        /// <summary> Computes the output of this node and stores it in the output ports </summary>
        virtual void Compute() const = 0;
        void RegisterOutputs() const; // TODO

    private:
        friend class Model;
        void AddDependent(const Node* dependent) const;
        void RegisterDependencies() const;

        NodeId _id;
        std::vector<InputPort*> _inputs;
        std::vector<OutputPortBase*> _outputs;
        mutable std::vector<const Node*> _dependentNodes;
    };
}

#include "../tcc/Node.tcc"
