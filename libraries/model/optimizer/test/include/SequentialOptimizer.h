////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SequentialOptimizer.h (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "GlobalOptimizer.h"

#include <memory>
#include <type_traits>
#include <vector>

/// <summary> Simple greedy optimizer that has a list of transformations and tries them in order </summary>
class SequentialOptimizer : public ell::model::optimizer::Optimizer
{
public:
    template <typename CostModelType, typename ObjectiveType>
    SequentialOptimizer(CostModelType costModel, ObjectiveType objective);

    template <typename TransformationType, std::enable_if_t<std::is_base_of_v<ell::model::Transformation, TransformationType>, void*> = nullptr>
    void AddTransformation(TransformationType transformation);

protected:
    void Reset() override;
    bool IsDone() const override;
    const ell::model::Transformation& GetTransformation() override;
    bool KeepTransformation(const ell::model::optimizer::Objective::ObjectiveValue& objectiveDelta) const override;

private:
    std::vector<std::unique_ptr<ell::model::Transformation>> _transformations;
    std::vector<std::unique_ptr<ell::model::Transformation>>::iterator _currentTransformation;
};

#pragma region implementation

template <typename CostModelType, typename ObjectiveType>
SequentialOptimizer::SequentialOptimizer(CostModelType costModel, ObjectiveType objective) :
    Optimizer(std::move(costModel), std::move(objective))
{}

template <typename TransformationType, std::enable_if_t<std::is_base_of_v<ell::model::Transformation, TransformationType>, void*>>
void SequentialOptimizer::AddTransformation(TransformationType transformation)
{
    _transformations.emplace_back(std::make_unique<TransformationType>(std::move(transformation)));
}

#pragma endregion implementation
