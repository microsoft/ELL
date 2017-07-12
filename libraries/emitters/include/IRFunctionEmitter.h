////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRFunctionEmitter.h (emitters)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IREmitter.h"
#include "IRIfEmitter.h"
#include "IRLoopEmitter.h"
#include "IROptimizer.h"
#include "LLVMInclude.h"
#include "Variable.h"

namespace ell
{
namespace emitters
{
    class IRModuleEmitter;

    /// <summary> Used to emit code into an existing LLVM IR Function </summary>
    class IRFunctionEmitter
    {
    public:
        IRFunctionEmitter() = default;

        IRFunctionEmitter(const IRFunctionEmitter&) = default;
        IRFunctionEmitter(IRFunctionEmitter&&) = default;
        IRFunctionEmitter& operator=(const IRFunctionEmitter&) = default;
        IRFunctionEmitter& operator=(IRFunctionEmitter&&) = default;

        /// <summary> Query if this IRFunctionEmitter is valid. </summary>
        ///
        /// <returns> True if valid, false if not. </returns>
        bool IsValid() const { return _pModuleEmitter != nullptr; }

        /// <summary> Verify (as far as possible) if the function's IR is valid. </summary>
        void Verify();

        /// <summary> Gets the name of the function being emitted. </summary>
        std::string GetFunctionName() { return _name; }

        /// <summary> Gets an emitted variable by scope and name. </summary>
        ///
        /// <param name="scope"> The variable scope. </param>
        /// <param name="name"> The variable name. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the variable. </returns>
        llvm::Value* GetEmittedVariable(const VariableScope scope, const std::string& name);

        /// <summary> Emit a literal into the function. </summary>
        ///
        /// <param name="valueType"> Type of literal to emit. </param>
        /// <param name="value"> The value of the literal. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the literal. </returns>
        template <typename ValueType>
        llvm::Value* Literal(ValueType value);

        /// <summary> Convenience function for returning a single-bit LLVM boolean true value. </summary>
        ///
        /// <returns> An llvm i1 type representing `true`. </returns>
        llvm::Value* TrueBit() { return _pEmitter->TrueBit(); }

        /// <summary> Convenience function for returning a single-bit LLVM boolean false value. </summary>
        ///
        /// <returns> An llvm i1 type representing `false`. </returns>
        llvm::Value* FalseBit() { return _pEmitter->FalseBit(); }

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

        /// <summary> Emit a cast. </summary>
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
        llvm::Value* Cast(llvm::Value* pValue, VariableType valueType);

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

        /// <summary> Emit a cast to a template type </summary>
        ///
        /// <typeparam name="ValueType"> The output type. </typeparam>
        /// <param name="pValue"> Pointer to the input value. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        template <typename ValueType>
        llvm::Value* Cast(llvm::Value* pValue);

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
        void ConcatBlocks(llvm::BasicBlock* pTopBlock, llvm::BasicBlock* pBottomBlock);

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
        /// <param name="name"> The variable name. </param>
        ///
        /// <returns> Pointer to the resulting variable. </returns>
        llvm::AllocaInst* Variable(VariableType type, const std::string& name);

        /// <summary> Emit a named stack variable. </summary>
        ///
        /// <param name="type"> The variable type. </param>
        /// <param name="name"> The variable name. </param>
        ///
        /// <returns> Pointer to the resulting variable. </returns>
        llvm::AllocaInst* Variable(llvm::Type* type, const std::string& name);

        /// <summary> Emit a stack array of the given size. </summary>
        ///
        /// <param name="type"> The array entry type. </param>
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> Pointer to the array. </returns>
        llvm::AllocaInst* Variable(VariableType type, int size);

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

        /// <summary> Emits a pointer to a global. </summary>
        ///
        /// <param name="pGlobal"> Pointer to the llvm global. </param>
        ///
        /// <returns> Pointer to the llvm pointer to the llvm global. </returns>
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

        //
        // Expressions
        //

        /// <summary> Gets a for loop emitter. </summary>
        ///
        /// <returns> A for loop emitter. </returns>
        IRForLoopEmitter ForLoop();

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

        //
        // Standard useful function calls
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

        /// <summary> Inserts arbitrary function-level metadata into generated IR code. </summary>
        ///
        /// <param name="tag"> The tag of the metadata to set. </param>
        /// <param name="content"> The content to insert for the given metadata tag. </param>
        /// <remarks> To insert well-known metadata, prefer the "IncludeInXXX" metadata methods. </remarks>
        void InsertMetadata(const std::string& tag, const std::string& content = "");

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

        /// <summary> Emit IR to compute a DOT product of floats. </summary>
        ///
        /// <param name="size"> Array size. </param>
        /// <param name="pLeftValue"> Pointer to the address of the first entry in the first array. </param>
        /// <param name="pRightValue"> Pointer to the address of the first entry in the second array. </param>
        ///
        /// <returns> Pointer to the result. </returns>
        llvm::Value* DotProductFloat(int size, llvm::Value* pLeftValue, llvm::Value* pRightValue);

        /// <summary> Emit IR to compute a DOT product of floats. </summary>
        ///
        /// <param name="size"> Array size. </param>
        /// <param name="pLeftValue"> Pointer to the address of the first entry in the first array. </param>
        /// <param name="pRightValue"> Pointer to the address of the first entry in the second array. </param>
        /// <param name="pDestination"> Pointer to the address where to write the result. </param>
        void DotProductFloat(int size, llvm::Value* pLeftValue, llvm::Value* pRightValue, llvm::Value* pDestination);

        /// <summary> Emit IR to compute a DOT product of floats. </summary>
        ///
        /// <param name="pSize"> [in,out] Pointer to the runtime size variable. </param>
        /// <param name="pLeftValue"> Pointer to the address of the first entry in the first array. </param>
        /// <param name="pRightValue"> Pointer to the address of the first entry in the second array. </param>
        /// <param name="pDestination"> Pointer to the address where to write the result. </param>
        void DotProductFloat(llvm::Value* pSize, llvm::Value* pLeftValue, llvm::Value* pRightValue, llvm::Value* pDestination);

        /// <summary> Emit IR to compute a DOT product. </summary>
        ///
        /// <param name="size"> Array size. </param>
        /// <param name="pLeftValue"> Pointer to the address of the first entry in the first array. </param>
        /// <param name="pRightValue"> Pointer to the address of the first entry in the second array. </param>
        ///
        /// <returns> Pointer to the result. </returns>
        llvm::Value* DotProduct(int size, llvm::Value* pLeftValue, llvm::Value* pRightValue);

        /// <summary> Emit IR to compute a DOT product. </summary>
        ///
        /// <param name="size"> Array size. </param>
        /// <param name="pLeftValue"> Pointer to the address of the first entry in the first array. </param>
        /// <param name="pRightValue"> Pointer to the address of the first entry in the second array. </param>
        /// <param name="pDestination"> Pointer to the address where to write the result. </param>
        void DotProduct(int size, llvm::Value* pLeftValue, llvm::Value* pRightValue, llvm::Value* pDestination);

        /// <summary> Emit IR to compute a DOT product. </summary>
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

        /// <summary> Dump out function to std::out. </summary>
        void Dump();

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
        void IncludeInProfilingInterface();

        /// <summary> Tags a callback function to be included in the SWIG interface. </summary>
        void IncludeInCallbackInterface();

        /// <summary> Tags the step function to be included in the SWIG interface. </summary>
        ///
        /// <param name="outputSize"> The output size. </param>
        void IncludeInStepInterface(size_t outputSize);

        /// <summary> Tags a step time function, such as GetInterval(), to be included in the SWIG interface. </summary>
        ///
        /// <param name="functionName"> The function name. </param>
        void IncludeInStepTimeInterface(const std::string& functionName);

    private:
        IRFunctionEmitter(IRModuleEmitter* pModule, IREmitter* pEmitter, llvm::Function* pFunction, const std::string& name);
        IRFunctionEmitter(IRModuleEmitter* pModule, IREmitter* pEmitter, llvm::Function* pFunction, const NamedVariableTypeList& arguments, const std::string& name);
        friend class IRModuleEmitter;

        class EntryBlockScope
        {
        public:
            EntryBlockScope(IRFunctionEmitter& function);
            ~EntryBlockScope();

        private:
            IRFunctionEmitter& _function;
            llvm::IRBuilder<>::InsertPoint _oldPos;
            llvm::TerminatorInst* _termInst;
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

        llvm::BasicBlock* GetEntryBlock() { return _entryBlock; }
        void SetUpFunction();
        void RegisterFunctionArgs(const NamedVariableTypeList& args);
        void CompleteFunction(bool optimize = true);
        void CompleteFunction(IRFunctionOptimizer& optimizer);

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
