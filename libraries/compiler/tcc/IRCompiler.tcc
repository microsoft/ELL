namespace emll
{
	namespace compiler
	{
		template<typename T>
		llvm::Value* IRCompiler::EmitLocal(InitializedScalarVar<T>& var)
		{
			if (!var.HasEmittedName())
			{
				var.AssignVar(AllocTemp());
			}
			llvm::Value* pVar = _fn.Var(var.Type(), var.EmittedName());
			_fn.Store(pVar, _fn.Literal(var.Data()));
			return pVar;
		}

		template<typename T>
		llvm::Value* IRCompiler::EmitRef(VectorRefScalarVar<T>& var)
		{
			if (!var.HasEmittedName())
			{
				var.AssignVar(AllocTemp());
			}

			llvm::Value* pVar = nullptr;
			switch (var.SrcScope())
			{
				case VariableScope::Local:
				{
					llvm::Value* pVector = GetVariable(var.SrcName());
					llvm::Value* pVal = _fn.ValueAtA(pVector, _fn.Literal(var.Offset()));
					pVar = _fn.Var(var.Type(), var.EmittedName());
					_fn.Store(pVar, pVal);
				}
				break;

				default:
					throw new CompilerException(CompilerError::notSupported);
			}
			return pVar;
		}

		template<typename T>
		llvm::Value* IRCompiler::EmitGlobal(InitializedScalarVar<T>& var)
		{
			if (!var.HasEmittedName())
			{
				var.AssignVar(AllocGlobal());
			}
			llvm::Value* pVal = nullptr;
			if (var.IsMutable())
			{
				pVal = _module.Global(var.Type(), var.EmittedName());
				_fn.Store(pVal, _fn.Literal(var.Data()));
			}
			else
			{
				pVal = _module.Constant(var.Type(), var.EmittedName(), var.Data());
			}
			return pVal;
		}
	}
}