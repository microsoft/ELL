////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoopNestPrinter.h (value)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "LoopNest.h"
#include "LoopNestVisitor.h"

#include <ostream>
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
        class LoopNestPrinter : public LoopNestVisitor
        {
        public:
            LoopNestPrinter(std::ostream& stream);
            void Print(const LoopNest& loopNest) const;

        private:
            std::ostream& _stream;
            mutable int _indentLevel;

            // RAII struct to manage indent level
            struct Indenter
            {
                Indenter(const LoopNestPrinter& printer) :
                    printer(printer) { ++printer._indentLevel; }
                ~Indenter() { --printer._indentLevel; }
                const LoopNestPrinter& printer;
            };

            void GenerateLoopRangeOld(const LoopRange& range, const RecursionState& state, const LoopVisitSchedule& schedule, std::function<void(Scalar)> codegenFn) const override;
            void GenerateLoopRangeNew(const LoopRange& range, const RecursionStateNew& state, const LoopVisitSchedule& schedule, std::function<void(Scalar)> codegenFn) const override;
            Scalar EmitIndexExpression(const Index& index, const IndexExpression& expr, const LoopIndexSymbolTable& indexVariables) const override;
            void InvokeKernel(const Kernel& kernel, const KernelPredicate& predicate, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const override;
            bool InvokeKernelGroup(const ScheduledKernelGroup& kernelGroup, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const override;

            void InvokeKernel(const Kernel& kernel, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const;
            void EmitIf(const KernelPredicate& predicate, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const;
            void EmitElseIf(const KernelPredicate& predicate, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const;
            void EmitElse() const;
            void EmitEndIf() const;

            std::string GetIndent() const;
            void WriteLine(std::string l) const;
            std::string GetIndexString(const Index& predicate, const LoopIndexSymbolTable& runtimeIndexVariables) const;
            std::string GetPredicateString(const KernelPredicate& predicate, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const;
        };

    } // namespace loopnests
} // namespace value
} // namespace ell
