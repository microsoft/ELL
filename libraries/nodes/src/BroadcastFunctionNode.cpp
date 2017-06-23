////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     BroadcastFunctionNode.cpp (nodes)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BroadcastFunctionNode.h"

namespace ell
{
namespace nodes
{
    //
    // BroadcastFunctionNode
    //

    BroadcastFunctionNode::BroadcastFunctionNode(const std::vector<model::InputPortBase*>& inputs, const std::vector<model::OutputPortBase*>& outputs)
        : CompilableNode(inputs, outputs) {}

    BroadcastFunctionNode::BroadcastFunctionNode(const std::vector<model::InputPortBase*>& inputs,
                                                 const Shape& inputStride, const Shape& inputOffset, const Shape& inputSize, size_t broadcastDimension,
                                                 const std::vector<model::OutputPortBase*>& outputs,
                                                 const Shape& outputStride, const Shape& outputOffset)
        : CompilableNode(inputs, outputs), _inputStride(inputStride), _inputOffset(inputOffset), _inputSize(inputSize), _outputStride(outputStride), _outputOffset(outputOffset), _broadcastDimension(broadcastDimension)
    {
    }

    size_t BroadcastFunctionNode::NumElements(const Shape& size)
    {
        size_t result = 1;
        for (auto s : size)
        {
            result *= s;
        }
        return result;
    }

    bool BroadcastFunctionNode::ShapesEqual(const Shape& shape1, const Shape& shape2)
    {
        auto size = shape1.size();
        if(size != shape2.size())
        {
            return false;
        }

        for(int index = 0; index < size; ++index)
        {
            if(shape1[index] != shape2[index])
            {
                return false;
            }
        }
        return true;
    }

} // nodes
} // ell
