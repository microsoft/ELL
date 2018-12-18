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
        _originalFunctionName(name) {}

    FunctionDeclaration& FunctionDeclaration::Returns(Value returnType)
    {
        _returnType = returnType;
        return *this;
    }

    FunctionDeclaration& FunctionDeclaration::ParametersImpl(std::vector<Value> paramTypes)
    {
        _paramTypes = paramTypes;
        return *this;
    }

    FunctionDeclaration& FunctionDeclaration::Decorated(FunctionDecorated shouldDecorate)
    {
        _isDecorated = shouldDecorate == FunctionDecorated::Yes;
        return *this;
    }

    std::optional<Value> FunctionDeclaration::Call(std::vector<Value> arguments) const
    {
        return GetContext().Call(*this, arguments);
    }

    FunctionDeclaration DeclareFunction(std::string name)
    {
        return FunctionDeclaration(name);
    }

    const std::string& FunctionDeclaration::GetFunctionName() const
    {
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

    const std::vector<Value>& FunctionDeclaration::GetParameterTypes() const { return _paramTypes; }

    const std::optional<Value>& FunctionDeclaration::GetReturnType() const { return _returnType; }

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
