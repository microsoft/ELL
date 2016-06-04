#pragma once

//
// NodeOutput
// 

#include <vector>
#include <memory>
#include <iostream>

class Node;

// TODO: rename NodeInput to something more general, add a _type and _size field to it, and use it as the base class for NodeOutput<T>.
// Maybe then just make a typedef of that base class to NodeInput to the code is less confusing.
// Maybe call the new base class NodeEdge.  
class NodeOutputBase
{
public:
    enum class OutputType { None, Real, Integer, Categorical, Boolean };    

    const Node* GetNode() const { return _node; }
    size_t GetOutputIndex() const { return _outputIndex; }
    size_t GetSize() const { return _size; } // dimension
    OutputType GetType() const { return _type; }

    template <typename ValueType>
    static OutputType GetTypeCode(); 

protected:
    NodeOutputBase(const Node* node, size_t outputIndex, size_t size, OutputType type) : _node(node), _outputIndex(outputIndex), _size(size), _type(type) 
    {
    };
    
private:
    const Node* _node = nullptr;
    size_t _outputIndex = 0;

    size_t _size = 0;
    OutputType _type = OutputType::None;
};

template <typename ValueType>
class NodeOutput : public NodeOutputBase
{
public:
    NodeOutput(const Node* node, size_t outputIndex, size_t size) : NodeOutputBase(node, outputIndex, size, NodeOutputBase::GetTypeCode<ValueType>()) {}

//    NodeOutput<ValueType> operator[](size_t index);

private:
    std::vector<ValueType> _cachedOutput;
};

// template <typename ValueType>
// NodeOutput<ValueType> NodeOutput<ValueType>::operator[](size_t index)
// {
//     return NodeOutput<ValueType>(1, index);
// }
