namespace emll
{
	namespace compiler
	{
		template<typename T>
		CppFunctionEmitter& CppFunctionEmitter::Literal(T value)
		{ 
			_emitter.Literal<T>(value); 
			return *this;
		}

		template<typename T>
		CppFunctionEmitter& CppFunctionEmitter::Var(const std::string& name, T data)
		{
			_emitter.Var<T>(name)
				.Assign()
				.Literal<T>(data)
				.Semicolon();
			return *this;					
		}
	}
}