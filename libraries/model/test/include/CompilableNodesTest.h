////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableNodesTest.h (compile_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace ell
{
void TestCompileIsEqualModel();
void TestCompilableScalarOutputNode();
void TestCompilableVectorOutputNode();
void TestCompilableAccumulatorNode();
void TestCompilableConstantNode();
void TestCompilableDotProductNode();
void TestCompilableDelayNode();
void TestCompilableDTWDistanceNode();
void TestCompilableMulticlassDTW();
void TestCompilableScalarSumNode();
void TestCompilableSumNode();
void TestCompilableUnaryOperationNode();
void TestCompilableBinaryOperationNode();
void TestCompilableScalarBinaryPredicateNode();
void TestCompilableBinaryPredicateNode();
void TestCompilableMultiplexerNode();
void TestCompilableTypeCastNode();
void TestCompilableAccumulatorNodeFunction();
void TestCompilableSourceNode(bool runJit);
}