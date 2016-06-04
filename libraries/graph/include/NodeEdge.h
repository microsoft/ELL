#pragma once

//
// NodeEdge
// 

#include <vector>
#include <memory>

class Node;

class NodeEdge
{
public:
    enum class OutputType { None, Real, Integer, Categorical, Boolean };

    const Node* GetNode() const { return _node; }
    size_t GetOutputIndex() const { return _outputIndex; }

protected:
    NodeEdge(const Node* node, size_t outputIndex): _node(node), _outputIndex(outputIndex) {}
    
    // `GetTypeCode` maps from C++ type to OutputType enum
    template <typename ValueType>
    static OutputType GetTypeCode(); 

private:
    // _node and _outputIndex keep info on where the input is coming from (which "port")
    const Node* _node = nullptr;
    size_t _outputIndex = 0;
};
