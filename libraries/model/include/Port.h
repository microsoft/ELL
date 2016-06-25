////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Port.h (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <memory>

/// <summary> model namespace </summary>
namespace model
{
    class Node;

    /// <summary> Port is the common base class for InputPort and OutputPort. </summary>
    class Port
    {
    public:
        typedef int PortId;
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

        /// <summary> Maps from C++ type to PortType enum </summary>
        ///
        /// <typeparam name="ValueType"> The C++ type </typeparam>
        /// <returns> The corresponding PortType enum value </returns>
        template <typename ValueType>
        static PortType GetTypeCode();

    protected:
        Port(const class Node* node, PortType type, size_t size) : _node(node), _type(type), _size(size) {}

    private:
        // _node keeps info on where the input is coming from
        const class Node* _node = nullptr;

        PortId _id;
        PortType _type = PortType::None;
        size_t _size = 0;
    };
}
