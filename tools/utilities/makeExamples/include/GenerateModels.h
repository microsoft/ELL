////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     GenerateModels.h (makeExamples)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Model.h"

namespace emll
{
model::Model GenerateIdentityModel();
model::Model GenerateTimesTwoModel();
model::Model GenerateModel1();
model::Model GenerateModel2();
model::Model GenerateModel3();
model::Model GenerateTreeModel(size_t numSplits);
model::Model GenerateRefinedTreeModel(size_t numSplits);
}