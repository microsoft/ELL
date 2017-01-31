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
        return Global(name, _emitter.Type(GetVariableType<ValueType>()), _emitter.Literal(value), true);
    }

    template <typename ValueType>
    llvm::GlobalVariable* IRModuleEmitter::Constant(const std::string& name, const std::vector<ValueType>& value)
    {
        return Global(name, _emitter.Type(GetVariableType<ValueType>()), _emitter.Literal(value), true);
    }

    template <typename ValueType>
    llvm::GlobalVariable* IRModuleEmitter::Global(const std::string& name, const std::vector<ValueType>& value)
    {
        return Global(name, _emitter.Type(GetVariableType<ValueType>()), _emitter.Literal(value), false);
    }

    //
    // Private methods
    //

    template <typename T>
    llvm::Value* IRModuleEmitter::Emit(Variable& var)
    {
        if (var.IsScalar())
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
    llvm::Value* IRModuleEmitter::EmitScalar(Variable& var)
    {
        llvm::Value* pVal = nullptr;
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
                _locals.Add(var.EmittedName(), pVal);
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
    llvm::Value* IRModuleEmitter::EmitVector(Variable& var)
    {
        llvm::Value* pVal = nullptr;
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
    llvm::Value* IRModuleEmitter::EmitLiteral(LiteralVariable<T>& var)
    {
        llvm::Value* pVar = _currentFunction.Literal(var.Data());
        return pVar;
    }

    template <typename T>
    llvm::Value* IRModuleEmitter::EmitGlobal(InitializedScalarVariable<T>& var)
    {
        llvm::Value* pVal = nullptr;
        if (var.IsMutable())
        {
            pVal = Global(var.Type(), var.EmittedName());
            _currentFunction.Store(pVal, _currentFunction.Literal(var.Data()));
        }
        else
        {
            pVal = Constant(var.Type(), var.EmittedName(), var.Data());
        }
        return pVal;
    }

    template <typename T>
    llvm::Value* IRModuleEmitter::EmitLocal(ScalarVariable<T>& var)
    {
        return _currentFunction.Variable(var.Type(), var.EmittedName());
    }

    template <typename T>
    llvm::Value* IRModuleEmitter::EmitLocal(InitializedScalarVariable<T>& var)
    {
        llvm::Value* pVar = _currentFunction.Variable(var.Type(), var.EmittedName());
        _currentFunction.Store(pVar, _currentFunction.Literal(var.Data()));
        return pVar;
    }

    template <typename T>
    llvm::Value* IRModuleEmitter::EmitLiteralVector(LiteralVectorVariable<T>& var)
    {
        return Constant(var.EmittedName(), var.Data());
    }

    template <typename T>
    llvm::Value* IRModuleEmitter::EmitGlobalVector(VectorVariable<T>& var)
    {
        return Global(GetVariableType<T>(), var.EmittedName(), var.Dimension());
    }

    template <typename T>
    llvm::Value* IRModuleEmitter::EmitGlobalVector(InitializedVectorVariable<T>& var)
    {
        return Global(var.EmittedName(), var.Data());
    }

    template <typename T>
    llvm::Value* IRModuleEmitter::EmitRef(VectorElementVariable<T>& var)
    {
        llvm::Value* pSrcVar = EnsureEmitted(var.Src());
        return _currentFunction.PtrOffsetA(pSrcVar, _currentFunction.Literal(var.Offset()), var.EmittedName());
    }
}
}