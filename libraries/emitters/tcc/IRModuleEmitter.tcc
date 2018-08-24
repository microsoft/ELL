////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     IRModuleEmitter.tcc (emitters)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ell
{
namespace emitters
{
    //
    // Public methods
    //
    template <typename ValueType>
    llvm::GlobalVariable* IRModuleEmitter::Constant(const std::string& name, ValueType value)
    {
        return AddGlobal(name, _emitter.Type(GetVariableType<ValueType>()), _emitter.Literal(value), true);
    }

    template <typename ValueType>
    llvm::GlobalVariable* IRModuleEmitter::Global(const std::string& name, ValueType value)
    {
        return AddGlobal(name, _emitter.Type(GetVariableType<ValueType>()), _emitter.Literal(value), false);
    }

    template <typename ValueType>
    llvm::GlobalVariable* IRModuleEmitter::ConstantArray(const std::string& name, const std::vector<ValueType>& value)
    {
        return AddGlobal(name, _emitter.ArrayType(GetVariableType<ValueType>(), value.size()), _emitter.Literal(value), true);
    }

    template <typename ValueType>
    llvm::GlobalVariable* IRModuleEmitter::GlobalArray(const std::string& name, size_t size)
    {
        return GlobalArray(GetVariableType<ValueType>(), name, size);
    }

    template <typename ValueType>
    llvm::GlobalVariable* IRModuleEmitter::GlobalArray(const std::string& name, const std::vector<ValueType>& value)
    {
        return AddGlobal(name, _emitter.ArrayType(GetVariableType<ValueType>(), value.size()), _emitter.Literal(value), false);
    }

    //
    // Private methods
    //

    template <typename T>
    LLVMValue IRModuleEmitter::EmitVariable(Variable& var)
    {
        // TODO: have a more specific check to see if the variable is mapped to a port, rather than if it's a function input/output
        if (var.IsScalar() && (var.Scope() != VariableScope::input && var.Scope() != VariableScope::output))
        {
            return EmitScalar<T>(var);
        }
        else if (var.IsVector())
        {
            return EmitVector<T>(var);
        }
        else
        {
            throw EmitterException(EmitterError::variableTypeNotSupported);
        }
    }

    template <typename T>
    LLVMValue IRModuleEmitter::EmitScalar(Variable& var)
    {
        LLVMValue pVal = nullptr;
        switch (var.Scope())
        {
            case VariableScope::literal:
                pVal = EmitLiteral<T>(static_cast<LiteralVariable<T>&>(var));
                _literals.Add(var.EmittedName(), pVal);
                break;

            case VariableScope::local:
                if (var.IsVectorRef())
                {
                    pVal = EmitRef<T>(static_cast<VectorElementVariable<T>&>(var));
                }
                else if (var.HasInitValue())
                {
                    pVal = EmitLocal<T>(static_cast<InitializedScalarVariable<T>&>(var));
                }
                else
                {
                    pVal = EmitLocal<T>(static_cast<ScalarVariable<T>&>(var));
                }
                break;

            case VariableScope::global:
                pVal = EmitGlobal<T>(static_cast<InitializedScalarVariable<T>&>(var));
                break;

            default:
                throw EmitterException(EmitterError::variableScopeNotSupported);
        }
        return pVal;
    }

    template <typename T>
    LLVMValue IRModuleEmitter::EmitVector(Variable& var)
    {
        LLVMValue pVal = nullptr;
        switch (var.Scope())
        {
            case VariableScope::literal:
                pVal = EmitLiteralVector<T>(static_cast<LiteralVectorVariable<T>&>(var));
                _literals.Add(var.EmittedName(), pVal);
                break;

            case VariableScope::global:
                if (var.HasInitValue())
                {
                    pVal = EmitGlobalVector<T>(static_cast<InitializedVectorVariable<T>&>(var));
                }
                else
                {
                    pVal = EmitGlobalVector<T>(static_cast<VectorVariable<T>&>(var));
                }
                _globals.Add(var.EmittedName(), pVal);
                break;

            default:
                throw EmitterException(EmitterError::variableScopeNotSupported);
        }
        assert(pVal != nullptr);
        return pVal;
    }

    template <typename T>
    LLVMValue IRModuleEmitter::EmitLiteral(LiteralVariable<T>& var)
    {
        auto& currentFunction = GetCurrentFunction();
        LLVMValue pVar = currentFunction.Literal(var.Data());
        return pVar;
    }

    template <typename T>
    LLVMValue IRModuleEmitter::EmitGlobal(InitializedScalarVariable<T>& var)
    {
        auto& currentFunction = GetCurrentFunction();
        LLVMValue pVal = nullptr;
        if (var.IsMutable())
        {
            pVal = Global(var.Type(), var.EmittedName());
            currentFunction.Store(pVal, currentFunction.Literal(var.Data()));
        }
        else
        {
            pVal = Constant(var.Type(), var.EmittedName(), var.Data());
        }
        return pVal;
    }

    template <typename T>
    LLVMValue IRModuleEmitter::EmitLocal(ScalarVariable<T>& var)
    {
        auto& currentFunction = GetCurrentFunction();
        return currentFunction.EmittedVariable(var.Type(), var.EmittedName());
    }

    template <typename T>
    LLVMValue IRModuleEmitter::EmitLocal(InitializedScalarVariable<T>& var)
    {
        auto& currentFunction = GetCurrentFunction();
        LLVMValue pVar = currentFunction.EmittedVariable(var.Type(), var.EmittedName());
        currentFunction.Store(pVar, currentFunction.Literal(var.Data()));
        return pVar;
    }

    template <typename T>
    LLVMValue IRModuleEmitter::EmitLiteralVector(LiteralVectorVariable<T>& var)
    {
        return ConstantArray(var.EmittedName(), var.Data());
    }

    template <typename T>
    LLVMValue IRModuleEmitter::EmitGlobalVector(VectorVariable<T>& var)
    {
        return GlobalArray(GetVariableType<T>(), var.EmittedName(), var.Dimension());
    }

    template <typename T>
    LLVMValue IRModuleEmitter::EmitGlobalVector(InitializedVectorVariable<T>& var)
    {
        return GlobalArray(var.EmittedName(), var.Data());
    }

    template <typename T>
    LLVMValue IRModuleEmitter::EmitRef(VectorElementVariable<T>& var)
    {
        auto& currentFunction = GetCurrentFunction();
        LLVMValue pSrcVar = EnsureEmitted(var.Src());
        return currentFunction.PtrOffsetA(pSrcVar, currentFunction.Literal(var.Offset()), var.EmittedName());
    }
}
}
