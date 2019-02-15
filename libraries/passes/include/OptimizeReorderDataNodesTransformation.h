////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizeReorderDataNodesTransformation.h (passes)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/Transformation.h>

#include <memory>

namespace ell
{
namespace passes
{
    /// <summary> A Transformation that removes unnecessary `ReorderDataNode`s. </summary>
    class OptimizeReorderDataNodesTransformation : public model::Transformation
    {
    public:
        OptimizeReorderDataNodesTransformation();
        OptimizeReorderDataNodesTransformation(OptimizeReorderDataNodesTransformation&&);
        ~OptimizeReorderDataNodesTransformation();
        ell::model::Submodel Transform(const ell::model::Submodel& submodel, ell::model::ModelTransformer& transformer, const ell::model::TransformContext& context) const override;
        std::string GetRuntimeTypeName() const override
        {
            return "OptimizeReorderDataNodesTransformation";
        }

    private:
        struct State;
        std::unique_ptr<State> _state;
    };
} // namespace passes
} // namespace ell
