////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     FunctionDeclaration.cpp (value)
//  Authors:  Kern Handa
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "FunctionDeclaration.h"

#include <utilities/include/Hash.h>

namespace ell
{
namespace value
{
    using namespace ell::utilities;

    FunctionDeclaration::FunctionDeclaration(std::string name) :
        _originalFunctionName(name),
        _isEmpty(false) 
    {}

    FunctionDeclaration& FunctionDeclaration::Returns(Value returnType)
    {
        CheckNonEmpty();

        _returnType = returnType;
        return *this;
    }

    FunctionDeclaration& FunctionDeclaration::Parameters(std::vector<Value> paramTypes)
    {
        CheckNonEmpty();

        _paramTypes = paramTypes;
        return *this;
    }

    FunctionDeclaration& FunctionDeclaration::Decorated(FunctionDecorated shouldDecorate)
    {
        CheckNonEmpty();

        _isDecorated = shouldDecorate == FunctionDecorated::Yes;
        return *this;
    }

    std::optional<Value> FunctionDeclaration::Call(std::vector<Value> arguments) const
    {
        CheckNonEmpty();

        return GetContext().Call(*this, arguments);
    }

    const std::string& FunctionDeclaration::GetFunctionName() const
    {
        CheckNonEmpty();

        if (_isDecorated)
        {
            if (!_decoratedFunctionName)
            {
                size_t hash = 0;
                HashCombine(hash, _returnType);
                HashCombine(hash, _paramTypes);
                _decoratedFunctionName = _originalFunctionName + std::to_string(hash);
            }
            return *_decoratedFunctionName;
        }
        else
        {
            return _originalFunctionName;
        }
    }

    const std::vector<Value>& FunctionDeclaration::GetParameterTypes() const
    {
        CheckNonEmpty();

        return _paramTypes;
    }

    const std::optional<Value>& FunctionDeclaration::GetReturnType() const
    {
        CheckNonEmpty();

        return _returnType;
    }

    bool FunctionDeclaration::IsDefined() const
    {
        CheckNonEmpty();

        return GetContext().IsFunctionDefined(*this);
    }

    bool FunctionDeclaration::IsEmpty() const { return _isEmpty; }

    void FunctionDeclaration::CheckNonEmpty() const
    {
        if (_isEmpty)
        {
            throw LogicException(LogicExceptionErrors::notInitialized, "FunctionDeclaration is empty");
        }
    }

    FunctionDeclaration DeclareFunction(std::string name)
    {
        return FunctionDeclaration(name);
    }

    /*extern*/ FunctionDeclaration AbsFunctionDeclaration = DeclareFunction("Abs");
    /*extern*/ FunctionDeclaration CosFunctionDeclaration = DeclareFunction("Cos");
    /*extern*/ FunctionDeclaration ExpFunctionDeclaration = DeclareFunction("Exp");
    /*extern*/ FunctionDeclaration LogFunctionDeclaration = DeclareFunction("Log");
    /*extern*/ FunctionDeclaration MaxNumFunctionDeclaration = DeclareFunction("MaxNum");
    /*extern*/ FunctionDeclaration MinNumFunctionDeclaration = DeclareFunction("MinNum");
    /*extern*/ FunctionDeclaration PowFunctionDeclaration = DeclareFunction("Pow");
    /*extern*/ FunctionDeclaration SinFunctionDeclaration = DeclareFunction("Sin");
    /*extern*/ FunctionDeclaration SqrtFunctionDeclaration = DeclareFunction("Sqrt");
    /*extern*/ FunctionDeclaration TanhFunctionDeclaration = DeclareFunction("Tanh");

} // namespace value
} // namespace ell

size_t std::hash<::ell::value::FunctionDeclaration>::operator()(const ::ell::value::FunctionDeclaration& value) const
{
    // The function name already encodes the return values and parameter names. And for undecorated functions, we
    // don't want to support overloading
    return std::hash<std::string>{}(value.GetFunctionName());
}
