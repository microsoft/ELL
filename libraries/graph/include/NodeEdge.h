#pragma once

//
// NodeEdge
// 

#include <vector>
#include <memory>

class Node;

//
// NodeEdge is the common base class for NodeInput and NodeOutput. 
//
class NodeEdge
{
public:
    enum class OutputType { None, Real, Integer, Categorical, Boolean };

    const class Node* Node() const { return _node; }
    size_t OutputIndex() const { return _outputIndex; }
    OutputType Type() const { return _type; }
    size_t Size() const { return _size; } // dimension

    // `GetTypeCode` maps from C++ type to OutputType enum
    template <typename ValueType>
    static OutputType GetTypeCode(); 

protected:
    NodeEdge(const class Node* node, size_t outputIndex, OutputType type, size_t size): _node(node), _outputIndex(outputIndex), _type(type), _size(size) {}
    
private:
    // _node and _outputIndex keep info on where the input is coming from (which "port")
    const class Node* _node = nullptr;
    size_t _outputIndex = 0;
    OutputType _type = OutputType::None;
    size_t _size = 0;
};
