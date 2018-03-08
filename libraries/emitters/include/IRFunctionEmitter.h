////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRFunctionEmitter.h (emitters)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "EmitterTypes.h"
#include "IRAsyncTask.h"
#include "IRBlockRegion.h"
#include "IREmitter.h"
#include "IRIfEmitter.h"
#include "IRLocalValue.h"
#include "IRLoopEmitter.h"
#include "IROptimizer.h"
#include "IRTask.h"
#include "Variable.h"

// llvm
#include <llvm/IR/Argument.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

// stl
#include <functional>
#include <initializer_list>
#include <ostream>
#include <string>
#include <vector>

namespace ell
{
namespace emitters
{
    class IRModuleEmitter;

    /// <summary> Used to emit code into an existing LLVM IR Function </summary>
    class IRFunctionEmitter
    {
    public:
        /// <summary> Query if this IRFunctionEmitter is valid. </summary>
        ///
        /// <returns> True if valid, false if not. </returns>
        bool IsValid() const { return _pModuleEmitter != nullptr; }

        /// <summary> Verify (as far as possible) if the function's IR is valid. </summary>
        void Verify();

        /// <summary> Gets the name of the function being emitted. </summary>
        std::string GetFunctionName() { return _name; }

        /// <summary> Gets an `IRLocalScalar` wrapper for an LLVM value object. </summary>
        ///
        /// <param name="value"> The value to wrap. </param>
        IRLocalScalar LocalScalar(llvm::Value* value);

        /// <summary> Gets an `IRLocalScalar` wrapper for a literal constant. </summary>
        ///
        /// <param name="value"> The literal value to wrap. </param>
        template <typename ValueType, utilities::IsFundamental<ValueType> = true>
        IRLocalScalar LocalScalar(ValueType value);

        /// <summary> Gets an uninitialized `IRLocalScalar` wrapper. </summary>
        IRLocalScalar LocalScalar();
        
        /// <summary> Gets an `IRLocalArray` wrapper for an LLVM value object that represents an indexable array. </summary>
        ///
        /// <param name="value"> The value to wrap. </param>
        IRLocalArray LocalArray(llvm::Value* value);

        /// <summary> Gets an emitted variable by scope and name. </summary>
        ///
        /// <param name="scope"> The variable scope. </param>
        /// <param name="name"> The variable name. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the variable. </returns>
        llvm::Value* GetEmittedVariable(const VariableScope scope, const std::string& name);

        /// <summary> Gets an argument to the function by name. </summary>
        ///
        /// <param name="name"> The variable name. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the variable. </returns>
        llvm::Value* GetFunctionArgument(const std::string& name);

        /// <summary> Emit a literal into the function. </summary>
        ///
        /// <param name="valueType"> Type of literal to emit. </param>
        /// <param name="value"> The value of the literal. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the literal. </returns>
        template <typename ValueType>
        llvm::Value* Literal(ValueType value);

        /// <summary> Emit a literal pointer into the function. </summary>
        ///
        /// <typeparam name="ValueType"> Type of pointer to emit. </param>
        /// <param name="value"> The value of the pointer. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the literal. </returns>
        template <typename ValueType>
        llvm::Value* Pointer(ValueType* value);

        /// <summary> Convenience function for returning a single-bit LLVM boolean true value. </summary>
        ///
        /// <returns> An llvm i1 type representing `true`. </returns>
        llvm::Value* TrueBit() { return _pEmitter->TrueBit(); }

        /// <summary> Convenience function for returning a single-bit LLVM boolean false value. </summary>
        ///
        /// <returns> An llvm i1 type representing `false`. </returns>
        llvm::Value* FalseBit() { return _pEmitter->FalseBit(); }

        /// <summary> Convenience function for returning a null pointer constant. </summary>
        ///
        /// <param name="pointerType"> The llvm type of the pointer to return. </param>
        ///
        /// <returns> Pointer to an llvm::ConstantPointerNull that represents a null pointer of the given pointer type. </returns>
        llvm::ConstantPointerNull* NullPointer(llvm::PointerType* pointerType) { return _pEmitter->NullPointer(pointerType); } // STYLE discrepancy

        /// <summary> Emit an instruction to load a function argument. </summary>
        ///
        /// <param name="argument"> The argument. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the function argument. </returns>
        llvm::Value* LoadArgument(llvm::ilist_iterator<llvm::Argument>& argument);

        /// <summary> Emit an instruction to load a function argument. </summary>
        ///
        /// <param name="argument"> The argument. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the function argument. </returns>
        llvm::Value* LoadArgument(llvm::Argument& argument);

        /// <summary> Emit a value-preserving cast operation from one type to another. </summary>
        ///
        /// <typeparam name="InputType"> The input type. </typeparam>
        /// <typeparam name="OuputType"> The output type. </typeparam>
        /// <param name="pValue"> Pointer to the input value. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        template <typename InputType, typename OutputType>
        llvm::Value* CastValue(llvm::Value* pValue);

        /// <summary> Emit cast to a given type. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="valueType"> The type to cast to. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        llvm::Value* BitCast(llvm::Value* pValue, VariableType valueType);

        /// <summary> Emit cast to a given type. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="valueType"> The type to cast to. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        llvm::Value* BitCast(llvm::Value* pValue, llvm::Type* valueType);

        /// <summary> Emit pointer cast to a given pointer type. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="valueType"> The type to cast to. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        llvm::Value* CastPointer(llvm::Value* pValue, llvm::Type* valueType);

        /// <summary> Emit pointer cast to a given pointer type. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="valueType"> The type to cast to. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        llvm::Value* CastPointer(llvm::Value* pValue, VariableType valueType);

        /// <summary> Emit a cast from an integer type to a pointer. </summary>
        ///
        /// <param name="pValue"> Input value. </param>
        /// <param name="destinationType"> Output pointer type. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        llvm::Value* CastIntToPointer(llvm::Value* pValue, llvm::Type* destinationType);

        /// <summary> Emit a cast from a pointer to an integer type. </summary>
        ///
        /// <param name="pValue"> Input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        llvm::Value* CastPointerToInt(llvm::Value* pValue, llvm::Type* destinationType);

        /// <summary> Emit a cast from int to float. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type (float or double). </param>
        /// <param name="isSigned"> true if the value is signed. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        llvm::Value* CastIntToFloat(llvm::Value* pValue, VariableType destinationType, bool isSigned);

        /// <summary> Emit a cast from float to int. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type (int32 or int64). </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        llvm::Value* CastFloatToInt(llvm::Value* pValue, VariableType destinationType = VariableType::Int32);

        /// <summary> Emit a cast from bool to byte. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        llvm::Value* CastBoolToByte(llvm::Value* pValue);

        /// <summary> Emit a cast from bool to int. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        llvm::Value* CastBoolToInt(llvm::Value* pValue);

        /// <summary> Emit a call to a function with a single optional argument. </summary>
        ///
        /// <param name="name"> The function name. </param>
        /// <param name="pArgument"> Pointer to the optional argument. </param>
        ///
        /// <returns> Pointer to the result of the function call. </returns>
        llvm::Value* Call(const std::string& name, llvm::Value* pArgument = nullptr);

        /// <summary> Emit a call to a function with arguments. </summary>
        ///
        /// <param name="name"> The function name. </param>
        /// <param name="arguments"> The function arguments. </param>
        ///
        /// <returns> Pointer to the result of the function call. </returns>
        llvm::Value* Call(const std::string& name, const IRValueList& arguments);

        /// <summary> Emit a call to a function with arguments. </summary>
        ///
        /// <param name="name"> The function name. </param>
        /// <param name="arguments"> The function arguments. </param>
        ///
        /// <returns> Pointer to the result of the function call. </returns>
        llvm::Value* Call(const std::string& name, std::initializer_list<llvm::Value*> arguments);

        /// <summary> Emit a call to a function with arguments. </summary>
        ///
        /// <param name="function"> The IRFunctionEmitter for the function to call. </param>
        /// <param name="arguments"> The function arguments. </param>
        ///
        /// <returns> Pointer to the result of the function call. </returns>
        llvm::Value* Call(IRFunctionEmitter& function, std::vector<llvm::Value*> arguments);

        /// <summary> Emit a call to a function with arguments. </summary>
        ///
        /// <param name="pFunction"> Pointer to the llvm::Function. </param>
        /// <param name="arguments"> The function arguments. </param>
        ///
        /// <returns> Pointer to the result of the function call. </returns>
        llvm::Value* Call(llvm::Function* pFunction, std::initializer_list<llvm::Value*> arguments);

        /// <summary> Emit a call to a function with arguments. </summary>
        ///
        /// <param name="pFunction"> Pointer to the llvm::Function. </param>
        /// <param name="arguments"> The function arguments. </param>
        ///
        /// <returns> Pointer to the result of the function call. </returns>
        llvm::Value* Call(llvm::Function* pFunction, std::vector<llvm::Value*> arguments);

        /// <summary> Emit a return from a function with no return value. </summary>
        void Return();

        /// <summary> Emit a return of a given value. </summary>
        ///
        /// <param name="value"> Pointer to the function return value. </param>
        ///
        /// <returns> Pointer to the result of the function call. </returns>
        llvm::Value* Return(llvm::Value* value);

        /// <summary> Emit a unary operator with a scalar argument. </summary>
        ///
        /// <param name="type"> The operator type. </param>
        /// <param name="value"> The argument of the operator. </param>
        ///
        /// <returns> Pointer to the return value of the operator. </returns>
        llvm::Value* Operator(UnaryOperationType type, llvm::Value* value);

        /// <summary> Emit a binary operator with 2 scalar arguments. </summary>
        ///
        /// <param name="type"> The operator type. </param>
        /// <param name="pLeftValue"> Pointer to the left argument of the operator. </param>
        /// <param name="pRightValue"> Pointer to the right value of the operator. </param>
        ///
        /// <returns> Pointer to the return value of the operator. </returns>
        llvm::Value* Operator(TypedOperator type, llvm::Value* pLeftValue, llvm::Value* pRightValue);

        /// <summary> Emit a binary operator with 2 scalar arguments. </summary>
        ///
        /// <param name="type"> The operator type. </param>
        /// <param name="arguments"> The operator arguments. </param>
        ///
        /// <returns> Pointer to the return value of the operator. </returns>
        llvm::Value* Operator(TypedOperator type, llvm::iterator_range<llvm::Function::arg_iterator>& arguments);

        /// <summary>
        /// Emit binary operator over 2 equal sized vector arguments. The operator is applied to each
        /// pair of scalars. Also supply an aggregator function.
        /// </summary>
        ///
        /// <param name="type"> The operator type. </param>
        /// <param name="size"> The vector size. </param>
        /// <param name="pLeftValue"> Pointer to the left vector. </param>
        /// <param name="pRightValue"> Pointer to the right vector. </param>
        /// <param name="aggregator"> The aggregator function. </param>
        void VectorOperator(TypedOperator type, size_t size, llvm::Value* pLeftValue, llvm::Value* pRightValue, std::function<void(llvm::Value*, llvm::Value*)> aggregator);

        /// <summary>
        /// Emit binary operator over 2 equal sized vector arguments. The operator is applied to each
        /// pair of scalars. Also supply an aggregator function.
        /// </summary>
        ///
        /// <param name="type"> The operator type. </param>
        /// <param name="pSize"> Pointer to the llvm:Value that holds the vector size. </param>
        /// <param name="pLeftValue"> Pointer to the left vector. </param>
        /// <param name="pRightValue"> Pointer to the right vector. </param>
        /// <param name="aggregator"> The aggregator function. </param>
        void VectorOperator(TypedOperator type, llvm::Value* pSize, llvm::Value* pLeftValue, llvm::Value* pRightValue, std::function<void(llvm::Value*, llvm::Value*)> aggregator);

        /// <summary>
        /// Emit binary operator over a scalar and a vector. The operator is applied to each
        /// pair of scalars. Also supply an aggregator function.
        /// </summary>
        ///
        /// <param name="type"> The operator type. </param>
        /// <param name="size"> The vector size. </param>
        /// <param name="leftValue"> The scalar value. </param>
        /// <param name="pRightValue"> The vector of values. </param>
        /// <param name="aggregator"> The aggregator function. </param>
        template <typename ValueType>
        void VectorOperator(TypedOperator type, size_t size, ValueType leftValue, llvm::Value* pRightValue, std::function<void(llvm::Value*, llvm::Value*)> aggregator);

        /// <summary>
        /// Emit binary operator over a scalar and a vector. The operator is applied to each
        /// pair of scalars. Also supply an aggregator function.
        /// </summary>
        ///
        /// <param name="type"> The operator type. </param>
        /// <param name="size"> The vector size. </param>
        /// <param name="pLeftValue"> The vector of values. </param>
        /// <param name="rightValue"> The scalar value. </param>
        /// <param name="aggregator"> The aggregator function. </param>
        template <typename ValueType>
        void VectorOperator(TypedOperator type, size_t size, llvm::Value* pLeftValue, ValueType rightValue, std::function<void(llvm::Value*, llvm::Value*)> aggregator);

        /// <summary>
        /// Emit binary operator over 2 equal sized vector arguments. The operator is applied to each
        /// pair of scalars. Also supply an aggregator function.
        /// </summary>
        ///
        /// <param name="type"> The operator type. </param>
        /// <param name="size"> The vector size. </param>
        /// <param name="pLeftValue"> Pointer to the left vector. </param>
        /// <param name="LeftStartAt"> Pointer to the offset of the left vector. </param>
        /// <param name="pRightValue"> Pointer to the right vector. </param>
        /// <param name="RightStartAt"> Pointer to the offset of the right vector. </param>
        /// <param name="aggregator"> The aggregator function. </param>
        void VectorOperator(TypedOperator type, size_t size, llvm::Value* pLeftValue, int LeftStartAt, llvm::Value* pRightValue, int RightStartAt, std::function<void(llvm::Value*, llvm::Value*)> aggregator);

        /// <summary> Emit an unconditional branch to the given block. </summary>
        ///
        /// <param name="pDestinationBlock"> Pointer to the destination block. </param>
        void Branch(llvm::BasicBlock* pDestinationBlock);

        /// <summary> Emit a conditional branch. </summary>
        ///
        /// <param name="pConditionValue"> Pointer to the condition value. </param>
        /// <param name="pThenBlock"> Pointer to the THEN block. </param>
        /// <param name="pElseBlock"> Pointer to the ELSE block. </param>
        void Branch(llvm::Value* pConditionValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock);

        /// <summary> Emit a conditional branch. </summary>
        ///
        /// <param name="comparison"> The type of comparison. </param>
        /// <param name="pValue"> Pointer to the value used in the comparison. </param>
        /// <param name="pTestValue"> Pointer to the test value, which is compared to PValue. </param>
        /// <param name="pThenBlock"> Pointer to the THEN block. </param>
        /// <param name="pElseBlock"> Pointer to the ELSE block. </param>
        void Branch(TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock);

        /// <summary> Emit a branch to the THEN block if value matches the boolean testValue, else branch to ELSE. </summary>
        ///
        /// <param name="pValue"> Pointer to the value used in the comparison. </param>
        /// <param name="testValue"> The boolean compared to PValue. </param>
        /// <param name="pThenBlock"> Pointer to the THEN block. </param>
        /// <param name="pElseBlock"> Pointer to the ELSE block. </param>
        void Branch(llvm::Value* pValue, bool testValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock);

        /// <summary> Emit a logical AND. </summary>
        ///
        /// <param name="pTestValue1"> Pointer to the first value. </param>
        /// <param name="pTestValue2"> Pointer to the second value. </param>
        ///
        /// <returns> Pointer to the logical AND of the two values. </returns>
        llvm::Value* LogicalAnd(llvm::Value* pTestValue1, llvm::Value* pTestValue2);

        /// <summary> Emit a logical OR. </summary>
        ///
        /// <param name="pTestValue1"> Pointer to the first value. </param>
        /// <param name="pTestValue2"> Pointer to the second value. </param>
        ///
        /// <returns> Pointer to the logical OR of the two values. </returns>
        llvm::Value* LogicalOr(llvm::Value* pTestValue1, llvm::Value* pTestValue2);

        /// <summary> Emit a logical NOT. </summary>
        ///
        /// <param name="pTestValue"> Pointer to the value. </param>
        ///
        /// <returns> Pointer to the logical NOT of the value. </returns>
        llvm::Value* LogicalNot(llvm::Value* pTestValue);

        /// <summary> Remove any terminating branch instruction in the current block. </summary>
        void DeleteTerminatingBranch();

        /// <summary> Emit a comparison. </summary>
        ///
        /// <param name="type"> The comparison type. </param>
        /// <param name="pValue"> Pointer to the value. </param>
        /// <param name="pTestValue"> Pointer to the test value, which is compared to the value. </param>
        ///
        /// <returns> Pointer to the result of the comparison. </returns>
        llvm::Value* Comparison(TypedComparison type, llvm::Value* pValue, llvm::Value* pTestValue);

        //
        // Select
        //

        /// <summary> Emit a select instruction. </summary>
        ///
        /// <param name="pCmp"> Pointer to the value use as a comparison. </param>
        /// <param name="pTrueValue"> Pointer to the value to return when the comparison is true. </param>
        /// <param name="pFalseValue"> Pointer to the value to return when the comparison is false. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the select result. </returns>
        llvm::Value* Select(llvm::Value* pCmp, llvm::Value* pTrueValue, llvm::Value* pFalseValue);

        //
        // Block management
        //

        /// <summary> Gets the current code block that code is being emitted into. </summary>
        ///
        /// <returns> The current block. </returns>
        llvm::BasicBlock* GetCurrentBlock() { return _pEmitter->GetCurrentBlock(); }

        /// <summary> Gets the current insert point that code is being emitted into. </summary>
        ///
        /// <returns> The current insert point for new instructions. </returns>
        llvm::IRBuilder<>::InsertPoint GetCurrentInsertPoint() { return _pEmitter->GetCurrentInsertPoint(); }

        /// <summary> Set the block that subsequent code will go into. </summary>
        ///
        /// <param name="pBlock"> Pointer to the block. </param>
        ///
        /// <returns> The previous current block. </returns>
        llvm::BasicBlock* SetCurrentBlock(llvm::BasicBlock* pBlock);

        /// <summary> Set the insert point for new code. </summary>
        ///
        /// <param name="pos"> The new insert point. </param>
        void SetCurrentInsertPoint(llvm::IRBuilder<>::InsertPoint position);

        /// <summary> Set the insert point for new code. </summary>
        ///
        /// <param name="pos"> The new insert point. </param>
        void SetCurrentInsertPoint(llvm::Instruction* position);

        /// <summary> Emit a new labeled code block and make it current. </summary>
        ///
        /// <param name="label"> The block label. </param>
        /// <param name="shouldConcatenate"> true if should concatenate, default is false. </param>
        ///
        /// <returns> :Pointer to the new block. </returns>
        llvm::BasicBlock* BeginBlock(const std::string& label, bool shouldConcatenate = false);

        /// <summary> Emit a new labeled code block. The "current" block does not change. </summary>
        ///
        /// <param name="label"> The label. </param>
        ///
        /// <returns> Pointer to the new block. </returns>
        llvm::BasicBlock* Block(const std::string& label);

        /// <summary> Emit a new labeled code block BEFORE the given block. </summary>
        ///
        /// <param name="pBlock"> Pointer to the block that the new block comes before. </param>
        /// <param name="label"> The label. </param>
        ///
        /// <returns> Pointer to the new Block. </returns>
        llvm::BasicBlock* BlockBefore(llvm::BasicBlock* pBlock, const std::string& label);

        /// <summary> Emit a new code block BEFORE the given block. </summary>
        ///
        /// <param name="pBlock"> Pointer to the block that the new block comes before. </param>
        /// <param name="pNewBlock"> Pointer to the new Block. </param>
        ///
        /// <returns> Pointer to the new Block. </returns>
        llvm::BasicBlock* BlockBefore(llvm::BasicBlock* pBlock, llvm::BasicBlock* pNewBlock);

        /// <summary> Emit a new labeled code block AFTER a given block. </summary>
        ///
        /// <param name="pBlock"> Pointer to the block that the new block comes after. </param>
        /// <param name="label"> The block label. </param>
        ///
        /// <returns> Pointer to the new Block. </returns>
        llvm::BasicBlock* BlockAfter(llvm::BasicBlock* pBlock, const std::string& label);

        /// <summary> Emit a new code block AFTER a given block. </summary>
        ///
        /// <param name="pBlock"> Pointer to the block that the new block comes after. </param>
        /// <param name="pNewBlock"> Pointer to the new Block. </param>
        ///
        /// <returns> Pointer to the new Block. </returns>
        llvm::BasicBlock* BlockAfter(llvm::BasicBlock* pBlock, llvm::BasicBlock* pNewBlock);

        /// <summary> Emit a new code block AFTER a given block. </summary>
        ///
        /// <param name="pBlock"> Pointer to the block that the new blocks comes after. </param>
        /// <param name="blocks"> The new blocks. </param>
        void BlocksAfter(llvm::BasicBlock* pBlock, const std::vector<llvm::BasicBlock*>& blocks);

        /// <summary> Emit a new code block AFTER a given block. </summary>
        ///
        /// <param name="pBlock"> Pointer to the block that the new blocks comes after. </param>
        /// <param name="pRegion"> The new blocks. </param>
        void BlocksAfter(llvm::BasicBlock* pBlock, const IRBlockRegion* pRegion);

        /// <summary>
        /// Emit the given block to the end of the function's block list. It is NOT made the current
        /// block.
        /// </summary>
        ///
        /// <param name="pBlock"> Pointer to the block being added. </param>
        void AppendBlock(llvm::BasicBlock* pBlock);

        /// <summary>
        /// Injects a branch from pTopBlock to pBottomBlock, removing any existing branches. Places
        /// pBottom below pTop in the function's block list.
        /// </summary>
        ///
        /// <param name="pTopBlock"> Pointer to the top block. </param>
        /// <param name="pBottomBlock"> Pointer to the bottom block. </param>
        void ConcatenateBlocks(llvm::BasicBlock* pTopBlock, llvm::BasicBlock* pBottomBlock);

        /// <summary>
        /// Injects a branch from each block in the list to the next, removing any existing terminating branches. Places
        /// them in order in the function block list.
        /// </summary>
        ///
        /// <param name="blocks"> The list of blocks to concatenate. </param>
        void ConcatenateBlocks(std::vector<llvm::BasicBlock*> blocks);

        /// <summary> Concats the new block to the current block and updates the current block. </summary>
        ///
        /// <param name="pBlock"> Pointer to the block. </param>
        void MergeBlock(llvm::BasicBlock* pBlock);

        /// <summary> Concats the region to the current block and updates the current block. </summary>
        ///
        /// <param name="pRegion"> Pointer to the block region. </param>
        void MergeRegion(IRBlockRegion* pRegion);

        /// <summary>
        /// Concat two block regions by inserting a branch from the end of pTop to pBottom.
        /// </summary>
        ///
        /// <param name="pTop"> Pointer to the top block region. </param>
        /// <param name="pBottom"> Pointer to the bottom block region. </param>
        /// <param name="moveBlocks"> true to move blocks. </param>
        void ConcatRegions(IRBlockRegion* pTop, IRBlockRegion* pBottom, bool moveBlocks = false);

        /// <summary> Concat regions in the given region list. </summary>
        ///
        /// <param name="regions"> The block region list. </param>
        void ConcatRegions(IRBlockRegionList& regions);

        /// <summary> Concat regions in the function. </summary>
        ///
        /// <param name="regions"> The block region list. </param>
        void ConcatRegions();

        //
        // Function arguments and Variables
        //

        /// <summary> Iterate over this function's arguments. </summary>
        ///
        /// <returns> A llvm::iterator_range over the function's arguments. </returns>
        llvm::iterator_range<llvm::Function::arg_iterator> Arguments() { return _pFunction->args(); }

        /// <summary> Emit a stack variable. </summary>
        ///
        /// <param name="type"> The variable type. </param>
        ///
        /// <returns> Pointer to the resulting variable. </returns>
        llvm::AllocaInst* Variable(VariableType type);

        /// <summary> Emit a named stack variable. </summary>
        ///
        /// <param name="type"> The variable type. </param>
        /// <param name="namePrefix"> The variable name prefix. If more than one variable share the prefix, they will be uniqued by appending a unique index. </param>
        ///
        /// <returns> Pointer to the resulting variable. </returns>
        llvm::AllocaInst* Variable(VariableType type, const std::string& namePrefix);

        /// <summary> Emit a named stack variable. </summary>
        ///
        /// <param name="type"> The variable type. </param>
        /// <param name="namePrefix"> The variable name prefix. If more than one variable share the prefix, they will be uniqued by appending a unique index. </param>
        ///
        /// <returns> Pointer to the resulting variable. </returns>
        llvm::AllocaInst* Variable(llvm::Type* type, const std::string& namePrefix);

        /// <summary> Emit a stack array of the given size. </summary>
        ///
        /// <param name="type"> The array entry type. </param>
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> Pointer to the array. </returns>
        llvm::AllocaInst* Variable(VariableType type, int size);

        /// <summary> Emit a stack array of the given size. </summary>
        ///
        /// <param name="type"> The array entry type. </param>
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> Pointer to the array. </returns>
        llvm::AllocaInst* Variable(llvm::Type* type, int size);

        /// <summary> Return an emitted stack variable and assign it a name. </summary>
        ///
        /// <param name="type"> The variable type. </param>
        /// <param name="size"> The variable name. </param>
        ///
        /// <returns> Pointer to the emitted variable. </returns>
        llvm::AllocaInst* EmittedVariable(VariableType type, const std::string& name);

        ///
        /// Load and Store
        ///

        /// <summary> Emit instruction to load the value referenced by a pointer. </summary>
        ///
        /// <param name="pPointer"> Pointer to the address to load. </param>
        ///
        /// <returns> Pointer to the loaded value. </returns>
        llvm::Value* Load(llvm::Value* pPointer);

        /// <summary> Emit instruction to load the value referenced by a pointer into a named register. </summary>
        ///
        /// <param name="pPointer"> Pointer to the adress being loaded. </param>
        /// <param name="name"> The register name. </param>
        ///
        /// <returns> Pointer to the resulting loaded value. </returns>
        llvm::Value* Load(llvm::Value* pPointer, const std::string& name);

        /// <summary> Emit instruction to store a value into the pointer location. </summary>
        ///
        /// <param name="pPointer"> Pointer to the adress where the value is being stored. </param>
        /// <param name="pValue"> Pointer to the value being stored. </param>
        ///
        /// <returns> Pointer to the stored value. </returns>
        llvm::Value* Store(llvm::Value* pPointer, llvm::Value* pValue);

        /// <summary> Emit instruction to initialize a 0 value into the pointer location. </summary>
        ///
        /// <param name="pPointer"> Pointer to the adress where the value is being stored. </param>
        /// <param name="numElements"> Number of elements to fill in with zero at address.
        /// defaults to 1. </param>
        ///
        /// <returns> Pointer to the stored value. </returns>
        llvm::Value* StoreZero(llvm::Value* pPointer, int numElements = 1);

        /// <summary> Emit a binary operation on to values, A and B, which replaces the value in B with the result. </summary>
        ///
        /// <param name="pPointer"> Pointer to the address of the left operator argument. </param>
        /// <param name="operation"> The operation. </param>
        /// <param name="pValue"> Pointer to the right operator argument. </param>
        ///
        /// <returns> Pointer to the value of the operation. </returns>
        llvm::Value* OperationAndUpdate(llvm::Value* pPointer, TypedOperator operation, llvm::Value* pValue);

        /// <summary> Emit a pointer to an entry in an array. </summary>
        ///
        /// <param name="pPointer"> Pointer to the array. </param>
        /// <param name="pOffset"> Pointer to the offset. </param>
        ///
        /// <returns> Pointer to a value that represents that entry in the array. </returns>
        llvm::Value* PointerOffset(llvm::Value* pPointer, llvm::Value* pOffset);

        /// <summary> Emit a pointer to an entry in an array. </summary>
        ///
        /// <param name="pPointer"> Pointer to the array. </param>
        /// <param name="offset"> The offset. </param>
        ///
        /// <returns> Pointer to a value that represents that entry in the array. </returns>
        llvm::Value* PointerOffset(llvm::Value* pPointer, int offset);

        /// <summary> Get the value at an offset in an array. </summary>
        ///
        /// <param name="pPointer"> Pointer to the array. </param>
        /// <param name="offset"> The offset. </param>
        ///
        /// <returns> The value of the entry at the given offset in the array. </returns>
        llvm::Value* ValueAt(llvm::Value* pPointer, llvm::Value* pOffset);

        /// <summary> Get the value at an offset in an array. </summary>
        ///
        /// <param name="pPointer"> Pointer to the array. </param>
        /// <param name="offset"> The offset. </param>
        ///
        /// <returns> The value of the entry at the given offset in the array. </returns>
        llvm::Value* ValueAt(llvm::Value* pPointer, int offset);

        /// <summary> Get the value stored in a pointer. </summary>
        ///
        /// <param name="pPointer"> Pointer to the array. </param>
        ///
        /// <returns> The value of the entry at the given offset in the array. </returns>
        llvm::Value* ValueAt(llvm::Value* pPointer);

        /// <summary> Get the value at an offset in a global array variable. </summary>
        ///
        /// <param name="pGlobal"> Pointer to the array. </param>
        /// <param name="pOffset"> The offset. </param>
        ///
        /// <returns> The value of the entry at the given offset in the array. </returns>
        llvm::Value* ValueAt(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset);

        /// <summary> Get the value at an offset in a global array variable. </summary>
        ///
        /// <param name="pGlobal"> Pointer to the array. </param>
        /// <param name="offset"> The offset. </param>
        ///
        /// <returns> The value of the entry at the given offset in the array. </returns>
        llvm::Value* ValueAt(llvm::GlobalVariable* pGlobal, int offset);

        /// <summary> Get the value stored in a pointer to a global variable. </summary>
        ///
        /// <param name="pGlobal"> Pointer to the array. </param>
        ///
        /// <returns> The value of the entry at the given offset in the array. </returns>
        llvm::Value* ValueAt(llvm::GlobalVariable* pGlobal);

        /// <summary> Set an element in an array. </summary>
        ///
        /// <param name="pPointer"> Pointer to the array. </param>
        /// <param name="pOffset"> The offset. </param>
        /// <param name="pValue"> The value to set. </param>
        llvm::Value* SetValueAt(llvm::Value* pPointer, llvm::Value* pOffset, llvm::Value* pValue);

        /// <summary> Set an element in an array. </summary>
        ///
        /// <param name="pPointer"> Pointer to the array. </param>
        /// <param name="offset"> The offset. </param>
        /// <param name="pValue"> The value to set. </param>
        llvm::Value* SetValueAt(llvm::Value* pPointer, int offset, llvm::Value* pValue);

        /// <summary> Set an element in a global variable array. </summary>
        ///
        /// <param name="pGlobal"> Pointer to the array. </param>
        /// <param name="pOffset"> The offset. </param>
        /// <param name="pValue"> The value to set. </param>
        llvm::Value* SetValueAt(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset, llvm::Value* pValue);

        /// <summary> Set the fields of a struct. </summary>
        ///
        /// <param name="structPtr"> The struct to be filled in. </param>
        /// <param name="fieldValues"> The values to set the fields to. </param>
        void FillStruct(llvm::Value* structPtr, const std::vector<llvm::Value*>& fieldValues);

        /// <summary> Emits a pointer to a global. </summary>
        ///
        /// <param name="pGlobal"> Pointer to the first entry in an llvm global array. </param>
        ///
        /// <returns> Pointer to the first entry in the llvm global array. </returns>
        llvm::Value* Pointer(llvm::GlobalVariable* pGlobal);

        /// <summary> Emit a pointer to an entry in an array. </summary>
        ///
        /// <param name="pArray"> Pointer to the array. </param>
        /// <param name="pOffset"> Pointer to the offset. </param>
        ///
        /// <returns> Pointer to a value that represents that entry in the array. </returns>
        llvm::Value* PointerOffset(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset);

        /// <summary> Emit a pointer to an entry in an array. </summary>
        ///
        /// <param name="pArray"> Pointer to the array. </param>
        /// <param name="offset"> The offset. </param>
        ///
        /// <returns> Pointer to a value that represents that entry in the array. </returns>
        llvm::Value* PointerOffset(llvm::GlobalVariable* pGlobal, int offset);

        /// <summary> Emit a pointer to a FIELD in a STRUCT in a global array of Structs. </summary>
        ///
        /// <param name="pArray"> Pointer to the array. </param>
        /// <param name="pOffset"> Pointer to the offset. </param>
        /// <param name="pFieldOffset"> Pointer to the field offset in the struct. </param>
        ///
        /// <returns> Pointer to a value that represents that field. </returns>
        llvm::Value* PointerOffset(llvm::GlobalVariable* pGlobal, llvm::Value* pOffset, llvm::Value* pFieldOffset);

        /// <summary> Extract a field from a struct held by value. </summary>
        ///
        /// <param name="structValue"> The struct value. </param>
        /// <param name="fieldIndex"> The index of the field to get. </param>
        ///
        /// <returns> The value of the specified field in the struct. </returns>
        llvm::Value* ExtractStructField(llvm::Value* structValue, size_t fieldIndex);

        /// <summary> Extract a field from a struct referenced by a pointer. </summary>
        ///
        /// <param name="structPtr"> A pointer to the struct. </param>
        /// <param name="fieldIndex"> The index of the field to get. </param>
        ///
        /// <returns> The value of the specified field in the struct. </returns>
        llvm::Value* GetStructFieldValue(llvm::Value* structPtr, size_t fieldIndex);

        /// <summary> Get a pointer to a field in a struct. </summary>
        ///
        /// <param name="structPtr"> Pointer to the struct. </param>
        /// <param name="fieldIndex"> The index of the field to get. </param>
        ///
        /// <returns> A pointer the specified field in the struct. </returns>
        llvm::Value* GetStructFieldPointer(llvm::Value* structPtr, size_t fieldIndex);

        //
        // Control flow
        //

        // Type aliases for lambda functions that define body regions

        /// <summary> Type alias for for-loop body lambda. </summary>
        using ForLoopBodyFunction = std::function<void(IRFunctionEmitter& function, llvm::Value* iterationVariable)>;

        /// <summary> Type alias for while-loop body lambda. </summary>
        using WhileLoopBodyFunction = std::function<void(IRFunctionEmitter& function)>;

        /// <summary> Type alias for if-else body lambda. </summary>
        using IfElseBodyFunction = std::function<void(IRFunctionEmitter& function)>;

        /// <summary> Emits a for loop counting from zero to a constant end value. </summary>
        ///
        /// <param name="count"> The number of iterations to make. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(size_t count, ForLoopBodyFunction body);

        /// <summary> Emits a for loop counting from zero to a constant end value. </summary>
        ///
        /// <param name="count"> The number of iterations to make. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(llvm::Value* count, ForLoopBodyFunction body);

        /// <summary> Emits a for loop counting from a begin value up to (but not including) a constant end value. </summary>
        ///
        /// <param name="beginValue"> The starting value of the loop iterator. </param>
        /// <param name="endValue"> The ending value of the loop iterator. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(size_t beginValue, size_t endValue, ForLoopBodyFunction body);

        /// <summary> Emits a for loop counting from a begin value up to (but not including) a constant end value. </summary>
        ///
        /// <param name="beginValue"> The starting value of the loop iterator. </param>
        /// <param name="endValue"> The ending value of the loop iterator. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(llvm::Value* beginValue, llvm::Value* endValue, ForLoopBodyFunction body);

        /// <summary> Emits a for loop counting from a begin value up to (but not including) a constant end value with a given increment. </summary>
        ///
        /// <param name="beginValue"> The starting value of the loop iterator. </param>
        /// <param name="endValue"> The ending value of the loop iterator. </param>
        /// <param name="increment"> The increment for the iterator. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(size_t beginValue, size_t endValue, size_t increment, ForLoopBodyFunction body);

        /// <summary> Emits a for loop counting from a begin value up to (but not including) a constant end value with a given increment. </summary>
        ///
        /// <param name="beginValue"> The starting value of the loop iterator. </param>
        /// <param name="endValue"> The ending value of the loop iterator. </param>
        /// <param name="increment"> The increment for the iterator. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(llvm::Value* beginValue, llvm::Value* endValue, llvm::Value* increment, ForLoopBodyFunction body);

        /// <summary> Emits a while loop. </summary>
        ///
        /// <param name="pTestValuePointer"> Pointer to a memory location that will be dereferenced for the test value. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void While(llvm::Value* pTestValuePointer, WhileLoopBodyFunction body);

        /// <summary> Emits an if statement. </summary>
        ///
        /// <param name="pTestValuePointer"> Pointer to a memory location that will be dereferenced for the test value. </param>
        /// <param name="body"> A function that emits the body of the "if true" block. </param>
        IRIfEmitter If(llvm::Value* pTestValuePointer, IfElseBodyFunction body);

        //
        // Deprecated loop emitter functions
        //

        /// <summary> Gets a for loop emitter. </summary>
        ///
        /// <returns> A for loop emitter. </returns>
        IRForLoopEmitter ForLoop();

        /// <summary> Gets a while loop emitter. </summary>
        ///
        /// <returns> A while loop emitter. </returns>
        IRWhileLoopEmitter WhileLoop();

        /// <summary> Gets an if statement emitter. </summary>
        ///
        /// <returns> An if statement emitter. </returns>
        IRIfEmitter If();

        /// <summary> Gets an if statement emitter. </summary>
        ///
        /// <param name="comparison"> The if-statement comparison. </param>
        /// <param name="pValue"> Pointer to the value used in the comparison. </param>
        /// <param name="pTestValue"> Pointer to the test value that is compared against pValue. </param>
        ///
        /// <returns> An IRIfEmitter. </returns>
        IRIfEmitter If(TypedComparison comparison, llvm::Value* pValue, llvm::Value* pTestValue);

        /// <summary> Creates an asynchronous task on a new thread. </summary>
        ///
        /// <param name="task"> The function to run asynchronously. </param>
        ///
        /// <returns> A task object representing the running task. </param>
        IRTask StartAsyncTask(llvm::Function* task);

        /// <summary> Creates an asynchronous task on a new thread. </summary>
        ///
        /// <param name="task"> The function to run asynchronously. </param>
        ///
        /// <returns> A task object representing the running task. </param>
        IRTask StartAsyncTask(IRFunctionEmitter& task);

        /// <summary> Creates an asynchronous task on a new thread. </summary>
        ///
        /// <param name="task"> The function to run asynchronously. </param>
        /// <param name="arguments"> The arguments to the task function. </param>
        ///
        /// <returns> A task object representing the running task. </param>
        IRTask StartAsyncTask(llvm::Function* task, const std::vector<llvm::Value*>& arguments);

        /// <summary> Creates an asynchronous task on a new thread. </summary>
        ///
        /// <param name="task"> The function to run asynchronously. </param>
        /// <param name="arguments"> The arguments to the task function. </param>
        ///
        /// <returns> A task object representing the running task. </param>
        IRTask StartAsyncTask(IRFunctionEmitter& task, const std::vector<llvm::Value*>& arguments);

        /// <summary> Starts an array of tasks using the thread pool, or new threads, depending on the value of the `useThreadPool` compiler setting. </summary>
        ///
        /// <param name="taskFunction"> The function to run asynchronously with many different arguments. </param>
        /// <param name="arguments"> For each task, a vector of arguments for that task. </param>
        ///
        /// <returns> A task array object representing the running tasks. </param>
        IRTaskArray StartTasks(IRFunctionEmitter& taskFunction, const std::vector<std::vector<llvm::Value*>>& arguments);

        /// <summary> Starts an array of tasks using the thread pool, or new threads, depending on the value of the `useThreadPool` compiler setting. </summary>
        ///
        /// <param name="taskFunction"> The function to run asynchronously with many different arguments. </param>
        /// <param name="arguments"> For each task, a vector of arguments for that task. </param>
        ///
        /// <returns> A task array object representing the running tasks. </param>
        IRTaskArray StartTasks(llvm::Function* taskFunction, const std::vector<std::vector<llvm::Value*>>& arguments);

        //
        // Standard C library function calls
        //

        /// <summary> Emits a malloc call. </summary>
        ///
        /// <param name="type"> The type being allocated. </param>
        /// <param name="size"> The size being allocated. </param>
        ///
        /// <returns> Pointer to the llvm::Value that points to the allocated memory. </returns>
        llvm::Value* Malloc(VariableType type, int64_t size);

        /// <summary> Emits a malloc call. </summary>
        ///
        /// <typeparam name="ValueType"> The type being allocated. </typeparam>
        /// <param name="size"> The size being allocated. </param>
        ///
        /// <returns> Pointer to the llvm::Value that points to the allocated memory. </returns>
        template <typename ValueType>
        llvm::Value* Malloc(int64_t size);

        /// <summary> Emits a free call. </summary>
        ///
        /// <param name="pValue"> Pointer to the address of the memory to be freed. </param>
        void Free(llvm::Value* pValue);

        /// <summary> Emits a print call. </summary>
        ///
        /// <param name="text"> The text to print. </param>
        ///
        /// <returns> Pointer to the return value of the call to the print function. </returns>
        llvm::Value* Print(const std::string& text);

        /// <summary> Emits a printf call. </summary>
        ///
        /// <param name="arguments"> Arguments to the printf call. </param>
        ///
        /// <returns> Pointer to the return value of the call to the printf function. </returns>
        llvm::Value* Printf(std::initializer_list<llvm::Value*> arguments);

        /// <summary> Emits a printf call. </summary>
        ///
        /// <param name="format"> Describes the printf format to use. </param>
        /// <param name="arguments"> Arguments to the printf call. </param>
        ///
        /// <returns> Pointer to the return value of the call to the printf function. </returns>
        llvm::Value* Printf(const std::string& format, std::initializer_list<llvm::Value*> arguments);

        /// <summary> Emits a printf call. </summary>
        ///
        /// <param name="format"> Describes the printf format to use. </param>
        /// <param name="arguments"> Arguments to the printf call. </param>
        ///
        /// <returns> Pointer to the return value of the call to the printf function. </returns>
        llvm::Value* Printf(const std::string& format, std::vector<llvm::Value*> arguments);

        /// <summary> Emits a memmove call, which moves an array of variables. </summary>
        ///
        /// <typeparam name="ValueType"> The type being moved. </typeparam>
        /// <param name="pPointer"> Pointer to the base address of the move operation. </param>
        /// <param name="sourceOffset"> Source address offset. </param>
        /// <param name="destinationOffset"> Destination address offset. </param>
        /// <param name="count"> Number of elements being moved. </param>
        template <typename ValueType>
        void MemoryMove(llvm::Value* pPointer, int sourceOffset, int destinationOffset, int count);

        /// <summary> Emits a memcpy call, which copies an array of variables. </summary>
        ///
        /// <typeparam name="ValueType"> The type being moved. </typeparam>
        /// <param name="pSourcePointer"> Pointer to the base address of the source. </param>
        /// <param name="pDestinationPointer"> Pointer to the base address of the destination. </param>
        /// <param name="count"> Number of elements being moved. </param>
        template <typename ValueType>
        void MemoryCopy(llvm::Value* pSourcePointer, llvm::Value* pDestinationPointer, int count);

        /// <summary> Emits a memcpy call, which copies an array of variables. </summary>
        ///
        /// <typeparam name="ValueType"> The type being moved. </typeparam>
        /// <param name="pSourcePointer"> Pointer to the base address of the source. </param>
        /// <param name="pDestinationPointer"> Pointer to the base address of the destination. </param>
        /// <param name="count"> Number of elements being moved. </param>
        template <typename ValueType>
        void MemoryCopy(llvm::Value* pSourcePointer, llvm::Value* pDestinationPointer, llvm::Value* count);

        /// <summary> Emits a memcpy call, which copies an array of variables. </summary>
        ///
        /// <typeparam name="ValueType"> The type being moved. </typeparam>
        /// <param name="pSourcePointer"> Pointer to the base address of the source. </param>
        /// <param name="sourceOffset"> Source address offset. </param>
        /// <param name="pDestinationPointer"> Pointer to the base address of the destination. </param>
        /// <param name="destinationOffset"> Destination address offset. </param>
        /// <param name="count"> Number of elements being moved. </param>
        template <typename ValueType>
        void MemoryCopy(llvm::Value* pSourcePointer, int sourceOffset, llvm::Value* pDestinationPointer, int destinationOffset, int count);

        /// <summary> Emits a memcpy call, which copies an array of variables. </summary>
        ///
        /// <typeparam name="ValueType"> The type being moved. </typeparam>
        /// <param name="pSourcePointer"> Pointer to the base address of the source. </param>
        /// <param name="sourceOffset"> Source address offset. </param>
        /// <param name="pDestinationPointer"> Pointer to the base address of the destination. </param>
        /// <param name="destinationOffset"> Destination address offset. </param>
        /// <param name="count"> Number of elements being moved. </param>
        template <typename ValueType>
        void MemoryCopy(llvm::Value* pSourcePointer, llvm::Value* sourceOffset, llvm::Value* pDestinationPointer, llvm::Value* destinationOffset, llvm::Value* count);

        /// <summary> Emits a memset call, which sets an array of memory to a given byte value. </summary>
        ///
        /// <typeparam name="ValueType"> The type of the array being set. </typeparam>
        /// <param name="pDestinationPointer"> Pointer to the base address of the destination. </param>
        /// <param name="destinationOffset"> Destination address offset. </param>
        /// <param name="value"> The byte value being set. </param>
        /// <param name="count"> Number of elements being set. </param>
        template <typename ValueType>
        void MemorySet(llvm::Value* pDestinationPointer, int destinationOffset, llvm::Value* value, int count);

        /// <summary> Emits a memset call, which sets an array of memory to a given byte value. </summary>
        ///
        /// <typeparam name="ValueType"> The type of the array being set. </typeparam>
        /// <param name="pDestinationPointer"> Pointer to the base address of the destination. </param>
        /// <param name="destinationOffset"> Destination address offset. </param>
        /// <param name="value"> The byte value being set. </param>
        /// <param name="count"> Number of elements being set. </param>
        template <typename ValueType>
        void MemorySet(llvm::Value* pDestinationPointer, llvm::Value* pDestinationOffset, llvm::Value* value, int count);

        /// <summary> Emits a memset call, which sets an array of memory to a given byte value. </summary>
        ///
        /// <typeparam name="ValueType"> The type of the array being set. </typeparam>
        /// <param name="pDestinationPointer"> Pointer to the base address of the destination. </param>
        /// <param name="destinationOffset"> Destination address offset. </param>
        /// <param name="value"> The byte value being set. </param>
        /// <param name="count"> Number of elements being set. </param>
        template <typename ValueType>
        void MemorySet(llvm::Value* pDestinationPointer, llvm::Value* pDestinationOffset, llvm::Value* value, llvm::Value* count);

        /// <summary> Inserts arbitrary function-level metadata into generated IR code. </summary>
        ///
        /// <param name="tag"> The tag of the metadata to set. </param>
        /// <param name="content"> The content to insert for the given metadata tag. </param>
        /// <remarks> To insert well-known metadata, prefer the "IncludeInXXX" metadata methods. </remarks>
        void InsertMetadata(const std::string& tag, const std::string& content = "");

        /// <summary> Inserts arbitrary function-level metadata into generated IR code. </summary>
        ///
        /// <param name="tag"> The tag of the metadata to set. </param>
        /// <param name="content"> The content to insert for the given metadata tag. </param>
        /// <remarks> To insert well-known metadata, prefer the "IncludeInXXX" metadata methods. </remarks>
        void InsertMetadata(const std::string& tag, const std::vector<std::string>& content);

        /// <summary> Emits an ELL_GetXXClockMilliseconds library function. </summary>
        ///
        /// <typeparam name="ClockType"> The clock type to use. </typeparam>
        ///
        /// <returns> Pointer to the value that receives the number of ticks. </returns>
        template <typename ClockType>
        llvm::Value* GetClockMilliseconds();

        //
        // Optimizations
        //

        /// <summary> Performs standard optimization passes. </summary>
        void Optimize();

        /// <summary> Applies an optimizer. </summary>
        ///
        /// <param name="optimizer"> The optimizer to apply. </param>
        void Optimize(IRFunctionOptimizer& optimizer);

        //
        // Inline common code generators
        //

        /// <summary> Emit IR to printf each item in the given vector. </summary>
        ///
        /// <param name="formatString"> The printf format string. </param>
        /// <param name="pVector"> Pointer to the address of the first entry in the array. </param>
        /// <param name="size"> The array size. </param>
        void PrintForEach(const std::string& formatString, llvm::Value* pVector, int size);

        /// <summary> Emit IR to compute a dot product. </summary>
        ///
        /// <param name="size"> Array size. </param>
        /// <param name="pLeftValue"> Pointer to the address of the first entry in the first array. </param>
        /// <param name="pRightValue"> Pointer to the address of the first entry in the second array. </param>
        ///
        /// <returns> Pointer to the result. </returns>
        llvm::Value* DotProduct(int size, llvm::Value* pLeftValue, llvm::Value* pRightValue);

        /// <summary> Emit IR to compute a dot product, storing the result in an existing variable. </summary>
        ///
        /// <param name="size"> Array size. </param>
        /// <param name="pLeftValue"> Pointer to the address of the first entry in the first array. </param>
        /// <param name="pRightValue"> Pointer to the address of the first entry in the second array. </param>
        /// <param name="pDestination"> Pointer to the address where to write the result. </param>
        void DotProduct(int size, llvm::Value* pLeftValue, llvm::Value* pRightValue, llvm::Value* pDestination);

        /// <summary> Emit IR to compute a dot product, storing the result in an existing variable. </summary>
        ///
        /// <param name="pSize"> [in,out] Pointer to the runtime size variable. </param>
        /// <param name="pLeftValue"> Pointer to the address of the first entry in the first array. </param>
        /// <param name="pRightValue"> Pointer to the address of the first entry in the second array. </param>
        /// <param name="pDestination"> Pointer to the address where to write the result. </param>
        void DotProduct(llvm::Value* pSize, llvm::Value* pLeftValue, llvm::Value* pRightValue, llvm::Value* pDestination);

        /// <summary> Emits a shift register. </summary>
        ///
        /// <typeparam name="ValueType"> Type of entry in the shift register. </typeparam>
        /// <param name="pBuffer"> Pointer to the location of the buffer. </param>
        /// <param name="bufferSize"> Buffer size. </param>
        /// <param name="shiftCount"> The shift count. </param>
        /// <param name="pNewData"> ?. </param>
        /// <param name="pShiftedData"> ?. </param>
        template <typename ValueType>
        void ShiftAndUpdate(llvm::Value* pBuffer, int bufferSize, int shiftCount, llvm::Value* pNewData, llvm::Value* pShiftedData = nullptr);

        /// <summary> Call the matrix-vector multiply routine that computes y = A*x </summary>
        ///
        /// <typeparam name="ValueType"> The datatype to use (must be `float` or `double`) </typeparam>
        /// <param name="m"> The number of rows in the matrix A and the output vector y </param>
        /// <param name="n"> The number of columns in the matrix A and the vector x </param>
        /// <param name="A"> The matrix to multiply with a vector </param>
        /// <param name="lda"> The stride of the matrix -- the number of elements between rows </param>
        /// <param name="x"> The input vector x </param>
        /// <param name="incx"> The increment between values of x </param>
        /// <param name="y"> The output vector y </param>
        /// <param name="incy"> The increment between values of y </param>
        template <typename ValueType>
        void CallGEMV(int m, int n, llvm::Value* A, int lda, llvm::Value* x, int incx, llvm::Value* y, int incy);

        /// <summary> Call the matrix-vector multiply routine that computes y = alpha*A*x + beta*y </summary>
        ///
        /// <typeparam name="ValueType"> The datatype to use (must be `float` or `double`) </typeparam>
        /// <param name="m"> The number of rows in the matrix A and the output vector y </param>
        /// <param name="n"> The number of columns in the matrix A and the vector x </param>
        /// <param name="alpha"> The scalar to multiply with the A*x product </param>
        /// <param name="A"> The matrix to multiply with a vector </param>
        /// <param name="lda"> The stride of the matrix -- the number of elements between rows </param>
        /// <param name="x"> The input vector x </param>
        /// <param name="incx"> The increment between values of x </param>
        /// <param name="beta"> The scalar to multiply with y before adding to the alpha*A*x product </param>
        /// <param name="y"> The output vector y </param>
        /// <param name="incy"> The increment between values of y </param>
        template <typename ValueType>
        void CallGEMV(int m, int n, ValueType alpha, llvm::Value* A, int lda, llvm::Value* x, int incx, ValueType beta, llvm::Value* y, int incy);

        /// <summary> Call the matrix-matrix multiply routine that computes the matrix product C = A*B </summary>
        ///
        /// <typeparam name="ValueType"> The datatype to use (must be `float` or `double`) </typeparam>
        /// <param name="m"> The number of rows in the matrix A and the output matrix  C </param>
        /// <param name="n"> The number of columns in the matrix B and the output matrix C </param>
        /// <param name="k"> The number of rows in the matrix A and columns in matrix B </param>
        /// <param name="A"> The matrix to multiply on the left </param>
        /// <param name="lda"> The stride of the matrix A -- the number of elements between rows </param>
        /// <param name="B"> The matrix to multiply on the right </param>
        /// <param name="ldb"> The stride of the matrix B -- the number of elements between rows </param>
        /// <param name="C"> The result matrix </param>
        /// <param name="ldc"> The stride of the matrix C -- the number of elements between rows </param>
        template <typename ValueType>
        void CallGEMM(int m, int n, int k, llvm::Value* A, int lda, llvm::Value* B, int ldb, llvm::Value* C, int ldc);

        /// <summary> Call the matrix-matrix multiply routine that computes the matrix product C = A*B, but with potentially-transposed matrices </summary>
        ///
        /// <typeparam name="ValueType"> The datatype to use (must be `float` or `double`) </typeparam>
        /// <param name="transposeA"> If `true`, use A' instead of A in the above equation </param>
        /// <param name="transposeB"> If `true`, use B' instead of B in the above equation </param>
        /// <param name="m"> The number of rows in the matrix A and the output matrix  C </param>
        /// <param name="n"> The number of columns in the matrix B and the output matrix C </param>
        /// <param name="k"> The number of rows in the matrix A and columns in matrix B </param>
        /// <param name="A"> The matrix to multiply on the left </param>
        /// <param name="lda"> The stride of the matrix A -- the number of elements between rows </param>
        /// <param name="B"> The matrix to multiply on the right </param>
        /// <param name="ldb"> The stride of the matrix B -- the number of elements between rows </param>
        /// <param name="C"> The result matrix </param>
        /// <param name="ldc"> The stride of the matrix C -- the number of elements between rows </param>
        template <typename ValueType>
        void CallGEMM(bool transposeA, bool transposeB, int m, int n, int k, llvm::Value* A, int lda, llvm::Value* B, int ldb, llvm::Value* C, int ldc);

        /// <summary> Utility function for getting number of threads used by OpenBLAS (if present) </summary>
        llvm::Value* GetNumOpenBLASThreads();

        /// <summary> Utility function for setting number of threads used by OpenBLAS (if present) </summary>
        void SetNumOpenBLASThreads(llvm::Value* numThreads);

        //
        // Calling POSIX functions
        //
        // Note: the arguments to the following functions are just llvm::Value* representations of the value to
        //   be passed to the underlying POSIX function. See IRPosixRuntime for more POSIX-specific information.

        /// <summary> Indicates if the target has POSIX functions available to it. </summary>
        bool HasPosixFunctions() const;

        /// <summary> Emits a call to the POSIX `pthread_create` function. </summary>
        llvm::Value* PthreadCreate(llvm::Value* threadVar, llvm::Value* attrPtr, llvm::Function* taskFunction, llvm::Value* taskArgument);

        /// <summary> Emits a call to the POSIX `pthread_equal` function. </summary>
        llvm::Value* PthreadEqual(llvm::Value* thread1, llvm::Value* thread2);

        /// <summary> Emits a call to the POSIX `pthread_exit` function. </summary>
        void PthreadExit(llvm::Value* status);

        /// <summary> Emits a call to the POSIX `pthread_getconcurrency` function. </summary>
        llvm::Value* PthreadGetConcurrency();

        /// <summary> Emits a call to the POSIX `pthread_detach` function. </summary>
        llvm::Value* PthreadDetach(llvm::Value* thread);

        /// <summary> Emits a call to the POSIX `pthread_join` function. </summary>
        llvm::Value* PthreadJoin(llvm::Value* thread, llvm::Value* statusOut);

        /// <summary> Emits a call to the POSIX `pthread_self` function. </summary>
        llvm::Value* PthreadSelf();

        /// <summary> Emits a call to the POSIX `pthread_mutex_init` function. </summary>
        llvm::Value* PthreadMutexInit(llvm::Value* mutexPtr, llvm::Value* attrPtr);

        /// <summary> Emits a call to the POSIX `pthread_mutex_destroy` function. </summary>
        llvm::Value* PthreadMutexDestroy(llvm::Value* mutexPtr);

        /// <summary> Emits a call to the POSIX `pthread_mutex_lock` function. </summary>
        llvm::Value* PthreadMutexLock(llvm::Value* mutexPtr);

        /// <summary> Emits a call to the POSIX `pthread_mutex_trylock` function. </summary>
        llvm::Value* PthreadMutexTryLock(llvm::Value* mutexPtr);

        /// <summary> Emits a call to the POSIX `pthread_mutex_unlock` function. </summary>
        llvm::Value* PthreadMutexUnlock(llvm::Value* mutexPtr);

        /// <summary> Emits a call to the POSIX `pthread_cond_init` function. </summary>
        llvm::Value* PthreadCondInit(llvm::Value* condPtr, llvm::Value* condAttrPtr);

        /// <summary> Emits a call to the POSIX `pthread_cond_destroy` function. </summary>
        llvm::Value* PthreadCondDestroy(llvm::Value* condPtr);

        /// <summary> Emits a call to the POSIX `pthread_cond_wait` function. </summary>
        llvm::Value* PthreadCondWait(llvm::Value* condPtr, llvm::Value* mutexPtr);

        /// <summary> Emits a call to the POSIX `pthread_cond_timedwait` function. </summary>
        llvm::Value* PthreadCondTimedwait(llvm::Value* condPtr, llvm::Value* mutexPtr, llvm::Value* timespecPtr);

        /// <summary> Emits a call to the POSIX `pthread_cond_signal` function. </summary>
        llvm::Value* PthreadCondSignal(llvm::Value* condPtr);

        /// <summary> Emits a call to the POSIX `pthread_cond_broadcast` function. </summary>
        llvm::Value* PthreadCondBroadcast(llvm::Value* condPtr);

        //
        // Experimental functions
        //

        /// <summary> Gets the CPU id of the currently-running thread. Currently only available on Linux. Returns -1 if unavailable. </summary>
        llvm::Value* GetCpu();

        //
        // Information about the current function begin emitted
        //

        /// <summary> Gets a pointer to the underlying llvm::Function. </summary>
        ///
        /// <returns> Pointer to an llvm::Function. </returns>
        llvm::Function* GetFunction() const { return _pFunction; }

        /// <summary> Gets a reference to the module emitter. </summary>
        ///
        /// <returns> Reference to an `IRModuleEmitter`. </returns>
        IRModuleEmitter& GetModule() const { return *_pModuleEmitter; }

        /// <summary> Returns the current block being emitted into </summary>
        IRBlockRegion* GetCurrentRegion() { return _pCurRegion; }

        /// <summary> Adds an`IRBlockRegion` to the region list and sets it as the current region </summary>
        IRBlockRegion* AddRegion(llvm::BasicBlock* pBlock);

        /// <summary> Get the current LLVM context. </summary>
        ///
        /// <returns> The LLVMContext being used. </returns>
        llvm::LLVMContext& GetLLVMContext();

        /// <summary> Get the low-level IREmitter being used. </summary>
        ///
        /// <returns> The low-level IREmitter being used. </returns>
        IREmitter& GetEmitter();

        //
        // Serialization
        //

        /// <summary> Output the function to the given stream. </summary>
        ///
        /// <param name="os"> The output stream to write to. </param>
        void WriteToStream(std::ostream& os);

        //
        // Metadata
        //

        /// <summary> Tags a function to be declared in a C/C++ header. </summary>
        void IncludeInHeader();

        /// <summary> Tags the predict function to be included in the SWIG interface. </summary>
        void IncludeInPredictInterface();

        /// <summary> Tags a profiling function to be included in the SWIG interface. </summary>
        void IncludeInSwigInterface();

    private:
        IRFunctionEmitter(IRModuleEmitter* pModule, IREmitter* pEmitter, llvm::Function* pFunction, const std::string& name);
        IRFunctionEmitter(IRModuleEmitter* pModule, IREmitter* pEmitter, llvm::Function* pFunction, const NamedVariableTypeList& arguments, const std::string& name);
        IRFunctionEmitter(IRModuleEmitter* pModule, IREmitter* pEmitter, llvm::Function* pFunction, const NamedLLVMTypeList& arguments, const std::string& name);

        friend class IRModuleEmitter;

        class EntryBlockScope
        {
        public:
            EntryBlockScope(IRFunctionEmitter& function);
            ~EntryBlockScope();

        private:
            IRFunctionEmitter& _function;
            llvm::IRBuilder<>::InsertPoint _oldPos;
        };

        llvm::Value* PtrOffsetA(llvm::Value* pPointer, int offset);
        llvm::Value* PtrOffsetA(llvm::Value* pPointer, llvm::Value* pOffset, const std::string& name = "");
        llvm::Value* ValueAtA(llvm::Value* pPointer, int offset);
        llvm::Value* ValueAtA(llvm::Value* pPointer, llvm::Value* pOffset);
        llvm::Value* SetValueAtA(llvm::Value* pPointer, int offset, llvm::Value* pValue);
        llvm::Value* SetValueAtA(llvm::Value* pPointer, llvm::Value* pOffset, llvm::Value* pValue);
        llvm::Value* PtrOffsetH(llvm::Value* pPointer, int offset);
        llvm::Value* PtrOffsetH(llvm::Value* pPointer, llvm::Value* pOffset);
        llvm::Value* ValueAtH(llvm::Value* pPointer, int offset);
        llvm::Value* ValueAtH(llvm::Value* pPointer, llvm::Value* pOffset);
        llvm::Value* SetValueAtH(llvm::Value* pPointer, int offset, llvm::Value* pValue);

        friend class IRModuleEmitter;

        llvm::BasicBlock* GetEntryBlock() { return _entryBlock; }
        void SetUpFunction();

        template <typename ArgsListType>
        void RegisterFunctionArgs(const ArgsListType& args);
        void CompleteFunction(bool optimize = true);
        void CompleteFunction(IRFunctionOptimizer& optimizer);

        bool CanUseBlas() const;
        void EnsurePrintf();

        llvm::Function* ResolveFunction(const std::string& name);
        llvm::Module* GetLLVMModule() { return _pFunction->getParent(); }
        friend void swap(IRFunctionEmitter& first, IRFunctionEmitter& second);

        IRVariableTable _locals; // Symbol table: name -> llvm::Value* (stack variables or function arguments)

        IRModuleEmitter* _pModuleEmitter = nullptr;
        IREmitter* _pEmitter = nullptr;
        IRBlockRegionList _regions;
        IRBlockRegion* _pCurRegion = nullptr;
        llvm::Function* _pFunction = nullptr;
        llvm::BasicBlock* _entryBlock = nullptr;

        std::string _name;
    };

    //
    // IRFunctionCallArguments
    //

    /// <summary> Class that helps build argument lists for a function call </summary>
    class IRFunctionCallArguments
    {
    public:
        /// <summary> Create a set of function call arguments. </summary>
        ///
        /// <param name="caller"> The caller function that will make the call and
        /// contain the caller variables. </param>
        IRFunctionCallArguments(IRFunctionEmitter& caller);

        /// <summary> Gets the number of arguments. </summary>
        ///
        /// <returns> The number of arguments. </returns>
        size_t NumArguments() const { return _arguments.size(); }

        /// <summary> Get the argument at the given index. </summary>
        ///
        /// <param name="index"> Zero-based index of the argument. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the requested argument. </returns>
        llvm::Value* GetArgumentAt(size_t index) const;

        /// <summary> Add an argument. </summary>
        ///
        /// <param name="pValue"> Pointer to the value being added. </param>
        void Append(llvm::Value* pValue);

        /// <summary>
        /// Allocates a variable to hold the output, appends it to the argument list, and returns the variable.
        /// </summary>
        ///
        /// <param name="type"> Variable type. </param>
        /// <param name="size"> Variable size. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the allocated variable. </returns>
        llvm::Value* AppendOutput(VariableType type, int size);

        /// <summary> Cast operator to the underlying IRValueList. </summary>
        ///
        /// <returns> Const reference to the underlying IRValueList. </returns>
        operator const IRValueList&() { return _arguments; }

    private:
        IRFunctionEmitter& _functionEmitter;
        IRValueList _arguments;
    };
}
}

#include "../tcc/IRFunctionEmitter.tcc"
