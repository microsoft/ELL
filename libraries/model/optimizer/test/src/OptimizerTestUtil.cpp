////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizerTestUtil.cpp (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OptimizerTestUtil.h"

#include <emitters/include/CompilerOptions.h>

#include <model/include/InputNode.h>
#include <model/include/Model.h>
#include <model/include/OutputNode.h>

#include <utilities/include/Exception.h>
#include <utilities/include/StlVectorUtil.h>

#include <algorithm>
#include <iostream>
#include <iterator>

using namespace ell;
using namespace ell::model;

namespace
{
template <typename Container, typename Function>
auto Transform(const Container& container, Function fn)
{
    return utilities::TransformVector(container.begin(), container.end(), fn);
}

template <typename T>
const OutputPort<T>& Input(Model& model, int size)
{
    auto inputNode = model.AddNode<InputNode<T>>(size);
    return inputNode->output;
}

template <typename T>
const OutputPort<T>& AddOutput(Model& model, const OutputPort<T>& input, std::string metadataKey = "")
{
    auto outputNode = model.AddNode<OutputNode<T>>(input);
    if (!metadataKey.empty())
    {
        outputNode->GetMetadata().SetEntry(metadataKey, "x");
    }
    return outputNode->output;
}

template <typename T>
const InputPort<T>& GetInputPort(const OutputPort<T>& output)
{
    // TODO: assert there's 1 and only 1 input port
    return dynamic_cast<const InputPort<T>&>(*output.GetNode()->GetInputPort(0));
}
} // namespace

bool IsOutputNode(const Node& node)
{
    return dynamic_cast<const OutputNodeBase*>(&node) != nullptr;
}

bool SubmodelsAreSameSize(const Submodel& a, const Submodel& b)
{
    return a.Size() == b.Size();
}

// Returns a simple submodel from a simple model.
// The model is a linear chain with an input node and 2 output nodes
// The submodel has a free input connected to the first output node, and the output of the last node
//
// model:     InputNode -> OutputNode1 -> OutputNode2
// submodel:            -> OutputNode1 -> OutputNode2 ->
Submodel GetSimpleSubmodel()
{
    Model m;
    const auto& in = Input<float>(m, 4);
    const auto& out1 = AddOutput(m, in);
    const auto& out2 = AddOutput(m, out1);
    return Submodel({ &GetInputPort(out1) }, { &out2 });
}

// Returns a simple submodel to test the CombineNodesTransformation
// The model is a linear chain with an input node and a series of output nodes, some
// of which contain metadata with the key 'a'.
// The submodel omits the first 3 nodes (the input and 2 output nodes)
//
// model:     InputNode -> Out1['a'] -> Out2['a'] -> Out3 -> Out4['a'] -> Out5['a'] -> Out6 -> Out7['a'] -> Out8['a'] -> Out9['a']  (10 nodes)
// submodel:                                      -> Out3 -> Out4['a'] -> Out5['a'] -> Out6 -> Out7['a'] -> Out8['a'] -> Out9['a']  (7 nodes)
// expected submodel after combining:             -> Out -> Out['b'] -> Out -> Out['b'] -> Out9  (5 nodes)
Submodel GetCombineNodesTestSubmodel()
{
    Model m;
    const auto& in = Input<float>(m, 4);
    const auto& out1 = AddOutput(m, in, "a");
    const auto& out2 = AddOutput(m, out1, "a");
    const auto& out3 = AddOutput(m, out2);
    const auto& out4 = AddOutput(m, out3, "a");
    const auto& out5 = AddOutput(m, out4, "a");
    const auto& out6 = AddOutput(m, out5);
    const auto& out7 = AddOutput(m, out6, "a");
    const auto& out8 = AddOutput(m, out7, "a");
    const auto& out9 = AddOutput(m, out8, "a");

    return Submodel({ &GetInputPort(out3) }, { &out9 });
}

TransformationTestData GetCombineNodesTestData()
{
    return { GetCombineNodesTestSubmodel(), 10, 7, 5 };
}
