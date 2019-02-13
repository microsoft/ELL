////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizeModelTransformation.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OptimizeModelTransformation.h"
#include "ModelTransformer.h"

#include <utilities/include/Exception.h>

namespace ell
{
namespace model
{
    OptimizeModelTransformation::OptimizeModelTransformation(const ModelOptimizer& optimizer) :
        _optimizer(optimizer)
    {}

    Submodel OptimizeModelTransformation::Transform(const Submodel& submodel, ModelTransformer& transformer, const TransformContext& context) const
    {
        ModelOptimizerContext optimizerContext(transformer);
        auto optimizedModel = _optimizer.OptimizeModel(submodel.GetModel(), optimizerContext);

        // TODO: get corresponding inputs/outputs in new model and return them in the result submodel
        return { optimizedModel };
    }
} // namespace model
} // namespace ell
