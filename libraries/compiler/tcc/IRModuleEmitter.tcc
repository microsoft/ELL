namespace emll
{
	namespace compiler
	{
		template<typename T>
		llvm::GlobalVariable* IRModuleEmitter::Constant(const std::string& name, T value)
		{
			return Global(name, _emitter.Type(GetValueType<T>()), _emitter.Literal(value), true);
		}

		template<typename T>
		llvm::GlobalVariable* IRModuleEmitter::Constant(const std::string&name, const std::vector<T>& value)
		{
			return Global(name, _emitter.Type(GetValueType<T>()), _emitter.Literal(value), true);
		}
		template<typename T>
		llvm::GlobalVariable* IRModuleEmitter::Global(const std::string&name, const std::vector<T>& value)
		{
			return Global(name, _emitter.Type(GetValueType<T>()), _emitter.Literal(value), false);
		}
	}
}