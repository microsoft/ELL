////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Port.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "IArchivable.h"
#include "PropertyBag.h"

// stl
#include <string>

namespace ell
{
/// <summary> model namespace </summary>
namespace model
{
    class Node;

    /// <summary> Port is the common base class for InputPort and OutputPort. </summary>
    class Port : public utilities::IArchivable
    {
    public:
        enum class PortType
        {
            none = 0,
            smallReal, // == float
            real, // == double
            integer, // == int32
            bigInt, // == int64
            categorical,
            boolean
        };

        Port() = default;
        Port(const Port& other) = delete;
        Port(Port&& other) = delete;

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
        std::string GetRuntimeTypeName() const override { return GetTypeName(); }

        /// <summary> Get this object's metadata object. </summary>
        ///
        /// <returns> A reference to the PropertyBag containing the metadata for this object. </returns>
        utilities::PropertyBag& GetMetadata() { return _metadata; }
        
        /// <summary> Get this object's metadata object. </summary>
        ///
        /// <returns> A const reference to the PropertyBag containing the metadata for this object. </returns>
        const utilities::PropertyBag& GetMetadata() const { return _metadata; }
        
    protected:
        Port(const class Node* node, std::string name, PortType type)
            : _node(node), _name(name), _type(type) {}
        ~Port() override = default;

        utilities::ArchiveVersion GetArchiveVersion() const override;
        bool CanReadArchiveVersion(const utilities::ArchiveVersion& version) const override;
        void WriteToArchive(utilities::Archiver& archiver) const override;
        void ReadFromArchive(utilities::Unarchiver& archiver) override;
    
    private:

        // _node keeps info on where the input is coming from
        const class Node* _node = nullptr;
        std::string _name;
        PortType _type = PortType::none;
        utilities::PropertyBag _metadata;
    };

    /// <summary> Returns a string with the 'C' type name to use for the given `PortType` </summary>
    ///
    /// <param name="type"> The type of the port </param>
    /// <returns> A string representation of the C type to use </returns>
    std::string GetPortCTypeName(ell::model::Port::PortType type);

    template <Port::PortType portType>
    struct PortTypeToValueType
    {
        typedef int value_type;
    };

    template <Port::PortType portType>
    using ValueType = typename PortTypeToValueType<portType>::value_type;
}
}

#include "../tcc/Port.tcc"
