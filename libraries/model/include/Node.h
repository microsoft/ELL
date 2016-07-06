////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Node.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "OutputPort.h"
#include "UniqueId.h"

// stl
#include <string>
#include <memory>
#include <vector>

/// <summary> model namespace </summary>
namespace model
{
    class InputPortBase;
    class Model;
    class ModelTransformer;

    /// <summary> Superclass for all node types. </summary>
    class Node
    {
    public:
        /// <summary> Type to use for our node id </summary>
        typedef utilities::UniqueId NodeId;

        /// <summary> Returns the unique ID for this node </summary>
        ///
        /// <returns> The unique ID for this node </returns>
        const NodeId GetId() const { return _id; }

        /// <summary> Returns the input "ports" for this node </summary>
        ///
        /// <returns> The input "ports" for this node </returns>
        const std::vector<InputPortBase*>& GetInputs() const { return _inputs; }

        /// <summary> Returns the output "ports" for this node </summary>
        ///
        /// <returns> The output "ports" for this node </returns>
        const std::vector<OutputPortBase*>& GetOutputs() const { return _outputs; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const = 0;

        /// <summary> Get all nodes that depend (directly) on us </summary>
        ///
        /// <returns> a vector of all the nodes that depend on this node </summary>
        const std::vector<const Node*>& GetDependentNodes() const { return _dependentNodes; }

        virtual void Copy(ModelTransformer& transformer) const = 0;
        virtual void Refine(ModelTransformer& transformer) const;
        virtual ~Node() {}

    protected:
        // TODO: the arguments (and the _inputs and _outputs members)
        // should perhaps be vectors of references instead of pointers.
        Node(const std::vector<InputPortBase*>& inputs, const std::vector<OutputPortBase*>& outputs);

        /// <summary> Computes the output of this node and stores it in the output ports </summary>
        virtual void Compute() const = 0;
        void AddInputPort(InputPortBase* input);

    private:
        friend class Model;
        friend class ModelTransformer;
        void AddDependent(const Node* dependent) const;
        void RegisterDependencies() const;

        NodeId _id;
        // TODO: these should probably be references, not pointers
        std::vector<InputPortBase*> _inputs;
        std::vector<OutputPortBase*> _outputs;

        mutable std::vector<const Node*> _dependentNodes;
    };
}
