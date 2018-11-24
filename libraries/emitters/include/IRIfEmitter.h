////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRIfEmitter.h (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterTypes.h"
#include "LLVMUtilities.h"

// llvm
#include <llvm/IR/BasicBlock.h>

// stl
#include <functional>

namespace ell
{
namespace emitters
{
    class IRBlockRegion;
    class IRFunctionEmitter;

    ///<summary> Class that helps with emitting If, Then, Else blocks. </summary>
    class IRIfEmitter
    {
    public:
        /// <summary> Type alias for if-else body lambda. </summary>
        using IfElseBodyFunction = std::function<void(IRFunctionEmitter& function)>;

        /// <summary> Destructor </summary>
        ~IRIfEmitter();

        /// <summary> Constructor with a body function. </summary>
        ///
        /// <param name="functionEmitter"> The function emitter. </param>
        /// <param name="pValue"> Pointer to the llvm::Value that contains the boolean condition value. </param>
        /// <param name="body"> A function that emits the body of the "if true" block. </param>
        IRIfEmitter(IRFunctionEmitter& functionEmitter, LLVMValue pValue, IfElseBodyFunction body);

        /// <summary> Constructor with a body function. </summary>
        ///
        /// <param name="functionEmitter"> The function emitter. </param>
        /// <param name="comparison"> A function that returns a boolean (LLVM) value. </param>
        /// <param name="body"> A function that emits the body of the "if true" block. </param>
        IRIfEmitter(IRFunctionEmitter& functionEmitter, std::function<LLVMValue()> comparison, IfElseBodyFunction body);

        /// <summary> Constructor with a body function. </summary>
        ///
        /// <param name="functionEmitter"> The function emitter. </param>
        /// <param name="comparison"> The if-statement comparison. </param>
        /// <param name="pValue"> Pointer to the value used in the comparison. </param>
        /// <param name="pTestValue"> Pointer to the test value that is compared against pValue. </param>
        /// <param name="body"> A function that emits the body of the "if true" block. </param>
        IRIfEmitter(IRFunctionEmitter& functionEmitter, TypedComparison comparison, LLVMValue pValue, LLVMValue pTestValue, IfElseBodyFunction body);

        /// <summary>
        /// Constuct an emitter for the given function. If pPrevBlock was supplied, injects new blocks
        /// after it.
        /// </summary>
        ///
        /// <param name="functionEmitter"> The function emitter. </param>
        /// <param name="pPrevBlock"> Pointer to the previous block. </param>
        IRIfEmitter(IRFunctionEmitter& functionEmitter, llvm::BasicBlock* pPrevBlock = nullptr);

        /// <summary>
        /// Constuct an emitter for the given function, and set up the first If statement with its "Then"
        /// in place.
        /// </summary>
        ///
        /// <param name="functionEmitter"> The function emitter. </param>
        /// <param name="comparison"> The comparison. </param>
        /// <param name="pValue"> Pointer to the llvm::Value that contains the value used to define the comparison. </param>
        /// <param name="pTestValue"> Pointer to the llvm::Value that contains the test value, which is compared against pValue. </param>
        IRIfEmitter(IRFunctionEmitter& functionEmitter, TypedComparison comparison, LLVMValue pValue, LLVMValue pTestValue);

        IRIfEmitter(const IRIfEmitter&) = delete;
        IRIfEmitter& operator=(const IRIfEmitter&) = delete;

        /// <summary> Move constructor </summary>
        IRIfEmitter(IRIfEmitter&& other);

        /// <summary> Move assignment operator </summary>
        IRIfEmitter& operator=(IRIfEmitter&& other);

        /// <summary> Emits an 'if' block. </summary>
        ///
        /// <param name="pValue"> Pointer to the llvm::Value that contains the boolean condition value. </param>
        /// <param name="body"> A function that emits the body of the "if true" block. </param>
        IRIfEmitter& If(LLVMValue pValue, IfElseBodyFunction body);

        /// <summary> Emits an 'if' block. </summary>
        ///
        /// <param name="comparison"> A comparison to use when comparing two condition values. </param>
        /// <param name="pValue"> One condition value. </param>
        /// <param name="pValue"> The other condition value. </param>
        /// <param name="body"> A function that emits the body of the "if true" block. </param>
        IRIfEmitter& If(TypedComparison comparison, LLVMValue pValue, LLVMValue pTestValue, IfElseBodyFunction body);

        /// <summary> Emits an 'else if' block. </summary>
        ///
        /// <param name="pValue"> Pointer to the llvm::Value that contains the boolean condition value. </param>
        /// <param name="body"> A function that emits the body of the "if true" block. </param>
        IRIfEmitter& ElseIf(LLVMValue pValue, IfElseBodyFunction body);

        /// <summary> Emits an 'else' block. </summary>
        ///
        /// <param name="body"> A function that emits the body of the "if true" block. </param>
        IRIfEmitter& Else(IfElseBodyFunction body);

        /// <summary> Ends the if else block, injecting appropriate branches. It is typically not necessary to call
        ///     this explicitly, since it is generally called by the destructor. </summary>
        void End();

    private:
        friend IRFunctionEmitter;

        llvm::BasicBlock* If(TypedComparison comparison, LLVMValue pValue, LLVMValue pTestValue);
        llvm::BasicBlock* If(LLVMValue pValue);
        llvm::BasicBlock* If(LLVMValue pValue, bool testValue);
        llvm::BasicBlock* If(std::function<LLVMValue()> comparison);
        llvm::BasicBlock* Else();

        void PrepareBlocks();
        void EndPrev();
        void BranchToAfterBlock();

        llvm::BasicBlock* GetParentBlock();
        IRFunctionEmitter* _functionEmitter = nullptr; // Function we are emitting into
        llvm::BasicBlock* _pConditionBlock = nullptr; // Block into which the conditional instructions are emitted -- the "if"
        llvm::BasicBlock* _pThenBlock = nullptr; // Block into which the "Then" instructions are being emitted
        llvm::BasicBlock* _pEndBlock = nullptr; // The end block.
        llvm::BasicBlock* _pAfterBlock = nullptr; // Block where code subsequent to the "if, else" will be emitted. The end block always branches here
        bool _isContinuation = false;
        bool _finished = false;
    };
} // namespace emitters
} // namespace ell