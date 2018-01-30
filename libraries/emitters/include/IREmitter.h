////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IREmitter.h (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "EmitterTypes.h"
#include "LLVMUtilities.h"
#include "SymbolTable.h"

// llvm
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

// stl
#include <unordered_map>
#include <vector>

namespace ell
{
namespace emitters
{
    /// <summary> A list of LLVM IR Value* </summary>
    using IRValueList = std::vector<llvm::Value*>;

    /// <summary> Symbol Table that maps symbol Names to emitted IR Value* </summary>
    using IRVariableTable = SymbolTable<llvm::Value*>;

    /// <summary> Symbol Table that maps type Names to emitted IR Type* </summary>
    using IRTypeTable = SymbolTable<llvm::Type*>;

    /// <summary> Convert LLVM errors into an ELL styled exception </summary>
    using LLVMException = utilities::ErrorCodeException<std::error_code>;

    /// <summary>
    /// Wraps the LLVM API with an easy to use object model that hides some unncessary detail.
    /// Incorporates our own x-compiler abstractions such as VariableType and TypedOperator.
    ///
    /// Note: IREmitter is stateful. It has a "current block" that it is emitting IR into.
    /// </summary>
    class IREmitter
    {
    public:
        /// <summary> Construct a new emitter </summary>
        IREmitter(llvm::LLVMContext& context);

        /// <summary> Get the LLVM Type information for a VariableType. </summary>
        ///
        /// <param name="type"> The VariableType. </param>
        ///
        /// <returns> Pointer to an llvm::Type object that corresponds to the given VariableType. </returns>
        llvm::Type* Type(VariableType type);

        /// <summary> Get the LLVM Type information for a pointer to a VariableType. </summary>
        ///
        /// <param name="type"> The VariableType. </param>
        ///
        /// <returns> Pointer to an llvm::Type object that corresponds to a pointer to the given VariableType. </returns>
        llvm::PointerType* PointerType(VariableType type);

        /// <summary> Get the LLVM Type information for a pointer to an LLVM  type. </summary>
        ///
        /// <param name="type"> The LLVM type representing a pointer to the given type. </param>
        ///
        /// <returns> Pointer to an llvm::Type object that corresponds to a pointer to the given VariableType. </returns>
        llvm::PointerType* PointerType(llvm::Type* type);

        /// <summary>
        /// Get the LLVM Type Information for an ARRAY of VariableType, with a given size.
        /// </summary>
        ///
        /// <param name="type"> The entry type. </param>
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> Pointer to an llvm::ArrayType object that represents the specified array. </returns>
        llvm::ArrayType* ArrayType(VariableType type, size_t size);

        /// <summary>
        /// Get the LLVM Type Information for an ARRAY of VariableType, with a given size.
        /// </summary>
        ///
        /// <param name="type"> The entry type. </param>
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> Pointer to an llvm::ArrayType object that represents the specified array. </returns>
        llvm::ArrayType* ArrayType(llvm::Type* type, size_t size);

        /// <summary>
        /// Get the LLVM Type Information for a vector of a given size.
        /// </summary>
        ///
        /// <param name="type"> The entry type. </param>
        /// <param name="size"> The vector size. </param>
        ///
        /// <returns> Pointer to an llvm::VectorType object that represents the specified vector. </returns>
        llvm::VectorType* VectorType(VariableType type, size_t size);

        /// <summary>
        /// Get the LLVM Type Information for a vector of a given size.
        /// </summary>
        ///
        /// <param name="type"> The entry type. </param>
        /// <param name="size"> The vector size. </param>
        ///
        /// <returns> Pointer to an llvm::VectorType object that represents the specified vector. </returns>
        llvm::VectorType* VectorType(llvm::Type* type, size_t size);

        /// <summary> Emit a boolean literal. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents the boolean literal. </returns>
        llvm::Constant* Literal(const bool value);

        /// <summary> Emit a byte literal. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents the byte literal. </returns>
        llvm::Constant* Literal(const uint8_t value);

        /// <summary> Emit a short literal. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents the short literal. </returns>
        llvm::Constant* Literal(const short value);

        /// <summary> Emit an Int32 literal. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents the Int32 literal. </returns>
        llvm::Constant* Literal(const int value);

        /// <summary> Emit a Int64 literal. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents the Int64 literal. </returns>
        llvm::Constant* Literal(const int64_t value);

        /// <summary> Emit a float literal. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents the float literal. </returns>
        llvm::Constant* Literal(const float value);

        /// <summary> Emit a double literal. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents the double literal. </returns>
        llvm::Constant* Literal(const double value);

        /// <summary> Emit a string literal. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the string literal. </returns>
        llvm::Value* Literal(const char* pValue);

        /// <summary> Emit a string literal. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the string literal. </returns>
        llvm::Value* Literal(const std::string& value);

        /// <summary> Emit a named string literal. </summary>
        ///
        /// <param name="name"> The literal name. </param>
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the string literal. </returns>
        llvm::Value* Literal(const std::string& name, const std::string& value);

        /// <summary> Emit a literal array of bytes. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents an array of bytes. </returns>
        llvm::Constant* Literal(const std::vector<uint8_t>& value);

        /// <summary> Emit a literal array of Int32. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents an array of Int32. </returns>
        llvm::Constant* Literal(const std::vector<int>& value);

        /// <summary> Emit a literal array of Int64. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents an array of Int64. </returns>
        llvm::Constant* Literal(const std::vector<int64_t>& value);

        /// <summary> Emit a literal array of floats. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents an array of floats. </returns>
        llvm::Constant* Literal(const std::vector<float>& value);

        /// <summary> Emit a literal array of doubles. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents an array of doubles. </returns>
        llvm::Constant* Literal(const std::vector<double>& value);

        /// <summary> Emit a literal pointer value. </summary>
        ///
        /// <param name="value"> The pointer value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents an pointer. </returns>
        template <typename ValueType>
        llvm::Constant* Pointer(ValueType* ptr);

        /// <summary> Emit a Zero value of the given type. </summary>
        ///
        /// <param name="type"> The type. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents the zero. </returns>
        llvm::Constant* Zero(VariableType type);

        /// <summary> Emit a Zero value of the given type. </summary>
        ///
        /// <param name="type"> The type. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents the zero. </returns>
        llvm::Constant* Zero(llvm::Type* type);

        /// <summary> Emit a true value in a byte. </summary>
        ///
        /// <returns> Pointer to an llvm::Constant that represents true. </returns>
        llvm::Constant* True();

        /// <summary> Emit a false value in a byte. </summary>
        ///
        /// <returns> Pointer to an llvm::Constant that represents false. </returns>
        llvm::Constant* False();

        /// <summary> Emit a true value in a single bit. </summary>
        ///
        /// <returns> Pointer to an llvm::Constant that represents true. </returns>
        llvm::Constant* TrueBit();

        /// <summary> Emit a false value in a single bit. </summary>
        ///
        /// <returns> Pointer to an llvm::Constant that represents false. </returns>
        llvm::Constant* FalseBit();

        /// <summary> Emit a null pointer constant. </summary>
        ///
        /// <param name="pointerType"> The llvm type of the pointer to return. </param>
        ///
        /// <returns> Pointer to an llvm::ConstantPointerNull that represents a null pointer of the given pointer type. </returns>
        llvm::ConstantPointerNull* NullPointer(llvm::PointerType* pointerType);

        /// <summary> Emit a value-preserving cast operation from one type to another. </summary>
        ///
        /// <typeparam name="InputType"> The input type. </typeparam>
        /// <typeparam name="OutputType"> The output type. </typeparam>
        /// <param name="pValue"> Pointer to the input value. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        template <typename InputType, typename OutputType>
        llvm::Value* CastValue(llvm::Value* pValue);

        /// <summary> Emit a bitwise ("reinterpret") cast operation from one type to another. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        llvm::Value* BitCast(llvm::Value* pValue, VariableType destinationType);

        /// <summary> Emit a bitwise ("reinterpret") cast operation from one type to another. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        llvm::Value* BitCast(llvm::Value* pValue, llvm::Type* destinationType);

        /// <summary> Emit a cast operation from one pointer type to another. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        llvm::Value* CastPointer(llvm::Value* pValue, llvm::Type* destinationType);

        /// <summary> Emit a cast operation from one pointer type to another. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        llvm::Value* CastPointer(llvm::Value* pValue, VariableType destinationType);

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

        /// <summary> Emit a cast operation from an int to a float. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        /// <param name="isSigned"> true if the value is signed. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        llvm::Value* CastIntToFloat(llvm::Value* pValue, VariableType destinationType, bool isSigned);

        /// <summary> Emit a cast operation from float to int. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        /// <param name="isSigned"> true if the integer value is signed. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        llvm::Value* CastFloatToInt(llvm::Value* pValue, VariableType destinationType, bool isSigned = true);

        /// <summary> Emit a cast operation from from an int to an int. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        /// <param name="isSigned"> true if the value is signed. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        llvm::Value* CastInt(llvm::Value* pValue, VariableType destinationType, bool isSigned);

        /// <summary> Emit a cast operation from from a boolean. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        llvm::Value* CastBool(llvm::Value* pValue);

        /// <summary> Emit a return VOID. </summary>
        ///
        /// <returns> Pointer to an llvm::ReturnInst that represents a return void. </returns>
        llvm::ReturnInst* ReturnVoid();

        /// <summary> Emit a return with the given data. </summary>
        ///
        /// <param name="pValue"> Pointer to the return value. </param>
        ///
        /// <returns> Pointer to an llvm::ReturnInst that represents the return value. </returns>
        llvm::ReturnInst* Return(llvm::Value* pValue);

        /// <summary> Emit a unary operation, with an optional name for the result. </summary>
        ///
        /// <param name="type"> The operator type. </param>
        /// <param name="pValue"> Pointer to an llvm::Value that represents the value in the operation. </param>
        /// <param name="variableName"> Name of the result. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the operation result. </returns>
        llvm::Value* UnaryOperation(const UnaryOperationType type, llvm::Value* pValue, const std::string& variableName = "");

        /// <summary> Emit a binary operation, with an optional name for the result. </summary>
        ///
        /// <param name="type"> The operator type. </param>
        /// <param name="pLeftValue"> Pointer to the left value in the operation. </param>
        /// <param name="pRightValue"> Pointer to the right value in the operation. </param>
        /// <param name="variableName"> Name of the result. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the operation result. </returns>
        llvm::Value* BinaryOperation(const TypedOperator type, llvm::Value* pLeftValue, llvm::Value* pRightValue, const std::string& variableName = "");

        /// <summary> Emit a binary comparison. </summary>
        ///
        /// <param name="type"> The comparison type. </param>
        /// <param name="pLeftValue"> Pointer to the left value in the comparison. </param>
        /// <param name="pRightValue"> Pointer to the right value in the comparison. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the comparison result. </returns>
        llvm::Value* Comparison(const TypedComparison type, llvm::Value* pLeftValue, llvm::Value* pRightValue);

        /// <summary> Emit code to check if pValue matches testValue. </summary>
        ///
        /// <param name="pValue"> Pointer to the value being compared to the test value. </param>
        /// <param name="testValue"> Boolean test value. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the comparison result. </returns>
        llvm::Value* Comparison(llvm::Value* pValue, bool testValue);

        /// <summary> Emit a comparison for whether the given value is True. </summary>
        ///
        /// <param name="pValue"> Pointer to the value being compared to true. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the comparison result. </returns>
        llvm::Value* IsTrue(llvm::Value* pValue) { return Comparison(pValue, true); } // STYLE discrepancy

        /// <summary> Emit a comparison for whether the given value is False. </summary>
        ///
        /// <param name="pValue"> Pointer to the value being compared to false. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the comparison result. </returns>
        llvm::Value* IsFalse(llvm::Value* pValue) { return Comparison(pValue, false); } // STYLE discrepancy

        /// <summary> Emit a select instruction. </summary>
        ///
        /// <param name="pCmp"> Pointer to the value use as a comparison. </param>
        /// <param name="pTrueValue"> Pointer to the value to return when the comparison is true. </param>
        /// <param name="pFalseValue"> Pointer to the value to return when the comparison is false. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the select result. </returns>
        llvm::Value* Select(llvm::Value* pCmp, llvm::Value* pTrueValue, llvm::Value* pFalseValue);

        /// <summary> Emit a declaration for an extern function with no arguments and no return value. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        llvm::Function* DeclareFunction(llvm::Module* pModule, const std::string& name);

        /// <summary> Emit a declaration for an extern function with no arguments. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function return type. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        llvm::Function* DeclareFunction(llvm::Module* pModule, const std::string& name, VariableType returnType);

        /// <summary> Emit a declaration for an extern function. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function return type. </param>
        /// <param name="arguments"> Function arguments. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        llvm::Function* DeclareFunction(llvm::Module* pModule, const std::string& name, VariableType returnType, const VariableTypeList& arguments);

        /// <summary> Emit a declaration for an extern function. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function return type. </param>
        /// <param name="pArguments"> Function arguments. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        llvm::Function* DeclareFunction(llvm::Module* pModule, const std::string& name, VariableType returnType, const NamedVariableTypeList& arguments);

        /// <summary> Emit a declaration for an extern function. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function type. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        llvm::Function* DeclareFunction(llvm::Module* pModule, const std::string& name, llvm::FunctionType* type);

        /// <summary> Emits the function declaration and arguments, when beginning a new function. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function return type. </param>
        /// <param name="linkage"> The linkage. </param>
        /// <param name="pArguments"> Function arguments. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        llvm::Function* Function(llvm::Module* pModule, const std::string& name, VariableType returnType, llvm::Function::LinkageTypes linkage, const VariableTypeList* pArguments);

        /// <summary> Emits the function declaration and arguments, when beginning a new function. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function return type. </param>
        /// <param name="linkage"> The linkage. </param>
        /// <param name="arguments"> Function arguments. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        llvm::Function* Function(llvm::Module* pModule, const std::string& name, VariableType returnType, llvm::Function::LinkageTypes linkage, const NamedVariableTypeList& arguments);

        /// <summary> Emits the function declaration and arguments, when beginning a new function. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function return type. </param>
        /// <param name="linkage"> The linkage. </param>
        /// <param name="arguments"> Function arguments. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        llvm::Function* Function(llvm::Module* pModule, const std::string& name, llvm::Type* returnType, llvm::Function::LinkageTypes linkage, const NamedVariableTypeList& arguments);

        /// <summary> Emits the function declaration and arguments, when beginning a new function. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function return type. </param>
        /// <param name="linkage"> The linkage. </param>
        /// <param name="argTypes"> The function argument types. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        llvm::Function* Function(llvm::Module* pModule, const std::string& name, llvm::Type* returnType, llvm::Function::LinkageTypes linkage, const std::vector<llvm::Type*>& argTypes);

        /// <summary> Emits the function declaration and arguments, when beginning a new function. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function return type. </param>
        /// <param name="linkage"> The linkage. </param>
        /// <param name="arguments"> Function arguments. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        llvm::Function* Function(llvm::Module* pModule, const std::string& name, llvm::Type* returnType, llvm::Function::LinkageTypes linkage, const NamedLLVMTypeList& arguments);

        /// <summary> Emit the beginning of a new code block in the given function. </summary>
        ///
        /// <param name="pFunction"> Pointer to the function to which the block is added. </param>
        /// <param name="label"> The block label. </param>
        ///
        /// <returns> Pointer to the new Block. </returns>
        llvm::BasicBlock* Block(llvm::Function* pFunction, const std::string& label);

        /// <summary>
        /// Emit the beginning of a new labeled code block in the given function. Add the block BEFORE the given one.
        /// Commonly used for loops and branch statements.
        /// </summary>
        ///
        /// <param name="pFunction"> Pointer to the function to which the block is added. </param>
        /// <param name="pBlock"> Pointer to the block that the new block comes before. </param>
        /// <param name="label"> The block label. </param>
        ///
        /// <returns> Pointer to the new Block. </returns>
        llvm::BasicBlock* BlockBefore(llvm::Function* pFunction, llvm::BasicBlock* pBlock, const std::string& label);

        /// <summary>
        /// Emit the beginning of a new labeled code block in the given function. Add the block BEFORE the given one.
        /// Commonly used for loops and branch statements.
        /// </summary>
        ///
        /// <param name="pFunction"> Pointer to the function to which the block is added. </param>
        /// <param name="pBlock"> Pointer to the block that the new block comes before. </param>
        /// <param name="pNewBlock"> Pointer to the new block. </param>
        ///
        /// <returns> Pointer to the new Block. </returns>
        llvm::BasicBlock* BlockBefore(llvm::Function* pFunction, llvm::BasicBlock* pBlock, llvm::BasicBlock* pNewBlock);

        /// <summary>
        /// Emit the beginning of a new labeled code block in the given function. Add the block AFTER the given one.
        /// Commonly used for loops and branch statements.
        /// </summary>
        ///
        /// <param name="pFunction"> Pointer to the function to which the block is added. </param>
        /// <param name="pBlock"> Pointer to the block that the new block comes after. </param>
        /// <param name="label"> The block label. </param>
        ///
        /// <returns> Pointer to the new Block. </returns>
        llvm::BasicBlock* BlockAfter(llvm::Function* pFunction, llvm::BasicBlock* pBlock, const std::string& label);

        /// <summary>
        /// Emit the beginning of a new labeled code block in the given function. Add the block AFTER the given one.
        /// Commonly used for loops and branch statements.
        /// </summary>
        ///
        /// <param name="pFunction"> Pointer to the function to which the block is added. </param>
        /// <param name="pBlock"> Pointer to the block that the new block comes after. </param>
        /// <param name="pNewBlock"> Pointer to the new block. </param>
        ///
        /// <returns> Pointer to the new Block. </returns>
        llvm::BasicBlock* BlockAfter(llvm::Function* pFunction, llvm::BasicBlock* pBlock, llvm::BasicBlock* pNewBlock);

        /// <summary>
        /// Emit the beginning of a new labeled block, which is not part of a function yet. You'll need to insert it into a function as
        /// needed.
        /// </summary>
        ///
        /// <param name="label"> The block label. </param>
        ///
        /// <returns> Pointer to the new Block. </returns>
        llvm::BasicBlock* Block(const std::string& label);

        /// <summary> Get the current code block, which all emitted instructions are being written to. </summary>
        ///
        /// <returns> Pointer to the current Block. </returns>
        llvm::BasicBlock* GetCurrentBlock() { return _irBuilder.GetInsertBlock(); }

        /// <summary> Get the current insert point for new instructions. </summary>
        ///
        /// <returns> Pointer to the current instruction. </returns>
        llvm::IRBuilder<>::InsertPoint GetCurrentInsertPoint() { return _irBuilder.saveIP(); }

        /// <summary> Sets the current code block, which all emitted instructions are being written to. </summary>
        ///
        /// <param name="pBlock"> Pointer to the block being set to be the current block. </param>
        void SetCurrentBlock(llvm::BasicBlock* pBlock);

        /// <summary> Sets the current insertion point. </summary>
        ///
        /// <param name="pos"> Pointer to the instruction to place new instructions after. </param>
        void SetCurrentInsertPoint(llvm::IRBuilder<>::InsertPoint pos);

        /// <summary> Sets the current insertion point. </summary>
        ///
        /// <param name="pos"> Pointer to the instruction to place new instructions after. </param>
        void SetCurrentInsertPoint(llvm::Instruction* pos);

        /// <summary> Emits a call to a given function. </summary>
        ///
        /// <param name="pFunction"> Pointer to the function being called. </param>
        ///
        /// <returns> Pointer to an llvm::CallInst. </returns>
        llvm::CallInst* Call(llvm::Function* pFunction);

        /// <summary> Emits a call to a function with one argument. </summary>
        ///
        /// <param name="pFunction"> Pointer to the function being called. </param>
        /// <param name="pArgument"> Pointer to the function argument. </param>
        ///
        /// <returns> Pointer to an llvm::CallInst. </returns>
        llvm::CallInst* Call(llvm::Function* pFunction, llvm::Value* pArgument);

        /// <summary> Emits a call to a function with a multiple arguments. </summary>
        ///
        /// <param name="pFunction"> Pointer to the function being called. </param>
        /// <param name="arguments"> The function arguments. </param>
        ///
        /// <returns> Pointer to an llvm::CallInst. </returns>
        llvm::CallInst* Call(llvm::Function* pFunction, const IRValueList& arguments);

        /// <summary>
        /// Locates an intrinsic function with a signature matching the description in arguments.
        /// </summary>
        ///
        /// <param name="pModule"> the module. </param>
        /// <param name="id"> The intrinsic id. </param>
        /// <param name="arguments"> The arguments. </param>
        ///
        /// <returns> Pointer to the intrinsic function. </returns>
        llvm::Function* GetIntrinsic(llvm::Module* pModule, llvm::Intrinsic::ID id, const VariableTypeList& arguments);

        /// <summary>
        /// Locates an intrinsic function with a signature matching the description in arguments.
        /// </summary>
        ///
        /// <param name="pModule"> the module. </param>
        /// <param name="id"> The intrinsic id. </param>
        /// <param name="arguments"> The arguments. </param>
        ///
        /// <returns> Pointer to the intrinsic function. </returns>
        llvm::Function* GetIntrinsic(llvm::Module* pModule, llvm::Intrinsic::ID id, const LLVMTypeList& arguments);

        /// <summary> Emit a Phi instruction. </summary>
        ///
        /// <param name="type"> The value type. </param>
        /// <param name="pLeftValue"> Pointer to the left value. </param>
        /// <param name="pLeftBlock"> Pointer to the left block. </param>
        /// <param name="pRightValue"> Pointer to the right value. </param>
        /// <param name="pRightBlock"> Pointer to the right block. </param>
        ///
        /// <returns> Pointer to the resulting phi node. </returns>
        llvm::PHINode* Phi(VariableType type, llvm::Value* pLeftValue, llvm::BasicBlock* pLeftBlock, llvm::Value* pRightValue, llvm::BasicBlock* pRightBlock);

        /// <summary> Emit a pointer to an entry in an array. </summary>
        ///
        /// <param name="pArray"> Pointer to the array. </param>
        /// <param name="pOffset"> Pointer to the offset. </param>
        /// <param name="name"> Name of the variable. </param>
        ///
        /// <returns> Pointer to a value that represents that entry in the array. </returns>
        llvm::Value* PointerOffset(llvm::Value* pArray, llvm::Value* pOffset, const std::string& name = "");

        /// <summary> Emits a pointer to the global array. </summary>
        ///
        /// <param name="pArray"> Pointer to the array. </param>
        ///
        /// <returns> Pointer to the first entry in the array. </returns>
        llvm::Value* Pointer(llvm::GlobalVariable* pArray);

        /// <summary> Emit a pointer to an entry in an array. </summary>
        ///
        /// <param name="pArray"> Pointer to the array. </param>
        /// <param name="pOffset"> Pointer to the offset. </param>
        ///
        /// <returns> Pointer to a value that represents that entry in the array. </returns>
        llvm::Value* PointerOffset(llvm::GlobalVariable* pArray, llvm::Value* pOffset);

        /// <summary> Emit a pointer to a FIELD in a STRUCT in a global array of Structs. </summary>
        ///
        /// <param name="pArray"> Pointer to the array. </param>
        /// <param name="pOffset"> Pointer to the offset. </param>
        /// <param name="pFieldOffset"> Pointer to the field offset in the struct. </param>
        ///
        /// <returns> Pointer to a value that represents that field. </returns>
        llvm::Value* PointerOffset(llvm::GlobalVariable* pArray, llvm::Value* pOffset, llvm::Value* pFieldOffset);

        /// <summary> Emit a pointer to a FIELD in a STRUCT in a local array of Structs. </summary>
        ///
        /// <param name="pArray"> Pointer to the array. </param>
        /// <param name="pOffset"> Pointer to the offset. </param>
        /// <param name="pFieldOffset"> Pointer to the field offset in the struct. </param>
        ///
        /// <returns> Pointer to a value that represents that field. </returns>
        llvm::Value* PointerOffset(llvm::AllocaInst* pArray, llvm::Value* pOffset, llvm::Value* pFieldOffset);

        /// <summary> Extract an element from a struct held by value. </summary>
        ///
        /// <param name="structValue"> Pointer to the struct value. </param>
        /// <param name="fieldIndex"> The index of the field to get. </param>
        ///
        /// <returns> A pointer the specified field in the struct. </returns>
        llvm::Value* ExtractStructField(llvm::Value* structValue, size_t fieldIndex);

        /// <summary> Get a pointer to a fields in a struct. </summary>
        ///
        /// <param name="structPtr"> Pointer to the struct pointer. </param>
        /// <param name="fieldIndex"> The index of the field to get. </param>
        ///
        /// <returns> A pointer the specified field in the struct. </returns>
        llvm::Value* GetStructFieldPointer(llvm::Value* structPtr, size_t fieldIndex);

        /// <summary> Emits an instruction to load a value referenced by a pointer into a register. </summary>
        ///
        /// <param name="pPointer"> Pointer to the adress being. </param>
        ///
        /// <returns> Pointer to the resulting llvm::LoadInst. </returns>
        llvm::LoadInst* Load(llvm::Value* pPointer);

        /// <summary> Emits an instruction to load the value referenced by a pointer into a named register. </summary>
        ///
        /// <param name="pPointer"> Pointer to the adress being loaded. </param>
        /// <param name="name"> The register name. </param>
        ///
        /// <returns> Pointer to the resulting llvm::LoadInst. </returns>
        llvm::LoadInst* Load(llvm::Value* pPointer, const std::string& name);

        /// <summary> Emits an instruction to store a value into a given address. </summary>
        ///
        /// <param name="pPointer"> Pointer to the adress where the value is being stored. </param>
        /// <param name="pValue"> Pointer to the value being stored. </param>
        ///
        /// <returns> Pointer to the resulting llvm::StoreInst. </returns>
        llvm::StoreInst* Store(llvm::Value* pPointer, llvm::Value* pValue);

        /// <summary> Emits instruction to create a stack variable. </summary>
        ///
        /// <param name="type"> The variable type. </param>
        ///
        /// <returns> Pointer to the resulting llvm::AllocaInst. </returns>
        llvm::AllocaInst* StackAllocate(VariableType type);

        /// <summary> Emits instruction to create a stack variable. </summary>
        ///
        /// <param name="type"> The variable type. </param>
        ///
        /// <returns> Pointer to the resulting llvm::AllocaInst. </returns>
        llvm::AllocaInst* StackAllocate(llvm::Type* type);

        /// <summary> Emits instruction to create a named stack variable. </summary>
        ///
        /// <param name="type"> The variable type. </param>
        /// <param name="name"> The variable name. </param>
        ///
        /// <returns> Pointer to the resulting llvm::AllocaInst. </returns>
        llvm::AllocaInst* StackAllocate(VariableType type, const std::string& name);

        /// <summary> Emits an instruction to create a named stack variable of some runtime type. </summary>
        ///
        /// <param name="pType"> The variable type. </param>
        /// <param name="name"> The variable name. </param>
        ///
        /// <returns> Pointer to the resulting llvm::AllocaInst. </returns>
        llvm::AllocaInst* StackAllocate(llvm::Type* pType, const std::string& name);

        /// <summary> Emits a stack alloc instruction for an array of primitive types. </summary>
        ///
        /// <param name="type"> The array entry type. </param>
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> Pointer to a llvm::AllocaInst that represents the allocated array. </returns>
        llvm::AllocaInst* StackAllocate(VariableType type, size_t size);

        /// <summary> Emits a stack alloc instruction for an array of primitive types. </summary>
        ///
        /// <param name="type"> The array entry type. </param>
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> Pointer to a llvm::AllocaInst that represents the allocated array. </returns>
        llvm::AllocaInst* StackAllocate(llvm::Type* type, size_t size);

        /// <summary> Emit a conditional branch. </summary>
        ///
        /// <param name="pConditionValue"> Pointer to the condition value. </param>
        /// <param name="pThenBlock"> Pointer to the THEN block. </param>
        /// <param name="pElseBlock"> Pointer to the ELSE block. </param>
        ///
        /// <returns> Pointer to a llvm::BranchInst that represents the branch. </returns>
        llvm::BranchInst* Branch(llvm::Value* pConditionValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock);

        /// <summary> Emit an UN-conditional branch - i.e. always jump to the destination block. </summary>
        ///
        /// <param name="pDestination"> Pointer to the destination block. </param>
        ///
        /// <returns> Pointer to a llvm::BranchInst that represents the branch. </returns>
        llvm::BranchInst* Branch(llvm::BasicBlock* pDestination);

        /// <summary> Emits a declaration of a new Struct with the given fields. </summary>
        ///
        /// <param name="name"> The struct name. </param>
        /// <param name="fields"> The list of struct fields. </param>
        ///
        /// <returns> Pointer to a llvm::StructType that represents the declared struct. </returns>
        llvm::StructType* DeclareStruct(const std::string& name, const VariableTypeList& fields);

        /// <summary> Emits a declaration of a new Struct with the given fields. </summary>
        ///
        /// <param name="name"> The struct name. </param>
        /// <param name="fields"> The list of struct fields. </param>
        ///
        /// <returns> Pointer to a llvm::StructType that represents the declared struct. </returns>
        llvm::StructType* DeclareStruct(const std::string& name, const LLVMTypeList& fields);

        /// <summary> Emit a module scoped struct with the given fields. </summary>
        ///
        /// <param name="name"> The struct name. </param>
        /// <param name="fields"> The struct fields. </param>
        ///
        /// <returns> Pointer to the llvm::StructType that represents the emitted structure. </returns>
        llvm::StructType* DeclareStruct(const std::string& name, const NamedVariableTypeList& args);

        /// <summary> Gets a type definition for an anonymous struct with the given fields. </summary>
        ///
        /// <param name="fields"> The struct fields. </param>
        /// <param name="packed"> If `true`, the fields will be packed together without any padding. </param>
        ///
        /// <returns> Pointer to the llvm::StructType that represents the emitted structure. </returns>
        llvm::StructType* GetAnonymousStructType(const LLVMTypeList& fields, bool packed = false);

        /// <summary> Gets a type definition for a struct with the given name.</summary>
        ///
        /// <param name="name"> The struct name. </param>
        ///
        /// <returns> Pointer to a llvm::StructType that represents the declared struct. </returns>
        llvm::StructType* GetStruct(const std::string& name);

        /// <summary> Emits a new module with a given name. </summary>
        ///
        /// <param name="name"> The module name. </param>
        ///
        /// <returns> A unique pointer to the new module. </returns>
        std::unique_ptr<llvm::Module> CreateModule(const std::string& name);

        /// <summary> Emits a memmove instruction. </summary>
        ///
        /// <param name="pSource"> Pointer to the value that holds the source address. </param>
        /// <param name="pDestination"> Pointer to the value that holds the destination address. </param>
        /// <param name="pCountBytes"> [in,out] Pointer to the value that holds the byte count. </param>
        ///
        /// <returns> Pointer to the resulting llvm::CallInst. </returns>
        llvm::CallInst* MemoryMove(llvm::Value* pSource, llvm::Value* pDestination, llvm::Value* pCountBytes);

        /// <summary> Emits a memcpy instruction. </summary>
        ///
        /// <param name="pSource"> Pointer to the value that holds the source address. </param>
        /// <param name="pDestination"> Pointer to the value that holds the destination address. </param>
        /// <param name="pCountBytes"> [in,out] Pointer to the value that holds the byte count. </param>
        ///
        /// <returns> Pointer to the resulting llvm::CallInst. </returns>
        llvm::CallInst* MemoryCopy(llvm::Value* pSource, llvm::Value* pDestination, llvm::Value* pCountBytes);

        /// <summary> Emits a memset instruction. </summary>
        ///
        /// <param name="pSource"> Pointer to the value that holds the source address. </param>
        /// <param name="value"> The value to set. </param>
        /// <param name="pCountBytes"> [in,out] Pointer to the value that holds the byte count. </param>
        ///
        /// <returns> Pointer to the resulting llvm::CallInst. </returns>
        llvm::CallInst* MemorySet(llvm::Value* pDestination, llvm::Value* value, llvm::Value* size);

        /// <summary> Gets the underlying LLVMContext. </summary>
        ///
        /// <returns> Reference to the underlying llvm::LLVMContext. </returns>
        llvm::LLVMContext& GetContext() { return _llvmContext; }

        /// <summary> Gets the underlying LLVM IRBuilder used for emitting code. </summary>
        ///
        /// <returns> Reference to the underlying LLVM IRBuilder. </returns>
        llvm::IRBuilder<>& GetIRBuilder() { return _irBuilder; }

        /// <summary> Converts a list of ELL types to LLVM Type* values. </summary>
        ///
        /// <param name="types"> The VariableTypeList of ELL types. </param>
        /// <returns> A LLVMTypeList with the corresponding LLVM types. </returns>
        LLVMTypeList GetLLVMTypes(const VariableTypeList& types);

    private:
        llvm::Type* GetVariableType(VariableType type);
        int SizeOf(VariableType type);
        llvm::Constant* Integer(VariableType type, const size_t value);

        std::vector<llvm::Type*> BindArgumentTypes(const NamedVariableTypeList& arguments);
        std::vector<llvm::Type*> BindArgumentTypes(const NamedLLVMTypeList& arguments);

        template <typename ListType>
        void BindArgumentNames(llvm::Function* pFunction, const ListType& arguments);

        llvm::Function* CreateFunction(llvm::Module* pModule, const std::string& name, llvm::Function::LinkageTypes linkage, llvm::FunctionType* pFunctionType);
        llvm::Value* Zero();

        llvm::LLVMContext& _llvmContext; // LLVM global context
        llvm::IRBuilder<> _irBuilder; // IRBuilder API
        IRVariableTable _stringLiterals; // String literals are emitted as constants. We have to track them ourselves to prevent dupes.
        llvm::Value* _pZeroLiteral = nullptr;
        std::unordered_map<std::string, llvm::StructType*> _structs;
    };
}
}

#include "../tcc/IREmitter.tcc"
