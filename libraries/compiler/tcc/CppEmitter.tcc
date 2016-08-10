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
		CppEmitter& CppEmitter::Var(const std::string& name, int size)
		{
			return Var<T>(name).Dimension(size);
		}

		template<typename T>
		CppEmitter& CppEmitter::Literal(T value)
		{
			_writer.Write<T>(value);
			return *this;
		}

		template<typename T>
		CppEmitter& CppEmitter::Literal(const std::vector<T>& value)
		{
			OpenBrace();
			for (size_t i = 0; i < value.size(); ++i)
			{
				if (i > 0)
				{
					Comma().Space();
				}
				Literal<T>(value[i]);
			}
			return CloseBrace();
		}
	}
}