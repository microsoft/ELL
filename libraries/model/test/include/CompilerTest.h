////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilerTest.h (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model_testing/include/ModelTestUtilities.h>

#include <emitters/include/IREmitter.h>

#include <model/include/CompiledMap.h>
#include <model/include/IRCompiledMap.h>
#include <model/include/IRMapCompiler.h>
#include <model/include/Map.h>
#include <model/include/OutputNode.h>

#include <nodes/include/LinearPredictorNode.h>

#include <string>

using namespace ell;

void SetOutputPathBase(std::string path);
std::string OutputPath(std::string relPath);

ell::model::Map MakeSimpleMap();
ell::model::Map MakeForestMap();

void TestBinaryVector(bool expanded, bool runJit = false);
void TestBinaryScalar();
void TestDotProduct();
void TestSimpleSum(bool expanded, bool optimize = false);
void TestSum(bool expanded, bool optimize = false);
void TestAccumulator(bool expanded);
void TestDelay();
void TestSqrt();
void TestBinaryPredicate(bool expanded);
void TestMultiplexer();
void TestSlidingAverage();
void TestDotProductOutput();
template <typename ElementType>
void TestLinearPredictor();
void TestForest();
void TestForestMap();
void TestNodeMetadata();

void TestSimpleMap(bool optimize);
void TestSqEuclideanDistanceMap();
void TestProtoNNPredictorMap();
void TestCombineOutputMap();
void TestMultiOutputMap();
void TestMultiSourceSinkMap();
void TestCompiledMapMove();

#include "../tcc/CompilerTest.tcc"
