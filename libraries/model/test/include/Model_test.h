////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Model_test.h (model_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

void TestStaticModel();
void TestNodeIterator();
void TestReverseNodeIterator();

void TestModelSerialization();
void TestModelMetadata();

void TestInputRouting();

void TestDeepCopyModel();
void TestShallowCopyModel();

void TestRefineSplitOutputs();
void TestCustomRefine();
void TestChangeInputForNode();
