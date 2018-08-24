////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRParallelLoopEmitter.h (emitters)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IRLocalScalar.h"
#include "LLVMUtilities.h"

// stl
#include <functional>
#include <vector>

namespace ell
{
namespace emitters
{
    class IRFunctionEmitter;

    /// <summary> Struct for options influencing parallel loop generation. </summary>
    struct ParallelLoopOptions
    {
        ParallelLoopOptions() : numTasks(0) {}
        ParallelLoopOptions(int numTasks) : numTasks(numTasks) {}

        int numTasks = 0; // The number of tasks to break the loop into. '0' is the special 'auto' flag
    };

    /// <summary> Class that simplifies parallel for loop creation. </summary>
    class IRParallelForLoopEmitter
    {
    public:
        /// <summary> Signature for function defining the body of the loop. </summary>
        ///
        /// <param name="function"> The function the loop body is being emitted into. </param>
        /// <param name="iterationVariable"> The iteration variable for the loop. </param>
        /// <param name="capturedValues"> The local values from the outside scope being passed in to the loop.
        ///    Unlike regular for loops, parallel loops will emit malformed code if any emitted local values
        ///    are imported from the enclosing scope via a lambda capture. They must be passed in through the
        ///    capturedValues argument instead. Other values (e.g., normal C++ values) can be passed in through
        ///    lambda captures, though. </param>
        using BodyFunction = std::function<void(IRFunctionEmitter& function, IRLocalScalar iterationVariable, std::vector<LLVMValue> capturedValues)>;

    private:
        friend IRFunctionEmitter;
        IRParallelForLoopEmitter(IRFunctionEmitter& functionEmitter);

        void EmitLoop(int begin, int end, int increment, const ParallelLoopOptions& options, const std::vector<LLVMValue>& capturedValues, BodyFunction body);
        void EmitLoop(IRLocalScalar begin, IRLocalScalar end, IRLocalScalar increment, const ParallelLoopOptions& options, const std::vector<LLVMValue>& capturedValues, BodyFunction body);

        IRFunctionEmitter GetTaskFunction(const std::vector<LLVMValue>& capturedValues, BodyFunction body);

        LLVMValue GetIterationVariable();
        LLVMValue LoadIterationVariable();

        IRFunctionEmitter& _functionEmitter; // Loop written into this function
    };
}
}
