////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Node.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "UniqueId.h"

// utilities
#include "IArchivable.h"

// stl
#include <memory>
#include <string>
#include <vector>
#include <ostream>

namespace ell
{
/// <summary> model namespace </summary>
namespace model
{
    class ModelTransformer;
    class InputPortBase;
    class OutputPortBase;
    class Port;

    /// <summary> Superclass for all node types. </summary>
    class Node : public utilities::IArchivable
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

        /// <summary> Returns the number of input ports for this node </summary>
        ///
        /// <returns> The number of input ports </returns>
        size_t NumInputPorts() const { return _inputs.size(); }

        /// <summary> Returns the number of output ports for this node </summary>
        ///
        /// <returns> The number of output ports </returns>
        size_t NumOutputPorts() const { return _outputs.size(); }

        /// <summary> Returns the input ports for this node </summary>
        ///
        /// <returns> The input "ports" for this node </returns>
        const std::vector<InputPortBase*>& GetInputPorts() const { return _inputs; }

        /// <summary> Returns the named input port </summary>
        ///
        /// <param name="portName"> The name of the port </param>
        /// <returns> A pointer to the port </returns>
        InputPortBase* GetInputPort(const std::string& portName);

        /// <summary> Returns the named input port </summary>
        ///
        /// <param name="portName"> The name of the port </param>
        /// <returns> A pointer to the port </returns>
        const InputPortBase* GetInputPort(const std::string& portName) const;

        /// <summary> Returns the output "ports" for this node </summary>
        ///
        /// <returns> The output "ports" for this node </returns>
        const std::vector<OutputPortBase*>& GetOutputPorts() const { return _outputs; }

        /// <summary> Returns the named output port </summary>
        ///
        /// <param name="portName"> The name of the port </param>
        /// <returns> A pointer to the port </returns>
        OutputPortBase* GetOutputPort(const std::string& portName);

        /// <summary> Returns the named output port </summary>
        ///
        /// <param name="portName"> The name of the port </param>
        /// <returns> A pointer to the port </returns>
        const OutputPortBase* GetOutputPort(const std::string& portName) const;

        /// <summary> Returns an output port by index </summary>
        ///
        /// <param name="portIndex"> The index of the port </param>
        /// <returns> A pointer to the port </returns>
        OutputPortBase* GetOutputPort(size_t portIndex);

        /// <summary> Returns an output port by index </summary>
        ///
        /// <param name="portIndex"> The index of the port </param>
        /// <returns> A pointer to the port </returns>
        const OutputPortBase* GetOutputPort(size_t portIndex) const;

        /// <summary> Returns the named port </summary>
        ///
        /// <param name="portName"> The name of the port </param>
        /// <returns> A pointer to the port </returns>
        Port* GetPort(const std::string& portName);

        /// <summary> Returns the named port </summary>
        ///
        /// <param name="portName"> The name of the port </param>
        /// <returns> A const pointer to the port </returns>
        const Port* GetPort(const std::string& portName) const;

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

        /// <summary> Indicates if this node is able to compile itself to code. </summary>
        virtual bool IsCompilable() const { return false; }

        /// <summary> Makes a copy of this node into the model being constructed by the transformer </summary>
        ///
        /// <param name="transformer"> The `ModelTransformer` object currently creating a new model </param>
        virtual void Copy(ModelTransformer& transformer) const = 0;

        /// <summary> Print a human-readable representation of the Node. </summary>
        ///
        /// <param name="os"> The stream to write data to. </param>
        void Print(std::ostream& os) const;

    protected:
        Node(const std::vector<InputPortBase*>& inputs, const std::vector<OutputPortBase*>& outputs);

        /// <summary> Refines this node in the model being constructed by the transformer </summary>
        virtual bool Refine(ModelTransformer& transformer) const;

        /// <summary> Computes the output of this node and stores it in the output ports </summary>
        virtual void Compute() const = 0;
        virtual bool HasState() const;

        void AddInputPort(InputPortBase* input);
        void AddOutputPort(OutputPortBase* output);

        // We're supplying a base implementation from WriteToArchive and ReadFromArchive, but also
        // declaring them as abstract so that subclasses need to implement this themselves.
        virtual void WriteToArchive(utilities::Archiver& archiver) const override = 0; 
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override = 0;

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
}
