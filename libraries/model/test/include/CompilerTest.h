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

namespace ell
{
void SetOutputPathBase(std::string path);
std::string OutputPath(const char* pRelPath);
std::string OutputPath(const std::string& relPath);

model::DynamicMap MakeSimpleMap();

model::Model MakeForestModel();
model::DynamicMap MakeForestMap();

void TestCompiledMapMove();
void TestBinaryVector(bool expanded, bool runJit = false);
void TestBinaryScalar();
void TestDotProduct();
void TestSum(bool expanded);
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
}