////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoopNestVisitor.h (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "KernelPredicate.h"
#include "LoopIndexInfo.h"
#include "LoopNest.h"

#include <functional>
#include <set>
#include <unordered_map>
#include <vector>

namespace ell
{
namespace value
{
    namespace loopnests
    {
        /// <summary>
        /// Abstract base class for objects that visit a loop nest (e.g., code generators)
        /// </summary>
        class LoopNestVisitor
        {
        public:
            static Range GetLoopRange(const Index& loopIndex, const LoopIndexSymbolTable& activeRanges, const LoopVisitSchedule& schedule);

        protected:
            virtual ~LoopNestVisitor() = default;

            void Visit(const LoopNest& loopNest) const;

            bool WillKernelRunInThisLoop(const ScheduledKernel& kernel, LoopFragmentFlags kernelFilter, const LoopVisitSchedule& schedule) const;

            std::vector<ScheduledKernel> GetValidKernels(std::vector<ScheduledKernel> activeKernels,
                                                         const std::unordered_map<Index, LoopFragmentFlags>& currentFragmentStates,
                                                         LoopFragmentFlags currentLoopFlags,
                                                         LoopFragmentFlags kernelFilter,
                                                         const LoopVisitSchedule& schedule) const;

            bool ShouldRunKernel(const ScheduledKernel& kernel,
                                 LoopFragmentType placement,
                                 const std::unordered_map<Index, LoopFragmentFlags>& constraintIndices,
                                 LoopFragmentFlags currentLoopFlags,
                                 const LoopVisitSchedule& schedule) const;

            bool IsIdentity(const IndexExpression& expr, const Index& index) const;

            /// <summary>
            /// Returns `true` if the current loop body is inside the loop for the given index (so, "inside" counts the current loop being emitted)
            /// </summary>
            bool IsFullyDefined(const Index& index, const LoopVisitSchedule& schedule) const;

            /// <summary>
            /// Returns `true` if the current loop body is inside the loop for the given index (so, "inside" counts the current loop being emitted)
            /// </summary>
            bool AreAllFullyDefined(const std::vector<Index>& indices, const LoopVisitSchedule& schedule) const;

            struct RecursionState
            {
                RecursionState(const LoopNest& loopNest);
                RecursionState(const RecursionState&) = default;

                LoopIndexSymbolTable loopIndices; // map from an loop Index variable -> the actual (Scalar) runtime loop index for that loop
                LoopFragmentFlags currentFragment;
                std::unordered_map<Index, Range> activeDimensionRanges; // map from dimension index variable -> active loop range for that dimension at this recursion level if that dimension has been previously visited
                std::vector<ScheduledKernel> activeKernels;
                std::unordered_map<Index, LoopFragmentFlags> fragmentStates;
            };

            struct Partition
            {
                Index index;
                Range range;
            };
            using PartitionList = std::vector<Partition>;

            using ActiveKernelGroupList = std::vector<std::pair<bool, ScheduledKernelGroup>>;

            struct RecursionStateNew
            {
                RecursionStateNew(const LoopNest& loopNest);
                RecursionStateNew(const RecursionStateNew&) = default;

                LoopIndexSymbolTable loopIndices; // map from an loop Index variable ->
                //   the actual (Scalar) runtime loop index for that loop
                //   range visited by that variable in this branch of the code (for loops that have already been visited)
                //   state of the variable's loop (before, inside, after)
                ActiveKernelGroupList kernelGroups;
            };

            struct LoopRange
            {
                Scalar start;
                Scalar stop;
                Scalar step;
                LoopFragmentFlags futureLoopFragmentFlags;
                LoopFragmentFlags currentLoopFragmentFlags;
            };

            bool UseNewVersion(const LoopNest& loopNest) const;
            void GenerateLoopsOld(const RecursionState& state, const LoopVisitSchedule& schedule) const;
            void GenerateLoopsNew(RecursionStateNew& state, const LoopVisitSchedule& schedule) const;
            std::function<void(Scalar)> GetCodegenFnOld(const LoopRange& r, const RecursionState& state, const LoopVisitSchedule& schedule) const;
            std::function<void(Scalar)> GetCodegenFnNew(const LoopRange& r, const RecursionStateNew& state, const LoopVisitSchedule& schedule) const;

            PartitionList GetPartitions(const Index& loopIndex, Range loopRange, const ActiveKernelGroupList& kernels, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const;
            void AddSplits(const Index& loopIndex, Range loopRange, const KernelPredicate& predicate, const LoopVisitSchedule& schedule, std::set<int>& splits) const;

            // Get the end of the "regular" part of this loop (the part that divides by the tile size evenly)
            int GetMainBodyLoopEnd(const RecursionState& state, const LoopVisitSchedule& schedule, const Range& loopRange) const;
            bool LoopInEndBoundaryFragment(const RecursionState& state, const LoopVisitSchedule& schedule) const;

            void DefineComputedIndexVariables(LoopIndexSymbolTable& runtimeLoopIndices, const std::vector<ScheduledKernel>& activeKernels, const LoopVisitSchedule& schedule) const;
            LoopIndexSymbolTable GetRuntimeIndexVariables(const LoopIndexSymbolTable& runtimeLoopIndices, const LoopNest& loopNest) const;
            void DefinePostLoopIndex(const Index& loopIndex, LoopIndexSymbolTable& runtimeLoopIndices, const LoopVisitSchedule& schedule) const;

            KernelPredicate GetKernelPredicate(const ScheduledKernel& kernel, const LoopVisitSchedule& schedule) const;
            bool IsPlacementValid(const ScheduledKernel& kernel, const LoopIndexSymbolTable& runtimeLoopIndices, const LoopVisitSchedule& schedule) const;
            std::vector<ScheduledKernel> GetValidKernels(const ScheduledKernelGroup& kernelGroup, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const;

            // abstract:
            virtual void GenerateLoopRangeOld(const LoopRange& range, const RecursionState& state, const LoopVisitSchedule& schedule, std::function<void(Scalar)> codegenFn) const = 0;
            virtual void GenerateLoopRangeNew(const LoopRange& range, const RecursionStateNew& state, const LoopVisitSchedule& schedule, std::function<void(Scalar)> codegenFn) const = 0;
            virtual Scalar EmitIndexExpression(const Index& index, const IndexExpression& expr, const LoopIndexSymbolTable& indexVariables) const = 0;
            virtual void InvokeKernel(const Kernel& kernel, const KernelPredicate& predicate, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const = 0;
            virtual bool InvokeKernelGroup(const ScheduledKernelGroup& kernel, const LoopIndexSymbolTable& runtimeIndexVariables, const LoopVisitSchedule& schedule) const = 0;
        };

    } // namespace loopnests
} // namespace value
} // namespace ell
