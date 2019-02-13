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

#include <algorithm>
#include <iostream>
#include <iterator>

using namespace ell::model;

namespace
{
template <typename Container, typename Function>
auto Transform(const Container& container, Function fn)
{
    std::vector<decltype(fn(container[0]))> result;
    result.reserve(container.size());
    std::transform(container.begin(), container.end(), std::back_inserter(result), fn);
    return result;
}
} // namespace

PortDescription GetDescription(const ell::model::Port& port)
{
    return { port.GetType(), port.GetMemoryLayout() };
}

NodeDescription GetDescription(const ell::model::Node& node)
{
    auto inputs = Transform(node.GetInputPorts(), [](InputPortBase* port) { return GetDescription(*port); });
    auto outputs = Transform(node.GetOutputPorts(), [](OutputPortBase* port) { return GetDescription(*port); });
    return { inputs, outputs, node.GetRuntimeTypeName() };
}

SubmodelDescription GetDescription(const ell::model::Submodel& submodel)
{
    auto inputs = Transform(submodel.GetInputPorts(), [](const InputPortBase* port) { return GetDescription(*port); });
    auto outputs = Transform(submodel.GetOutputPorts(), [](const OutputPortBase* port) { return GetDescription(*port); });
    std::vector<NodeDescription> nodes;
    submodel.Visit([&nodes](const Node& node) {
        nodes.push_back(GetDescription(node));
    });
    return { inputs, outputs, nodes };
}

EnvironmentDescription GetDescription(const ell::model::optimizer::Environment& environment)
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
bool CostDatabase::HasCostMeasurement(const ell::model::Submodel& submodel, const ell::model::optimizer::Environment& environment) const
{
    auto key = GetMeasurementKey(submodel, environment);
    return _measurements.find(key) != _measurements.end();
}

CostDatabase::Cost CostDatabase::GetCostMeasurement(const ell::model::Submodel& submodel, const ell::model::optimizer::Environment& environment) const
{
    if (!HasCostMeasurement(submodel, environment))
    {
        throw ell::utilities::InputException(ell::utilities::InputExceptionErrors::invalidArgument);
    }

    auto key = GetMeasurementKey(submodel, environment);
    return _measurements.at(key);
}

void CostDatabase::AddCostMeasurement(const ell::model::Submodel& submodel, const ell::model::optimizer::Environment& environment, const Cost& cost)
{
    auto key = GetMeasurementKey(submodel, environment);
    _measurements[key] = cost;
}

CostDatabase::Key CostDatabase::GetMeasurementKey(const ell::model::Submodel& submodel, const ell::model::optimizer::Environment& environment) const
{
    if (!environment.HasTargetDevice() || submodel.NumOutputPorts() == 0)
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
    return ell::utilities::HashValue(arg);
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
