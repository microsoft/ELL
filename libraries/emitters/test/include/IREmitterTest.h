////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IREmitterTest.h (emitters_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <ostream>
#include <string>

void TestLLVM();
void TestLLVMShiftRegister();
void TestIfElseComplex();
void TestIfElseBlockRegions(bool runJit = false);
void TestLogical();

void SetOutputPathBase(std::string path);
std::string OutputPath(const char* pRelPath);
