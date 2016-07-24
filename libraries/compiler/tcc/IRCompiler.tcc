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
			llvm::Value* pSrc = LoadVar(var.Src());
			T increment = var.IncrementBy();
			T multiplyBy = var.MultiplyBy();

			llvm::Value* pVar = _fn.Var(var.Type(), var.EmittedName());
			llvm::Value* pResult;
			if (increment == 0.0)
			{
				if (multiplyBy != 1.0)
				{
					pResult = _fn.Op(GetMultiplyForValueType<T>(), pSrc, _fn.Literal(multiplyBy));
					_fn.Store(pVar, pResult);
				}
				else
				{
					_fn.Store(pVar, pSrc);
				}
			}
			else
			{
				if (multiplyBy == 0.0)
				{
					_fn.Store(pVar, _fn.Literal(multiplyBy));
				}
				else if (multiplyBy == 1.0)
				{
					pResult = _fn.Op(GetAddForValueType<T>(), pSrc, _fn.Literal(increment));
					_fn.Store(pVar, pResult);
				}
				else
				{
					pResult = _fn.Op(GetAddForValueType<T>(),
								_fn.Op(GetMultiplyForValueType<T>(), _fn.Literal(multiplyBy), pSrc),
								_fn.Literal(increment));
					_fn.Store(pVar, pResult);
				}
			}
			return pVar;
		}

	}
}