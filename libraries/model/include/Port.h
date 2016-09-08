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
#include "IArchivable.h"

// stl
#include <vector>
#include <memory>
#include <string>

/// <summary> model namespace </summary>
namespace model
{
    class Node;

    /// <summary> Port is the common base class for InputPort and OutputPort. </summary>
    class Port: public utilities::IArchivable
    {
    public:
        enum class PortType
        {
            none,
            real,
            integer,
            categorical,
            boolean
        };

        Port() = default;
        Port(const Port& other) = default;
        virtual ~Port() = default;

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
        virtual size_t Size() const = 0;

        /// <summary> Returns the name of this port </summary>
        ///
        /// <returns> The name of this port </returns>
        std::string GetName() const { return _name; }

        /// <summary> Maps from C++ type to PortType enum </summary>
        ///
        /// <typeparam name="ValueType"> The C++ type </typeparam>
        /// <returns> The corresponding PortType enum value </returns>
        template <typename ValueType>
        static PortType GetPortType();

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        static std::string GetTypeName() { return "Port"; }

        /// <summary> Gets the name of this type (for serialization). </summary>
        ///
        /// <returns> The name of this type. </returns>
        virtual std::string GetRuntimeTypeName() const override { return GetTypeName(); }
        
        /// <summary> Adds an object's properties to an `Archiver` </summary>
        ///
        /// <param name="archiver"> The `Archiver` to add the values from the object to </param>
        virtual void WriteToArchive(utilities::Archiver& archiver) const override;

        /// <summary> Sets the internal state of the object according to the archiver passed in </summary>
        ///
        /// <param name="archiver"> The `Archiver` to get state from </param>
        virtual void ReadFromArchive(utilities::Unarchiver& archiver) override;

    protected:
        Port(const class Node* node, std::string name, PortType type) : _node(node), _name(name), _type(type) {}

    private:
        // _node keeps info on where the input is coming from
        const class Node* _node = nullptr;
        std::string _name;
        PortType _type = PortType::none;
    };
}
