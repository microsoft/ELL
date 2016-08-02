namespace emll
{
	namespace compiler
	{
		template<typename T>
		llvm::Function* IRRuntime::Sqrt()
		{
			return _module.GetIntrinsic(llvm::Intrinsic::sqrt, { GetValueType<T>() });
		}
	}
}