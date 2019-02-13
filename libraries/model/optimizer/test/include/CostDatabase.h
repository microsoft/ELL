////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CostDatabase.h (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Cost.h"
#include "Environment.h"

#include <model/include/OutputNode.h>
#include <model/include/Port.h>
#include <model/include/Submodel.h>
#include <model/include/Transformation.h>

#include <utilities/include/Exception.h>
#include <utilities/include/Hash.h>
#include <utilities/include/MemoryLayout.h>

#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

//
// Databases for storing performance measurements (or heuristics)
//

struct PortDescription
{
    ell::model::Port::PortType type;
    ell::utilities::MemoryLayout layout;
};
bool operator==(const PortDescription& a, const PortDescription& b);

struct NodeDescription
{
    std::vector<PortDescription> inputs;
    std::vector<PortDescription> outputs;
    std::string type;
};
bool operator==(const NodeDescription& a, const NodeDescription& b);

struct SubmodelDescription
{
    std::vector<PortDescription> inputs;
    std::vector<PortDescription> outputs;
    std::vector<NodeDescription> nodes;
};
bool operator==(const SubmodelDescription& a, const SubmodelDescription& b);

using EnvironmentDescription = std::string;

PortDescription GetDescription(const ell::model::Port& port);
NodeDescription GetDescription(const ell::model::Node& node);
SubmodelDescription GetDescription(const ell::model::Submodel& submodel);
EnvironmentDescription GetDescription(const ell::model::optimizer::Environment& environment);

class CostDatabase
{
public:
    using Cost = ell::model::optimizer::Cost;
    bool HasCostMeasurement(const ell::model::Submodel& submodel, const ell::model::optimizer::Environment& environment) const;
    Cost GetCostMeasurement(const ell::model::Submodel& submodel, const ell::model::optimizer::Environment& environment) const;
    void AddCostMeasurement(const ell::model::Submodel& submodel, const ell::model::optimizer::Environment& environment, const Cost& cost);

private:
    using Key = std::tuple<SubmodelDescription, EnvironmentDescription>;
    struct KeyHash
    {
        size_t operator()(const Key& key) const;
    };

    Key NullKey() const;
    Key GetMeasurementKey(const ell::model::Submodel& submodel, const ell::model::optimizer::Environment& environment) const;

    // map from (submodel, environment) -> cost
    // break it down into individual node types
    std::unordered_map<Key, Cost, KeyHash> _measurements;
};

namespace std
{
template <>
struct hash<PortDescription>
{
    size_t operator()(const PortDescription& arg) const;
};

template <>
struct hash<NodeDescription>
{
    size_t operator()(const NodeDescription& arg) const;
};

template <>
struct hash<SubmodelDescription>
{
    size_t operator()(const SubmodelDescription& arg) const;
};
} // namespace std
