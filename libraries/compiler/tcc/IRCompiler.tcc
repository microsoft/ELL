namespace emll
{
	namespace compiler
	{
		template<typename T>
		llvm::Value* IRCompiler::EmitLocalScalar(LocalScalarVar<T>& var)
		{
			if (!var.HasEmittedName())
			{
				var.AssignTempVar(AllocTemp());
			}
			llvm::Value* pVal = nullptr;
			if (var.IsMutable())
			{
				pVal = _fn.Var(var.Type(), var.EmittedName());
			}
			else
			{
				pVal = _module.Constant(var.Type(), var.EmittedName(), var.Data());
			}
			assert(pVal != nullptr);
			return pVal;
		}

		template<typename T>
		llvm::Value* IRCompiler::EmitGlobalScalar(GlobalScalarVar<T>& var)
		{
			if (!var.HasEmittedName())
			{
				var.AssignVar(AllocGlobal());
			}
			llvm::Value* pVal = nullptr;
			if (var.IsMutable())
			{
				_module.Global(var.Type(), var.EmittedName(), var.Data());
			}
			else
			{
				pVal = _module.Constant(var.Type(), var.EmittedName(), var.Data());
			}
			assert(pVal != nullptr);
			return pVal;
		}
	}
}