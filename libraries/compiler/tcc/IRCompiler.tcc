namespace emll
{
	namespace compiler
	{
		template<typename T>
		llvm::Value* IRCompiler::EmitLiteral(LiteralVar<T>& var)
		{
			llvm::Value* pVar = _fn.Literal(var.Data());
			return pVar;
		}

		template<typename T>
		llvm::Value* IRCompiler::EmitLocal(ScalarVar<T>& var)
		{
			return _fn.Var(var.Type(), var.EmittedName());
		}

		template<typename T>
		llvm::Value* IRCompiler::EmitLocal(InitializedScalarVar<T>& var)
		{
			llvm::Value* pVar = _fn.Var(var.Type(), var.EmittedName());
			_fn.Store(pVar, _fn.Literal(var.Data()));
			return pVar;
		}

		template<typename T>
		llvm::Value* IRCompiler::EmitRef(VectorElementVar<T>& var)
		{
			llvm::Value* pSrcVar = EnsureEmitted(var.Src());
			return _fn.PtrOffsetA(pSrcVar, _fn.Literal(var.Offset()), var.EmittedName());
		}

		template<typename T>
		llvm::Value* IRCompiler::EmitGlobal(InitializedScalarVar<T>& var)
		{
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

		template<typename T>
		llvm::Value* IRCompiler::EmitComputed(ComputedVar<T>& var)
		{
			llvm::Value* pSrc = EnsureEmitted(var.Src());
			if (!(var.HasIncrement() || var.HasMultiply()))
			{
				return pSrc;
			}
			llvm::Value* pVar = _fn.Var(var.Type(), var.EmittedName());
			if (var.HasIncrement())
			{
				llvm::Value* pResult = _fn.Op(GetAddForValueType<T>(), pSrc, _fn.Literal(var.IncrementBy()));
				_fn.Store(pVar, pResult);
			}
			if (var.HasMultiply())
			{
				llvm::Value* pResult = _fn.Op(GetMultiplyForValueType<T>(), pSrc, _fn.Literal(var.MultiplyBy()));
				_fn.Store(pVar, pResult);
			}
			return pVar;
		}

	}
}