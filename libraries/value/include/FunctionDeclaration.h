////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FunctionDeclaration.h (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterContext.h"
#include "Value.h"

#include <utilities/include/FunctionUtils.h>
#include <utilities/include/StringUtil.h>

#include <optional>
#include <string>
#include <tuple>
#include <vector>

namespace ell
{
namespace value
{
    /// <summary> Helper enum used to specify whether a FunctionDeclaration object should decorate its function name </summary>
    enum class FunctionDecorated
    {
        Yes,
        No
    };

    /// <summary> Describes a function that can be acted upon by an EmitterContext instance </summary>
    class [[nodiscard]] FunctionDeclaration
    {
    public:
        /// <summary> Default constructor. Creates an empty function declaration </summary>
        FunctionDeclaration() = default;

        /// <summary> Constructor </summary>
        /// <param name="name"> The name of the function </param>
        FunctionDeclaration(std::string name);

        /// <summary> Sets the return type for this function declaration </summary>
        /// <param name="returnType"> A Value instance describing type of the value that is expected and its memory layout to be returned by the function </param>
        /// <returns> A reference to this instance </returns>
        /// <remarks> If this function is not called, the instance defaults to a void return type </remarks>
        FunctionDeclaration& Returns(Value returnType);

        /// <summary> Sets whether this function should be decorated (mangled) </summary>
        /// <param name="shouldDecorate"> An enum value specifying whether this function should be decorated </param>
        /// <returns> A reference to this instance </returns>
        /// <remarks> By default, a function is decorated, which means the name gets suffixed by an encoding of the function's parameter and return types.
        /// Functions that are declared externally should probably not be decorated </remarks>
        FunctionDeclaration& Decorated(FunctionDecorated shouldDecorate);

        /// <summary> Specifies a function definition for this declaration </summary>
        /// <param name="fn"> A function object that takes zero or more Value library observer types and returns void or a Value library observer type.
        /// This function object defines this function. </param>
        /// <returns> A std::function function object that matches the signature of the function passed in </returns>
        /// <remarks> If this function is not called, this function declaration is treated as an external function. Not all contexts may support an external
        /// function </remarks>
        template <typename Fn>
        [[maybe_unused]] auto Define(Fn&& fn);

        /// <summary> Sets the parameters this function requires </summary>
        /// <param name="paramTypes"> Zero or more Value instances describing the types of the arguments and their memory layout expected by the function </param>
        /// <returns> A reference to this instance </returns>
        /// <remarks> If this function is not called, the instance defaults to taking no arguments </remarks>
        template <typename... Types>
        FunctionDeclaration& Parameters(Types&& ... paramTypes)
        {
            static_assert(utilities::AllSame<Value, std::decay_t<Types>...>);
            return Parameters(std::vector<Value>{ std::forward<Types>(paramTypes)... });
        }

        [[nodiscard]] FunctionDeclaration& Parameters(std::vector<Value> parameters);

        /// <summary> Emits a call to the function declaration </summary>
        /// <param name="arguments"> A vector of Value instances that hold the arguments for the function call </param>
        /// <returns> A std::optional instance that holds a Value instance with the return value of the call, if it is expected, otherwise empty </returns>
        /// <remarks> If the function is not defined and the context is capable of it, this will emit a call to an external function </remarks>
        std::optional<Value> Call(std::vector<Value> arguments = {}) const;

        /// <summary> Gets the final function name, including any decoration if so applicable </summary>
        const std::string& GetFunctionName() const;

        /// <summary> Gets the vector of Value instances describing the parameter types the function requires </summary>
        const std::vector<Value>& GetParameterTypes() const;

        /// <summary> Gets the return type, wrapped in a std::optional. If the function expects to return a value, its type is described.
        /// Otherwise, the std::optional instance is empty </summary>
        const std::optional<Value>& GetReturnType() const;

        /// <summary> Returns true if function is defined for current context, false otherwise </summary>
        [[nodiscard]] bool IsDefined() const;

        /// <summary> Returns true if the instance is an empty function declaration </summary>
        [[nodiscard]] bool IsEmpty() const;

    private:
        template <typename ReturnT, typename... Args>
        [[maybe_unused]] std::function<ReturnT(Args...)> DefineImpl(std::function<ReturnT(Args...)> fn);

        void CheckNonEmpty() const;

        std::string _originalFunctionName;
        mutable std::optional<std::string> _decoratedFunctionName;
        std::optional<Value> _returnType;
        std::vector<Value> _paramTypes;
        bool _isDecorated = true;
        bool _isEmpty = true;
    };

    inline bool operator==(const FunctionDeclaration& decl1, const FunctionDeclaration& decl2)
    {
        return decl1.GetFunctionName() == decl2.GetFunctionName();
    }

    [[nodiscard]] FunctionDeclaration DeclareFunction(std::string name);

} // namespace value
} // namespace ell

namespace std
{
template <>
struct hash<::ell::value::FunctionDeclaration>
{
    using Type = ::ell::value::FunctionDeclaration;

    size_t operator()(const Type& value) const;
};
} // namespace std

#pragma region implementation

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
            Function(const std::function<Fn>& fn) : std::function<Fn>(fn) {}
            Function(std::function<Fn>&& fn) : std::function<Fn>(std::move(fn)) {}
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

        // Function pointer
        template <typename ReturnT, typename... Args>
        Function(ReturnT (*)(Args...)) -> Function<ReturnT(Args...)>;

        // Functor
        template <typename Functor,
                  typename Signature = typename StdFunctionDeductionGuideHelper<decltype(&Functor::operator())>::Type>
        Function(Functor) -> Function<Signature>;
#endif // defined(__APPLE__)

        template <typename ViewType, std::enable_if_t<std::is_same_v<decltype(std::declval<ViewType>().GetValue()), Value>, void*> = nullptr>
        Value GetValue(ViewType value)
        {
            return value.GetValue();
        }

        inline Value GetValue(Value value) { return value; }

    } // namespace detail

#if defined(__APPLE__)
#define FUNCTION_TYPE detail::Function
#else
#define FUNCTION_TYPE std::function
#endif // defined(__APPLE__)

    template <typename Fn>
    [[maybe_unused]] auto FunctionDeclaration::Define(Fn&& fn)
    {
        return DefineImpl(FUNCTION_TYPE{ std::forward<Fn>(fn) });
    }

#undef FUNCTION_TYPE

    template <typename ReturnT, typename... Args>
    [[maybe_unused]] std::function<ReturnT(Args...)> FunctionDeclaration::DefineImpl(std::function<ReturnT(Args...)> fn) {
        if constexpr (std::is_same_v<ReturnT, void>)
        {
            if (_returnType.has_value())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Defining function has a return value, but declaration does not");
            }
        }
        else
        {
            if (!_returnType.has_value())
            {
                throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, "Defining function returns void, but declaration does not");
            }

            // Try to instantiate an instance of the return type (R) with the Value instance that reprepsents the return type (_returnType)
            // If this throws, the return value of the defining function is not compatible with the Value instance specified in the declaration
            ReturnT returnType = *_returnType;
        }

        if (sizeof...(Args) != _paramTypes.size())
        {
            throw utilities::InputException(utilities::InputExceptionErrors::typeMismatch, utilities::FormatString("Defining function takes %zu parameters, but declaration was specific to have %zu.", sizeof...(Args), _paramTypes.size()));
        }

        if constexpr (sizeof...(Args) > 0)
        {
            // Same as the return type checking, instantiate the arguments of the function with the Value instances in _paramTypes to ensure everything
            // is correct. If this throws, there's a mismatch between the defining function's parameters and the declaration.
            auto paramTypes = utilities::VectorToTuple<Args...>(_paramTypes);
        }

        auto createdFn = GetContext().CreateFunction(*this, [fn = std::move(fn)](std::vector<Value> args) -> std::optional<Value> {
            std::tuple<Args...> tupleArgs = utilities::VectorToTuple<Args...>(args);
            if constexpr (std::is_same_v<ReturnT, void>)
            {
                std::apply(fn, tupleArgs);
                return std::nullopt;
            }
            else
            {
                ReturnT r = std::apply(fn, tupleArgs);
                return detail::GetValue(r);
            }
        });

        return [createdFn = std::move(createdFn)](Args&&... args) -> ReturnT {
            constexpr auto argSize = sizeof...(Args);
            std::vector<Value> argValues;
            argValues.reserve(argSize);
            (argValues.push_back(detail::GetValue(args)), ...);

            auto fnReturn = createdFn(argValues);
            if constexpr (std::is_same_v<void, ReturnT>)
            {
                if (fnReturn)
                {
                    throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState,
                                                    "Function is supposed to return void, but a value was returned from the defining function");
                }
            }
            else
            {
                return ReturnT(*fnReturn);
            }
        };
    }
} // namespace value
} // namespace ell

#pragma endregion implementation
