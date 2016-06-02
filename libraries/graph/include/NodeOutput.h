#pragma once

//
// NodeOutput
// 

#include <vector>
#include <memory>

template <typename ValueType>
class NodeOutput
{
public:
    NodeOutput(size_t size);

    NodeOutput<ValueType> operator[](size_t index);


private:
    // Note: this is not a real thing. It's a debugging tool only available in debug mode.
    
};

template <typename ValueType>
NodeOutput<ValueType> NodeOutput<ValueType>::operator[](size_t index)
{
    return NodeOutput<ValueType>(1, index);
}