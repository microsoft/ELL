////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SetCompilerOptionsTransformation.cpp (model)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SetCompilerOptionsTransformation.h"

#include <utilities/include/Exception.h>
#include <utilities/include/StlVectorUtil.h>

namespace ell
{
namespace model
{
    namespace
    {
        std::vector<const OutputPortBase*> GetReferencedPorts(const std::vector<const InputPortBase*>& inputs)
        {
            return utilities::TransformVector(inputs.begin(), inputs.end(), [](auto input) { return &input->GetReferencedPort(); });
        }

        void CopyAndSetOptionsMetadata(const Node& node, ModelTransformer& transformer, const utilities::PropertyBag& nodeOptions)
        {
            auto idStr = node.GetId().ToString();
            if (nodeOptions.HasEntry(idStr))
            {
                utilities::PropertyBag nodeMetadata;
                nodeMetadata["compileOptions"] = nodeOptions.GetEntry<utilities::PropertyBag>(idStr);
                transformer.CopyNodeWithMetadata(node, nodeMetadata);
            }
            else
            {
                transformer.CopyNode(node);
            }
        }
    } // namespace

    SetCompilerOptionsTransformation::SetCompilerOptionsTransformation(const utilities::PropertyBag& options) :
        _options(options)
    {
    }

    Submodel SetCompilerOptionsTransformation::Transform(const Submodel& submodel, ModelTransformer& transformer, const TransformContext& context) const
    {
        if (!_options.HasEntry("model") && !_options.HasEntry("nodes"))
        {
            return submodel;
        }

        auto onto = transformer.GetCorrespondingOutputs(GetReferencedPorts(submodel.GetInputs()));
        model::Model destModel;
        Submodel result(destModel);
        if (_options.HasEntry("nodes"))
        {
            result = transformer.TransformSubmodelOnto(submodel, destModel, onto, context, [this](const Node& node, ModelTransformer& transformer) {
                CopyAndSetOptionsMetadata(node, transformer, _options.GetEntry<utilities::PropertyBag>("nodes"));
            });
        }
        else
        {
            result = transformer.CopySubmodelOnto(submodel, destModel, onto, context);
        }

        if (_options.HasEntry("model"))
        {
            result.GetModel().GetMetadata()["compileOptions"] = _options.GetEntry<utilities::PropertyBag>("model");
        }
        return result;
    }
} // namespace model
} // namespace ell
