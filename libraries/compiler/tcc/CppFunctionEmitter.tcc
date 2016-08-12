namespace emll
{
	namespace compiler
	{
		template<typename T>
		CppFunctionEmitter& CppFunctionEmitter::Literal(T value)
		{ 
			_code.Literal<T>(value); 
			return *this;
		}

		template<typename T>
		CppFunctionEmitter& CppFunctionEmitter::Var(const std::string& name, T data)
		{
			_code.Var<T>(name)
				.Assign()
				.Literal<T>(data).Semicolon();
			return *this;					
		}

		template<typename T>
		CppFunctionEmitter& CppFunctionEmitter::Assign(const std::string& varName, T value)
		{
			_code.Assign(varName).Space();
			_code.Literal(value);
			EndStatement();
			return *this;
		}

		template<typename T>
		CppFunctionEmitter& CppFunctionEmitter::BeginIf(const std::string&varName, ComparisonType cmp, T value)
		{
			_code.If()
				.OpenParan()
				.Cmp<T>(varName, cmp, value)
				.CloseParan().NewLine()
				.BeginBlock();
			return *this;
		}
	}
}