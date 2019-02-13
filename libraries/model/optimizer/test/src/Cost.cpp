////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Cost.cpp (model/optimizer)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Cost.h"

#include <unordered_set>

namespace ell
{
namespace model
{
    namespace optimizer
    {
        // HeuristicCostValue
        HeuristicCostValue::HeuristicCostValue(double value) :
            _value(value)
        {}

        // MeasuredCostValue
        MeasuredCostValue::MeasuredCostValue(double value, double variance) :
            _value(value),
            _variance(variance)
        {}

        // CostValue
        CostValue::CostValue() :
            _value(UnknownCostValue())
        {
        }

        CostValue::CostValue(HeuristicCostValue v) :
            _value(v)
        {
        }

        CostValue::CostValue(MeasuredCostValue v) :
            _value(v)
        {
        }

        double CostValue::GetValue() const
        {
            return std::visit([](auto&& arg) { return arg.GetValue(); }, _value);
        }

        // Cost
        Cost::Cost(const std::unordered_map<std::string, CostValue>& components) :
            _components(components)
        {}

        bool Cost::HasCostComponent(std::string name) const
        {
            return _components.find(name) != _components.end();
        }

        CostValue Cost::GetCostComponent(std::string name) const
        {
            auto it = _components.find(name);
            if (it == _components.end())
            {
                return {};
            }

            return it->second;
        }

        CostValue& Cost::operator[](std::string name)
        {
            return _components[name];
        }
    } // namespace optimizer
} // namespace model
} // namespace ell
