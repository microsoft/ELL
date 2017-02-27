////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Model_test.h (model_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Model.h"

namespace ell
{
void TestNodeIterator();
void TestStaticModel();
void TestNodeIterator();
void TestExampleModel();

void TestInputRouting1();
void TestInputRouting2();

void TestCopyModel();

void TestRefineSplitOutputs();
void TestCustomRefine();
}