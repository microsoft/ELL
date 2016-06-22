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
        }; // Maybe these should be C types (double, int, bool) ? TODO: put this type info somewhere common, not in Port

        /// <summary> Returns the node the output port connected to this port belongs to </summary>
        const class Node* Node() const { return _node; }

        /// <summary> Returns the index of the port within the node </summary>
        size_t Index() const { return _portIndex; }

        /// <summary> Returns the datatype of the output </summary>
        PortType Type() const { return _type; }

        /// <summary> Returns the dimensionality of the output </summary>
        size_t Size() const { return _size; } // dimension

        /// <summary> Maps from C++ type to PortType enum </summary>
        template <typename ValueType>
        static PortType GetTypeCode();

        void SetType(PortType type) { _type = type; }
        void SetSize(size_t size) { _size = size; }

    protected:
        Port(const class Node* node, size_t outputIndex, PortType type, size_t size) : _node(node), _portIndex(outputIndex), _type(type), _size(size) {}

    private:
        // _node and _portIndex keep info on where the input is coming from
        const class Node* _node = nullptr;
        size_t _portIndex = 0;
        PortId _outputId;
        PortId _id;
        PortType _type = PortType::None;
        size_t _size = 0;
    };
}
