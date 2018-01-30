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

        /// <summary> Destructor </summary>
        ~IRIfEmitter();

        /// <summary> Type alias for if-else body lambda. </summary>
        using IfElseBodyFunction = std::function<void(IRFunctionEmitter& function)>;

        /// <summary> Emits an 'else' block. </summary>
        ///
        /// <param name="body"> A function that emits the body of the "if true" block. </param>
        IRIfEmitter& Else(IfElseBodyFunction body);

        /// <summary> Emits an 'else if' block. </summary>
        ///
        /// <param name="pValue"> Pointer to the llvm::Value that contains the boolean condition value. </param>
        /// <param name="body"> A function that emits the body of the "if true" block. </param>
        IRIfEmitter& ElseIf(llvm::Value* pValue, IfElseBodyFunction body);

        /// <summary>
        /// Emit a comparison of pValue to pTestValue and a branch to the "Then" block. Makes the Then
        /// block the current block.
        /// </summary>
        ///
        /// <param name="comparison"> The comparison. </param>
        /// <param name="pValue"> Pointer to the llvm::Value that contains the value used to define the comparison. </param>
        /// <param name="pTestValue"> Pointer to the llvm::Value that contains the test value, which is compared against pValue. </param>
        ///
        /// <returns> Returns the "Then" block. </returns>
        llvm::BasicBlock* If(TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue);

        /// <summary>
        /// Emit a comparison testing if pValue is true and a branch to the "Then" block. Makes
        /// the Then block the current block.
        /// </summary>
        ///
        /// <param name="pValue"> Pointer to the llvm::Value that contains the boolean condition value. </param>
        ///
        /// <returns> Returns the "Then" block. </returns>
        llvm::BasicBlock* If(llvm::Value* pValue);

        /// <summary>
        /// Emit a comparison testing if pValue matches testValue and a branch to the "Then" block. Makes
        /// the Then block the current block.
        /// </summary>
        ///
        /// <param name="pValue"> Pointer to the llvm::Value that contains the value used to define the comparison. </param>
        /// <param name="testValue"> The boolean test value. </param>
        ///
        /// <returns> Returns the "Then" block. </returns>
        llvm::BasicBlock* If(llvm::Value* pValue, bool testValue);

        /// <summary> Emit a comparision testing the value from the lambda. </summary>
        ///
        /// <param name="comparison"> A comparison function. </param>
        ///
        /// <returns> Returns the "Then" block. </returns>
        llvm::BasicBlock* If(std::function<llvm::Value*()> comparison);

        /// <summary> Pure syntactic sugar for readability - works just like "If". </summary>
        ///
        /// <param name="comparison"> The comparison. </param>
        /// <param name="pValue"> Pointer to the llvm::Value that contains the value used to define the comparison. </param>
        /// <param name="pTestValue"> Pointer to the llvm::Value that contains the test value, which is compared against pValue. </param>
        ///
        /// <returns> Returns the "Then" block. </returns>
        llvm::BasicBlock* IfElse(TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue);

        /// <summary> Emits an 'else' block, and makes it the current block. </summary>
        ///
        /// <returns> Returns the Else block. </returns>
        llvm::BasicBlock* Else();

        /// <summary> Ends the if else block, injecting appropriate branches. </summary>
        void End();

        /// <summary> Conditional using pre-existing then and else blocks. </summary>
        ///
        /// <param name="comparison"> The comparison.
        /// <param name="pValue"> Pointer to the llvm::Value that contains the value used to define the comparison. </param>
        /// <param name="pTestValue"> Pointer to the llvm::Value that contains the test value, which is compared against pValue. </param>
        /// <param name="pThenBlock"> Pointer to the llvm::BasicBlock that represents the Then block. </param>
        /// <param name="pElseBlock"> Pointer to the llvm::BasicBlock that represents the Else block. </param>
        ///
        /// <returns> Returns the "Then" block. </returns>
        llvm::BasicBlock* IfThenElse(TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock);

        /// <summary> Conditional using pre-existing then and else blocks. </summary>
        ///
        /// <param name="comparison"> The comparison.
        /// <param name="pValue"> Pointer to the llvm::Value that contains the value used to define the comparison. </param>
        /// <param name="pTestValue"> Pointer to the llvm::Value that contains the test value, which is compared against pValue. </param>
        /// <param name="pThenRegion"> Pointer to the Then block region. </param>
        /// <param name="pElseRegion"> Pointer to the Else block region. </param>
        ///
        /// <returns> Returns the "Then" block. </returns>
        llvm::BasicBlock* IfThenElse(TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue, IRBlockRegion* pThenRegion, IRBlockRegion* pElseRegion);

    private:
        friend IRFunctionEmitter;

        // Private constructor for new lambda-based IRFunctionEmitter::If() method
        IRIfEmitter(IRFunctionEmitter& functionEmitter, bool endOnDestruct, llvm::BasicBlock* pPrevBlock = nullptr);

        void IfThen(TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue);
        void IfThen(llvm::Value* pValue);
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
    };
}
}