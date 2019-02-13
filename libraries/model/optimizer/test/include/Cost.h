////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Cost.h (model/optimizer)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <string>
#include <unordered_map>
#include <variant>

namespace ell
{
namespace model
{
    namespace optimizer
    {
        class UnknownCostValue
        {
        public:
            double GetValue() const { return 0; }
        };

        class HeuristicCostValue
        {
        public:
            HeuristicCostValue() = default;
            HeuristicCostValue(double value);
            double GetValue() const { return _value; }

            // heuristic costs should (?) have a value of "yes/no/don't know" as well as a weight
            // how do we compare heuristic costs vs. measured ones?

        private:
            double _value = 0;
        };

        class MeasuredCostValue
        {
        public:
            MeasuredCostValue() = default;
            MeasuredCostValue(double value, double variance);
            double GetValue() const { return _value; }
            double GetVariance() const { return _variance; }

        private:
            double _value = 0.0;
            double _variance = 0.0;
        };

        class CostValue
        {
        public:
            CostValue();
            CostValue(HeuristicCostValue v);
            CostValue(MeasuredCostValue v);

            double GetValue() const;

            template <typename T>
            bool IsCostType() const
            {
                return std::holds_alternative<T>(_value);
            }

        private:
            std::variant<UnknownCostValue, HeuristicCostValue, MeasuredCostValue> _value; // TODO: add "infinite" cost
        };

        class Cost
        {
        public:
            Cost() = default;
            bool HasCostComponent(std::string name) const;
            CostValue GetCostComponent(std::string name) const;
            CostValue& operator[](std::string name);

        protected:
            Cost(const std::unordered_map<std::string, CostValue>& components);
            std::unordered_map<std::string, CostValue> _components;
        };
    } // namespace optimizer
} // namespace model
} // namespace ell
