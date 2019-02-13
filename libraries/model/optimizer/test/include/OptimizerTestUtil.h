////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     OptimizerTestUtil.h (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Cost.h"
#include "Objective.h"

#include <model/include/Submodel.h>
#include <model/include/Transformation.h>

//
// Optimizer test utilities
//

struct TransformationTestData
{
    ell::model::Submodel submodel;
    int initialModelSize;
    int initialSubmodelSize;
    int transformedSubmodelSize;
};

ell::model::Submodel GetSimpleSubmodel();
ell::model::Submodel GetCombineNodesTestSubmodel();
TransformationTestData GetCombineNodesTestData();

bool SubmodelsAreSameSize(const ell::model::Submodel& a, const ell::model::Submodel& b);
bool ReferencedModelsAreSameSize(const ell::model::Submodel& a, const ell::model::Submodel& b);

template <typename Predicate>
bool AnyNodeOf(const ell::model::Submodel& submodel, Predicate predicate);

template <typename Predicate>
bool AllNodesOf(const ell::model::Submodel& submodel, Predicate predicate);

bool IsOutputNode(const ell::model::Node& node);

#pragma region implementation

template <typename Predicate>
bool AnyNodeOf(const ell::model::Submodel& submodel, Predicate predicate)
{
    bool found = false;
    submodel.Visit([&found, &predicate](const ell::model::Node& node) {
        found |= predicate(node);
    });
    return found;
}

template <typename Predicate>
bool AllNodesOf(const ell::model::Submodel& submodel, Predicate predicate)
{
    bool foundAll = true;
    submodel.Visit([&foundAll, &predicate](const ell::model::Node& node) {
        foundAll &= predicate(node);
    });
    return foundAll;
}
#pragma endregion implementation
