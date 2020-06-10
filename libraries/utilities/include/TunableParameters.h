////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TunableParameters.h (utilities)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "FunctionUtils.h"

#include <map>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

namespace ell
{
namespace utilities
{
    /// <summary> Represents a range of values to iterate over, in conjunction with the `TuningEngine` class below. </summary>
    /// <remarks>
    /// Construction of parameters should take place before construction of the TuningEngine, which should be passed
    /// the set of parameters to iterate over, creating a full set of combinatorial possibilities. The parameters should
    /// then be used in place of `T`, resulting in a full exploration of the possibilities. For example,
    /// ```
    /// TunableParameter M = std::vector{ 2, 4, 6 }, N = std::vector{ 3, 5 };
    /// TuningEngine engine(M, N);
    /// do {
    ///   std::cout << (int)M * (int)N << " ";
    /// } while (engine.Next());
    /// ```
    /// will produce the following output:
    /// ```
    /// 6 10 12 20 18 30
    /// ```
    /// The internal states of `TunableParameter` instances are modified when iterated over by `TuningEngine`. To reset the state
    /// the `Reset()` function can be called either on an individual instance of `TunableParameter` or on `TuningEngine`, which will
    /// call `Reset()` on the set of parameters the engine is operating over.
    /// </remarks>
    template <typename T>
    class TunableParameter
    {
    public:
        TunableParameter(std::vector<T> range, const std::string& name) :
            _name(name),
            _range(std::move(range)),
            _current(_range.begin())
        {}

        operator T() const
        {
            return *_current;
        }

        bool Next()
        {
            return ++_current != _range.end();
        }

        void Reset()
        {
            _current = _range.begin();
        }

        std::string Name() const
        {
            return _name;
        }

        std::string ValueString() const
        {
            // Obviously, this will only work if there's an overload for std::to_string that takes T
            return std::to_string(*_current);
        }

        std::string ToString() const
        {
            return Name() + ValueString();
        }

    private:
        std::string _name;
        std::vector<T> _range;
        typename std::vector<T>::iterator _current;
    };

    /// <summary> Takes an arbitrary number of lvalue references to `TunableParameter<T>` instances and iterates over them in a
    /// combinatorial manner. `TunableParameter<T>` instances have their state modified by the iteration of the engine,
    /// as explained above in the documentation for `TunableParameter<T>`. </summary>
    template <typename... Ts>
    class TuningEngine
    {
    public:
        TuningEngine(TunableParameter<Ts>&... params) :
            _params(std::tie(params...))
        {
        }

        bool Next()
        {
            ++_currentIteration;
            return NextImpl(std::make_integer_sequence<int64_t, sizeof...(Ts)>());
        }

        void Reset()
        {
            ApplyToEach([](auto& param) { param.Reset(); }, _params);
        }

        size_t CurrentIteration() const { return _currentIteration; }

        std::string ToString(const std::string& sep = "_") const
        {
            return ToStringImpl(sep, std::make_index_sequence<sizeof...(Ts) - 1>());
        }

        std::map<std::string, std::string> CurrentValues() const
        {
            return CurrentValuesImpl(std::make_integer_sequence<int64_t, sizeof...(Ts)>());
        }

    private:
        template <int64_t... Is>
        bool NextImpl(std::integer_sequence<int64_t, Is...> seq)
        {
            // Uses fold expressions combined with boolean OR short-circuiting
            // to iteratively call Next() on the individual parameters, starting
            // with the last one and working our way to the first one.
            return (
                [](auto& param) {
                    auto b = param.Next();
                    if (!b)
                    {
                        param.Reset();
                    }
                    return b;
                }(std::get<(seq.size() - 1) - Is>(_params)) ||
                ...);
        }

        template <size_t... Is>
        std::string ToStringImpl(const std::string& sep, std::index_sequence<Is...> seq) const
        {
            return ((std::get<Is>(_params).ToString() + sep) + ... + (std::get<seq.size()>(_params).ToString()));
        }

        template <int64_t... Is>
        std::map<std::string, std::string> CurrentValuesImpl(std::integer_sequence<int64_t, Is...> seq) const
        {
            std::map<std::string, std::string> result;
            ([&](auto& param) {
                result[param.Name()] = param.ValueString();
            }(std::get<(seq.size() - 1) - Is>(_params)),
             ...);
            return result;
        }

        std::tuple<TunableParameter<Ts>&...> _params;
        size_t _currentIteration = 0;
    };
} // namespace utilities
} // namespace ell
