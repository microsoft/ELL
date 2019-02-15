////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CostDatabase.cpp (model/optimizer_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CostDatabase.h"

#include <model/include/InputNode.h>
#include <model/include/Model.h>
#include <model/include/OutputNode.h>

#include <utilities/include/Exception.h>
#include <utilities/include/StlVectorUtil.h>

#include <algorithm>
#include <iostream>
#include <iterator>

using namespace ell;
using namespace ell::model;

namespace
{
template <typename Container, typename Function>
auto Transform(const Container& container, Function fn)
{
    return utilities::TransformVector(container.begin(), container.end(), fn);
}
} // namespace

PortDescription GetDescription(const model::Port& port)
{
    return { port.GetType(), port.GetMemoryLayout() };
}

NodeDescription GetDescription(const model::Node& node)
{
    auto inputs = Transform(node.GetInputPorts(), [](InputPortBase* port) { return GetDescription(*port); });
    auto outputs = Transform(node.GetOutputPorts(), [](OutputPortBase* port) { return GetDescription(*port); });
    return { inputs, outputs, node.GetRuntimeTypeName() };
}

SubmodelDescription GetDescription(const model::Submodel& submodel)
{
    auto inputs = Transform(submodel.GetInputs(), [](const InputPortBase* port) { return GetDescription(*port); });
    auto outputs = Transform(submodel.GetOutputs(), [](const OutputPortBase* port) { return GetDescription(*port); });
    std::vector<NodeDescription> nodes;
    submodel.Visit([&nodes](const Node& node) {
        nodes.push_back(GetDescription(node));
    });
    return { inputs, outputs, nodes };
}

EnvironmentDescription GetDescription(const model::optimizer::Environment& environment)
{
    return environment.GetTargetDevice().deviceName;
}

bool operator==(const PortDescription& a, const PortDescription& b)
{
    return (a.type == b.type) && (a.layout == b.layout);
}

bool operator==(const NodeDescription& a, const NodeDescription& b)
{
    return (a.inputs == b.inputs) && (a.outputs == b.outputs) && (a.type == b.type);
}

bool operator==(const SubmodelDescription& a, const SubmodelDescription& b)
{
    return (a.inputs == b.inputs) && (a.outputs == b.outputs) && (a.nodes == b.nodes);
}

// CostDatabase
bool CostDatabase::HasCostMeasurement(const model::Submodel& submodel, const model::optimizer::Environment& environment) const
{
    auto key = GetMeasurementKey(submodel, environment);
    return _measurements.find(key) != _measurements.end();
}

CostDatabase::Cost CostDatabase::GetCostMeasurement(const model::Submodel& submodel, const model::optimizer::Environment& environment) const
{
    if (!HasCostMeasurement(submodel, environment))
    {
        throw utilities::InputException(utilities::InputExceptionErrors::invalidArgument);
    }

    auto key = GetMeasurementKey(submodel, environment);
    return _measurements.at(key);
}

void CostDatabase::AddCostMeasurement(const model::Submodel& submodel, const model::optimizer::Environment& environment, const Cost& cost)
{
    auto key = GetMeasurementKey(submodel, environment);
    _measurements[key] = cost;
}

CostDatabase::Key CostDatabase::GetMeasurementKey(const model::Submodel& submodel, const model::optimizer::Environment& environment) const
{
    if (!environment.HasTargetDevice() || submodel.NumOutputs() == 0)
    {
        return NullKey();
    }

    auto submodelDesc = GetDescription(submodel);
    auto environmentDesc = GetDescription(environment);
    return { submodelDesc, environmentDesc };
}

CostDatabase::Key CostDatabase::NullKey() const
{
    return {};
}

size_t CostDatabase::KeyHash::operator()(const CostDatabase::Key& arg) const
{
    return utilities::HashValue(arg);
}

namespace std
{
size_t std::hash<PortDescription>::operator()(const PortDescription& arg) const
{
    using ::ell::utilities::HashCombine;

    size_t hash = 0;
    HashCombine(hash, arg.type);
    HashCombine(hash, arg.layout);
    return hash;
}

size_t std::hash<NodeDescription>::operator()(const NodeDescription& arg) const
{
    using ::ell::utilities::HashCombine;

    size_t hash = 0;
    HashCombine(hash, arg.inputs);
    HashCombine(hash, arg.outputs);
    HashCombine(hash, arg.type);
    return hash;
}

size_t std::hash<SubmodelDescription>::operator()(const SubmodelDescription& arg) const
{
    using ::ell::utilities::HashCombine;

    size_t hash = 0;
    HashCombine(hash, arg.inputs);
    HashCombine(hash, arg.outputs);
    HashCombine(hash, arg.nodes);
    return hash;
}
} // namespace std
