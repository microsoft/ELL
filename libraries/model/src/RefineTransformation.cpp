////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     RefineTransformation.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "RefineTransformation.h"

#include <model/include/ModelTransformer.h>

#include <utilities/include/Exception.h>

namespace ell
{
namespace model
{
    namespace
    {
        bool IsModelCompilable(const Model& model, const TransformContext& context)
        {
            auto iter = model.GetNodeIterator();
            while (iter.IsValid())
            {
                if (!context.IsNodeCompilable(*iter.Get()))
                {
                    return false;
                }
                iter.Next();
            }
            return true;
        }
    } // namespace

    RefineTransformation::RefineTransformation(int maxIterations) :
        _maxIterations(maxIterations)
    {
    }

    Submodel RefineTransformation::Transform(const Submodel& submodel, ModelTransformer& transformer, const TransformContext& context) const
    {
        bool didRefineAny = false;
        auto refineFunction = [&didRefineAny](const Node& node, ModelTransformer& transformer) {
            bool didRefineNode = transformer.RefineNode(node);
            didRefineAny |= didRefineNode;
        };

        Submodel newSubmodel = submodel;
        for (int i = 0; i < _maxIterations; ++i)
        {
            didRefineAny = false;
            auto currentSubmodel = std::move(newSubmodel);
            Model newModel;
            newSubmodel = transformer.TransformSubmodelOnto(currentSubmodel, newModel, {}, context, refineFunction);

            // check for early end condition
            if (!didRefineAny || IsModelCompilable(newModel, context))
            {
                break;
            }
        }

        return newSubmodel;
    }

} // namespace model
} // namespace ell
