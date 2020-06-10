////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CodeGenerator.h (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "LoopNest.h"
#include "LoopNestVisitor.h"

#include <unordered_map>

namespace ell
{
namespace value
{
    namespace loopnests
    {
        /// <summary>
        /// Takes a loop nest and generates code for it
        /// </summary>
        class CodeGenerator : public LoopNestVisitor
        {
        public:
            void Run(const LoopNest& loopNest) const;

        private:
            void GenerateLoopRangeOld(const LoopRange& range, const RecursionState& state, const LoopVisitSchedule& schedule, std::function<void(Scalar)> codegenFn) const override;
            void GenerateLoopRangeNew(const LoopRange& range, const RecursionStateNew& state, const LoopVisitSchedule& schedule, std::function<void(Scalar)> codegenFn) const override;
            Scalar EmitIndexExpression(const Index& index, const IndexExpression& expr, const LoopIndexSymbolTable& indexVariables) const override;
            void InvokeKernel(const Kernel& kernel, const KernelPredicate& predicate, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const override;
            bool InvokeKernelGroup(const ScheduledKernelGroup& kernelGroup, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const override;

            void InvokeKernel(const Kernel& kernel, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const;
            Scalar EmitKernelPredicate(const KernelPredicate& predicate, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const;
        };

    } // namespace loopnests
} // namespace value
} // namespace ell
