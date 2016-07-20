////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Port.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "UniqueId.h"
#include "ISerializable.h"

// stl
#include <vector>
#include <memory>
#include <string>

/// <summary> model namespace </summary>
namespace model
{
    class Node;

    /// <summary> Port is the common base class for InputPort and OutputPort. </summary>
    class Port : utilities::ISerializable
    {
    public:
        enum class PortType
        {
            None,
            Real,
            Integer,
            Categorical,
            Boolean
        };

        /// <summary> Returns the node the output port connected to this port belongs to </summary>
        ///
        /// <returns> The node the output port connected to this port belongs to </returns>
        const class Node* GetNode() const { return _node; }

        /// <summary> Returns the datatype of the output </summary>
        ///
        /// <returns> The datatype of the output </returns>
        PortType GetType() const { return _type; }

        /// <summary> Returns the dimensionality of the output </summary>
        ///
        /// <returns> The dimensionality of the output </returns>
        size_t Size() const { return _size; } // dimension

        /// <summary> Returns the name of this port </summary>
        ///
        /// <returns> The name of this port </returns>
        std::string GetName() const { return _name; }

        /// <summary> Maps from C++ type to PortType enum </summary>
        ///
        /// <typeparam name="ValueType"> The C++ type </typeparam>
        /// <returns> The corresponding PortType enum value </returns>
        template <typename ValueType>
        static PortType GetTypeCode();

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "Port"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const { return GetTypeName(); }

        /// Inherited from ISerializable
        virtual utilities::ObjectDescription GetDescription() const override;

        virtual ~Port() = default;

    protected:
        Port(const class Node* node, std::string name, PortType type, size_t size) : _node(node), _name(name), _type(type), _size(size) {}

    private:
        // _node keeps info on where the input is coming from
        const class Node* _node = nullptr;
        std::string _name;
        PortType _type = PortType::None;
        size_t _size = 0;
    };
}
