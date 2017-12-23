////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableNodesTest.cpp (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs, Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelTestUtilities.h"

// model
#include "CompiledMap.h"
#include "Map.h"
#include "IRCompiledMap.h"
#include "IREmitter.h"
#include "IRMapCompiler.h"

// nodes
#include "ConstantNode.h"
#include "DotProductNode.h"

using namespace ell;

template <typename ElementType>
void TestCompilableDotProductNode2(int dimension)
{
    model::Model model;
    std::vector<ElementType> constValue(dimension);
    for (int index = 0; index < dimension; ++index)
    {
        constValue[index] = index + 0.5;
    }
    auto inputNode = model.AddNode<model::InputNode<ElementType>>(dimension);
    auto constantNode = model.AddNode<nodes::ConstantNode<ElementType>>(constValue);
    auto dotNode = model.AddNode<nodes::DotProductNode<ElementType>>(inputNode->output, constantNode->output);
    auto map = model::Map(model, { { "input", inputNode } }, { { "output", dotNode->output } });
    model::IRMapCompiler compiler;
    auto compiledMap = compiler.Compile(map);
    PrintIR(compiledMap);

    // compare output
    std::vector<std::vector<ElementType>> signal;
    for (int index1 = 0; index1 < 8; ++index1)
    {
        std::vector<ElementType> x;
        for (int index2 = 0; index2 < dimension; ++index2)
        {
            x.push_back(index2);
        }
        signal.push_back(x);
    }

    VerifyCompiledOutput(map, compiledMap, signal, "DotProductNode");
}
