////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilerTest.h (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "InputNode.h"
#include "MapCompiler.h"
#include "OutputNode.h"

// nodes
#include "AccumulatorNode.h"
#include "BinaryOperationNode.h"
#include "BinaryPredicateNode.h"
#include "ConstantNode.h"
#include "DelayNode.h"
#include "DotProductNode.h"
#include "MultiplexerNode.h"
#include "SumNode.h"
#include "UnaryOperationNode.h"

// stl
#include <string>

void SetOutputPathBase(std::string path);
std::string OutputPath(std::string relPath);

ell::model::DynamicMap MakeSimpleMap();
ell::model::DynamicMap MakeForestMap();

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
void TestLinearPredictor();
void TestForest();
void TestForestMap();

void TestSimpleMap(bool optimize);
void TestSteppableMap(bool runJit = false);
void TestMultiOutputMap();
void TestMultiOutputMap2();
void TestCompiledMapMove();
