////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     InputPort.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "OutputPort.h"
#include "Port.h"
#include "PortElements.h"

// utilities
#include "Exception.h"
#include "IArchivable.h"

// data
#include "IndexValue.h"

// stl
#include <cassert>
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
        /// <param name="inputs"> The input group to fetch input values from </param>
        /// <param name="name"> The name of this port </param>
        template <typename ValueType>
        InputPortBase(const class Node* owningNode, const PortElements<ValueType>& inputs, const std::string& name);

        /// <summary> Constructor </summary>
        ///
        /// <param name="owningNode"> The node that contains this port </param>
        /// <param name="inputs"> The input group to fetch input values from </param>
        /// <param name="name"> The name of this port </param>
        InputPortBase(const class Node* owningNode, const PortElementsBase& inputs, const std::string& name);

        /// <summary> Returns the list of nodes this input port gets values from </summary>
        ///
        /// <returns> The list nodes this input port gets values from </returns>
        const std::vector<const Node*>& GetParentNodes() const { return _parentNodes; }

        /// <summary> Returns the list of port ranges this input port gets values from </summary>
        ///
        /// <returns> The list nodes this input port gets values from </returns>
        const PortElementsBase& GetInputElements() const { return _inputElements; }

        /// <summary> The size of the output </summary>
        ///
        /// <returns> The size of the output </returns>
        virtual size_t Size() const override { return _inputElements.Size(); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "InputPortBase"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary>Gets the port element at the specified index</summary>
        ///
        /// <returns>The element at the specified index</returns>
        model::PortElementBase GetInputElement(size_t index) const
        {
            return _inputElements.GetElement(index);
        }

    protected:
        /// Note: Subclasses _must_ call this method in their constructor
        void ComputeParents();

    private:
        const PortElementsBase& _inputElements; // Just a reference to the typed elements in concrete subclass
        std::vector<const Node*> _parentNodes;
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
        InputPort(const class Node* owningNode, const PortElements<ValueType>& input, const std::string& name);

        /// <summary> Assignment operator </summary>
        ///
        /// <param name="other"> The port to assign to this one </param>
        /// <returns> A reference to this port </returns>
        InputPort& operator=(const InputPort& other);

        /// <summary> Returns the (already-computed) output value corresponding to this input </summary>
        ///
        /// <returns> The (already-computed) output value corresponding to this input </returns>
        std::vector<ValueType> GetValue() const;

        /// <summary> Returns an element from the (already-computed) output value corresponding to this input </summary>
        ///
        /// <param name="index"> The index of the element to return </param>
        /// <returns> The output value at the corresponding index </returns>
        ValueType GetValue(size_t index) const;

        /// <summary> Returns the PortElements containing the referenced locations this port gets its values from </summary>
        ///
        /// <returns> The PortElements containing the referenced locations to get values from </returns>
        PortElements<ValueType> GetPortElements() const;

        /// <summary> Returns an element from the (already-computed) output value corresponding to this input </summary>
        ///
        /// <param name="index"> The index of the element to return </param>
        /// <returns> The output value at the corresponding index </returns>
        ValueType operator[](size_t index) const;

        /// <summary> Gets a forward read-only index-value iterator that iterates over nonzeros. </summary>
        ///
        /// <returns> The iterator. </returns>
        PortElementsBase::Iterator GetIterator() const { return _input.GetIterator(); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return utilities::GetCompositeTypeName<ValueType>("InputPort"); }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }

    protected:
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    private:
        PortElements<ValueType> _input;
    };
}
}

#include "../tcc/InputPort.tcc"
