////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IREmitter.h (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "EmitterTypes.h"
#include "FunctionDeclaration.h"
#include "LLVMUtilities.h"
#include "SymbolTable.h"

#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constant.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <unordered_map>
#include <vector>

namespace ell
{
namespace emitters
{
    // forward declaration
    class IRModuleEmitter;

    /// <summary> A list of LLVM IR Value* </summary>
    using IRValueList = std::vector<LLVMValue>;

    /// <summary> Symbol Table that maps symbol Names to emitted IR Value* </summary>
    using IRValueTable = SymbolTable<LLVMValue>;

    /// <summary> Symbol Table that maps type Names to emitted IR Type* </summary>
    using IRTypeTable = SymbolTable<LLVMType>;

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
        IREmitter(const IREmitter&) = delete;
        IREmitter(IREmitter&&) = default;
        IREmitter& operator=(const IREmitter&) = delete;
        IREmitter& operator=(IREmitter&&) = default;
        ~IREmitter() = default;

        /// <summary> Get the LLVM Type information for a VariableType. </summary>
        ///
        /// <param name="type"> The VariableType. </param>
        ///
        /// <returns> Pointer to an llvm::Type object that corresponds to the given VariableType. </returns>
        LLVMType Type(VariableType type) const;

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
        llvm::PointerType* PointerType(LLVMType type);

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
        /// Get the LLVM Type Information for a 2D array of VariableType, with the given number of rows and columns.
        /// </summary>
        ///
        /// <param name="type"> The entry type. </param>
        /// <param name="rows"> The number of rows in the array. </param>
        /// <param name="columns"> The number of columns in the array. </param>
        ///
        /// <returns> Pointer to an llvm::ArrayType object that represents the specified array. </returns>
        llvm::ArrayType* ArrayType(VariableType type, size_t rows, size_t columns);

        /// <summary>
        /// Get the LLVM Type Information for an ARRAY of VariableType, with a given size.
        /// </summary>
        ///
        /// <param name="type"> The entry type. </param>
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> Pointer to an llvm::ArrayType object that represents the specified array. </returns>
        llvm::ArrayType* ArrayType(LLVMType type, size_t size);

        /// <summary>
        /// Get the LLVM Type Information for a 2D array of VariableType, with the given number of rows and columns.
        /// </summary>
        ///
        /// <param name="type"> The entry type. </param>
        /// <param name="rows"> The number of rows in the array. </param>
        /// <param name="columns"> The number of columns in the array. </param>
        ///
        /// <returns> Pointer to an llvm::ArrayType object that represents the specified array. </returns>
        llvm::ArrayType* ArrayType(LLVMType type, size_t rows, size_t columns);

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
        llvm::VectorType* VectorType(LLVMType type, size_t size);

        /// <summary> Emit a boolean literal. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents the boolean literal. </returns>
        llvm::Constant* Literal(const bool value);

        /// <summary> Emit an unsigned byte literal. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents the unsigned byte literal. </returns>
        llvm::Constant* Literal(const uint8_t value);

        /// <summary> Emit a signed byte literal. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents the signed byte literal. </returns>
        llvm::Constant* Literal(const int8_t value);

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
        LLVMValue Literal(const char* pValue);

        /// <summary> Emit a string literal. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the string literal. </returns>
        LLVMValue Literal(const std::string& value);

        /// <summary> Emit a named string literal. </summary>
        ///
        /// <param name="name"> The literal name. </param>
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the string literal. </returns>
        LLVMValue Literal(const std::string& name, const std::string& value);

        /// <summary> Emit a literal array of unsigned bytes. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents an array of unsigned bytes. </returns>
        llvm::Constant* Literal(const std::vector<uint8_t>& value);

        /// <summary> Emit a literal array of signed bytes. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents an array of signed bytes. </returns>
        llvm::Constant* Literal(const std::vector<int8_t>& value);

        /// <summary> Emit a literal array of char. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents an array of char. </returns>
        llvm::Constant* Literal(const std::vector<char>& value);

        /// <summary> Emit a literal array of Int16. </summary>
        ///
        /// <param name="value"> The literal value. </param>
        ///
        /// <returns> Pointer to an llvm::Constant that represents an array of Int16. </returns>
        llvm::Constant* Literal(const std::vector<int16_t>& value);

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
        /// <returns> Pointer to an llvm::Constant that represents a pointer. </returns>
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
        llvm::Constant* Zero(LLVMType type);

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

        /// <summary> Emit a value-preserving cast operation to another type. </summary>
        ///
        /// <typeparam name="OutputType"> The output type. </typeparam>
        /// <param name="pValue"> Pointer to the input value. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        template <typename OutputType>
        LLVMValue CastValue(LLVMValue pValue);

        /// <summary> Emit a value-preserving cast operation from one type to another. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastValue(LLVMValue pValue, VariableType destinationType);

        /// <summary> Emit a value-preserving cast operation from one type to another. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastValue(LLVMValue pValue, LLVMType destinationType);

        /// <summary> Emit a value-preserving cast operation from an unsigned integral type to another type. </summary>
        ///
        /// <typeparam name="OutputType"> The output type. </typeparam>
        /// <param name="pValue"> Pointer to the input value. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        template <typename OutputType>
        LLVMValue CastUnsignedValue(LLVMValue pValue);

        /// <summary> Emit a value-preserving cast operation from an unsigned integral type to another type. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastUnsignedValue(LLVMValue pValue, VariableType destinationType);

        /// <summary> Emit a value-preserving cast operation from an unsigned integral type to another type. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastUnsignedValue(LLVMValue pValue, LLVMType destinationType);

        /// <summary> Emit a bitwise ("reinterpret") cast operation from one type to another. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue BitCast(LLVMValue pValue, VariableType destinationType);

        /// <summary> Emit a bitwise ("reinterpret") cast operation from one type to another. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue BitCast(LLVMValue pValue, LLVMType destinationType);

        /// <summary> Emit a cast operation from one pointer type to another. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastPointer(LLVMValue pValue, VariableType destinationType);

        /// <summary> Emit a cast operation from one pointer type to another. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastPointer(LLVMValue pValue, LLVMType destinationType);

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
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        LLVMValue CastPointerToInt(LLVMValue pValue, VariableType destinationType);

        /// <summary> Emit a cast from a pointer to an integer type. </summary>
        ///
        /// <param name="pValue"> Input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the casted value. </returns>
        LLVMValue CastPointerToInt(LLVMValue pValue, LLVMType destinationType);

        /// <summary> Emit a cast operation from an int to a float. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        /// <param name="isSigned"> true if the value is signed. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastIntToFloat(LLVMValue pValue, VariableType destinationType, bool isSigned);

        /// <summary> Emit a cast operation from an int to a float. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        /// <param name="isSigned"> true if the value is signed. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastIntToFloat(LLVMValue pValue, LLVMType destinationType, bool isSigned);

        /// <summary> Emit a cast operation from float to int. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        /// <param name="isSigned"> true if the integer value is signed. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastFloatToInt(LLVMValue pValue, VariableType destinationType, bool isSigned);

        /// <summary> Emit a cast operation from float to int. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        /// <param name="isSigned"> true if the integer value is signed. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastFloatToInt(LLVMValue pValue, LLVMType destinationType, bool isSigned);

        /// <summary> Emit a cast operation from an int to an int. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        /// <param name="isSigned"> true if the value is signed. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastInt(LLVMValue pValue, VariableType destinationType, bool isSigned);

        /// <summary> Emit a cast operation from an int to an int. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        /// <param name="isSigned"> true if the value is signed. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastInt(LLVMValue pValue, LLVMType destinationType, bool isSigned);

        /// <summary> Emit a cast operation from a float to a float. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastFloat(LLVMValue pValue, VariableType destinationType);

        /// <summary> Emit a cast operation from a float to a float. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        /// <param name="destinationType"> Output type. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastFloat(LLVMValue pValue, LLVMType destinationType);

        /// <summary> Emit a cast operation from an arbitrary value to a boolean bit value. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastToConditionalBool(LLVMValue pValue);

        /// <summary> Emit a cast operation to a boolean-valued byte. </summary>
        ///
        /// <param name="pValue"> Pointer to the input value. </param>
        ///
        /// <returns> Pointer to the output value. </returns>
        LLVMValue CastBoolToByte(LLVMValue pValue);

        /// <summary> Emit a return VOID. </summary>
        ///
        /// <returns> Pointer to an llvm::ReturnInst that represents a return void. </returns>
        llvm::ReturnInst* ReturnVoid();

        /// <summary> Emit a return with the given data. </summary>
        ///
        /// <param name="pValue"> Pointer to the return value. </param>
        ///
        /// <returns> Pointer to an llvm::ReturnInst that represents the return value. </returns>
        llvm::ReturnInst* Return(LLVMValue pValue);

        /// <summary> Emit a unary operation, with an optional name for the result. </summary>
        ///
        /// <param name="type"> The unary operator type. </param>
        /// <param name="pValue"> Pointer to an llvm::Value that represents the value in the operation. </param>
        /// <param name="variableName"> Name of the result. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the operation result. </returns>
        LLVMValue UnaryOperator(const UnaryOperatorType type, LLVMValue pValue, const std::string& variableName = "");

        /// <summary> Emit a binary operation, with an optional name for the result. </summary>
        ///
        /// <param name="type"> The operator type. </param>
        /// <param name="pLeftValue"> Pointer to the left value in the operation. </param>
        /// <param name="pRightValue"> Pointer to the right value in the operation. </param>
        /// <param name="variableName"> Name of the result. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the operation result. </returns>
        LLVMValue BinaryOperation(const TypedOperator type, LLVMValue pLeftValue, LLVMValue pRightValue, const std::string& variableName = "");

        /// <summary> Emit a binary comparison. </summary>
        ///
        /// <param name="type"> The comparison type. </param>
        /// <param name="pLeftValue"> Pointer to the left value in the comparison. </param>
        /// <param name="pRightValue"> Pointer to the right value in the comparison. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the comparison result. </returns>
        LLVMValue Comparison(const TypedComparison type, LLVMValue pLeftValue, LLVMValue pRightValue);

        /// <summary> Emit code to check if pValue matches testValue. </summary>
        ///
        /// <param name="pValue"> Pointer to the value being compared to the test value. </param>
        /// <param name="testValue"> Boolean test value. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the comparison result. </returns>
        LLVMValue Comparison(LLVMValue pValue, bool testValue);

        /// <summary> Emit a comparison for whether the given value is True. </summary>
        ///
        /// <param name="pValue"> Pointer to the value being compared to true. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the comparison result. </returns>
        LLVMValue IsTrue(LLVMValue pValue);

        /// <summary> Emit a comparison for whether the given value is False. </summary>
        ///
        /// <param name="pValue"> Pointer to the value being compared to false. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the comparison result. </returns>
        LLVMValue IsFalse(LLVMValue pValue);

        /// <summary> Emit a select instruction. </summary>
        ///
        /// <param name="pCmp"> Pointer to the value use as a comparison. </param>
        /// <param name="pTrueValue"> Pointer to the value to return when the comparison is true. </param>
        /// <param name="pFalseValue"> Pointer to the value to return when the comparison is false. </param>
        ///
        /// <returns> Pointer to an llvm::Value that represents the select result. </returns>
        LLVMValue Select(LLVMValue pCmp, LLVMValue pTrueValue, LLVMValue pFalseValue);

        /// <summary> Emit a declaration for an extern function with no arguments and no return value. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        LLVMFunction DeclareFunction(llvm::Module* pModule, const std::string& name);

        /// <summary> Emit a declaration for an extern function with no arguments. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function return type. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        LLVMFunction DeclareFunction(llvm::Module* pModule, const std::string& name, VariableType returnType);

        /// <summary> Emit a declaration for an extern function. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function return type. </param>
        /// <param name="arguments"> Function arguments. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        LLVMFunction DeclareFunction(llvm::Module* pModule, const std::string& name, VariableType returnType, const VariableTypeList& arguments);

        /// <summary> Emit a declaration for an extern function. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function return type. </param>
        /// <param name="pArguments"> Function arguments. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        LLVMFunction DeclareFunction(llvm::Module* pModule, const std::string& name, VariableType returnType, const NamedVariableTypeList& arguments);

        /// <summary> Emit a declaration for an extern function. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function type. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        LLVMFunction DeclareFunction(llvm::Module* pModule, const std::string& name, llvm::FunctionType* type);

        /// <summary> Emits the function declaration and arguments, when beginning a new function. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function return type. </param>
        /// <param name="linkage"> The linkage. </param>
        /// <param name="pArguments"> Function arguments. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        LLVMFunction Function(llvm::Module* pModule, const std::string& name, VariableType returnType, llvm::Function::LinkageTypes linkage, const VariableTypeList* pArguments);

        /// <summary> Emits the function declaration and arguments, when beginning a new function. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function return type. </param>
        /// <param name="linkage"> The linkage. </param>
        /// <param name="arguments"> Function arguments. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        LLVMFunction Function(llvm::Module* pModule, const std::string& name, VariableType returnType, llvm::Function::LinkageTypes linkage, const NamedVariableTypeList& arguments);

        /// <summary> Emits the function declaration and arguments, when beginning a new function. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function return type. </param>
        /// <param name="linkage"> The linkage. </param>
        /// <param name="arguments"> Function arguments. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        LLVMFunction Function(llvm::Module* pModule, const std::string& name, LLVMType returnType, llvm::Function::LinkageTypes linkage, const NamedVariableTypeList& arguments);

        /// <summary> Emits the function declaration and arguments, when beginning a new function. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function return type. </param>
        /// <param name="linkage"> The linkage. </param>
        /// <param name="argTypes"> The function argument types. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        LLVMFunction Function(llvm::Module* pModule, const std::string& name, LLVMType returnType, llvm::Function::LinkageTypes linkage, const std::vector<LLVMType>& argTypes);

        /// <summary> Emits the function declaration and arguments, when beginning a new function. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function return type. </param>
        /// <param name="linkage"> The linkage. </param>
        /// <param name="arguments"> Function arguments. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        LLVMFunction Function(llvm::Module* pModule, const std::string& name, LLVMType returnType, llvm::Function::LinkageTypes linkage, const NamedLLVMTypeList& arguments);

        /// <summary> Emits the function declaration and arguments, when beginning a new function. </summary>
        ///
        /// <param name="pModule"> The module in which the function is declared. </param>
        /// <param name="name"> The function name. </param>
        /// <param name="returnType"> Function return type. </param>
        /// <param name="linkage"> The linkage. </param>
        /// <param name="arguments"> Function arguments. </param>
        ///
        /// <returns> Pointer to the declared function. </returns>
        LLVMFunction Function(llvm::Module* pModule, const std::string& name, LLVMType returnType, llvm::Function::LinkageTypes linkage, const FunctionArgumentList& arguments);

        /// <summary> Emit the beginning of a new code block in the given function. </summary>
        ///
        /// <param name="pFunction"> Pointer to the function to which the block is added. </param>
        /// <param name="label"> The block label. </param>
        ///
        /// <returns> Pointer to the new Block. </returns>
        llvm::BasicBlock* Block(LLVMFunction pFunction, const std::string& label);

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
        llvm::BasicBlock* BlockBefore(LLVMFunction pFunction, llvm::BasicBlock* pBlock, const std::string& label);

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
        llvm::BasicBlock* BlockBefore(LLVMFunction pFunction, llvm::BasicBlock* pBlock, llvm::BasicBlock* pNewBlock);

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
        llvm::BasicBlock* BlockAfter(LLVMFunction pFunction, llvm::BasicBlock* pBlock, const std::string& label);

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
        llvm::BasicBlock* BlockAfter(LLVMFunction pFunction, llvm::BasicBlock* pBlock, llvm::BasicBlock* pNewBlock);

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
        llvm::CallInst* Call(LLVMFunction pFunction);

        /// <summary> Emits a call to a function with one argument. </summary>
        ///
        /// <param name="pFunction"> Pointer to the function being called. </param>
        /// <param name="pArgument"> Pointer to the function argument. </param>
        ///
        /// <returns> Pointer to an llvm::CallInst. </returns>
        llvm::CallInst* Call(LLVMFunction pFunction, LLVMValue pArgument);

        /// <summary> Emits a call to a function with a multiple arguments. </summary>
        ///
        /// <param name="pFunction"> Pointer to the function being called. </param>
        /// <param name="arguments"> The function arguments. </param>
        ///
        /// <returns> Pointer to an llvm::CallInst. </returns>
        llvm::CallInst* Call(LLVMFunction pFunction, const IRValueList& arguments);

        /// <summary>
        /// Locates an intrinsic function with a signature matching the description in arguments.
        /// </summary>
        ///
        /// <param name="pModule"> the module. </param>
        /// <param name="id"> The intrinsic id. </param>
        /// <param name="arguments"> The arguments. </param>
        ///
        /// <returns> Pointer to the intrinsic function. </returns>
        LLVMFunction GetIntrinsic(llvm::Module* pModule, llvm::Intrinsic::ID id, const VariableTypeList& arguments);

        /// <summary>
        /// Locates an intrinsic function with a signature matching the description in arguments.
        /// </summary>
        ///
        /// <param name="pModule"> the module. </param>
        /// <param name="id"> The intrinsic id. </param>
        /// <param name="arguments"> The arguments. </param>
        ///
        /// <returns> Pointer to the intrinsic function. </returns>
        LLVMFunction GetIntrinsic(llvm::Module* pModule, llvm::Intrinsic::ID id, const LLVMTypeList& arguments);

        /// <summary> Emit a Phi instruction. </summary>
        ///
        /// <param name="type"> The value type. </param>
        /// <param name="pLeftValue"> Pointer to the left value. </param>
        /// <param name="pLeftBlock"> Pointer to the left block. </param>
        /// <param name="pRightValue"> Pointer to the right value. </param>
        /// <param name="pRightBlock"> Pointer to the right block. </param>
        ///
        /// <returns> Pointer to the resulting phi node. </returns>
        llvm::PHINode* Phi(VariableType type, LLVMValue pLeftValue, llvm::BasicBlock* pLeftBlock, LLVMValue pRightValue, llvm::BasicBlock* pRightBlock);

        /// <summary> Emit a pointer to an entry in an array. </summary>
        ///
        /// <param name="pArray"> Pointer to the array. </param>
        /// <param name="pOffset"> Pointer to the offset. </param>
        /// <param name="name"> Name of the variable. </param>
        ///
        /// <returns> Pointer to a value that represents that entry in the array. </returns>
        LLVMValue PointerOffset(LLVMValue pArray, LLVMValue pOffset, const std::string& name = "");

        /// <summary> Emits a dereference of a pointer to a global variable. </summary>
        ///
        /// <param name="pArray"> Pointer to the array. </param>
        ///
        /// <returns> Pointer to the first entry in the array. </returns>
        LLVMValue DereferenceGlobalPointer(LLVMValue pArray);

        /// <summary> Emit a pointer to an entry in an array. </summary>
        ///
        /// <param name="pArray"> Pointer to the array. </param>
        /// <param name="pOffset"> Pointer to the offset. </param>
        ///
        /// <returns> Pointer to a value that represents that entry in the array. </returns>
        LLVMValue PointerOffset(llvm::GlobalVariable* pArray, LLVMValue pOffset);

        /// <summary> Emit a pointer to a FIELD in a STRUCT in a global array of Structs. </summary>
        ///
        /// <param name="pArray"> Pointer to the array. </param>
        /// <param name="pOffset"> Pointer to the offset. </param>
        /// <param name="pFieldOffset"> Pointer to the field offset in the struct. </param>
        ///
        /// <returns> Pointer to a value that represents that field. </returns>
        LLVMValue PointerOffset(llvm::GlobalVariable* pArray, LLVMValue pOffset, LLVMValue pFieldOffset);

        /// <summary> Emit a pointer to a FIELD in a STRUCT in a local array of Structs. </summary>
        ///
        /// <param name="pArray"> Pointer to the array. </param>
        /// <param name="pOffset"> Pointer to the offset. </param>
        /// <param name="pFieldOffset"> Pointer to the field offset in the struct. </param>
        ///
        /// <returns> Pointer to a value that represents that field. </returns>
        LLVMValue PointerOffset(llvm::AllocaInst* pArray, LLVMValue pOffset, LLVMValue pFieldOffset);

        /// <summary> Extract an element from a struct held by value. </summary>
        ///
        /// <param name="structValue"> Pointer to the struct value. </param>
        /// <param name="fieldIndex"> The index of the field to get. </param>
        ///
        /// <returns> A pointer the specified field in the struct. </returns>
        LLVMValue ExtractStructField(LLVMValue structValue, size_t fieldIndex);

        /// <summary> Get a pointer to a fields in a struct. </summary>
        ///
        /// <param name="structPtr"> Pointer to the struct pointer. </param>
        /// <param name="fieldIndex"> The index of the field to get. </param>
        ///
        /// <returns> A pointer the specified field in the struct. </returns>
        LLVMValue GetStructFieldPointer(LLVMValue structPtr, size_t fieldIndex);

        /// <summary> Emits an instruction to load a value referenced by a pointer into a register. </summary>
        ///
        /// <param name="pPointer"> Pointer to the adress being. </param>
        ///
        /// <returns> Pointer to the resulting llvm::LoadInst. </returns>
        llvm::LoadInst* Load(LLVMValue pPointer);

        /// <summary> Emits an instruction to load the value referenced by a pointer into a named register. </summary>
        ///
        /// <param name="pPointer"> Pointer to the adress being loaded. </param>
        /// <param name="name"> The register name. </param>
        ///
        /// <returns> Pointer to the resulting llvm::LoadInst. </returns>
        llvm::LoadInst* Load(LLVMValue pPointer, const std::string& name);

        /// <summary> Emits an instruction to store a value into a given address. </summary>
        ///
        /// <param name="pPointer"> Pointer to the adress where the value is being stored. </param>
        /// <param name="pValue"> Pointer to the value being stored. </param>
        ///
        /// <returns> Pointer to the resulting llvm::StoreInst. </returns>
        llvm::StoreInst* Store(LLVMValue pPointer, LLVMValue pValue);

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
        llvm::AllocaInst* StackAllocate(LLVMType type);

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
        llvm::AllocaInst* StackAllocate(LLVMType pType, const std::string& name);

        /// <summary> Emits a stack alloc instruction for an array of primitive types. </summary>
        ///
        /// <param name="type"> The array entry type. </param>
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> Pointer to a llvm::AllocaInst that represents the allocated array. </returns>
        llvm::AllocaInst* StackAllocate(VariableType type, size_t size);

        /// <summary> Emits a stack alloc instruction for a 2D array of primitive types. </summary>
        ///
        /// <param name="type"> The array entry type. </param>
        /// <param name="rows"> The number of rows in the array. </param>
        /// <param name="columns"> The number of columns in the array. </param>
        ///
        /// <returns> Pointer to a llvm::AllocaInst that represents the allocated array. </returns>
        llvm::AllocaInst* StackAllocate(VariableType type, size_t rows, size_t columns);

        /// <summary> Emits a stack alloc instruction for an array of primitive types. </summary>
        ///
        /// <param name="type"> The array entry type. </param>
        /// <param name="size"> The array size. </param>
        ///
        /// <returns> Pointer to a llvm::AllocaInst that represents the allocated array. </returns>
        llvm::AllocaInst* StackAllocate(LLVMType type, size_t size);

        /// <summary> Emits a stack alloc instruction for a 2D array of primitive types. </summary>
        ///
        /// <param name="type"> The array entry type. </param>
        /// <param name="rows"> The number of rows in the array. </param>
        /// <param name="columns"> The number of columns in the array. </param>
        ///
        /// <returns> Pointer to a llvm::AllocaInst that represents the allocated array. </returns>
        llvm::AllocaInst* StackAllocate(LLVMType type, size_t rows, size_t columns);

        /// <summary> Emit a conditional branch. </summary>
        ///
        /// <param name="pConditionValue"> Pointer to the condition value. </param>
        /// <param name="pThenBlock"> Pointer to the THEN block. </param>
        /// <param name="pElseBlock"> Pointer to the ELSE block. </param>
        ///
        /// <returns> Pointer to a llvm::BranchInst that represents the branch. </returns>
        llvm::BranchInst* Branch(LLVMValue pConditionValue, llvm::BasicBlock* pThenBlock, llvm::BasicBlock* pElseBlock);

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

        /// <summary> Emits a memmove instruction. </summary>
        ///
        /// <param name="pSource"> Pointer to the value that holds the source address. </param>
        /// <param name="pDestination"> Pointer to the value that holds the destination address. </param>
        /// <param name="pCountBytes"> [in,out] Pointer to the value that holds the byte count. </param>
        ///
        /// <returns> Pointer to the resulting llvm::CallInst. </returns>
        llvm::CallInst* MemoryMove(LLVMValue pSource, LLVMValue pDestination, LLVMValue pCountBytes);

        /// <summary> Emits a memcpy instruction. </summary>
        ///
        /// <param name="pSource"> Pointer to the value that holds the source address. </param>
        /// <param name="pDestination"> Pointer to the value that holds the destination address. </param>
        /// <param name="pCountBytes"> [in,out] Pointer to the value that holds the byte count. </param>
        ///
        /// <returns> Pointer to the resulting llvm::CallInst. </returns>
        llvm::CallInst* MemoryCopy(LLVMValue pSource, LLVMValue pDestination, LLVMValue pCountBytes);

        /// <summary> Emits a memset instruction. </summary>
        ///
        /// <param name="pSource"> Pointer to the value that holds the source address. </param>
        /// <param name="value"> The value to set. </param>
        /// <param name="pCountBytes"> [in,out] Pointer to the value that holds the byte count. </param>
        ///
        /// <returns> Pointer to the resulting llvm::CallInst. </returns>
        llvm::CallInst* MemorySet(LLVMValue pDestination, LLVMValue value, LLVMValue size);

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

        /// <summary> Returns the offset in bytes between successive objects of the specified type, including alignment padding. </summary>
        ///
        /// <typeparam name="ValueType"> The type </typeparam>
        /// <returns> The size of the type in bytes, including alignment padding. </returns>
        template <typename ValueType>
        uint64_t SizeOf() const;

        /// <summary> Returns the offset in bytes between successive objects of the specified type, including alignment padding. </summary>
        ///
        /// <param name="type"> The type </param>
        /// <returns> The size of the type in bytes, including alignment padding. </returns>
        uint64_t SizeOf(LLVMType type) const;

        /// <summary> Returns the offset in bytes between successive objects of the specified type, including alignment padding. </summary>
        ///
        /// <param name="type"> The type </param>
        /// <returns> The size of the type in bytes, including alignment padding. </returns>
        uint64_t SizeOf(VariableType type) const;

    private:
        friend class IRModuleEmitter;

        IREmitter(IRModuleEmitter& moduleEmitter, llvm::LLVMContext& context);

        LLVMType GetBaseVariableType(VariableType type) const;
        llvm::Constant* Integer(VariableType type, const size_t value);

        std::vector<LLVMType> BindArgumentTypes(const NamedVariableTypeList& arguments);
        std::vector<LLVMType> BindArgumentTypes(const NamedLLVMTypeList& arguments);
        std::vector<LLVMType> BindArgumentTypes(const FunctionArgumentList& args);

        void BindArgumentNames(LLVMFunction pFunction, const NamedVariableTypeList& arguments);
        void BindArgumentNames(LLVMFunction pFunction, const NamedLLVMTypeList& arguments);
        void BindArgumentNames(LLVMFunction pFunction, const FunctionArgumentList& arguments);

        LLVMFunction CreateFunction(llvm::Module* pModule, const std::string& name, llvm::Function::LinkageTypes linkage, llvm::FunctionType* pFunctionType);
        LLVMValue Zero();

        IRModuleEmitter& _moduleEmitter;
        llvm::LLVMContext& _llvmContext; // LLVM global context
        mutable llvm::IRBuilder<> _irBuilder; // IRBuilder API
        IRValueTable _stringLiterals; // String literals are emitted as constants. We have to track them ourselves to prevent dupes.
        LLVMValue _pZeroLiteral = nullptr;
        std::unordered_map<std::string, llvm::StructType*> _structs;
    };

    // Helper function to dump the LLVM module to stderr for debugging
    void DebugDump(llvm::Module* module, const std::string& tag = "", llvm::raw_ostream* stream = nullptr);

    // Helper function to dump the LLVM type to stderr for debugging
    void DebugDump(llvm::Type* type, const std::string& tag = "", llvm::raw_ostream* stream = nullptr);

    // Helper function to dump the LLVM value to stderr for debugging
    void DebugDump(llvm::Value* value, const std::string& tag = "", llvm::raw_ostream* stream = nullptr);

    // Helper function to dump the LLVM function to stderr for debugging
    void DebugDump(llvm::Function* function, const std::string& tag = "", llvm::raw_ostream* stream = nullptr);
} // namespace emitters
} // namespace ell

#pragma region implementation

namespace ell
{
namespace emitters
{
    template <typename OutputType>
    LLVMValue IREmitter::CastValue(LLVMValue pValue)
    {
        auto outputType = GetVariableType<OutputType>();
        return CastValue(pValue, outputType);
    }

    template <typename OutputType>
    LLVMValue IREmitter::CastUnsignedValue(LLVMValue pValue)
    {
        auto outputType = GetVariableType<OutputType>();
        return CastUnsignedValue(pValue, outputType);
    }

    template <typename ValueType>
    llvm::Constant* IREmitter::Pointer(ValueType* ptr)
    {
        auto ptrValue = Literal(reinterpret_cast<int64_t>(ptr));
        auto ptrType = PointerType(GetVariableType<ValueType>());
        return llvm::ConstantExpr::getIntToPtr(ptrValue, ptrType);
    }

    template <typename ValueType>
    uint64_t IREmitter::SizeOf() const
    {
        return SizeOf(GetVariableType<ValueType>());
    }
} // namespace emitters
} // namespace ell

#pragma endregion implementation
