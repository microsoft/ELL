////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     GenerateModels.h (makeExamples)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "Model.h"

ell::model::Model GenerateIdentityModel(size_t dimension);
ell::model::Model GenerateTimesTwoModel(size_t dimension);
template <typename ElementType>
ell::model::Model GenerateBroadcastTimesTwoModel(size_t dimension);
ell::model::Model GenerateIsEqualModel();
ell::model::Model GenerateArgMaxModel(size_t dimension);
ell::model::Model GenerateMultiOutModel(size_t dimension);
ell::model::Model GenerateModel1();
ell::model::Model GenerateModel2();
ell::model::Model GenerateModel3();
ell::model::Model GenerateTreeModel(size_t numSplits);
ell::model::Model GenerateRefinedTreeModel(size_t numSplits);
