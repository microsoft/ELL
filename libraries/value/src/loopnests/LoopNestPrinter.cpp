////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoopNestPrinter.cpp (value)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "loopnests/LoopNestPrinter.h"

#include <utilities/include/Exception.h>
#include <utilities/include/StringUtil.h>

#include <numeric>
#include <variant>

namespace ell
{
namespace value
{
    namespace loopnests
    {
        namespace
        {
            // computes ceil(a/b)
            int CeilDiv(int a, int b)
            {
                return (a - 1) / b + 1;
            }
        } // namespace

        LoopNestPrinter::LoopNestPrinter(std::ostream& stream) :
            _stream(stream),
            _indentLevel(0)
        {
        }

        void LoopNestPrinter::Print(const LoopNest& loopNest) const
        {
            Visit(loopNest);
        }

        void LoopNestPrinter::GenerateLoopRangeNew(const LoopRange& r, const RecursionStateNew& state, const LoopVisitSchedule& schedule, std::function<void(Scalar)> codegenFn) const
        {
            const LoopNest& loopNest = schedule.GetLoopNest();
            auto loopIndex = schedule.CurrentLoopIndex();

            bool isParallelized = loopNest.IsParallelized(loopIndex);
            bool isUnrolled = loopNest.IsUnrolled(loopIndex);
            assert(!(isParallelized && isUnrolled) && "An index cannot be both unrolled and parallelized");

            const int startInt = r.start.Get<int>();
            const int stopInt = r.stop.Get<int>();
            const int stepInt = r.step.Get<int>();

            int numIterations = CeilDiv(stopInt - startInt, stepInt);
            if (numIterations < 2)
            {
                isParallelized = false;
            }

            std::vector<std::string> properties;
            if (isParallelized)
            {
                properties.push_back("parallel");
            }
            if (isUnrolled)
            {
                properties.push_back("unrolled");
            }
            if (numIterations == 1)
            {
                properties.push_back("single");
            }

            std::string propertiesStr;
            if (!properties.empty())
            {
                propertiesStr = ": (" + utilities::Join(properties, ", ") + ")";
            }

            WriteLine("For (" + GetIndexString(loopIndex, state.loopIndices) + " = " + std::to_string(startInt) + " to " + std::to_string(stopInt) + " by " + std::to_string(stepInt) + ")" + propertiesStr);
            WriteLine("{");
            {
                Indenter i(*this);
                codegenFn(startInt);
            }
            WriteLine("}");
        }

        void LoopNestPrinter::GenerateLoopRangeOld(const LoopRange& r, const RecursionState& state, const LoopVisitSchedule& schedule, std::function<void(Scalar)> codegenFn) const
        {
            const LoopNest& loopNest = schedule.GetLoopNest();
            auto loopIndex = schedule.CurrentLoopIndex();

            bool isParallelized = loopNest.IsParallelized(loopIndex);
            bool isUnrolled = loopNest.IsUnrolled(loopIndex);
            assert(!(isParallelized && isUnrolled) && "An index cannot be both unrolled and parallelized");

            const int startInt = r.start.Get<int>();
            const int stopInt = r.stop.Get<int>();
            const int stepInt = r.step.Get<int>();

            std::vector<std::string> properties;
            if (isParallelized)
            {
                properties.push_back("parallel");
            }
            if (isUnrolled)
            {
                properties.push_back("unrolled");
            }

            auto currentLoopHasPrologue = r.currentLoopFragmentFlags.GetFlag(LoopFragmentType::prologue);
            auto currentLoopHasEpilogue = r.currentLoopFragmentFlags.GetFlag(LoopFragmentType::epilogue);

            if (currentLoopHasPrologue)
            {
                properties.push_back("prologue_kernel");
            }

            if (currentLoopHasEpilogue)
            {
                properties.push_back("epilogue_kernel");
            }

            std::string propertiesStr;
            if (!properties.empty())
            {
                propertiesStr = ": (" + utilities::Join(properties, ", ") + ")";
            }

            WriteLine("For (" + GetIndexString(loopIndex, state.loopIndices) + " = " + std::to_string(startInt) + " to " + std::to_string(stopInt) + " by " + std::to_string(stepInt) + ")" + propertiesStr);
            WriteLine("{");
            {
                Indenter i(*this);
                codegenFn(startInt);
            }
            WriteLine("}");
        }

        std::string LoopNestPrinter::GetIndent() const
        {
            constexpr int indentSize = 4;
            return std::string(static_cast<size_t>(indentSize * _indentLevel), ' ');
        }

        void LoopNestPrinter::WriteLine(std::string l) const
        {
            _stream << GetIndent() << l << "\n";
        }

        Scalar LoopNestPrinter::EmitIndexExpression(const Index& index, const IndexExpression& expr, const LoopIndexSymbolTable& indexVariables) const
        {
            if (!expr.indices.empty())
            {
                // We can't currently optimize away the "identity" expression, because the result (a loops "index" Scalar)
                // would be a register variable (pointer valence 0), and the generated kernel function expects a stored value
                // (pointer valence 1). So, we need to call `Allocate()` to get a stored variable.
                std::vector<std::string> terms;
                for (auto scaledIndex : expr.indices)
                {
                    if (auto it = indexVariables.find(scaledIndex.index); it != indexVariables.end())
                    {
                        auto name = GetIndexString(scaledIndex.index, indexVariables);

                        if (scaledIndex.scale == 1)
                        {
                            terms.push_back(name);
                        }
                        else
                        {
                            terms.push_back(std::to_string(scaledIndex.scale) + "*" + name);
                        }
                    }
                }
                terms.push_back(std::to_string(expr.begin));

                WriteLine("int " + GetIndexString(index, indexVariables) + " = " + utilities::Join(terms, " + ") + ";");
            }
            return 0; // Silly but necessary according to the `EmitIndexExpression` API
        }

        std::string LoopNestPrinter::GetIndexString(const Index& index, const LoopIndexSymbolTable& runtimeIndexVariables) const
        {
            auto name = index.GetName();
            if (auto it = runtimeIndexVariables.find(index); it != runtimeIndexVariables.end())
            {
                auto range = it->second.loopRange;
                if (range.Increment() > 0)
                {
                    int numIterations = CeilDiv(range.End() - range.Begin(), range.Increment());
                    if (numIterations == 1)
                    {
                        name = "[" + name + "=" + std::to_string(range.Begin()) + "]";
                    }
                }
            }
            return name;
        }

        std::string LoopNestPrinter::GetPredicateString(const KernelPredicate& predicate, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const
        {
            if (predicate.IsAlwaysTrue())
            {
                return "true";
            }
            else if (predicate.IsAlwaysFalse())
            {
                return "false";
            }
            else if (auto fragmentPred = predicate.As<FragmentTypePredicate>(); fragmentPred != nullptr)
            {
                auto condition = fragmentPred->GetCondition();
                if (condition == Fragment::all)
                {
                    return "true";
                }

                auto index = fragmentPred->GetIndex();
                const auto& domain = schedule.GetLoopNest().GetDomain();
                const auto range = domain.GetDimensionRange(index);

                auto loopIndices = range.GetDependentLoopIndices(index);
                if (loopIndices.empty())
                {
                    loopIndices = { index };
                }
                bool first = true;
                std::string result = "";
                for (auto loopIndex : loopIndices)
                {
                    auto range = GetLoopRange(loopIndex, runtimeIndexVariables, schedule);

                    int testVal = 0;
                    bool valid = true;
                    switch (condition)
                    {
                    case Fragment::first:
                        testVal = range.Begin();
                        break;
                    case Fragment::last:
                        testVal = range.End() - (range.Size() % range.Increment());
                        if (testVal == range.End()) // not a boundary
                        {
                            testVal = range.End() - range.Increment();
                        }
                        break;
                    case Fragment::endBoundary:
                        testVal = range.End() - (range.Size() % range.Increment());
                        if (testVal == range.End()) // not a boundary
                        {
                            valid = false;
                        }
                        break;
                    default:
                        valid = false;
                        // throw?
                        break;
                    }

                    if (valid)
                    {
                        if (first)
                        {
                            result = "(";
                        }
                        else
                        {
                            result += " && ";
                        }
                        first = false;
                        result += "(" + GetIndexString(loopIndex, runtimeIndexVariables) + " == " + std::to_string(testVal) + ")";
                    }
                }
                return result.empty() ? "" : result + ")";
            }
            else if (predicate.Is<IndexDefinedPredicate>())
            {
                throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "IsDefined predicate not implemented");
            }
            else if (auto conjunction = predicate.As<KernelPredicateConjunction>(); conjunction != nullptr)
            {
                const auto& terms = conjunction->GetTerms();
                if (terms.size() == 0)
                {
                    return "true";
                }
                else if (terms.size() == 1)
                {
                    return GetPredicateString(*terms[0], runtimeIndexVariables, schedule);
                }
                else
                {
                    std::string result = "(";
                    bool first = true;
                    for (const auto& t : terms)
                    {
                        if (!first)
                        {
                            result += " && ";
                        }
                        first = false;
                        result += GetPredicateString(*t, runtimeIndexVariables, schedule);
                    }
                    result += ")";
                    return result;
                }
            }
            else if (auto disjunction = predicate.As<KernelPredicateDisjunction>(); disjunction != nullptr)
            {
                const auto& terms = disjunction->GetTerms();
                if (terms.size() == 0)
                {
                    return "true";
                }
                else if (terms.size() == 1)
                {
                    return GetPredicateString(*terms[0], runtimeIndexVariables, schedule);
                }
                else
                {
                    std::string result = "(";
                    bool first = true;
                    for (const auto& t : terms)
                    {
                        result += GetPredicateString(*t, runtimeIndexVariables, schedule);
                        if (!first)
                        {
                            result += " || ";
                        }
                        first = false;
                    }
                    result += ")";
                    return result;
                }
            }
            else
            {
                throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Unknown predicate type");
            }
        }

        void LoopNestPrinter::EmitIf(const KernelPredicate& predicate, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const
        {
            WriteLine("If (" + GetPredicateString(predicate, runtimeIndexVariables, schedule) + ")");
            WriteLine("{");
            ++_indentLevel;
        }
        void LoopNestPrinter::EmitElseIf(const KernelPredicate& predicate, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const
        {
            EmitEndIf();
            WriteLine("ElseIf (" + GetPredicateString(predicate, runtimeIndexVariables, schedule) + ")");
            WriteLine("{");
            ++_indentLevel;
        }
        void LoopNestPrinter::EmitElse() const
        {
            EmitEndIf();
            WriteLine("Else");
            WriteLine("{");
            ++_indentLevel;
        }
        void LoopNestPrinter::EmitEndIf() const
        {
            --_indentLevel;
            WriteLine("}");
        }

        void LoopNestPrinter::InvokeKernel(const Kernel& kernel, const KernelPredicate& predicate, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const
        {
            if (!predicate.IsEmpty())
            {
                EmitIf(predicate, runtimeIndexVariables, schedule);
            }

            InvokeKernel(kernel, runtimeIndexVariables, schedule);

            if (!predicate.IsEmpty())
            {
                EmitEndIf();
            }
        }

        void LoopNestPrinter::InvokeKernel(const Kernel& kernel, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const
        {
            const auto& renameActions = schedule.GetLoopNest().GetRenameActions();

            auto rename = [&, this](const Value& arg) {
                for (const auto& action : renameActions)
                {
                    const auto& excludedKernels = action.excludedKernels;
                    if (std::find(excludedKernels.begin(), excludedKernels.end(), kernel.GetId()) == excludedKernels.end() &&
                        std::equal_to<Value>{}(arg, action.oldValue) &&
                        AreAllFullyDefined(action.where, schedule))
                    {
                        auto newValue = action.newValue;
                        WriteLine("Using " + newValue.GetName() + " in place of " + arg.GetName());
                        return newValue;
                    }
                }
                return arg;
            };

            std::vector<std::string> args;
            for (auto v : kernel.GetArgs())
            {
                auto newV = rename(v);
                if (auto name = newV.GetName(); name.empty())
                {
                    args.push_back("<arg>");
                }
                else
                {
                    args.push_back(name);
                }
            }

            for (const auto i : kernel.GetIndices())
            {
                args.push_back(GetIndexString(i, runtimeIndexVariables));
            }

            WriteLine(kernel.GetName() + "(" + utilities::Join(args, ", ") + ");");
        }

        bool LoopNestPrinter::InvokeKernelGroup(const ScheduledKernelGroup& kernelGroup, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const
        {
            // preprocess to get only valid kernels
            auto validKernels = GetValidKernels(kernelGroup, runtimeIndexVariables, schedule);
            if (validKernels.empty())
            {
                return false;
            }

            bool first = true;
            for (const auto& kernel : validKernels)
            {
                auto predicate = schedule.GetKernelPredicate(kernel).Simplify(runtimeIndexVariables, schedule);
                if (predicate.IsAlwaysFalse())
                {
                    throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Always-false predicates should have been removed here");
                }

                if (predicate.IsAlwaysTrue())
                {
                    if (!first)
                    {
                        EmitElse();
                    }
                }
                else
                {
                    if (first)
                    {
                        EmitIf(predicate, runtimeIndexVariables, schedule);
                    }
                    else
                    {
                        EmitElseIf(predicate, runtimeIndexVariables, schedule);
                    }
                }

                InvokeKernel(kernel.kernel, runtimeIndexVariables, schedule);
                if (predicate.IsAlwaysTrue())
                {
                    // Stop evaluating, we're done
                    break;
                }

                first = false;
            }

            if (!first)
            {
                EmitEndIf();
            }

            return true;
        }

    } // namespace loopnests
} // namespace value
} // namespace ell
