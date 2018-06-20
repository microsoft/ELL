////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRIfEmitter.h (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterTypes.h"

// llvm
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Value.h>

// stl
#include <functional>

namespace ell
{
namespace emitters
{
    class IRBlockRegion;
    class IRFunctionEmitter;

    ///<summary>Class that helps with emitting If, Then, Else blocks.</summary>
    class IRIfEmitter
    {
    public:
        /// <summary> Type alias for if-else body lambda. </summary>
        using IfElseBodyFunction = std::function<void(IRFunctionEmitter& function)>;

        /// <summary> Destructor </summary>
        ~IRIfEmitter();

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
        IRIfEmitter(IRFunctionEmitter& functionEmitter, TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue);

        IRIfEmitter(const IRIfEmitter&) = delete;
        IRIfEmitter& operator=(const IRIfEmitter&) = delete;

        /// <summary> Move constructor </summary>
        IRIfEmitter(IRIfEmitter&& other);

        /// <summary> Move assignment operator </summary>
        IRIfEmitter& operator=(IRIfEmitter&& other);

        /// <summary> Emits an 'else' block. </summary>
        ///
        /// <param name="body"> A function that emits the body of the "if true" block. </param>
        IRIfEmitter& Else(IfElseBodyFunction body);

        /// <summary> Emits an 'else if' block. </summary>
        ///
        /// <param name="pValue"> Pointer to the llvm::Value that contains the boolean condition value. </param>
        /// <param name="body"> A function that emits the body of the "if true" block. </param>
        IRIfEmitter& ElseIf(llvm::Value* pValue, IfElseBodyFunction body);

        /// <summary> Ends the if else block, injecting appropriate branches. It is typically not necessary to call 
        ///     this explicitly, since it is generally called by the destructor. </summary>
        void End();

    private:
        friend IRFunctionEmitter;

        // Private constructor used by IRFunctionEmitter
        IRIfEmitter(IRFunctionEmitter& functionEmitter, bool endOnDestruct, llvm::BasicBlock* pPrevBlock = nullptr);

        llvm::BasicBlock* If(TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue);
        llvm::BasicBlock* If(llvm::Value* pValue);
        llvm::BasicBlock* If(llvm::Value* pValue, bool testValue);
        llvm::BasicBlock* If(std::function<llvm::Value*()> comparison);
        void IfThen(TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue);
        llvm::BasicBlock* Else();
        
        void PrepareBlocks();
        void PrepareBlocks(llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock);
        void PrepareBlocks(IRBlockRegion* pThenRegion, IRBlockRegion* pElseRegion);
        void EndPrev();

        llvm::BasicBlock* GetParentBlock();
        IRFunctionEmitter* _functionEmitter = nullptr; // Function we are emitting into
        llvm::BasicBlock* _pConditionBlock = nullptr; // Block into which the conditional instructions are emitted -- the "if"
        llvm::BasicBlock* _pThenBlock = nullptr; // Block into which the "Then" instructions are being emitted
        llvm::BasicBlock* _pEndBlock = nullptr; // The end block.
        llvm::BasicBlock* _pAfterBlock = nullptr; // Block where code subsequent to the "if, else" will be emitted. The end block always branches here
        bool _endOnDestruct = false;
        bool _finished = false;
    };
}
}