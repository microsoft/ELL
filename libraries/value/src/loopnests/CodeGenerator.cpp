////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CodeGenerator.cpp (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "loopnests/CodeGenerator.h"
#include "loopnests/KernelPredicate.h"

#include "LLVMContext.h"

#include <algorithm>
#include <numeric>
#include <optional>
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

        void CodeGenerator::Run(const LoopNest& loopNest) const
        {
            Visit(loopNest);
        }

        void CodeGenerator::GenerateLoopRangeNew(const LoopRange& r, const RecursionStateNew& state, const LoopVisitSchedule& schedule, std::function<void(Scalar)> codegenFn) const
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

            if (!(isParallelized || isUnrolled))
            {
                ForRange(UniqueName(loopNest.Name()), r.start, r.stop, r.step, codegenFn);
            }
            else if (isParallelized)
            {
                int numThreads = numIterations;

                std::vector<Value> kernelInputs;
                for (const auto& g : state.kernelGroups)
                {
                    if (g.first)
                    {
                        for (const auto& scheduledKernel : g.second.kernels)
                        {
                            auto& kernel = scheduledKernel.kernel;
                            const auto& inputs = kernel.GetArgs();
                            kernelInputs.insert(kernelInputs.end(), inputs.begin(), inputs.end());
                        }
                    }
                }

                std::once_flag onceFlag;
                GetContext().Parallelize(numThreads, kernelInputs, [&](Scalar index, std::vector<Value> captured) mutable {
                    assert(kernelInputs.size() == captured.size());

                    std::call_once(onceFlag, [&] {
                        for (unsigned i = 0; i < captured.size(); ++i)
                        {
                            // TODO: figure out what to do with the "where" parameter
                            // TODO: get rid of const_cast
                            const_cast<LoopNest&>(loopNest).RenameVariable(kernelInputs[i], captured[i], { loopIndex });
                        }
                    });

                    codegenFn(index * stepInt);
                });
            }
            else if (isUnrolled)
            {
                for (int i = startInt; i < stopInt; i += stepInt)
                {
                    codegenFn(i);
                }
            }
        }

        void CodeGenerator::GenerateLoopRangeOld(const LoopRange& r, const RecursionState& state, const LoopVisitSchedule& schedule, std::function<void(Scalar)> codegenFn) const
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

            if (!(isParallelized || isUnrolled))
            {
                ForRange(UniqueName(loopNest.Name()), r.start, r.stop, r.step, codegenFn);
            }
            else if (isParallelized)
            {
                int numThreads = numIterations;

                const auto& scheduledKernels = state.activeKernels;
                std::vector<Value> kernelInputs;
                for (const auto& scheduledKernel : scheduledKernels)
                {
                    auto& kernel = scheduledKernel.kernel;

                    const auto& inputs = kernel.GetArgs();
                    kernelInputs.insert(kernelInputs.end(), inputs.begin(), inputs.end());
                }

                std::once_flag onceFlag;
                GetContext().Parallelize(numThreads, kernelInputs, [&](Scalar index, std::vector<Value> captured) mutable {
                    assert(kernelInputs.size() == captured.size());

                    std::call_once(onceFlag, [&] {
                        for (unsigned i = 0; i < captured.size(); ++i)
                        {
                            // TODO: figure out what to do with the "where" parameter
                            // TODO: get rid of const_cast
                            const_cast<LoopNest&>(loopNest).RenameVariable(kernelInputs[i], captured[i], { loopIndex });
                        }
                    });

                    codegenFn(index * stepInt);
                });
            }
            else if (isUnrolled)
            {
                for (int i = startInt; i < stopInt; i += stepInt)
                {
                    codegenFn(i);
                }
            }
        }

        Scalar CodeGenerator::EmitIndexExpression(const Index& index, const IndexExpression& expr, const LoopIndexSymbolTable& indexVariables) const
        {
            if (!expr.indices.empty())
            {
                // We can't currently optimize away the "identity" expression, because the result (a loops "index" Scalar)
                // would be a register variable (pointer valence 0), and the generated kernel function expects a stored value
                // (pointer valence 1). So, we need to call `Allocate()` to get a stored variable.
                auto sum = Scalar(Allocate<int>(utilities::ScalarLayout));
                sum = expr.begin;
                for (auto scaledIndex : expr.indices)
                {
                    if (auto it = indexVariables.find(scaledIndex.index); it != indexVariables.end())
                    {
                        auto indexValue = it->second.value;
                        sum += indexValue * scaledIndex.scale;
                    }
                }

                return sum;
            }
            return 0;
        }

        Scalar CodeGenerator::EmitKernelPredicate(const KernelPredicate& predicate, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const
        {
            const auto& domain = schedule.GetLoopNest().GetDomain();
            auto predResult = MakeScalar<int>("predResult");
            predResult = 1; // "true"

            auto emitPredicate = [&domain, &runtimeIndexVariables, &schedule](const auto& emitPredicate, const KernelPredicate& p, Scalar& result, bool defaultIsTrue) -> void {
                if (p.IsAlwaysTrue())
                {
                    if (defaultIsTrue)
                    {
                        // nothing
                    }
                    else
                    {
                        result = Scalar(1); // "true"
                    }
                }
                if (p.IsAlwaysFalse())
                {
                    if (defaultIsTrue)
                    {
                        result = Scalar(0); // "false"
                    }
                    else
                    {
                        // nothing
                    }
                }
                else if (auto simplePredicate = p.As<FragmentTypePredicate>(); simplePredicate != nullptr)
                {
                    auto condition = simplePredicate->GetCondition();
                    if (condition == Fragment::all)
                    {
                        return; // do nothing for 'all' predicates
                    }

                    auto index = simplePredicate->GetIndex();
                    const auto range = domain.GetDimensionRange(index);

                    auto loopIndices = range.GetDependentLoopIndices(index);
                    if (loopIndices.empty())
                    {
                        loopIndices = { index };
                    }
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
                            if (testVal == range.End())
                            {
                                valid = false;
                            }
                            break;
                        default:
                            // throw?
                            valid = false;
                            break;
                        }

                        if (valid)
                        {
                            // if loop index not present, assume 0
                            Scalar indexVal = MakeScalar<int>("predIndexVal");
                            if (runtimeIndexVariables.count(loopIndex) != 0)
                            {
                                indexVal = runtimeIndexVariables.at(loopIndex).value;
                            }

                            if (defaultIsTrue)
                            {
                                If(indexVal != testVal, [&] {
                                    result = Scalar(0); // "false"
                                });
                            }
                            else
                            {
                                If(indexVal == testVal, [&] {
                                    result = Scalar(1); // "true"
                                });
                            }
                        }
                    }
                }
                else if (p.Is<IndexDefinedPredicate>())
                {
                    throw utilities::LogicException(utilities::LogicExceptionErrors::notImplemented, "IsDefined predicate not implemented");
                }
                else if (auto conjunction = p.As<KernelPredicateConjunction>(); conjunction != nullptr)
                {
                    auto conjResult = MakeScalar<int>("conj");
                    conjResult = Scalar(1); // "true"
                    for (const auto& t : conjunction->GetTerms())
                    {
                        emitPredicate(emitPredicate, *t, conjResult, true);
                    }

                    if (defaultIsTrue)
                    {
                        If(conjResult == 0, [&result] {
                            result = Scalar(0); // "false"
                        });
                    }
                    else
                    {
                        If(conjResult != 0, [&result] {
                            result = Scalar(1); // "true"
                        });
                    }
                }
                else if (auto disjunction = p.As<KernelPredicateDisjunction>(); disjunction != nullptr)
                {
                    auto disjResult = MakeScalar<int>("disj");
                    disjResult = Scalar(0); // "false"
                    for (const auto& t : disjunction->GetTerms())
                    {
                        emitPredicate(emitPredicate, *t, disjResult, false);
                    }
                    if (defaultIsTrue)
                    {
                        If(disjResult == 0, [&result] {
                            result = Scalar(0); // "false"
                        });
                    }
                    else
                    {
                        If(disjResult != 0, [&result] {
                            result = Scalar(1); // "true"
                        });
                    }
                }
                else
                {
                    throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Unknown predicate type");
                }
            };

            emitPredicate(emitPredicate, predicate, predResult, true);
            return predResult;
        }

        void CodeGenerator::InvokeKernel(const Kernel& kernel, const KernelPredicate& predicate, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const
        {
            if (predicate.IsAlwaysTrue())
            {
                InvokeKernel(kernel, runtimeIndexVariables, schedule);
            }
            else
            {
                If(EmitKernelPredicate(predicate, runtimeIndexVariables, schedule) == 1, [&] {
                    InvokeKernel(kernel, runtimeIndexVariables, schedule);
                });
            }
        }

        void CodeGenerator::InvokeKernel(const Kernel& kernel, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const
        {
            const auto& kernelArgs = kernel.GetArgs();
            const auto& kernelIndices = kernel.GetIndices();

            const auto& renameActions = schedule.GetLoopNest().GetRenameActions();

            // Create argument list
            std::vector<Value> kernelArgValues;
            kernelArgValues.reserve(kernelArgs.size());

            auto rename = [&](const Value& arg) {
                for (const auto& action : renameActions)
                {
                    const auto& excludedKernels = action.excludedKernels;
                    if (std::find(excludedKernels.begin(), excludedKernels.end(), kernel.GetId()) == excludedKernels.end() &&
                        std::equal_to<Value>{}(arg, action.oldValue) &&
                        AreAllFullyDefined(action.where, schedule))
                    {
                        return action.newValue;
                    }
                }
                return arg;
            };

            for (const auto& arg : kernelArgs)
            {
                kernelArgValues.push_back(rename(arg));
            }

            std::vector<Value> kernelIndexValues;
            kernelIndexValues.reserve(kernelIndices.size());
            for (auto index : kernelIndices)
            {
                kernelIndexValues.push_back(runtimeIndexVariables.at(index).value.GetValue());
                auto name = kernelIndexValues.back().GetName();
                kernelIndexValues.back().SetName(index.GetName());
            }

            kernel.Call(kernelArgValues, kernelIndexValues);
        }

        bool CodeGenerator::InvokeKernelGroup(const ScheduledKernelGroup& kernelGroup, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const
        {
            // preprocess to get only valid kernels
            auto validKernels = GetValidKernels(kernelGroup, runtimeIndexVariables, schedule);

            if (validKernels.empty())
            {
                return false;
            }

            std::optional<EmitterContext::IfContext> ifContext;
            for (const auto& kernel : validKernels)
            {
                auto predicate = schedule.GetKernelPredicate(kernel).Simplify(runtimeIndexVariables, schedule);
                if (predicate.IsAlwaysFalse())
                {
                    throw utilities::LogicException(utilities::LogicExceptionErrors::illegalState, "Always-false predicates should have been removed here");
                }

                if (predicate.IsAlwaysTrue())
                {
                    if (ifContext)
                    {
                        // We're already inside an 'if' cascade, so add final 'else' clause
                        ifContext.value().Else([&] {
                            InvokeKernel(kernel.kernel, runtimeIndexVariables, schedule);
                        });
                    }
                    else
                    {
                        // If the first kernel's predicate is trivially 'true', just invoke the kernel and exit
                        InvokeKernel(kernel.kernel, runtimeIndexVariables, schedule);
                    }
                    break;
                }
                else
                {
                    if (!ifContext)
                    {
                        auto predicateVal = EmitKernelPredicate(predicate, runtimeIndexVariables, schedule);
                        ifContext.emplace(If(predicateVal == 1, [&] {
                            InvokeKernel(kernel.kernel, runtimeIndexVariables, schedule);
                        }));
                    }
                    else
                    {
                        auto predicateVal = EmitKernelPredicate(predicate, runtimeIndexVariables, schedule);
                        ifContext.value().ElseIf(predicateVal == 1, [&] {
                            InvokeKernel(kernel.kernel, runtimeIndexVariables, schedule);
                        });
                    }
                }
            }

            return true;
        }
    } // namespace loopnests
} // namespace value
} // namespace ell
