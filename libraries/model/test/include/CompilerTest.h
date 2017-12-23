////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilerTest.h (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Map.h"

// stl
#include <string>

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

void TestSimpleMap(bool optimize);
void TestSqEuclideanDistanceMap();
void TestProtoNNPredictorMap();
void TestMultiOutputMap();
void TestMultiOutputMap2();
void TestMultiSourceSinkMap();
void TestCompiledMapMove();

#include "../tcc/CompilerTest.tcc"
