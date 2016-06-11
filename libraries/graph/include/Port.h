#pragma once

//
// Port
// 

#include <vector>
#include <memory>

class Node;

//
// Port is the common base class for InputPort and OutputPort. 
//
class Port
{
public:
    typedef int PortId;
    enum class PortType { None, Real, Integer, Categorical, Boolean };

    // The node the output port connected to this port belongs to
    const class Node* Node() const { return _node; }
    
    // The index of the output port within the node
    size_t OutputIndex() const { return _outputIndex; }
    
    // The datatype of the output
    PortType Type() const { return _type; }
    
    // The dimensionality of the output
    size_t Size() const { return _size; } // dimension

    // `GetTypeCode` maps from C++ type to PortType enum
    template <typename ValueType>
    static PortType GetTypeCode(); 

protected:
    Port(const class Node* node, size_t outputIndex, PortType type, size_t size): _node(node), _outputIndex(outputIndex), _type(type), _size(size) {}
    
private:
    // _node and _outputIndex keep info on where the input is coming from
    const class Node* _node = nullptr;
    size_t _outputIndex = 0;
    PortId _outputId;
    PortId _id; 
    PortType _type = PortType::None;
    size_t _size = 0;
};
