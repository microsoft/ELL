////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     PerformanceCountersTest.cpp (compile_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PerformanceCountersTest.h"
#include "ModelTestUtilities.h"

// model
#include "CompiledMap.h"
#include "DynamicMap.h"
#include "EmitterException.h"
#include "EmitterTypes.h"
#include "IRCompiledMap.h"
#include "IRMapCompiler.h"
#include "InputNode.h"
#include "Model.h"
#include "OutputNode.h"

// nodes
#include "ConstantNode.h"
#include "MatrixMatrixMultiplyNode.h"

// testing
#include "testing.h"

// utilities
#include "RandomEngines.h"

// stl
#include <iostream>
#include <ostream>
#include <string>

using namespace ell;

std::vector<double> GenerateMatrixValues(size_t m, size_t n)
{
    auto rnd = utilities::GetRandomEngine("123");

    auto size = m * n;
    std::vector<double> result(size);
    for (int index = 0; index < size; ++index)
    {
        result[index] = (double)rnd() / (double)(rnd.max() - rnd.min());
    }
    return result;
}

void TestPerformanceCounters()
{
    model::Model model;
    int m = 20;
    int k = 50;
    int n = 30; // (m x k) x (k x n) ==> (m x n)
    int numIter = 4;

    std::vector<std::vector<double>> matrix1Series;
    for (int index = 0; index < numIter; ++index)
    {
        matrix1Series.push_back(GenerateMatrixValues(m, k));
    }
    std::vector<double> matrix2Values = GenerateMatrixValues(k, n);

    auto inputNode = model.AddNode<model::InputNode<double>>(m * k);
    auto matrix2Node = model.AddNode<nodes::ConstantNode<double>>(matrix2Values);
    auto matrixMultNode = model.AddNode<nodes::MatrixMatrixMultiplyNode<double>>(inputNode->output, m, n, k, k, matrix2Node->output, n, n);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", matrixMultNode->output } });

    model::MapCompilerParameters settings;
    settings.profile = true;

    // Create first map
    model::IRMapCompiler compiler1(settings);
    auto compiledMap1 = compiler1.Compile(map);

    // Create second map
    settings.profile = true;
    settings.compilerSettings.useBlas = true;
    model::IRMapCompiler compiler2(settings);
    auto compiledMap2 = compiler2.Compile(map);

    for (const auto& input : matrix1Series)
    {
        compiledMap1.SetInputValue(0, input);
        auto compiledResult = compiledMap1.ComputeOutput<double>(0);
    }

    for (const auto& input : matrix1Series)
    {
        compiledMap2.SetInputValue(0, input);
        auto compiledResult = compiledMap2.ComputeOutput<double>(0);
    }

    // Test the functions
    int numNodes = compiledMap1.GetNumProfiledNodes();
    testing::ProcessTest("ModelProfiler GetNumNodes", numNodes == compiledMap1.GetModel().Size());

    // Now print out results
    std::cout << "Map 1 results (no BLAS)" << std::endl;
    compiledMap1.PrintNodeProfilingInfo();
    std::cout << std::endl
              << std::endl;
    compiledMap1.PrintNodeTypeProfilingInfo();

    std::cout << std::endl
              << "----------------" << std::endl
              << std::endl;

    std::cout << "Map 2 results (with BLAS)" << std::endl;
    compiledMap2.PrintNodeProfilingInfo();
    std::cout << std::endl
              << std::endl;
    compiledMap2.PrintNodeTypeProfilingInfo();

    std::cout << std::endl
              << "----------------" << std::endl
              << std::endl;

    std::cout << "Resetting map 2 results" << std::endl;
    compiledMap2.ResetNodeProfilingInfo();
    compiledMap2.PrintNodeProfilingInfo();

    // Now test functions to iterate over node info and perf counter structs
    std::cout << std::endl
              << "----------------" << std::endl
              << std::endl;
    for (int nodeIndex = 0; nodeIndex < numNodes; ++nodeIndex)
    {
        auto nodeInfo = compiledMap1.GetNodeInfo(nodeIndex);
        auto nodeStats = compiledMap1.GetNodePerformanceCounters(nodeIndex);
        std::cout << "Node [" << nodeIndex << "]: " << nodeInfo->nodeName << " = " << nodeInfo->nodeType << std::endl;
        testing::ProcessTest("ModelProfiler GetNodePerformanceCounters", nodeStats->count == numIter);
    }
}
