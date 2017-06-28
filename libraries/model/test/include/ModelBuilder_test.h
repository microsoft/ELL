////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelBuilder_test.h (model_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

// Test passthrough to Model::AddNode
void TestTemplateAddNode();

// Test ModelBuilder::GetAddNodeArgs(string)
void TestInputNodeGetArgumentTypes();
void TestConstantNodeGetArgumentTypes();
void TestOutputNodeGetArgumentTypes();
void TestBinaryOpNodeGetArgumentTypes();

// Test ModelBuilder::AddNode(string, vector<Variant>) with exact types
void TestVariantAddInputNodes();
void TestVariantAddConstantNodes();
void TestVariantAddOutputNode();
void TestVariantAddBinaryOpNode();

// Test ModelBuilder::AddNode(string, vector<Variant>) with convertable types
void TestVariantAddInputNodesConvertableArgs();

// Test ModelBuilder::AddNode(string, vector<string>)
void TestVariantAddInputNodesParsedArgs();
