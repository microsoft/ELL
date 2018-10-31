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
        // Until MacOS's compiler has proper std::function deduction guides
#if defined(__APPLE__)
        template <typename Fn>
        struct Function : public std::function<Fn>
        {
            using std::function<Fn>::function;
        };

        template <typename>
        struct StdFunctionDeductionGuideHelper
        {};

        template <typename ReturnT, typename Class, bool IsNoExcept, typename... Args>
        struct StdFunctionDeductionGuideHelper<ReturnT (Class::*)(Args...) noexcept(IsNoExcept)>
        {
            using Type = ReturnT(Args...);
        };

        template <typename ReturnT, typename Class, bool IsNoExcept, typename... Args>
        struct StdFunctionDeductionGuideHelper<ReturnT (Class::*)(Args...) & noexcept(IsNoExcept)>
        {
            using Type = ReturnT(Args...);
        };

        template <typename ReturnT, typename Class, bool IsNoExcept, typename... Args>
        struct StdFunctionDeductionGuideHelper<ReturnT (Class::*)(Args...) const noexcept(IsNoExcept)>
        {
            using Type = ReturnT(Args...);
        };

        template <typename ReturnT, typename Class, bool IsNoExcept, typename... Args>
        struct StdFunctionDeductionGuideHelper<ReturnT (Class::*)(Args...) const& noexcept(IsNoExcept)>
        {
            using Type = ReturnT(Args...);
        };

        template <typename ReturnT, typename... Args>
        Function(ReturnT (*)(Args...))->Function<ReturnT(Args...)>;

        template <typename Functor,
                  typename Signature = typename StdFunctionDeductionGuideHelper<decltype(&Functor::operator())>::Type>
        Function(Functor)->Function<Signature>;
#endif // defined(__APPLE__)

        std::function<void()> CreateFunction(std::string fnName, std::function<void()> fn);

        template <typename ReturnT>
        std::function<ReturnT()> CreateFunction(std::string fnName, Value returnValue, std::function<ReturnT()> fn)
        {
            auto createdFn = GetContext().CreateFunction(fnName, returnValue, [fn = std::move(fn)]() -> Value {
                ReturnT r = fn();
                return static_cast<Value>(r);
            });

            return [createdFn = std::move(createdFn)]() -> ReturnT { return ReturnT(createdFn()); };
        }

        template <typename... Args>
        std::function<void(Args...)> CreateFunction(std::string fnName, std::vector<Value> argValues,
                                                    std::function<void(Args...)> fn)
        {
            constexpr auto argSize = sizeof...(Args);
            if (argValues.size() != argSize)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidSize);
            }

            auto createdFn =
                GetContext().CreateFunction(fnName, argValues, [fn = std::move(fn)](std::vector<Value> args) -> void {
                    std::tuple<Args...> tupleArgs = utilities::VectorToTuple<Args...>(args);
                    std::apply(fn, tupleArgs);
                });

            return [createdFn = std::move(createdFn)](Args&&... args) -> void {
                constexpr auto argSize = sizeof...(Args);
                std::vector<Value> argValues;
                argValues.reserve(argSize);
                (argValues.push_back(static_cast<Value>(args)), ...);

                createdFn(argValues);
            };
        }

        template <typename ReturnT, typename... Args>
        std::function<ReturnT(Args...)> CreateFunction(std::string fnName, Value returnValue,
                                                       std::vector<Value> argValues, std::function<ReturnT(Args...)> fn)
        {
            constexpr auto argSize = sizeof...(Args);
            if (argValues.size() != argSize)
            {
                throw utilities::InputException(utilities::InputExceptionErrors::invalidSize);
            }

            auto createdFn = GetContext().CreateFunction(fnName,
                                                         returnValue,
                                                         argValues,
                                                         [fn = std::move(fn)](std::vector<Value> args) -> Value {
                                                             std::tuple<Args...> tupleArgs =
                                                                 utilities::VectorToTuple<Args...>(args);
                                                             ReturnT r = std::apply(fn, tupleArgs);
                                                             return static_cast<Value>(r);
                                                         });

            return [createdFn = std::move(createdFn)](Args&&... args) -> ReturnT {
                constexpr auto argSize = sizeof...(Args);
                std::vector<Value> argValues;
                argValues.reserve(argSize);
                (argValues.push_back(static_cast<Value>(args)), ...);

                return ReturnT(createdFn(argValues));
            };
        }

    } // namespace detail

#if defined(__APPLE__)
#define FUNCTION_TYPE detail::Function
#else
#define FUNCTION_TYPE std::function
#endif // defined(__APPLE__)

    template <typename Fn>
    auto CreateFunction(std::string fnName, Fn&& fn)
    {
        return detail::CreateFunction(fnName, FUNCTION_TYPE(fn));
    }

    template <typename Fn>
    auto CreateFunction(std::string fnName, Value returnValue, Fn&& fn)
    {
        return detail::CreateFunction(fnName, returnValue, FUNCTION_TYPE(fn));
    }

    template <typename Fn>
    auto CreateFunction(std::string fnName, std::vector<Value> argValues, Fn&& fn)
    {
        return detail::CreateFunction(fnName, argValues, FUNCTION_TYPE(fn));
    }

    template <typename Fn>
    auto CreateFunction(std::string fnName, Value returnValue, std::vector<Value> argValues, Fn&& fn)
    {
        return detail::CreateFunction(fnName, returnValue, argValues, FUNCTION_TYPE(fn));
    }

#undef FUNCTION_TYPE

    template <typename ContextType, typename Fn>
    void InvokeForContext(Fn&& fn)
    {
        static_assert(std::is_base_of_v<EmitterContext, std::decay_t<ContextType>>,
                      "ContextType must be derived from EmitterContext");

        if (auto ptr = dynamic_cast<ContextType*>(&GetContext()); ptr != nullptr)
        {
            fn(*ptr);
        }
    }

} // namespace value
} // namespace ell