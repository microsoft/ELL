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
				.Literal<T>(data).Semicolon();
			return *this;					
		}

		template<typename T>
		CppFunctionEmitter& CppFunctionEmitter::Assign(const std::string& varName, T value)
		{
			_emitter.Assign(varName).Space();
			_emitter.Literal(value);
			EndStatement();
			return *this;
		}

		template<typename T>
		CppFunctionEmitter& CppFunctionEmitter::BeginIf(const std::string&varName, ComparisonType cmp, T value)
		{
			_emitter.If()
				.OpenParan()
				.Cmp<T>(varName, cmp, value)
				.CloseParan().NewLine()
				.BeginBlock();
			return *this;
		}
	}
}