namespace emll
{
	namespace compiler
	{
		template<typename T>
		CppModuleEmitter& CppModuleEmitter::Global(const std::string& name, bool needsInit)
		{
			_globals.NewLine();
			_globals.Var<T>(name);
			if (needsInit)
			{
				_globals.Space().Assign();
			}
			else
			{
				_globals.Semicolon();
			}
			return *this;
		}

		template<typename T>
		CppModuleEmitter& CppModuleEmitter::Global(const std::string& name, T value)
		{
			Global<T>(name, true);
			_globals.Space()
					.Literal<T>(value)
					.Semicolon();
			return *this;
		}
	}
}