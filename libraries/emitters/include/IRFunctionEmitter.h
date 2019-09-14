////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRFunctionEmitter.h (emitters)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CompilerOptions.h"
#include "EmitterTypes.h"
#include "IRAsyncTask.h"
#include "IRBlockRegion.h"
#include "IREmitter.h"
#include "IRIfEmitter.h"
#include "IRLocalArray.h"
#include "IRLocalMultidimArray.h"
#include "IRLocalScalar.h"
#include "IRLocalValue.h"
#include "IRLoopEmitter.h"
#include "IROptimizer.h"
#include "IRParallelLoopEmitter.h"
#include "IRTask.h"
#include "LLVMUtilities.h"
#include "Variable.h"

#include <llvm/IR/Argument.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>

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

    /// <summary> A list of IRLocalScalar values </summary>
    using IRScalarList = std::vector<IRLocalScalar>;

    /// <summary> Used to emit code into an existing LLVM IR Function </summary>
    class IRFunctionEmitter
    {
    public:
        /// <summary> Attributes that can be applied to function parameters </summary>
        enum class Attributes
        {
            None = 0,
            /// <summary> Suppress alias analysis </summary>
            NoAlias
        };

        /// <summary> Query if this IRFunctionEmitter is valid. </summary>
        ///
        /// <returns> True if valid, false if not. </returns>
        bool IsValid() const { return _pModuleEmitter != nullptr; }

        /// <summary> Verify (as far as possible) if the function's IR is valid. </summary>
        void Verify();

        /// <summary> Gets the name of the function being emitted. </summary>
        std::string GetFunctionName() { return _name; }

        /// <summary> Gets an `IRLocalPointer` wrapper for an LLVM value object. </summary>
        ///
        /// <param name="value"> The value to wrap. </param>
        IRLocalPointer LocalPointer(LLVMValue value);

        /// <summary> Gets an `IRLocalScalar` wrapper for an LLVM value object. </summary>
        ///
        /// <param name="value"> The value to wrap. </param>
        IRLocalScalar LocalScalar(LLVMValue value);

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
        IRLocalArray LocalArray(LLVMValue value);

        /// <summary> Gets an `IRLocalMatrix` wrapper for an LLVM value object that represents a fixed-size array. </summary>
        ///
        /// <param name="value"> The value to wrap. </param>
        /// <param name="shape"> A two-element vector describing the shape of the tensor. </param>
        /// <param name="layout"> The order of the dimensions, outermost to innermost. </param>
        IRLocalMatrix LocalMatrix(LLVMValue value, const std::vector<int>& shape, std::array<int, 2> layout);

        /// <summary> Gets an `IRLocalTensor` wrapper for an LLVM value object that represents a fixed-size array. </summary>
        ///
        /// <param name="value"> The value to wrap. </param>
        /// <param name="shape"> A three-element vector describing the shape of the tensor. </param>
        /// <param name="layout"> The order of the dimensions, outermost to innermost. </param>
        IRLocalTensor LocalTensor(LLVMValue value, const std::vector<int>& shape, std::array<int, 3> layout);

        /// <summary> Gets an `IRLocalMultidimArray` wrapper for an LLVM value object that represents a fixed-size array. </summary>
        ///
        /// <param name="value"> The value to wrap. </param>
        /// <param name="dimensions"> The sizes of the array's dimensions. </param>
        IRLocalMultidimArray LocalMultidimArray(LLVMValue value, std::initializer_list<int> dimensions);

        /// <summary> Gets an emitted variable by scope and name. </summary>
        ///
        /// <param name="scope"> The variable scope. </param>
        /// <param name="name"> The variable name. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the variable. </returns>
        LLVMValue GetEmittedVariable(const VariableScope scope, const std::string& name);

        /// <summary> Gets an argument to the function by name. </summary>
        ///
        /// <param name="name"> The variable name. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the variable. </returns>
        LLVMValue GetFunctionArgument(const std::string& name);

        /// <summary> Emit a literal into the function. </summary>
        ///
        /// <param name="valueType"> Type of literal to emit. </param>
        /// <param name="value"> The value of the literal. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the literal. </returns>
        template <typename ValueType>
        LLVMValue Literal(ValueType value);

        /// <summary> Emit a literal pointer into the function. </summary>
        ///
        /// <typeparam name="ValueType"> Type of pointer to emit. </param>
        /// <param name="value"> The value of the pointer. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the literal. </returns>
        template <typename ValueType>
        LLVMValue Pointer(ValueType* value);

        /// <summary> Convenience function for returning a single-bit LLVM boolean true value. </summary>
        ///
        /// <returns> An llvm i1 type representing `true`. </returns>
        LLVMValue TrueBit() { return GetEmitter().TrueBit(); }

        /// <summary> Convenience function for returning a single-bit LLVM boolean false value. </summary>
        ///
        /// <returns> An llvm i1 type representing `false`. </returns>
        LLVMValue FalseBit() { return GetEmitter().FalseBit(); }

        /// <summary> Convenience function for returning a null pointer constant. </summary>
        ///
        /// <param name="pointerType"> The llvm type of the pointer to return. </param>
        ///
        /// <returns> Pointer to an llvm::ConstantPointerNull that represents a null pointer of the given pointer type. </returns>
        llvm::ConstantPointerNull* NullPointer(llvm::PointerType* pointerType) { return GetEmitter().NullPointer(pointerType); } // STYLE discrepancy

        /// <summary> Emit an instruction to load a function argument. </summary>
        ///
        /// <param name="argument"> The argument. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the function argument. </returns>
        LLVMValue LoadArgument(llvm::Argument& argument);

        /// <summary> Emit a value-preserving cast operation to another type. </summary>
        ///
        /// <typeparam name="OuputType"> The output type. </typeparam>
        /// <param name="pValue"> Pointer to the input value. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        template <typename OutputType>
        LLVMValue CastValue(LLVMValue pValue);

        /// <summary> Emit a value-preserving cast operation from one type to another. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastValue(LLVMValue pValue, VariableType destinationType);

        /// <summary> Emit a value-preserving cast operation from an unsigned integral type to another type. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastValue(LLVMValue pValue, LLVMType destinationType);

        /// <summary> Emit a value-preserving cast operation from an unsigned integral type to another type. </summary>
        ///
        /// <typeparam name="OuputType"> The output type. </typeparam>
        /// <param name="pValue"> Pointer to the input value. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        template <typename OutputType>
        LLVMValue CastUnsignedValue(LLVMValue pValue);

        /// <summary> Emit a value-preserving cast operation from an unsigned integral type to another type. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastUnsignedValue(LLVMValue pValue, VariableType destinationType);

        /// <summary> Emit a value-preserving cast operation from one type to another. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastUnsignedValue(LLVMValue pValue, LLVMType destinationType);

        /// <summary> Emit cast to a given type. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="valueType"> The type to cast to. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        LLVMValue BitCast(LLVMValue pValue, VariableType valueType);

        /// <summary> Emit cast to a given type. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="valueType"> The type to cast to. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        LLVMValue BitCast(LLVMValue pValue, LLVMType valueType);

        /// <summary> Emit pointer cast to a given pointer type. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="valueType"> The type to cast to. Must be a pointer type. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        LLVMValue CastPointer(LLVMValue pValue, VariableType valueType);

        /// <summary> Emit pointer cast to a given pointer type. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="valueType"> The type to cast to. Must be a pointer type. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        LLVMValue CastPointer(LLVMValue pValue, LLVMType valueType);

        /// <summary> Emit a cast from an integer type to a pointer. </summary>
        ///
        /// <param name="pValue"> Input value. </param>
        /// <param name="destinationType"> Output pointer type. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        LLVMValue CastIntToPointer(LLVMValue pValue, VariableType destinationType);

        /// <summary> Emit a cast from an integer type to a pointer. </summary>
        ///
        /// <param name="pValue"> Input value. </param>
        /// <param name="destinationType"> Output pointer type. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        LLVMValue CastIntToPointer(LLVMValue pValue, LLVMType destinationType);

        /// <summary> Emit a cast from a pointer to an integer type. </summary>
        ///
        /// <param name="pValue"> Input value. </param>
        /// <param name="destinationType"> Output pointer type. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        LLVMValue CastPointerToInt(LLVMValue pValue, VariableType destinationType);

        /// <summary> Emit a cast from a pointer to an integer type. </summary>
        ///
        /// <param name="pValue"> Input value. </param>
        /// <param name="destinationType"> Output pointer type. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        LLVMValue CastPointerToInt(LLVMValue pValue, LLVMType destinationType);

        /// <summary> Emit a cast from bool to byte. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        LLVMValue CastBoolToByte(LLVMValue pValue);

        /// <summary> Emit a cast from an arbitrary value to a 1-bit boolean. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        LLVMValue CastToConditionalBool(LLVMValue pValue);

        /// <summary> Emit a call to a function with a single optional argument. </summary>
        ///
        /// <param name="name"> The function name. </param>
        /// <param name="pArgument"> Pointer to the optional argument. </param>
        ///
        /// <returns> Pointer to the result of the function call. </returns>
        LLVMValue Call(const std::string& name, LLVMValue pArgument = nullptr);

        /// <summary> Emit a call to a function with arguments. </summary>
        ///
        /// <param name="name"> The function name. </param>
        /// <param name="arguments"> The function arguments. </param>
        ///
        /// <returns> Pointer to the result of the function call. </returns>
        LLVMValue Call(const std::string& name, const IRValueList& arguments);

        /// <summary> Emit a call to a function with arguments. </summary>
        ///
        /// <param name="name"> The function name. </param>
        /// <param name="arguments"> The function arguments. </param>
        ///
        /// <returns> Pointer to the result of the function call. </returns>
        LLVMValue Call(const std::string& name, IRScalarList arguments);

        /// <summary> Emit a call to a function with arguments. </summary>
        ///
        /// <param name="name"> The function name. </param>
        /// <param name="arguments"> The function arguments. </param>
        ///
        /// <returns> Pointer to the result of the function call. </returns>
        LLVMValue Call(const std::string& name, std::initializer_list<LLVMValue> arguments);

        /// <summary> Emit a call to a function with arguments. </summary>
        ///
        /// <param name="function"> The IRFunctionEmitter for the function to call. </param>
        /// <param name="arguments"> The function arguments. </param>
        ///
        /// <returns> Pointer to the result of the function call. </returns>
        LLVMValue Call(IRFunctionEmitter& function, IRValueList arguments);

        /// <summary> Emit a call to a function with arguments. </summary>
        ///
        /// <param name="pFunction"> Pointer to the llvm::Function. </param>
        /// <param name="arguments"> The function arguments. </param>
        ///
        /// <returns> Pointer to the result of the function call. </returns>
        LLVMValue Call(LLVMFunction pFunction, std::initializer_list<LLVMValue> arguments);

        /// <summary> Emit a call to a function with arguments. </summary>
        ///
        /// <param name="pFunction"> Pointer to the llvm::Function. </param>
        /// <param name="arguments"> The function arguments. </param>
        ///
        /// <returns> Pointer to the result of the function call. </returns>
        LLVMValue Call(LLVMFunction pFunction, IRValueList arguments);

        /// <summary> Emit a call to a function with arguments. </summary>
        ///
        /// <param name="pFunction"> Pointer to the llvm::Function. </param>
        /// <param name="arguments"> The function arguments. </param>
        ///
        /// <returns> Pointer to the result of the function call. </returns>
        LLVMValue Call(LLVMFunction pFunction, IRScalarList arguments);

        /// <summary> Emit a return from a function with no return value. </summary>
        void Return();

        /// <summary> Emit a return of a given value. </summary>
        ///
        /// <param name="value"> Pointer to the function return value. </param>
        ///
        /// <returns> Pointer to the result of the function call. </returns>
        LLVMValue Return(LLVMValue value);

        /// <summary> Emit a unary operator with a scalar argument. </summary>
        ///
        /// <param name="type"> The unary operator type. </param>
        /// <param name="value"> The argument of the operator. </param>
        ///
        /// <returns> Pointer to the return value of the operator. </returns>
        LLVMValue Operator(UnaryOperatorType type, LLVMValue value);

        /// <summary> Emit a binary operator with 2 scalar arguments. </summary>
        ///
        /// <param name="type"> The operator type. </param>
        /// <param name="pLeftValue"> Pointer to the left argument of the operator. </param>
        /// <param name="pRightValue"> Pointer to the right value of the operator. </param>
        ///
        /// <returns> Pointer to the return value of the operator. </returns>
        LLVMValue Operator(TypedOperator type, LLVMValue pLeftValue, LLVMValue pRightValue);

        /// <summary> Emit a binary operator with 2 scalar arguments. </summary>
        ///
        /// <param name="type"> The operator type. </param>
        /// <param name="arguments"> The operator arguments. </param>
        ///
        /// <returns> Pointer to the return value of the operator. </returns>
        LLVMValue Operator(TypedOperator type, llvm::iterator_range<llvm::Function::arg_iterator>& arguments);

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
        void VectorOperator(TypedOperator type, size_t size, LLVMValue pLeftValue, LLVMValue pRightValue, std::function<void(LLVMValue, LLVMValue)> aggregator);

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
        void VectorOperator(TypedOperator type, LLVMValue pSize, LLVMValue pLeftValue, LLVMValue pRightValue, std::function<void(LLVMValue, LLVMValue)> aggregator);

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
        void VectorOperator(TypedOperator type, size_t size, ValueType leftValue, LLVMValue pRightValue, std::function<void(LLVMValue, LLVMValue)> aggregator);

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
        void VectorOperator(TypedOperator type, size_t size, LLVMValue pLeftValue, ValueType rightValue, std::function<void(LLVMValue, LLVMValue)> aggregator);

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
        void VectorOperator(TypedOperator type, size_t size, LLVMValue pLeftValue, int LeftStartAt, LLVMValue pRightValue, int RightStartAt, std::function<void(LLVMValue, LLVMValue)> aggregator);

        /// <summary> Emit an unconditional branch to the given block. </summary>
        ///
        /// <param name="pDestinationBlock"> Pointer to the destination block. </param>
        ///
        /// <returns> Pointer to a llvm::BranchInst that represents the branch. </returns>
        llvm::BranchInst* Branch(llvm::BasicBlock* pDestinationBlock);

        /// <summary> Emit a conditional branch. </summary>
        ///
        /// <param name="pConditionValue"> Pointer to the condition value. A nonzero value is interpreted as true, and zero as false. </param>
        /// <param name="pThenBlock"> Pointer to the THEN block. </param>
        /// <param name="pElseBlock"> Pointer to the ELSE block. </param>
        ///
        /// <returns> Pointer to a llvm::BranchInst that represents the branch. </returns>
        llvm::BranchInst* Branch(LLVMValue pConditionValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock);

        /// <summary> Emit a conditional branch. </summary>
        ///
        /// <param name="comparison"> The type of comparison. </param>
        /// <param name="pValue"> Pointer to the value used in the comparison. </param>
        /// <param name="pTestValue"> Pointer to the test value, which is compared to PValue. </param>
        /// <param name="pThenBlock"> Pointer to the THEN block. </param>
        /// <param name="pElseBlock"> Pointer to the ELSE block. </param>
        ///
        /// <returns> Pointer to a llvm::BranchInst that represents the branch. </returns>
        llvm::BranchInst* Branch(TypedComparison comparison, LLVMValue pValue, LLVMValue pTestValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock);

        /// <summary> Emit a branch to the THEN block if value matches the boolean testValue, else branch to ELSE. </summary>
        ///
        /// <param name="pValue"> Pointer to the value used in the comparison. </param>
        /// <param name="testValue"> The boolean compared to PValue. </param>
        /// <param name="pThenBlock"> Pointer to the THEN block. </param>
        /// <param name="pElseBlock"> Pointer to the ELSE block. </param>
        ///
        /// <returns> Pointer to a llvm::BranchInst that represents the branch. </returns>
        llvm::BranchInst* Branch(LLVMValue pValue, bool testValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock);

        /// <summary> Emit a logical AND. </summary>
        ///
        /// <param name="pTestValue1"> Pointer to the first value. </param>
        /// <param name="pTestValue2"> Pointer to the second value. </param>
        ///
        /// <returns> Pointer to the logical AND of the two values. </returns>
        LLVMValue LogicalAnd(LLVMValue pTestValue1, LLVMValue pTestValue2);

        /// <summary> Emit a logical OR. </summary>
        ///
        /// <param name="pTestValue1"> Pointer to the first value. </param>
        /// <param name="pTestValue2"> Pointer to the second value. </param>
        ///
        /// <returns> Pointer to the logical OR of the two values. </returns>
        LLVMValue LogicalOr(LLVMValue pTestValue1, LLVMValue pTestValue2);

        /// <summary> Emit a logical NOT. </summary>
        ///
        /// <param name="pTestValue"> Pointer to the value. </param>
        ///
        /// <returns> Pointer to the logical NOT of the value. </returns>
        LLVMValue LogicalNot(LLVMValue pTestValue);

        /// <summary> Remove any terminating branch instruction in the current block. </summary>
        void DeleteTerminatingBranch();

        /// <summary> Emit a comparison. </summary>
        ///
        /// <param name="type"> The comparison type. </param>
        /// <param name="pValue"> Pointer to the value. </param>
        /// <param name="pTestValue"> Pointer to the test value, which is compared to the value. </param>
        ///
        /// <returns> Pointer to the result of the comparison. </returns>
        LLVMValue Comparison(TypedComparison type, LLVMValue pValue, LLVMValue pTestValue);

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
        LLVMValue Select(LLVMValue pCmp, LLVMValue pTrueValue, LLVMValue pFalseValue);

        //
        // Block management
        //

        /// <summary> Gets the current code block that code is being emitted into. </summary>
        ///
        /// <returns> The current block. </returns>
        llvm::BasicBlock* GetCurrentBlock() { return GetEmitter().GetCurrentBlock(); }

        /// <summary> Gets the current insert point that code is being emitted into. </summary>
        ///
        /// <returns> The current insert point for new instructions. </returns>
        llvm::IRBuilder<>::InsertPoint GetCurrentInsertPoint() { return GetEmitter().GetCurrentInsertPoint(); }

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
        /// <returns> Pointer to the new block. </returns>
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

        /// <summary> Sets an attribute for the argument at the given index </summary>
        ///
        /// <param name="index"> The index of the argument </param>
        /// <param name="attribute"> The attribute </param>
        void SetAttributeForArgument(size_t index, Attributes attribute);

        /// <summary> Sets an attribute for all arguments </summary>
        ///
        /// <param name="attribute"> The attribute </summary>
        void SetAttributeForArguments(Attributes attribute);

        /// <summary> Sets an attribute for arguments at the specified indices </summary>
        ///
        /// <param name="indices"> The indices of the arguments </param>
        /// <param name="attribute"> The attribute </param>
        void SetAttributeForArguments(std::vector<size_t> indices, Attributes attribute);

        /// <summary> Emit a stack variable. </summary>
        ///
        /// <param name="type"> The variable type. </param>
        ///
        /// <returns> Pointer to the resulting variable. </returns>
        llvm::AllocaInst* Variable(VariableType type);

        /// <summary> Emit a stack variable. </summary>
        ///
        /// <param name="type"> The variable type. </param>
        ///
        /// <returns> Pointer to the resulting variable. </returns>
        llvm::AllocaInst* Variable(LLVMType type);

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
        llvm::AllocaInst* Variable(LLVMType type, const std::string& namePrefix);

        /// <summary> Emit a stack array of the given size. </summary>
        ///
        /// <param name="type"> The array entry type. </param>
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> Pointer to the array. </returns>
        llvm::AllocaInst* Variable(VariableType type, int size);

        /// <summary> Emit a 2D stack array of the given dimensions. </summary>
        ///
        /// <param name="type"> The array entry type. </param>
        /// <param name="rows"> The number of rows in the array. </param>
        /// <param name="columns"> The number of columns in the array. </param>
        ///
        /// <returns> Pointer to the array. </returns>
        llvm::AllocaInst* Variable(VariableType type, int rows, int columns);

        /// <summary> Emit a stack array of the given size. </summary>
        ///
        /// <param name="type"> The array entry type. </param>
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> Pointer to the array. </returns>
        llvm::AllocaInst* Variable(LLVMType type, int size);

        /// <summary> Emit a 2D stack array of the given dimensions. </summary>
        ///
        /// <param name="type"> The array entry type. </param>
        /// <param name="rows"> The number of rows in the array. </param>
        /// <param name="columns"> The number of columns in the array. </param>
        ///
        /// <returns> Pointer to the array. </returns>
        llvm::AllocaInst* Variable(LLVMType type, int rows, int columns);

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
        LLVMValue Load(LLVMValue pPointer);

        /// <summary> Emit instruction to load the value referenced by a pointer into a named register. </summary>
        ///
        /// <param name="pPointer"> Pointer to the adress being loaded. </param>
        /// <param name="name"> The register name. </param>
        ///
        /// <returns> Pointer to the resulting loaded value. </returns>
        LLVMValue Load(LLVMValue pPointer, const std::string& name);

        /// <summary> Emit instruction to store a value into the pointer location. </summary>
        ///
        /// <param name="pPointer"> Pointer to the adress where the value is being stored. </param>
        /// <param name="pValue"> Pointer to the value being stored. </param>
        ///
        /// <returns> Pointer to the stored value. </returns>
        LLVMValue Store(LLVMValue pPointer, LLVMValue pValue);

        /// <summary> Emit instruction to initialize a 0 value into the pointer location. </summary>
        ///
        /// <param name="pPointer"> Pointer to the address where the value is being stored. </param>
        /// <param name="numElements"> Number of elements to fill in with zero at address.
        /// defaults to 1. </param>
        ///
        /// <returns> Pointer to the stored value. </returns>
        LLVMValue StoreZero(LLVMValue pPointer, int numElements = 1);

        /// <summary> Emit a binary operation on to values, A and B, which replaces the value in B with the result. </summary>
        ///
        /// <param name="pPointer"> Pointer to the address of the left operator argument. </param>
        /// <param name="operation"> The operation. </param>
        /// <param name="pValue"> Pointer to the right operator argument. </param>
        ///
        /// <returns> Pointer to the value of the operation. </returns>
        LLVMValue OperationAndUpdate(LLVMValue pPointer, TypedOperator operation, LLVMValue pValue);

        /// <summary> Emit a pointer to an entry in an array. </summary>
        ///
        /// <param name="pPointer"> Pointer to the array. </param>
        /// <param name="pOffset"> Pointer to the offset. </param>
        ///
        /// <returns> Pointer to a value that represents that entry in the array. </returns>
        LLVMValue PointerOffset(LLVMValue pPointer, LLVMValue pOffset);

        /// <summary> Emit a pointer to an entry in an array. </summary>
        ///
        /// <param name="pPointer"> Pointer to the array. </param>
        /// <param name="offset"> The offset. </param>
        ///
        /// <returns> Pointer to a value that represents that entry in the array. </returns>
        LLVMValue PointerOffset(LLVMValue pPointer, int offset);

        /// <summary> Get the value at an offset in an array. </summary>
        ///
        /// <param name="pPointer"> Pointer to the array. </param>
        /// <param name="offset"> The offset. </param>
        ///
        /// <returns> The value of the entry at the given offset in the array. </returns>
        LLVMValue ValueAt(LLVMValue pPointer, LLVMValue pOffset);

        /// <summary> Get the value at an offset in an array. </summary>
        ///
        /// <param name="pPointer"> Pointer to the array. </param>
        /// <param name="offset"> The offset. </param>
        ///
        /// <returns> The value of the entry at the given offset in the array. </returns>
        LLVMValue ValueAt(LLVMValue pPointer, int offset);

        /// <summary> Get the value stored in a pointer. </summary>
        ///
        /// <param name="pPointer"> Pointer to the array. </param>
        ///
        /// <returns> The value of the entry at the given offset in the array. </returns>
        LLVMValue ValueAt(LLVMValue pPointer);

        /// <summary> Get the value at an offset in a global array variable. </summary>
        ///
        /// <param name="pGlobal"> Pointer to the array. </param>
        /// <param name="pOffset"> The offset. </param>
        ///
        /// <returns> The value of the entry at the given offset in the array. </returns>
        LLVMValue ValueAt(llvm::GlobalVariable* pGlobal, LLVMValue pOffset);

        /// <summary> Get the value at an offset in a global array variable. </summary>
        ///
        /// <param name="pGlobal"> Pointer to the array. </param>
        /// <param name="offset"> The offset. </param>
        ///
        /// <returns> The value of the entry at the given offset in the array. </returns>
        LLVMValue ValueAt(llvm::GlobalVariable* pGlobal, int offset);

        /// <summary> Get the value stored in a pointer to a global variable. </summary>
        ///
        /// <param name="pGlobal"> Pointer to the array. </param>
        ///
        /// <returns> The value of the entry at the given offset in the array. </returns>
        LLVMValue ValueAt(llvm::GlobalVariable* pGlobal);

        /// <summary> Set an element in an array. </summary>
        ///
        /// <param name="pPointer"> Pointer to the array. </param>
        /// <param name="pOffset"> The offset. </param>
        /// <param name="pValue"> The value to set. </param>
        LLVMValue SetValueAt(LLVMValue pPointer, LLVMValue pOffset, LLVMValue pValue);

        /// <summary> Set an element in an array. </summary>
        ///
        /// <param name="pPointer"> Pointer to the array. </param>
        /// <param name="offset"> The offset. </param>
        /// <param name="pValue"> The value to set. </param>
        LLVMValue SetValueAt(LLVMValue pPointer, int offset, LLVMValue pValue);

        /// <summary> Set an element in a global variable array. </summary>
        ///
        /// <param name="pGlobal"> Pointer to the array. </param>
        /// <param name="pOffset"> The offset. </param>
        /// <param name="pValue"> The value to set. </param>
        LLVMValue SetValueAt(llvm::GlobalVariable* pGlobal, LLVMValue pOffset, LLVMValue pValue);

        /// <summary> Set the fields of a struct. </summary>
        ///
        /// <param name="structPtr"> The struct to be filled in. </param>
        /// <param name="fieldValues"> The values to set the fields to. </param>
        void FillStruct(LLVMValue structPtr, const std::vector<LLVMValue>& fieldValues);

        /// <summary> Emit a pointer to an entry in an array. </summary>
        ///
        /// <param name="pArray"> Pointer to the array. </param>
        /// <param name="pOffset"> Pointer to the offset. </param>
        ///
        /// <returns> Pointer to a value that represents that entry in the array. </returns>
        LLVMValue PointerOffset(llvm::GlobalVariable* pGlobal, LLVMValue pOffset);

        /// <summary> Emit a pointer to an entry in an array. </summary>
        ///
        /// <param name="pArray"> Pointer to the array. </param>
        /// <param name="offset"> The offset. </param>
        ///
        /// <returns> Pointer to a value that represents that entry in the array. </returns>
        LLVMValue PointerOffset(llvm::GlobalVariable* pGlobal, int offset);

        /// <summary> Emit a pointer to a FIELD in a STRUCT in a global array of Structs. </summary>
        ///
        /// <param name="pArray"> Pointer to the array. </param>
        /// <param name="pOffset"> Pointer to the offset. </param>
        /// <param name="pFieldOffset"> Pointer to the field offset in the struct. </param>
        ///
        /// <returns> Pointer to a value that represents that field. </returns>
        LLVMValue PointerOffset(llvm::GlobalVariable* pGlobal, LLVMValue pOffset, LLVMValue pFieldOffset);

        /// <summary> Extract a field from a struct held by value. </summary>
        ///
        /// <param name="structValue"> The struct value. </param>
        /// <param name="fieldIndex"> The index of the field to get. </param>
        ///
        /// <returns> The value of the specified field in the struct. </returns>
        LLVMValue ExtractStructField(LLVMValue structValue, size_t fieldIndex);

        /// <summary> Extract a field from a struct referenced by a pointer. </summary>
        ///
        /// <param name="structPtr"> A pointer to the struct. </param>
        /// <param name="fieldIndex"> The index of the field to get. </param>
        ///
        /// <returns> The value of the specified field in the struct. </returns>
        LLVMValue GetStructFieldValue(LLVMValue structPtr, size_t fieldIndex);

        /// <summary> Get a pointer to a field in a struct. </summary>
        ///
        /// <param name="structPtr"> Pointer to the struct. </param>
        /// <param name="fieldIndex"> The index of the field to get. </param>
        ///
        /// <returns> A pointer the specified field in the struct. </returns>
        LLVMValue GetStructFieldPointer(LLVMValue structPtr, size_t fieldIndex);

        //
        // Control flow
        //

        /// <summary> Types used to represent ranges for loops to iterate over </summary>
        struct ConstLoopRange
        {
            int begin;
            int end;
        };

        struct ConstTiledLoopRange
        {
            int begin;
            int end;
            int blockSize; // increment
        };

        struct LoopRange
        {
            emitters::IRLocalScalar begin;
            emitters::IRLocalScalar end;
        };

        struct TiledLoopRange
        {
            emitters::IRLocalScalar begin;
            emitters::IRLocalScalar end;
            emitters::IRLocalScalar blockSize; // increment
        };

        /// <summary> Structure passed in to tiled loops instead of just a scalar index </summary>
        struct BlockInterval
        {
            emitters::IRLocalScalar begin;
            emitters::IRLocalScalar end;
            emitters::IRLocalScalar size; // always == end-begin
            emitters::IRLocalScalar index; // zero-based index of this block (so, begin = loop start + index*size)
        };

        /// <summary> Type aliases for for-loop body lambdas. </summary>
        using ForLoopBodyFunction = std::function<void(IRFunctionEmitter& function, IRLocalScalar iterationVariable)>;
        using MultiDimForLoopBodyFunction = std::function<void(emitters::IRFunctionEmitter& function, std::vector<emitters::IRLocalScalar> indices)>;
        using TiledForLoopBodyFunction = std::function<void(emitters::IRFunctionEmitter& function, BlockInterval interval)>;
        using TiledMultiDimForLoopBodyFunction = std::function<void(emitters::IRFunctionEmitter& function, std::vector<BlockInterval> intervals)>;
        using ParallelForLoopBodyFunction = IRParallelForLoopEmitter::BodyFunction;

        /// <summary> Type alias for while-loop body lambda. </summary>
        using WhileLoopBodyFunction = std::function<void(IRFunctionEmitter& function)>;

        /// <summary> Type alias for if-else body lambda. </summary>
        using IfElseBodyFunction = std::function<void(IRFunctionEmitter& function)>;

        /// <summary> Emits a for loop counting from zero to a constant end value. </summary>
        ///
        /// <param name="count"> The number of iterations to make. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(int count, ForLoopBodyFunction body);

        /// <summary> Emits a for loop counting from zero to a constant end value. </summary>
        ///
        /// <param name="count"> The number of iterations to make. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(LLVMValue count, ForLoopBodyFunction body);

        /// <summary> Emits a for loop counting from a begin value up to (but not including) a constant end value. </summary>
        ///
        /// <param name="beginValue"> The starting value of the loop iterator. </param>
        /// <param name="endValue"> The ending value of the loop iterator. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(int beginValue, int endValue, ForLoopBodyFunction body);

        /// <summary> Emits a for loop counting from a begin value up to (but not including) a constant end value. </summary>
        ///
        /// <param name="beginValue"> The starting value of the loop iterator. </param>
        /// <param name="endValue"> The ending value of the loop iterator. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(LLVMValue beginValue, LLVMValue endValue, ForLoopBodyFunction body);

        /// <summary> Emits a for loop counting from a begin value up to (but not including) a constant end value with a given increment. </summary>
        ///
        /// <param name="beginValue"> The starting value of the loop iterator. </param>
        /// <param name="endValue"> The ending value of the loop iterator. </param>
        /// <param name="increment"> The increment for the iterator. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(int beginValue, int endValue, int increment, ForLoopBodyFunction body);

        /// <summary> Emits a for loop counting from a begin value up to (but not including) a constant end value with a given increment. </summary>
        ///
        /// <param name="beginValue"> The starting value of the loop iterator. </param>
        /// <param name="endValue"> The ending value of the loop iterator. </param>
        /// <param name="increment"> The increment for the iterator. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(LLVMValue beginValue, LLVMValue endValue, LLVMValue increment, ForLoopBodyFunction body);

        //
        // Extended for loops
        //

        /// <summary> Emits a set of nested for loops, each counting from a begin value up to (but not including) an end value. </summary>
        ///
        /// <param name="ranges"> The range objects describing the ranges to iterate over (begin, end). </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(const std::vector<ConstLoopRange>& ranges, MultiDimForLoopBodyFunction body);

        /// <summary> Emits a set of nested for loops, each counting from a begin value up to (but not including) an end value. </summary>
        ///
        /// <param name="ranges"> The range objects describing the ranges to iterate over (begin, end). </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(const std::vector<LoopRange>& ranges, MultiDimForLoopBodyFunction body);

        /// <summary> Emits a tiled for loop counting from a begin value up to (but not including) an end value with a given increment. </summary>
        ///
        /// <param name="range"> The range object describing the range to iterate over (begin, end, and increment). </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(ConstTiledLoopRange range, TiledForLoopBodyFunction body);

        /// <summary> Emits a tiled for loop counting from a begin value up to (but not including) an end value with a given increment. </summary>
        ///
        /// <param name="range"> The range object describing the range to iterate over (begin, end, and increment). </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(TiledLoopRange range, TiledForLoopBodyFunction body);

        /// <summary> Emits a set of nested tiled for loops, each counting from a begin value up to (but not including) an end value, with a given increment. </summary>
        ///
        /// <param name="ranges"> The range objects describing the ranges to iterate over (begin, end, increment). </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(const std::vector<ConstTiledLoopRange>& ranges, TiledMultiDimForLoopBodyFunction body);

        /// <summary> Emits a set of nested tiled for loops, each counting from a begin value up to (but not including) an end value, with a given increment. </summary>
        ///
        /// <param name="ranges"> The range objects describing the ranges to iterate over (begin, end, increment). </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void For(const std::vector<TiledLoopRange>& ranges, TiledMultiDimForLoopBodyFunction body);

        /// <summary> Emits a parallel for loop counting from zero to a constant end value. </summary>
        ///
        /// <param name="count"> The number of iterations to make. </param>
        /// <param name="capturedValues"> A list of values to be used inside the loop. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void ParallelFor(int count, const std::vector<LLVMValue>& capturedValues, ParallelForLoopBodyFunction body);

        /// <summary> Emits a parallel for loop counting from zero to a constant end value. </summary>
        ///
        /// <param name="count"> The number of iterations to make. </param>
        /// <param name="options"> The options used to modify how the loop is emitted. </param>
        /// <param name="capturedValues"> A list of values to be used inside the loop. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void ParallelFor(int count, const ParallelLoopOptions& options, const std::vector<LLVMValue>& capturedValues, ParallelForLoopBodyFunction body);

        /// <summary> Emits a for loop counting from a begin value up to (but not including) a constant end value with a given increment. </summary>
        ///
        /// <param name="beginValue"> The starting value of the loop iterator. </param>
        /// <param name="endValue"> The ending value of the loop iterator. </param>
        /// <param name="increment"> The increment for the iterator. </param>
        /// <param name="options"> The options used to modify how the loop is emitted. </param>
        /// <param name="capturedValues"> A list of values to be used inside the loop. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void ParallelFor(int beginValue, int endValue, int increment, const ParallelLoopOptions& options, const std::vector<LLVMValue>& capturedValues, ParallelForLoopBodyFunction body);

        /// <summary> Emits a parallel for loop counting from zero to a constant end value. </summary>
        ///
        /// <param name="count"> The number of iterations to make. </param>
        /// <param name="capturedValues"> A list of values to be used inside the loop. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void ParallelFor(LLVMValue count, const std::vector<LLVMValue>& capturedValues, ParallelForLoopBodyFunction body);

        /// <summary> Emits a parallel for loop counting from zero to a constant end value. </summary>
        ///
        /// <param name="count"> The number of iterations to make. </param>
        /// <param name="numTasks"> The number of tasks (chunks) to split the loop into. </param>
        /// <param name="capturedValues"> A list of values to be used inside the loop. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void ParallelFor(LLVMValue count, const ParallelLoopOptions& options, const std::vector<LLVMValue>& capturedValues, ParallelForLoopBodyFunction body);

        /// <summary> Emits a for loop counting from a begin value up to (but not including) a constant end value with a given increment. </summary>
        ///
        /// <param name="beginValue"> The starting value of the loop iterator. </param>
        /// <param name="endValue"> The ending value of the loop iterator. </param>
        /// <param name="increment"> The increment for the iterator. </param>
        /// <param name="options"> The options used to modify how the loop is emitted. </param>
        /// <param name="capturedValues"> A list of values to be used inside the loop. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void ParallelFor(LLVMValue beginValue, LLVMValue endValue, LLVMValue increment, const ParallelLoopOptions& options, const std::vector<LLVMValue>& capturedValues, ParallelForLoopBodyFunction body);

        /// <summary> Emits a while loop. </summary>
        ///
        /// <param name="pTestValuePointer"> Pointer to a memory location that will be dereferenced for the test value. </param>
        /// <param name="body"> A function that emits the body of the loop. </param>
        void While(LLVMValue pTestValuePointer, WhileLoopBodyFunction body);

        /// <summary> Emits a while loop. </summary>
        ///
        /// </param name="condition"> A function the emits code returning a single-bit boolean test value </param>
        ///
        /// <param name="body"> A function that emits the body of the loop. </param>
        void While(std::function<LLVMValue(IRFunctionEmitter&)> condition, WhileLoopBodyFunction body);

        /// <summary> Emits an if statement. </summary>
        ///
        /// <param name="pTestValuePointer"> Pointer to a memory location that will be dereferenced for the test value. </param>
        /// <param name="body"> A function that emits the body of the "if true" block. </param>
        ///
        /// <returns>
        ///    An IRIfEmitter, used to allow chaining of ElseIf() and Else() calls. The typical pattern is to _not_ assign
        ///    the return value of `If()` to a variable, but to let the destructor automatically end the if block at the end of the statement.
        /// </returns>
        IRIfEmitter If(LLVMValue pTestValuePointer, IfElseBodyFunction body);

        /// <summary> Emits an if statement. </summary>
        ///
        /// <param name="comparison"> A function that returns a boolean (LLVM) value. </param>
        /// <param name="body"> A function that emits the body of the "if true" block. </param>
        ///
        /// <returns>
        ///    An IRIfEmitter, used to allow chaining of ElseIf() and Else() calls. The typical pattern is to _not_ assign
        ///    the return value of `If()` to a variable, but to let the destructor automatically end the if block at the end of the statement.
        /// </returns>
        IRIfEmitter If(std::function<LLVMValue()> comparison, IfElseBodyFunction body);

        /// <summary> Gets an if statement emitter. </summary>
        ///
        /// <param name="comparison"> The if-statement comparison. </param>
        /// <param name="pValue"> Pointer to the value used in the comparison. </param>
        /// <param name="pTestValue"> Pointer to the test value that is compared against pValue. </param>
        /// <param name="body"> A function that emits the body of the "if true" block. </param>
        ///
        /// <returns>
        ///    An IRIfEmitter, used to allow chaining of ElseIf() and Else() calls. The typical pattern is to _not_ assign
        ///    the return value of `If()` to a variable, but to let the destructor automatically end the if block at the end of the statement.
        /// </returns>
        IRIfEmitter If(TypedComparison comparison, LLVMValue pValue, LLVMValue pTestValue, IfElseBodyFunction body);

        //
        // Async tasks
        //

        /// <summary> Creates an asynchronous task on a new thread. </summary>
        ///
        /// <param name="task"> The function to run asynchronously. </param>
        ///
        /// <returns> A task object representing the running task. </param>
        IRTask StartAsyncTask(LLVMFunction task);

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
        IRTask StartAsyncTask(LLVMFunction task, const std::vector<LLVMValue>& arguments);

        /// <summary> Creates an asynchronous task on a new thread. </summary>
        ///
        /// <param name="task"> The function to run asynchronously. </param>
        /// <param name="arguments"> The arguments to the task function. </param>
        ///
        /// <returns> A task object representing the running task. </param>
        IRTask StartAsyncTask(IRFunctionEmitter& task, const std::vector<LLVMValue>& arguments);

        /// <summary> Starts an array of tasks using the thread pool, or new threads, depending on the value of the `useThreadPool` compiler setting. </summary>
        ///
        /// <param name="taskFunction"> The function to run asynchronously with many different arguments. </param>
        /// <param name="arguments"> For each task, a vector of arguments for that task. </param>
        ///
        /// <returns> A task array object representing the running tasks. </param>
        IRTaskArray StartTasks(IRFunctionEmitter& taskFunction, const std::vector<std::vector<LLVMValue>>& arguments);

        /// <summary> Starts an array of tasks using the thread pool, or new threads, depending on the value of the `useThreadPool` compiler setting. </summary>
        ///
        /// <param name="taskFunction"> The function to run asynchronously with many different arguments. </param>
        /// <param name="arguments"> For each task, a vector of arguments for that task. </param>
        ///
        /// <returns> A task array object representing the running tasks. </param>
        IRTaskArray StartTasks(LLVMFunction taskFunction, const std::vector<std::vector<LLVMValue>>& arguments);

        //
        // Standard C library function calls
        //

        /// <summary> Emits a malloc call. </summary>
        ///
        /// <param name="type"> The type being allocated. </param>
        /// <param name="size"> The size being allocated. </param>
        ///
        /// <returns> Pointer to the llvm::Value that points to the allocated memory. </returns>
        LLVMValue Malloc(VariableType type, int64_t size);

        /// <summary> Emits a malloc call. </summary>
        ///
        /// <param name="type"> The type being allocated. </param>
        /// <param name="size"> The size being allocated. </param>
        ///
        /// <returns> Pointer to the llvm::Value that points to the allocated memory. </returns>
        LLVMValue Malloc(LLVMType type, int64_t size);

        /// <summary> Emits a malloc call. </summary>
        ///
        /// <param name="type"> The type being allocated. </param>
        /// <param name="size"> The size being allocated. </param>
        ///
        /// <returns> Pointer to the llvm::Value that points to the allocated memory. </returns>
        LLVMValue Malloc(LLVMType type, LLVMValue size);

        /// <summary> Emits a malloc call. </summary>
        ///
        /// <typeparam name="ValueType"> The type being allocated. </typeparam>
        /// <param name="size"> The size being allocated. </param>
        ///
        /// <returns> Pointer to the llvm::Value that points to the allocated memory. </returns>
        template <typename ValueType>
        LLVMValue Malloc(int64_t size);

        /// <summary> Emits a free call. </summary>
        ///
        /// <param name="pValue"> Pointer to the address of the memory to be freed. </param>
        void Free(LLVMValue pValue);

        /// <summary> Emits a print call. </summary>
        ///
        /// <param name="text"> The text to print. </param>
        ///
        /// <returns> Pointer to the return value of the call to the print function. </returns>
        LLVMValue Print(const std::string& text);

        /// <summary> Emits a printf call. </summary>
        ///
        /// <param name="arguments"> Arguments to the printf call. </param>
        ///
        /// <returns> Pointer to the return value of the call to the printf function. </returns>
        LLVMValue Printf(std::initializer_list<LLVMValue> arguments);

        /// <summary> Emits a printf call. </summary>
        ///
        /// <param name="format"> Describes the printf format to use. </param>
        /// <param name="arguments"> Arguments to the printf call. </param>
        ///
        /// <returns> Pointer to the return value of the call to the printf function. </returns>
        LLVMValue Printf(const std::string& format, std::initializer_list<LLVMValue> arguments);

        /// <summary> Emits a printf call. </summary>
        ///
        /// <param name="format"> Describes the printf format to use. </param>
        /// <param name="arguments"> Arguments to the printf call. </param>
        ///
        /// <returns> Pointer to the return value of the call to the printf function. </returns>
        LLVMValue Printf(const std::string& format, std::vector<LLVMValue> arguments);

        /// <summary> Emits a memmove call, which moves an array of variables. </summary>
        ///
        /// <typeparam name="ValueType"> The type being moved. </typeparam>
        /// <param name="pPointer"> Pointer to the base address of the move operation. </param>
        /// <param name="sourceOffset"> Source address offset. </param>
        /// <param name="destinationOffset"> Destination address offset. </param>
        /// <param name="count"> Number of elements being moved. </param>
        template <typename ValueType>
        void MemoryMove(LLVMValue pPointer, int sourceOffset, int destinationOffset, int count);

        /// <summary> Emits a memcpy call, which copies an array of variables. </summary>
        ///
        /// <typeparam name="ValueType"> The type being moved. </typeparam>
        /// <param name="pSourcePointer"> Pointer to the base address of the source. </param>
        /// <param name="pDestinationPointer"> Pointer to the base address of the destination. </param>
        /// <param name="count"> Number of elements being moved. </param>
        template <typename ValueType>
        void MemoryCopy(LLVMValue pSourcePointer, LLVMValue pDestinationPointer, int count);

        /// <summary> Emits a memcpy call, which copies an array of variables. </summary>
        ///
        /// <typeparam name="ValueType"> The type being moved. </typeparam>
        /// <param name="pSourcePointer"> Pointer to the base address of the source. </param>
        /// <param name="pDestinationPointer"> Pointer to the base address of the destination. </param>
        /// <param name="count"> Number of elements being moved. </param>
        template <typename ValueType>
        void MemoryCopy(LLVMValue pSourcePointer, LLVMValue pDestinationPointer, LLVMValue count);

        /// <summary> Emits a memcpy call, which copies an array of variables. </summary>
        ///
        /// <typeparam name="ValueType"> The type being moved. </typeparam>
        /// <param name="pSourcePointer"> Pointer to the base address of the source. </param>
        /// <param name="sourceOffset"> Source address offset. </param>
        /// <param name="pDestinationPointer"> Pointer to the base address of the destination. </param>
        /// <param name="destinationOffset"> Destination address offset. </param>
        /// <param name="count"> Number of elements being moved. </param>
        template <typename ValueType>
        void MemoryCopy(LLVMValue pSourcePointer, int sourceOffset, LLVMValue pDestinationPointer, int destinationOffset, int count);

        /// <summary> Emits a memcpy call, which copies an array of variables. </summary>
        ///
        /// <typeparam name="ValueType"> The type being moved. </typeparam>
        /// <param name="pSourcePointer"> Pointer to the base address of the source. </param>
        /// <param name="sourceOffset"> Source address offset. </param>
        /// <param name="pDestinationPointer"> Pointer to the base address of the destination. </param>
        /// <param name="destinationOffset"> Destination address offset. </param>
        /// <param name="count"> Number of elements being moved. </param>
        template <typename ValueType>
        void MemoryCopy(LLVMValue pSourcePointer, LLVMValue sourceOffset, LLVMValue pDestinationPointer, LLVMValue destinationOffset, LLVMValue count);

        /// <summary> Emits a memset call, which sets an array of memory to a given byte value. </summary>
        ///
        /// <typeparam name="ValueType"> The type of the array being set. </typeparam>
        /// <param name="pDestinationPointer"> Pointer to the base address of the destination. </param>
        /// <param name="destinationOffset"> Destination address offset. </param>
        /// <param name="value"> The byte value being set. </param>
        /// <param name="count"> Number of elements being set. </param>
        template <typename ValueType>
        void MemorySet(LLVMValue pDestinationPointer, int destinationOffset, LLVMValue value, int count);

        /// <summary> Emits a memset call, which sets an array of memory to a given byte value. </summary>
        ///
        /// <typeparam name="ValueType"> The type of the array being set. </typeparam>
        /// <param name="pDestinationPointer"> Pointer to the base address of the destination. </param>
        /// <param name="destinationOffset"> Destination address offset. </param>
        /// <param name="value"> The byte value being set. </param>
        /// <param name="count"> Number of elements being set. </param>
        template <typename ValueType>
        void MemorySet(LLVMValue pDestinationPointer, LLVMValue pDestinationOffset, LLVMValue value, int count);

        /// <summary> Emits a memset call, which sets an array of memory to a given byte value. </summary>
        ///
        /// <typeparam name="ValueType"> The type of the array being set. </typeparam>
        /// <param name="pDestinationPointer"> Pointer to the base address of the destination. </param>
        /// <param name="destinationOffset"> Destination address offset. </param>
        /// <param name="value"> The byte value being set. </param>
        /// <param name="count"> Number of elements being set. </param>
        template <typename ValueType>
        void MemorySet(LLVMValue pDestinationPointer, LLVMValue pDestinationOffset, LLVMValue value, LLVMValue count);

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
        LLVMValue GetClockMilliseconds();

        //
        // Optimizations
        //

        //
        // Inline common code generators
        //

        /// <summary> Emit IR to printf each item in the given vector. </summary>
        ///
        /// <param name="formatString"> The printf format string. </param>
        /// <param name="pVector"> Pointer to the address of the first entry in the array. </param>
        /// <param name="size"> The array size. </param>
        void PrintForEach(const std::string& formatString, LLVMValue pVector, int size);

        /// <summary> Emit IR to compute a dot product. </summary>
        ///
        /// <param name="size"> Array size. </param>
        /// <param name="pLeftValue"> Pointer to the address of the first entry in the first array. </param>
        /// <param name="pRightValue"> Pointer to the address of the first entry in the second array. </param>
        ///
        /// <returns> Pointer to the result. </returns>
        LLVMValue DotProduct(int size, LLVMValue pLeftValue, LLVMValue pRightValue);

        /// <summary> Emit IR to compute a dot product, storing the result in an existing variable. </summary>
        ///
        /// <param name="size"> Array size. </param>
        /// <param name="pLeftValue"> Pointer to the address of the first entry in the first array. </param>
        /// <param name="pRightValue"> Pointer to the address of the first entry in the second array. </param>
        /// <param name="pDestination"> Pointer to the address where to write the result. </param>
        void DotProduct(int size, LLVMValue pLeftValue, LLVMValue pRightValue, LLVMValue pDestination);

        /// <summary> Emit IR to compute a dot product, storing the result in an existing variable. </summary>
        ///
        /// <param name="pSize"> [in,out] Pointer to the runtime size variable. </param>
        /// <param name="pLeftValue"> Pointer to the address of the first entry in the first array. </param>
        /// <param name="pRightValue"> Pointer to the address of the first entry in the second array. </param>
        /// <param name="pDestination"> Pointer to the address where to write the result. </param>
        void DotProduct(LLVMValue pSize, LLVMValue pLeftValue, LLVMValue pRightValue, LLVMValue pDestination);

        /// <summary> Emits a shift register. </summary>
        ///
        /// <typeparam name="ValueType"> Type of entry in the shift register. </typeparam>
        /// <param name="pBuffer"> Pointer to the location of the buffer. </param>
        /// <param name="bufferSize"> Buffer size. </param>
        /// <param name="shiftCount"> The shift count. </param>
        /// <param name="pNewData"> ?. </param>
        /// <param name="pShiftedData"> ?. </param>
        template <typename ValueType>
        void ShiftAndUpdate(LLVMValue pBuffer, int bufferSize, int shiftCount, LLVMValue pNewData, LLVMValue pShiftedData = nullptr);

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
        void CallGEMV(int m, int n, LLVMValue A, int lda, LLVMValue x, int incx, LLVMValue y, int incy);

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
        void CallGEMV(int m, int n, ValueType alpha, LLVMValue A, int lda, LLVMValue x, int incx, ValueType beta, LLVMValue y, int incy);

        /// <summary> Call the matrix-vector multiply routine that computes y = alpha*A*x + beta*y </summary>
        ///
        /// <typeparam name="ValueType"> The datatype to use (must be `float` or `double`) </typeparam>
        /// <param name="transposeA"> If `true`, use A' instead of A in the above equation </param>
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
        void CallGEMV(bool transposeA, int m, int n, ValueType alpha, LLVMValue A, int lda, LLVMValue x, int incx, ValueType beta, LLVMValue y, int incy);

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
        void CallGEMM(int m, int n, int k, LLVMValue A, int lda, LLVMValue B, int ldb, LLVMValue C, int ldc);

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
        void CallGEMM(bool transposeA, bool transposeB, int m, int n, int k, LLVMValue A, int lda, LLVMValue B, int ldb, LLVMValue C, int ldc);

        /// <summary> Utility function for getting number of threads used by OpenBLAS (if present) </summary>
        LLVMValue GetNumOpenBLASThreads();

        /// <summary> Utility function for setting number of threads used by OpenBLAS (if present) </summary>
        void SetNumOpenBLASThreads(LLVMValue numThreads);

        //
        // Calling POSIX functions
        //
        // Note: the arguments to the following functions are just LLVMValue representations of the value to
        //   be passed to the underlying POSIX function. See IRPosixRuntime for more POSIX-specific information.

        /// <summary> Indicates if the target has POSIX functions available to it. </summary>
        bool HasPosixFunctions() const;

        /// <summary> Emits a call to the POSIX `pthread_create` function. </summary>
        LLVMValue PthreadCreate(LLVMValue threadVar, LLVMValue attrPtr, LLVMFunction taskFunction, LLVMValue taskArgument);

        /// <summary> Emits a call to the POSIX `pthread_equal` function. </summary>
        LLVMValue PthreadEqual(LLVMValue thread1, LLVMValue thread2);

        /// <summary> Emits a call to the POSIX `pthread_exit` function. </summary>
        void PthreadExit(LLVMValue status);

        /// <summary> Emits a call to the POSIX `pthread_getconcurrency` function. </summary>
        LLVMValue PthreadGetConcurrency();

        /// <summary> Emits a call to the POSIX `pthread_detach` function. </summary>
        LLVMValue PthreadDetach(LLVMValue thread);

        /// <summary> Emits a call to the POSIX `pthread_join` function. </summary>
        LLVMValue PthreadJoin(LLVMValue thread, LLVMValue statusOut);

        /// <summary> Emits a call to the POSIX `pthread_self` function. </summary>
        LLVMValue PthreadSelf();

        /// <summary> Emits a call to the POSIX `pthread_mutex_init` function. </summary>
        LLVMValue PthreadMutexInit(LLVMValue mutexPtr, LLVMValue attrPtr);

        /// <summary> Emits a call to the POSIX `pthread_mutex_destroy` function. </summary>
        LLVMValue PthreadMutexDestroy(LLVMValue mutexPtr);

        /// <summary> Emits a call to the POSIX `pthread_mutex_lock` function. </summary>
        LLVMValue PthreadMutexLock(LLVMValue mutexPtr);

        /// <summary> Emits a call to the POSIX `pthread_mutex_trylock` function. </summary>
        LLVMValue PthreadMutexTryLock(LLVMValue mutexPtr);

        /// <summary> Emits a call to the POSIX `pthread_mutex_unlock` function. </summary>
        LLVMValue PthreadMutexUnlock(LLVMValue mutexPtr);

        /// <summary> Emits a call to the POSIX `pthread_cond_init` function. </summary>
        LLVMValue PthreadCondInit(LLVMValue condPtr, LLVMValue condAttrPtr);

        /// <summary> Emits a call to the POSIX `pthread_cond_destroy` function. </summary>
        LLVMValue PthreadCondDestroy(LLVMValue condPtr);

        /// <summary> Emits a call to the POSIX `pthread_cond_wait` function. </summary>
        LLVMValue PthreadCondWait(LLVMValue condPtr, LLVMValue mutexPtr);

        /// <summary> Emits a call to the POSIX `pthread_cond_timedwait` function. </summary>
        LLVMValue PthreadCondTimedwait(LLVMValue condPtr, LLVMValue mutexPtr, LLVMValue timespecPtr);

        /// <summary> Emits a call to the POSIX `pthread_cond_signal` function. </summary>
        LLVMValue PthreadCondSignal(LLVMValue condPtr);

        /// <summary> Emits a call to the POSIX `pthread_cond_broadcast` function. </summary>
        LLVMValue PthreadCondBroadcast(LLVMValue condPtr);

        //
        // Experimental functions
        //

        /// <summary> Gets the CPU id of the currently-running thread. Currently only available on Linux. Returns -1 if unavailable. </summary>
        LLVMValue GetCpu();

        //
        // Information about the current function begin emitted
        //

        /// <summary> Gets a pointer to the underlying llvm::Function. </summary>
        ///
        /// <returns> Pointer to an llvm::Function. </returns>
        LLVMFunction GetFunction() const { return _pFunction; }

        /// <summary> Gets a reference to the module emitter. </summary>
        ///
        /// <returns> Reference to an `IRModuleEmitter`. </returns>
        IRModuleEmitter& GetModule() const { return *_pModuleEmitter; }

        /// <summary> Returns the current block being emitted into </summary>
        IRBlockRegion* GetCurrentRegion() { return _pCurRegion; }

        /// <summary> Adds an`IRBlockRegion` to the region list and sets it as the current region </summary>
        IRBlockRegion* AddRegion(llvm::BasicBlock* pBlock);

        /// <summary> Get the compiler options for the function being emitted. </summary>
        ///
        /// <returns> A `CompilerOptions` object containing the options for this function. </returns>
        const CompilerOptions& GetCompilerOptions() const;

        /// <summary> Set the compiler options for the function being emitted. </summary>
        ///
        /// <param name=options> A `CompilerOptions` object containing the options to use for this function. </param]>
        void SetCompilerOptions(const CompilerOptions& parameters);

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
        void WriteToStream(std::ostream& os) const;

        /// <summary> Emit LLVM IR to std::out for debugging. </summary>
        void DebugDump() const;

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
        friend class IRModuleEmitter;

        IRFunctionEmitter(IRModuleEmitter* pModule, LLVMFunction pFunction, const std::string& name);
        IRFunctionEmitter(IRModuleEmitter* pModule, LLVMFunction pFunction, const NamedVariableTypeList& arguments, const std::string& name);
        IRFunctionEmitter(IRModuleEmitter* pModule, LLVMFunction pFunction, const NamedLLVMTypeList& arguments, const std::string& name);
        IRFunctionEmitter(IRModuleEmitter* pModule, LLVMFunction pFunction, const FunctionArgumentList& arguments, const std::string& name);

        class EntryBlockScope
        {
        public:
            EntryBlockScope(IRFunctionEmitter& function);
            void ExitScope();
            ~EntryBlockScope();

        private:
            IRFunctionEmitter& _function;
            llvm::IRBuilder<>::InsertPoint _oldPos;
            bool _inScope = true;
        };

        LLVMValue PtrOffsetA(LLVMValue pPointer, int offset);
        LLVMValue PtrOffsetA(LLVMValue pPointer, LLVMValue pOffset, const std::string& name = "");
        LLVMValue ValueAtA(LLVMValue pPointer, int offset);
        LLVMValue ValueAtA(LLVMValue pPointer, LLVMValue pOffset);
        LLVMValue SetValueAtA(LLVMValue pPointer, int offset, LLVMValue pValue);
        LLVMValue SetValueAtA(LLVMValue pPointer, LLVMValue pOffset, LLVMValue pValue);
        LLVMValue PtrOffsetH(LLVMValue pPointer, int offset);
        LLVMValue PtrOffsetH(LLVMValue pPointer, LLVMValue pOffset);
        LLVMValue ValueAtH(LLVMValue pPointer, int offset);
        LLVMValue ValueAtH(LLVMValue pPointer, LLVMValue pOffset);
        LLVMValue SetValueAtH(LLVMValue pPointer, int offset, LLVMValue pValue);

        llvm::BasicBlock* GetEntryBlock() { return _entryBlock; }
        void SetUpFunction();

        void RegisterFunctionArgs(const NamedVariableTypeList& args);
        void RegisterFunctionArgs(const NamedLLVMTypeList& args);
        void RegisterFunctionArgs(const FunctionArgumentList& args);

        void CompleteFunction();

        bool CanUseBlas() const;
        void EnsurePrintf();

        LLVMFunction ResolveFunction(const std::string& name);
        llvm::Module* GetLLVMModule() { return _pFunction->getParent(); }
        friend void swap(IRFunctionEmitter& first, IRFunctionEmitter& second);

        IRValueTable _locals; // Symbol table: name -> LLVMValue (stack variables or function arguments)

        IRModuleEmitter* _pModuleEmitter = nullptr;
        CompilerOptions _options;
        IRBlockRegionList _regions;
        IRBlockRegion* _pCurRegion = nullptr;
        LLVMFunction _pFunction = nullptr;
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
        LLVMValue GetArgumentAt(size_t index) const;

        /// <summary> Add an argument. </summary>
        ///
        /// <param name="pValue"> Pointer to the value being added. </param>
        void Append(LLVMValue pValue);

        /// <summary>
        /// Allocates a variable to hold the output, appends it to the argument list, and returns the variable.
        /// </summary>
        ///
        /// <param name="type"> Variable type. </param>
        /// <param name="size"> Variable size. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the allocated variable. </returns>
        LLVMValue AppendOutput(VariableType type, int size);

        /// <summary> Cast operator to the underlying IRValueList. </summary>
        ///
        /// <returns> Const reference to the underlying IRValueList. </returns>
        operator const IRValueList&() { return _arguments; }

    private:
        IRFunctionEmitter& _functionEmitter;
        IRValueList _arguments;
    };
} // namespace emitters
} // namespace ell

#pragma region implementation

namespace ell
{
namespace emitters
{
    template <typename ValueType, utilities::IsFundamental<ValueType>>
    IRLocalScalar IRFunctionEmitter::LocalScalar(ValueType value)
    {
        return IRLocalScalar(*this, Literal(value));
    }

    template <typename ValueType>
    LLVMValue IRFunctionEmitter::Literal(ValueType value)
    {
        return GetEmitter().Literal(value);
    }

    template <typename ValueType>
    LLVMValue IRFunctionEmitter::Pointer(ValueType* value)
    {
        return GetEmitter().Pointer(value);
    }

    template <typename OutputType>
    LLVMValue IRFunctionEmitter::CastValue(LLVMValue pValue)
    {
        return GetEmitter().CastValue<OutputType>(pValue);
    }

    template <typename OutputType>
    LLVMValue IRFunctionEmitter::CastUnsignedValue(LLVMValue pValue)
    {
        return GetEmitter().CastValue<OutputType>(pValue);
    }

    template <typename ValueType>
    void IRFunctionEmitter::VectorOperator(TypedOperator type, size_t size, ValueType leftValue, LLVMValue pRightValue, std::function<void(LLVMValue, LLVMValue)> aggregator)
    {
        assert(pRightValue != nullptr);

        LLVMValue pLeftItem = Literal(leftValue);
        For(size, [pLeftItem, pRightValue, type, aggregator](IRFunctionEmitter& fn, LLVMValue i) {
            LLVMValue pRightItem = fn.ValueAt(pRightValue, i);
            LLVMValue pTemp = fn.Operator(type, pLeftItem, pRightItem);
            aggregator(i, pTemp);
        });
    }

    template <typename ValueType>
    void IRFunctionEmitter::VectorOperator(TypedOperator type, size_t size, LLVMValue pLeftValue, ValueType rightValue, std::function<void(LLVMValue, LLVMValue)> aggregator)
    {
        assert(pLeftValue != nullptr);

        LLVMValue pRightItem = Literal(rightValue);
        For(size, [pLeftValue, pRightItem, type, aggregator](IRFunctionEmitter& fn, LLVMValue i) {
            LLVMValue pLeftItem = fn.ValueAt(pLeftValue, i);
            LLVMValue pTemp = fn.Operator(type, pLeftItem, pRightItem);
            aggregator(i, pTemp);
        });
    }

    template <typename ValueType>
    LLVMValue IRFunctionEmitter::Malloc(int64_t size)
    {
        return Malloc(GetVariableType<ValueType>(), size);
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemoryMove(LLVMValue pPointer, int sourceOffset, int destinationOffset, int count)
    {
        assert(pPointer != nullptr);
        auto pSource = PointerOffset(pPointer, Literal(sourceOffset));
        auto pDestination = PointerOffset(pPointer, Literal(destinationOffset));
        int byteCount = count * GetEmitter().SizeOf<ValueType>();
        GetEmitter().MemoryMove(pSource, pDestination, Literal(byteCount));
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemoryCopy(LLVMValue pSourcePointer, LLVMValue pDestinationPointer, int count)
    {
        auto pSource = PointerOffset(pSourcePointer, 0);
        auto pDestination = PointerOffset(pDestinationPointer, 0);
        auto byteCount = count * GetEmitter().SizeOf<ValueType>();
        GetEmitter().MemoryCopy(pSource, pDestination, Literal<int>(byteCount));
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemoryCopy(LLVMValue pSourcePointer, LLVMValue pDestinationPointer, LLVMValue count)
    {
        auto pSource = PointerOffset(pSourcePointer, 0);
        auto pDestination = PointerOffset(pDestinationPointer, 0);
        auto byteCount = Operator(emitters::TypedOperator::multiply, count, Literal<int>(sizeof(ValueType)));
        GetEmitter().MemoryCopy(pSource, pDestination, byteCount);
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemoryCopy(LLVMValue pSourcePointer, int sourceOffset, LLVMValue pDestinationPointer, int destinationOffset, int count)
    {
        auto pSource = PointerOffset(pSourcePointer, Literal(sourceOffset));
        auto pDestination = PointerOffset(pDestinationPointer, Literal(destinationOffset));
        int byteCount = count * GetEmitter().SizeOf<ValueType>();
        GetEmitter().MemoryCopy(pSource, pDestination, Literal(byteCount));
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemoryCopy(LLVMValue pSourcePointer, LLVMValue sourceOffset, LLVMValue pDestinationPointer, LLVMValue destinationOffset, LLVMValue count)
    {
        auto pSource = PointerOffset(pSourcePointer, sourceOffset);
        auto pDestination = PointerOffset(pDestinationPointer, destinationOffset);
        auto byteCount = Operator(emitters::TypedOperator::multiply, count, Literal<int>(sizeof(ValueType)));
        GetEmitter().MemoryCopy(pSource, pDestination, byteCount);
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemorySet(LLVMValue pDestinationPointer, int destinationOffset, LLVMValue value, int count)
    {
        auto pDestination = PointerOffset(pDestinationPointer, Literal(destinationOffset));
        int byteCount = count * GetEmitter().SizeOf<ValueType>();
        GetEmitter().MemorySet(pDestination, value, Literal(byteCount));
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemorySet(LLVMValue pDestinationPointer, LLVMValue pDestinationOffset, LLVMValue value, int count)
    {
        auto pDestination = PointerOffset(pDestinationPointer, pDestinationOffset);
        int byteCount = count * GetEmitter().SizeOf<ValueType>();
        GetEmitter().MemorySet(pDestination, value, Literal(byteCount));
    }

    template <typename ValueType>
    void IRFunctionEmitter::MemorySet(LLVMValue pDestinationPointer, LLVMValue pDestinationOffset, LLVMValue value, LLVMValue count)
    {
        auto pDestination = PointerOffset(pDestinationPointer, pDestinationOffset);
        auto byteCount = Operator(emitters::TypedOperator::multiply, count, Literal<int>(sizeof(ValueType)));
        GetEmitter().MemorySet(pDestination, value, byteCount);
    }

    template <typename ValueType>
    void IRFunctionEmitter::ShiftAndUpdate(LLVMValue buffer, int bufferSize, int shiftCount, LLVMValue pNewData, LLVMValue pShiftedData)
    {
        assert(buffer != nullptr);
        assert(shiftCount <= bufferSize);

        if (pShiftedData != nullptr)
        {
            MemoryCopy<ValueType>(buffer, 0, pShiftedData, 0, shiftCount);
        }
        if (shiftCount < bufferSize)
        {
            MemoryMove<ValueType>(buffer, shiftCount, 0, (bufferSize - shiftCount));
        }
        MemoryCopy<ValueType>(pNewData, 0, buffer, (bufferSize - shiftCount), shiftCount);
    }

} // namespace emitters
} // namespace ell

#pragma endregion implementation
