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
void TestIfElseComplex();
void TestIfElseBlockRegions(bool runJit = false);
void TestLogical();
void TestForLoop(bool runJit = false);
void TestWhileLoop();
void TestMetadata();
void TestHeader();
void TestTwoEmitsInOneSession();
void TestStruct();
void TestDuplicateStructs();

void TestScopedIf();
void TestScopedIfElse();
void TestScopedIfElse2();
