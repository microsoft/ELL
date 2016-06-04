#pragma once

//
// NodeOutput
// 

#include <vector>
#include <memory>

class Node;

// TODO: get rid of size somehow?
class NodeOutputBase
{
public:
    enum class OutputType { None, Real, Integer, Categorical, Boolean };    

    virtual ~NodeOutputBase() = default;
    const Node* GetNode() const { return _node; }
    size_t GetOutputIndex() const { return _outputIndex; }
    size_t GetSize() const { return _size; } // dimension
    OutputType GetType() const { return _type; }

protected:
    NodeOutputBase(const Node* node, size_t outputIndex, size_t size, OutputType type) : _node(node), _outputIndex(outputIndex), _size(size), _type(type) {};

    template <typename ValueType>
    static OutputType GetTypeCode(); 
    
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