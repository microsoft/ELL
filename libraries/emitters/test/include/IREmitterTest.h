////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IREmitterTest.h (emitters_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>

void SetOutputPathBase(std::string path);
std::string OutputPath(const char* pRelPath);

// Tests of low-level IREmitter class
void TestIREmitter();

// Tests for higher-level classes (mostly IRModuleEmitter)
void TestEmitLLVM();
void TestLLVMShiftRegister();
void TestHighLevelNestedIf();
void TestMixedLevelNestedIf();
void TestLogicalAnd();
void TestLogicalOr();
void TestLogicalNot();
void TestForLoop();
void TestWhileLoopWithVariableCondition();
void TestWhileLoopWithFunctionCondition();
void TestMetadata();
void TestHeader();
void TestTwoEmitsInOneSession();
void TestStruct();
void TestDuplicateStructs();

void TestScopedIf();
void TestScopedIfElse();
void TestScopedIfElse2();
void TestElseIfWithComputedCondition();
