////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ExampleTransformations.h (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <model/include/ModelTransformer.h>
#include <model/include/OutputNode.h>
#include <model/include/Submodel.h>
#include <model/include/Transformation.h>

//
// Example transformations for testing
//
class TrivialTransformation : public ell::model::Transformation
{
public:
    ell::model::Submodel Transform(const ell::model::Submodel& submodel, ell::model::ModelTransformer& transformer, const ell::model::TransformContext& context) const override
    {
        return submodel;
    }
    /// <summary> Gets the name of this type. </summary>
    static std::string GetTypeName() { return "TrivialTransformation"; }

    /// <summary> Gets the name of this type. </summary>
    std::string GetRuntimeTypeName() const override { return GetTypeName(); }
};

class AddMetadataToOutputTransformation : public ell::model::Transformation
{
public:
    ell::model::Submodel Transform(const ell::model::Submodel& submodel, ell::model::ModelTransformer& transformer, const ell::model::TransformContext& context) const override;

    /// <summary> Gets the name of this type. </summary>
    static std::string GetTypeName() { return "AddMetadataToOutputTransformation"; }

    /// <summary> Gets the name of this type. </summary>
    std::string GetRuntimeTypeName() const override { return GetTypeName(); }

private:
    void CopyOutputNode(const ell::model::OutputNodeBase* node, ell::model::ModelTransformer& transformer) const;
    template <typename T>
    void CopyOutputNode(const ell::model::OutputNode<T>* node, ell::model::ModelTransformer& transformer) const;
};

class CombineNodesTransformation : public ell::model::Transformation
{
public:
    ell::model::Submodel Transform(const ell::model::Submodel& submodel, ell::model::ModelTransformer& transformer, const ell::model::TransformContext& context) const override;

    /// <summary> Gets the name of this type. </summary>
    static std::string GetTypeName() { return "CombineNodesTransformation"; }

    /// <summary> Gets the name of this type. </summary>
    std::string GetRuntimeTypeName() const override { return GetTypeName(); }

private:
    bool ShouldReplaceNode(const ell::model::Node& node) const;
    void ReplaceNodeAndParent(const ell::model::Node& node, ell::model::ModelTransformer& transformer) const;
    void ReplaceNodeAndParent(const ell::model::OutputNodeBase* node, const ell::model::OutputNodeBase* parent, ell::model::ModelTransformer& transformer) const;
    template <typename T>
    void ReplaceNodeAndParent(const ell::model::OutputNode<T>* node, const ell::model::OutputNode<T>* parent, ell::model::ModelTransformer& transformer) const;
};
