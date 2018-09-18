////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InputPort.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Model.h"
#include "Node.h"
#include "OutputPort.h"
#include "Port.h"
#include "PortElements.h"

// utilities
#include "Exception.h"
#include "IArchivable.h"

// stl
#include <string>
#include <vector>

namespace ell
{
/// <summary> model namespace </summary>
namespace model
{
    class Node;

    /// <summary> Class representing an input to a node </summary>
    class InputPortBase : public Port
    {
    public:
        /// <summary> Constructor </summary>
        ///
        /// <param name="owningNode"> The node that contains this port </param>
        /// <param name="input"> The port to fetch input values from </param>
        /// <param name="name"> The name of this port </param>
        template <typename ValueType>
        InputPortBase(const Node* owningNode, const OutputPort<ValueType>& input, const std::string& name);

        /// <summary> Constructor </summary>
        ///
        /// <param name="owningNode"> The node that contains this port </param>
        /// <param name="input"> The port to fetch input values from </param>
        /// <param name="name"> The name of this port </param>
        InputPortBase(const Node* owningNode, const OutputPortBase& input, const std::string& name);

        ~InputPortBase() override = default;

        /// <summary> Returns the list of nodes this input port gets values from </summary>
        ///
        /// <returns> The list nodes this input port gets values from </returns>
        std::vector<const Node*> GetParentNodes() const;

        /// <summary> Returns the output port this port gets its values from </summary>
        ///
        /// <returns> The OutputPort to get values from </returns>
        const OutputPortBase& GetReferencedPort() const;

        /// <summary> The size of the output </summary>
        ///
        /// <returns> The size of the output </returns>
        size_t Size() const override;

        /// <summary> Indicates if this input port has its input set </summary>
        ///
        /// <returns> true if this port is connected to an output port </returns>
        bool IsValid() const;

        /// <summary> Returns the memory layout of the output </summary>
        ///
        /// <returns> The memory layout of the output </returns>
        PortMemoryLayout GetMemoryLayout() const override;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "InputPortBase"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Gets the port element at the specified index </summary>
        ///
        /// <returns> The element at the specified index </returns>
        PortElementBase GetInputElement(size_t index) const;

    protected:
        InputPortBase(Port::PortType portType);
        const OutputPortBase* _referencedPort;

    private:
        friend class ModelEditor;
        friend class ModelTransformer;

        void SetInput(const OutputPortBase* input);
    };

    template <typename ValueType>
    class InputPort : public InputPortBase
    {
    public:
        /// <summary> Default Constructor </summary>
        InputPort();

        /// <summary> Creates an input port </summary>
        ///
        /// <param name="owningNode"> The node this port belongs to </param>
        /// <param name="input"> A reference to the output port(s) this input port is consuming from </param>
        InputPort(const Node* owningNode, const OutputPort<ValueType>& input, const std::string& name);

        /// <summary> Assignment operator </summary>
        ///
        /// <param name="other"> The port to assign to this one </param>
        /// <returns> A reference to this port </returns>
        InputPort& operator=(const InputPort& other);

        /// <summary> Returns the PortElements containing the referenced locations this port gets its values from </summary>
        ///
        /// <returns> The PortElements containing the referenced locations to get values from </returns>
        PortElements<ValueType> GetPortElements() const;
        
        /// <summary> Returns the port this port gets its values from </summary>
        ///
        /// <returns> The OutputPort to get values from </returns>
        const OutputPort<ValueType>& GetReferencedPort() const;

        /// <summary> Returns the (already-computed) output value corresponding to this input </summary>
        ///
        /// <returns> The (already-computed) output value corresponding to this input </returns>
        std::vector<ValueType> GetValue() const;

        /// <summary> Returns an element from the (already-computed) output value corresponding to this input </summary>
        ///
        /// <param name="index"> The index of the element to return </param>
        /// <returns> The output value at the corresponding index </returns>
        ValueType GetValue(size_t index) const;

        /// <summary> Returns an element from the (already-computed) output value corresponding to this input </summary>
        ///
        /// <param name="index"> The index of the element to return </param>
        /// <returns> The output value at the corresponding index </returns>
        ValueType operator[](size_t index) const;

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("InputPort"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
    };
}
}

#include "../tcc/InputPort.tcc"
