////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     AppendNodeToModel.h (common)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// model
#include "DynamicMap.h"
#include "Model.h"

namespace emll
{
namespace common
{
    template <typename PredictorNodeType, typename PredictorType>
    model::Model AppendNodeToModel(DynamicMap& map, PredictorType& predictor);
}
}
