////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     EmitterContext.tcc (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace value
{

namespace detail
{

std::function<void()> CreateFunction(std::string fnName, std::function<void()> fn);

template <typename ReturnT>
std::function<ReturnT()> CreateFunction(std::string fnName, Value returnValue, std::function<ReturnT()> fn)
{
    auto createdFn = GetContext().CreateFunction(fnName, returnValue, [fn = std::move(fn)]() -> Value
    {
        ReturnT r = fn();
        return static_cast<Value>(r);
    });

    return [createdFn = std::move(createdFn)]() -> ReturnT
    {
        return ReturnT(createdFn());
    };
}


template <typename... Args>
std::function<void(Args...)> CreateFunction(std::string fnName, std::vector<Value> argValues, std::function<void(Args...)> fn)
{
    constexpr auto argSize = sizeof...(Args);
    if (argValues.size() != argSize)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::invalidSize);
    }

    auto createdFn = GetContext().CreateFunction(fnName, argValues, [fn = std::move(fn)](std::vector<Value> args) -> void
    {
        std::tuple<Args...> tupleArgs = utilities::VectorToTuple<Args...>(args);
        std::apply(fn, tupleArgs);
    });

    return [createdFn = std::move(createdFn)](Args&&... args) -> void
    {
        constexpr auto argSize = sizeof...(Args);
        std::vector<Value> argValues;
        argValues.reserve(argSize);
        (argValues.push_back(static_cast<Value>(args)), ...);

        createdFn(argValues);
    };
}

template <typename ReturnT, typename... Args>
std::function<ReturnT(Args...)> CreateFunction(std::string fnName, Value returnValue, std::vector<Value> argValues,
    std::function<ReturnT(Args...)> fn)
{
    constexpr auto argSize = sizeof...(Args);
    if (argValues.size() != argSize)
    {
        throw utilities::InputException(utilities::InputExceptionErrors::invalidSize);
    }

    auto createdFn = GetContext().CreateFunction(fnName, returnValue, argValues,
    [fn = std::move(fn)](std::vector<Value> args) -> Value
    {
        std::tuple<Args...> tupleArgs = utilities::VectorToTuple<Args...>(args);
        ReturnT r = std::apply(fn, tupleArgs);
        return static_cast<Value>(r);
    });

    return [createdFn = std::move(createdFn)](Args&&... args) -> ReturnT
    {
        constexpr auto argSize = sizeof...(Args);
        std::vector<Value> argValues;
        argValues.reserve(argSize);
        (argValues.push_back(static_cast<Value>(args)), ...);

        return ReturnT(createdFn(argValues));
    };
}

} // detail

template <typename Fn>
auto CreateFunction(std::string fnName, Fn&& fn)
{
    return detail::CreateFunction(fnName, std::function(fn));
}


template <typename Fn>
auto CreateFunction(std::string fnName, Value returnValue, Fn&& fn)
{
    return detail::CreateFunction(fnName, returnValue, std::function(fn));
}

template <typename Fn>
auto CreateFunction(std::string fnName, std::vector<Value> argValues, Fn&& fn)
{
    return detail::CreateFunction(fnName, argValues, std::function(fn));
}

template <typename Fn>
auto CreateFunction(std::string fnName, Value returnValue, std::vector<Value> argValues,
    Fn&& fn)
{
    return detail::CreateFunction(fnName, returnValue, argValues, std::function(fn));
}

template <typename ContextType, typename Fn>
void InvokeForContext(Fn&& fn)
{
    static_assert(std::is_base_of_v<EmitterContext, std::decay_t<ContextType>>, "ContextType must be derived from EmitterContext");

    if (auto ptr = dynamic_cast<ContextType*>(&GetContext()); ptr != nullptr)
    {
        fn(*ptr);
    }
}

} // value
} // ell