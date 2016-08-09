namespace emll
{
	namespace compiler
	{
		template<typename T>
		CppEmitter& CppEmitter::Var(const std::string& name)
		{
			return Var(GetValueType<T>(), name);
		}

		template<typename T>
		CppEmitter& CppEmitter::Literal(T value)
		{
			_writer.Write<T>(value);
			return *this;
		}
	}
}