#pragma once

#include "Model.h"

//
// Model tests
//

namespace ell
{
// Helpful stuff
void PrintModel(const model::Model& model);
void PrintModel(const model::Model& model, const model::Node* output);
model::Model GetSimpleModel();
model::Model GetComplexModel();

// Tests
void TestNodeIterator();
void TestStaticModel();
void TestNodeIterator();
void TestExampleModel();

void TestInputRouting1();
void TestInputRouting2();

void TestCopyModel();

void TestRefineSplitOutputs();
}