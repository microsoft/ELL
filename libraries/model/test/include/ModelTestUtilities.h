////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelTestUtilities.h (model_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Model.h"

namespace ell
{
// Helpful stuff
void PrintModel(const model::Model& model);
void PrintModel(const model::Model& model, const model::Node* output);
model::Model GetSimpleModel();
model::Model GetComplexModel();
}