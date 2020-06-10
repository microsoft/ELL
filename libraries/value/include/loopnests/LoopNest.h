////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LoopNest.h (value)
//  Authors:  Chuck Jacobs, Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CodePositionConstraints.h"
#include "Index.h"
#include "IndexRange.h"
#include "IterationDomain.h"
#include "Kernel.h"
#include "KernelPredicate.h"
#include "SplitIndexRange.h"
#include "SplitIterationDomain.h"

#include "../Value.h"

#include <ostream>
#include <unordered_map>
#include <vector>

namespace ell
{
namespace value
{
    namespace loopnests
    {
        class LoopNest;

        struct ScheduledKernel
        {
            bool newVersion = false; // Temporary workaround... to be removed

            Kernel kernel;
            CodePositionConstraints constraints;
            KernelPredicate predicate;
            KernelPredicate placement;
        };

        struct ScheduledKernelGroup
        {
            Kernel::Id id;
            std::vector<ScheduledKernel> kernels;
        };

        struct RenameAction
        {
            Value oldValue;
            Value newValue;
            std::vector<Index> where;
            std::vector<Kernel::Id> excludedKernels;
        };

        struct ScaledIndex
        {
            int scale;
            Index index;
        };

        struct IndexExpression
        {
            std::vector<ScaledIndex> indices;
            int begin = 0;
        };

        struct LoopInfo
        {
            Index loopIndex;
            Range fullRange; // the range for the "unswitched" part of this loop
            Range fragmentRange;
        };

        /// <summary>
        /// Represents the concrete sequence of loops to be generated, in detail. Derived from the loop nest and the
        /// order of the loops.
        /// </summary>
        class LoopVisitSchedule
        {
        public:
            struct LoopInfo
            {
                Index dimension;
                IndexRange indexRange;
                int boundarySize = 0;
                int scale = 0;
            };

            using StateQueue = std::vector<LoopInfo>;

            /// <summary> Copy and move constructors / assignment operators </summary>
            LoopVisitSchedule(const LoopVisitSchedule& other);
            LoopVisitSchedule(LoopVisitSchedule&&) = default;
            LoopVisitSchedule& operator=(const LoopVisitSchedule& other);
            LoopVisitSchedule& operator=(LoopVisitSchedule&& other) = default;

            /// <summary> Returns the global nest level: how many total loops are current (over all dimensions) </summary>
            int CurrentNestLevel() const;

            /// <summary> Returns `true` if all the loops have been visited </summary>
            bool IsDone() const;

            /// <summary> Returns `true` if the current loop is the innermost level </summary>
            bool IsInnermostLoop() const;

            /// <summary> The index of the current loop (e.g., `i_1`) </summary>
            Index CurrentLoopIndex() const;

            /// <summary> Returns the logical (dimension) index of the current loop (i.e., `i`, not `i_0`, `i_1`, ...) </summary>
            Index CurrentDimension() const;

            /// <summary> The span (start-end) of the entire dimension the current loop is part of. </summary>
            int DimensionSize() const;

            /// <summary> The range [start, end) of the current loop.
            Range LoopRange() const;

            /// <summary> The span (start-end) of the current loop. Only the same as the
            /// number of iterations if the increment is 1. </summary>
            int LoopSize() const;
            int LoopIncrement() const;
            int NonBoundaryEnd() const;

            /// <summary> The amount this loop index needs to be scaled when generating the expression for the original dimension index. </summary>
            int LoopIndexScale() const;

            /// <summary> Returns `true` if the current loop index has a prologue / epilogue of the given type (because there is a kernel associated with it). </summary>
            bool CurrentLoopHasFragment(std::vector<ScheduledKernel> activeKernels, LoopFragmentType fragmentType) const;

            /// <summary> Returns `true` if a future loop (one inside this loop) has a prologue / epilogue of the given type (because there is a kernel associated with it) on the same dimension as the current loop. </summary>
            bool FutureLoopHasFragmentForThisIndex(std::vector<ScheduledKernel> activeKernels, LoopFragmentType fragmentType) const;

            bool FragmentCanRunAlone(std::vector<ScheduledKernel> activeKernels, LoopFragmentType fragmentType) const; // Returns `true` if the current loop index has a prologue / epilogue of the given type (because there is a kernel associated with it), and if all such kernels are able to be in their own fragment

            int CurrentIndexEndBoundarySize() const; // Returns the size of the last inner loop if the current loop has a potentially-unswitched boundary condition at the end, or zero if the increment divides the size evenly

            bool WillVisitIndex(const Index& index) const;
            bool IsFullyDefined(const Index& index) const;
            bool IsFullyDefinedByThisLoop(const Index& index) const;
            bool WasIterationVariableDefined(const Index& index) const;
            KernelPredicate GetKernelPredicate(const ScheduledKernel& kernel) const;

            LoopVisitSchedule Next() const;
            LoopVisitSchedule Prev() const;

            const LoopInfo& Front() const;

            const SplitIterationDomain& GetDomain() const;
            const LoopNest& GetLoopNest() const { return _nest.get(); }

        private:
            friend class LoopNest;
            LoopVisitSchedule(const LoopNest& nest, StateQueue state);
            LoopVisitSchedule(const LoopNest& nest, int level, StateQueue state);

            int _level; // == current position in state queue
            StateQueue _state;
            std::reference_wrapper<const LoopNest> _nest;
        };

        /// <summary>
        /// A nested set of loops, and the code (kernels) that run inside them
        /// </summary>
        class LoopNest
        {
        public:
            LoopNest(IterationDomain domain);

            enum class ConstraintType
            {
                constraint,
                predicate
            };

            /// <summary> Add a "body" kernel to be run in the middle of the loop nest </summary>
            void AddKernel(const Kernel& kernel, ConstraintType type = ConstraintType::constraint);

            /// <summary> Add a kernel to be run as the prologue or epilogue of a loop </summary>
            void AddKernel(const Kernel& kernel, LoopFragmentType where);

            /// <summary> Add a kernel to be run as the prologue or epilogue of a loop </summary>
            void AddKernel(const Kernel& kernel, const CodePositionConstraints& where);

            /// <summary> Add a kernel to be run as allowed by a predicate </summary>
            void AddKernel(const Kernel& kernel, const KernelPredicate& predicate);

            /// <summary> Add a kernel to be run as allowed by a predicate and a placement predicate </summary>
            void AddKernel(const Kernel& kernel, const KernelPredicate& predicate, const KernelPredicate& placement);

            /// <summary> Add a kernel to be run as the prologue or epilogue of a loop </summary>
            void AddKernel(const Kernel& kernel, const CodePositionConstraints& where, const KernelPredicate& predicate, const KernelPredicate& placement);

            const std::vector<ScheduledKernel>& GetKernels() const;

            std::vector<ScheduledKernelGroup> GetKernelGroups() const;

            void Parallelize(Index index);
            [[maybe_unused]] SplitIndex Parallelize(Index index, int factor);

            void Unroll(Index index);
            [[maybe_unused]] SplitIndex Unroll(Index index, int factor);

            [[maybe_unused]] SplitIndex Split(Index index, int size);

            void SetLoopOrder(const std::vector<Index>& order);

            void RenameVariable(ViewAdapter oldVariable, ViewAdapter newVariable, const std::vector<Index>& where, const std::vector<Kernel>& excludedKernels = {});

            int NumDimensions() const;
            Range GetIndexRange(Index index) const;
            std::vector<IndexRange> GetLoopIndexRanges() const;
            const SplitIndexRange& GetDimensionRange(int dimension) const;
            const SplitIndexRange& GetDimensionRange(const Index& dimension) const;
            int NumSplits(const Index& dimension) const;
            const std::vector<Index>& GetLoopSequence() const;
            LoopVisitSchedule GetLoopSchedule() const;

            // Methods used by code generators
            int GetLoopIndexScale(const Index& index) const;

            /// <summary> Get the concrete loop index given a logical dimension index and split level </summary>
            Index GetLoopIndex(const Index& dimension, int level) const;

            bool IsParallelized(const Index& index) const;

            bool IsUnrolled(const Index& index) const;

            /// <summary> See if an Index is used as a parameter to a kernel </summary>
            bool IsUsed(const Index& index, const std::vector<ScheduledKernel>& activeKernels) const;

            /// Preliminary "variable-renaming" support
            const std::vector<RenameAction>& GetRenameActions() const;

            const SplitIterationDomain& GetDomain() const;

            Index GetBaseIndex(const Index& index) const;

            /// <summary> Return `true` iff `index` is a concrete index for a loop. </summary>
            bool IsLoopIndex(const Index& index) const;

            /// <summary> Return `true` iff `index` must be computed from other indices (i.e., it has been split). </summary>
            bool IsComputedIndex(const Index& index) const;

            IndexExpression GetIndexExpression(const Index& index) const;

            void DebugDump(std::string tag, std::ostream* stream) const;

            const std::string& Name() const { return _name; }

        private:
            void InitLoopSequence();
            void ConvertKernelConstraints();
            void ConvertKernelConstraints(ScheduledKernel& kernel);

            SplitIterationDomain _domain;
            std::vector<Index> _loopSequence;
            std::vector<ScheduledKernel> _kernels;
            std::vector<RenameAction> _renameActions;
            std::vector<Index> _parallelizedIndices;
            std::vector<Index> _unrolledIndices;
            std::string _name = UniqueName("LoopNest");
        };

        void DebugDump(const LoopNest& nest, std::string tag = "", std::ostream* stream = nullptr);

        LoopNest Fuse(const LoopNest& nest1, const LoopNest& nest2);
        LoopNest Fuse(const LoopNest& nest1, const LoopNest& nest2, const std::vector<Index>& dependentIndices1, const std::vector<Index>& dependentIndices2);

        bool operator==(const ScheduledKernel& i1, const ScheduledKernel& i2);
        bool operator!=(const ScheduledKernel& i1, const ScheduledKernel& i2);
    } // namespace loopnests
} // namespace value
} // namespace ell

namespace std
{
/// <summary> Implements a hash function for the ScheduledKernel class, so that it can be used with associative containers (maps, sets, and the like). </summary>
template <>
struct hash<::ell::value::loopnests::ScheduledKernel>
{
    using argument_type = ell::value::loopnests::ScheduledKernel;
    using result_type = std::size_t;

    /// <summary> Computes a hash of the input value. </summary>
    ///
    /// <returns> A hash value for the given input. </returns>
    result_type operator()(const argument_type& constraints) const;
};
} // namespace std
