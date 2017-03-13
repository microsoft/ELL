////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRRuntime.cpp (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "IRRuntime.h"
#include "IRModuleEmitter.h"

namespace ell
{
namespace emitters
{
    static const std::string& countName = "count";
    static const std::string& lVectorName = "pLVector";
    static const std::string& rVectorName = "pRVector";
    static const std::string& resultName = "pResult";

    static const std::string& dotProductFloatName = "ELL_DotProductF";
    static const std::string& dotProductIntName = "ELL_DotProduct";

    IRRuntime::IRRuntime(IRModuleEmitter& module)
        : _module(module)
    {
    }

    llvm::Function* IRRuntime::GetDotProductFloatFunction()
    {
        if (_pDotProductFunctionFloat == nullptr)
        {
            _pDotProductFunctionFloat = EmitDotProductFunctionF();
        }
        return _pDotProductFunctionFloat;
    }

    llvm::Function* IRRuntime::GetDotProductFunction()
    {
        if (_pDotProductFunction == nullptr)
        {
            _pDotProductFunction = EmitDotProductFunction();
        }
        return _pDotProductFunction;
    }

    llvm::Function* IRRuntime::EmitDotProductFunctionF()
    {
        auto curBlock = _module.GetIREmitter().GetCurrentBlock();

        NamedVariableTypeList argList = { { countName, VariableType::Int32 },
                                          { lVectorName, VariableType::DoublePointer },
                                          { rVectorName, VariableType::DoublePointer },
                                          { resultName, VariableType::DoublePointer } };
        auto function = _module.Function(dotProductFloatName, VariableType::Void, argList);
        auto arguments = function.Arguments().begin();
        llvm::Argument& count = *arguments++;
        llvm::Argument& leftValue = *arguments++;
        llvm::Argument& rightValue = *arguments++;
        llvm::Argument& result = *arguments++;
        function.DotProductFloat(&count, &leftValue, &rightValue, &result);
        function.Return();
        function.Complete(true);

        _module.GetIREmitter().SetCurrentBlock(curBlock); // TODO: Remove this when IRFunctionEmitter has its own emitter

        return function.GetFunction();
    }

    llvm::Function* IRRuntime::EmitDotProductFunction()
    {
        auto curBlock = _module.GetIREmitter().GetCurrentBlock();

        NamedVariableTypeList argList = { { countName, VariableType::Int32 },
                                          { lVectorName, VariableType::Int32Pointer },
                                          { rVectorName, VariableType::Int32Pointer },
                                          { resultName, VariableType::Int32Pointer } };
        auto function = _module.Function(dotProductIntName, VariableType::Void, argList);
        auto arguments = function.Arguments().begin();
        llvm::Argument& count = *arguments++;
        llvm::Argument& leftValue = *arguments++;
        llvm::Argument& rightValue = *arguments++;
        llvm::Argument& result = *arguments++;
        function.DotProduct(&count, &leftValue, &rightValue, &result);
        function.Return();
        function.Complete(true);

        _module.GetIREmitter().SetCurrentBlock(curBlock); // TODO: Remove this when IRFunctionEmitter has its own emitter

        return function.GetFunction();
    }

    llvm::Function* IRRuntime::GetSqrtFunction(VariableType argType)
    {
        return _module.GetIntrinsic(llvm::Intrinsic::sqrt, { argType });
    }

    llvm::Function* IRRuntime::GetAbsFunction(VariableType argType)
    {
        return _module.GetIntrinsic(llvm::Intrinsic::fabs, { argType });
    }
}
}