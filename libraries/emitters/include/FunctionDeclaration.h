////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FunctionDeclaration.h (emitters)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "EmitterException.h"
#include "LLVMUtilities.h"

namespace ell
{
namespace emitters
{
    /// <summary> Additional information about function arguments. </summary>
    enum ArgumentFlags
    {
        None = 0,
        ///<summary> Argument is an input to the function </summary>
        Input = 1,
        ///<summary> Argument is an output from the function </summary>
        Output = 2,
        ///<summary> Argument is both an input and an output </summary>
        InOut = 4
    };

    /// <summary> A function argument definition for FunctionDeclaration </summary>
    class FunctionArgument
    {
    public:
        FunctionArgument(std::string name, VariableType type, ArgumentFlags flags = ArgumentFlags::None, LLVMType llvmType = nullptr) :
            _name(name),
            _type(type),
            _flags(flags),
            _llvmType(llvmType)
        {
        }
        /// <summary> Gets the name of the parameter. </summary>
        std::string GetName() const { return _name; }

        /// <summary> Gets the type of the parameter. </summary>
        VariableType GetType() const { return _type; }

        /// <summary> Gets additional flags about the parameter. </summary>
        ArgumentFlags GetFlags() const { return _flags; }

        /// <summary> Provide additional flags about the parameter. </summary>
        void SetFlags(ArgumentFlags flags) { _flags = flags; }

        /// <summary> Get the LLVM type, if we have it. </summary>
        LLVMType GetLLVMType() const { return _llvmType; }

    private:
        std::string _name;
        VariableType _type;
        ArgumentFlags _flags;
        LLVMType _llvmType;
    };

    /// <summary> Collections of argument flags </summary>
    using FunctionArgumentList = std::vector<FunctionArgument>;

    /// <summary> A function definition that defines the name, return type and arguments of a function </summary>
    class FunctionDeclaration
    {
    public:
        /// <summary> Default constructor </summary>
        FunctionDeclaration() :
            _returnType(VariableType::Custom) {}

        /// <summary> Construct a new Function wrapper </summary>
        FunctionDeclaration(const std::string& name, VariableType returnType) :
            _name(name),
            _returnType(returnType)
        {
        }

        /// <summary> Construct a new Function wrapper </summary>
        FunctionDeclaration(const std::string& name, VariableType returnType, const NamedVariableTypeList& args) :
            _name(name),
            _returnType(returnType)
        {
            for (auto pair : args)
            {
                _args.push_back(FunctionArgument(pair.first, pair.second));
            }
        }

        /// <summary> Construct a new Function wrapper </summary>
        FunctionDeclaration(const std::string& name, VariableType returnType, const FunctionArgumentList& args) :
            _name(name),
            _returnType(returnType),
            _args(args)
        {
        }

        /// <summary> Gets the name of the function being emitted. </summary>
        std::string GetFunctionName() const { return _name; }

        /// <summary> Gets the return type of the function. </summary>
        VariableType GetReturnType() const { return _returnType; }

        /// <summary> Gets the function argument names and types. </summary>
        const FunctionArgumentList& GetArguments() const { return _args; }

        /// <summary> Indicates if the given function has any associated comments. </summary>
        ///
        /// <returns> `true` if the function has any comments. </returns>
        bool HasComments() const { return !_comments.empty(); }

        /// <summary> Get edit access to the comments.  You can use this method to add comments. </summary>
        std::vector<std::string>& GetComments() { return _comments; }

        /// <summary> Get readonly access to the comments </summary>
        const std::vector<std::string>& GetComments() const { return _comments; }

    private:
        std::vector<std::string> _comments;
        std::string _name;
        VariableType _returnType;
        FunctionArgumentList _args;
    };
} // namespace emitters
} // namespace ell
