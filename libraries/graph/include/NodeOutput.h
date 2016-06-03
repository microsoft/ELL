#pragma once

//
// NodeOutput
// 

#include <vector>
#include <memory>

class Node;

class NodeOutputBase
{
public:
    enum class OutputType { Real, Integer, Categorical, Boolean };    

    virtual ~NodeOutputBase() = default;
    OutputType GetType() { return _type; }
    const Node* GetNode() { return _node; }    

protected:
    NodeOutputBase(const Node* node, size_t size, OutputType type) : _node(node), _size(size), _type(type) {};

    template <typename ValueType>
    static OutputType GetTypeCode(); 
    
private:
    size_t _size;
    OutputType _type;
    const Node* _node;
};

template <typename ValueType>
class NodeOutput : public NodeOutputBase
{
public:
    NodeOutput(const Node* node, size_t size) : NodeOutputBase(node, size, NodeOutputBase::GetTypeCode<ValueType>()) {}

//    NodeOutput<ValueType> operator[](size_t index);

private:
    std::vector<ValueType> _cachedOutput;
};

template<>
NodeOutputBase::OutputType NodeOutputBase::GetTypeCode<double>() { return NodeOutputBase::OutputType::Real; }

template<>
NodeOutputBase::OutputType NodeOutputBase::GetTypeCode<int>() { return NodeOutputBase::OutputType::Integer; }

template<>
NodeOutputBase::OutputType NodeOutputBase::GetTypeCode<bool>() { return NodeOutputBase::OutputType::Boolean; }


// template <typename ValueType>
// NodeOutput<ValueType> NodeOutput<ValueType>::operator[](size_t index)
// {
//     return NodeOutput<ValueType>(1, index);
// }