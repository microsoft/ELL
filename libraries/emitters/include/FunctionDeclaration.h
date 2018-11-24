////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     Function.h (emitters)
//  Authors:  Chris Lovett
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "EmitterException.h"

namespace ell
{
namespace emitters
{
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
            _returnType(returnType),
            _args(args)
        {
        }

        /// <summary> Gets the name of the function being emitted. </summary>
        std::string GetFunctionName() const { return _name; }

        /// <summary> Gets the return type of the function. </summary>
        VariableType GetReturnType() const { return _returnType; }

        /// <summary> Gets the function argument names and types. </summary>
        const NamedVariableTypeList& GetArguments() const { return _args; }

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
        NamedVariableTypeList _args;
    };
} // namespace emitters
} // namespace ell