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

// utilities
#include "ISerializable.h"
#include "ObjectDescription.h"

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
    class Node : public utilities::IDescribable
    {
    public:
        Node() = default;
        virtual ~Node() = default;

        /// <summary> Type to use for our node id </summary>
        typedef utilities::UniqueId NodeId;

        /// <summary> Returns the unique ID for this node </summary>
        ///
        /// <returns> The unique ID for this node </returns>
        const NodeId GetId() const { return _id; }

        /// <summary> Returns the input "ports" for this node </summary>
        ///
        /// <returns> The input "ports" for this node </returns>
        const std::vector<InputPortBase*>& GetInputPorts() const { return _inputs; }

        /// <summary> Returns the output "ports" for this node </summary>
        ///
        /// <returns> The output "ports" for this node </returns>
        const std::vector<OutputPortBase*>& GetOutputPorts() const { return _outputs; }

        /// <summary> Get all nodes that this nodes uses for input (and therefore depends on) </summary>
        ///
        /// <returns> a vector of all the nodes used for input </summary>
        std::vector<const Node*> GetParentNodes() const;

        /// <summary> Get all nodes that depend (directly) on us </summary>
        ///
        /// <returns> a vector of all the nodes that depend on this node </summary>
        const std::vector<const Node*>& GetDependentNodes() const { return _dependentNodes; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "Node"; }

        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        virtual void Serialize(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        virtual void Deserialize(utilities::Unarchiver& archiver) override;

        
    protected:
        Node(const std::vector<InputPortBase*>& inputs, const std::vector<OutputPortBase*>& outputs);

        /// <summary> Makes a copy of this node in the model being constructed by the transformer. </summary>
        ///
        /// <param name="transformer"> [in,out] The transformer. </param>
        virtual void Copy(ModelTransformer& transformer) const = 0;

        /// <summary> Refines this node in the model being constructed by the transformer </summary>
        virtual bool Refine(ModelTransformer& transformer) const;

        /// <summary> Computes the output of this node and stores it in the output ports </summary>
        virtual void Compute() const = 0;
        void AddInputPort(InputPortBase* input);

    private:
        friend class Model;
        friend class ModelTransformer;
        void AddDependent(const Node* dependent) const;
        void RegisterDependencies() const;
        void InvokeCopy(ModelTransformer& transformer) const;
        bool InvokeRefine(ModelTransformer& transformer) const;

        NodeId _id;
        std::vector<InputPortBase*> _inputs;
        std::vector<OutputPortBase*> _outputs;

        mutable std::vector<const Node*> _dependentNodes;
    };
}
