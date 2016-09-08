////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CppModuleEmitter.tcc (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
namespace emll
{
	namespace compiler
	{
		template<typename T>
		CppModuleEmitter& CppModuleEmitter::Global(const std::string& name, bool needsInit)
		{
			_globals.Var<T>(name);
			if (needsInit)
			{
				_globals.Space().Assign();
			}
			else
			{
				_globals.Semicolon().NewLine();
			}
			return *this;
		}

		template<typename T>
		CppModuleEmitter& CppModuleEmitter::Global(const std::string& name, T value)
		{
			Global<T>(name, true);
			_globals.Space()
					.Literal<T>(value)
					.Semicolon().NewLine();
			return *this;
		}

		template<typename T>
		CppModuleEmitter& CppModuleEmitter::GlobalV(const std::string& name, const int size)
		{
			_globals
				.Var<T>(name, size)
				.Semicolon().NewLine();
			return *this;
		}

		template<typename T>
		CppModuleEmitter& CppModuleEmitter::GlobalV(const std::string& name, const std::vector<T>& value)
		{
			_globals
				.Var<T>(name, value.size()).Space()
				.Assign().Space()
				.Literal<T>(value)
				.Semicolon().NewLine();
			return *this;
		}

		template<typename T>
		CppModuleEmitter& CppModuleEmitter::Constant(const std::string& name, T value)
		{
			_constants
				.Static().Space()
				.Const().Space()
				.Var<T>(name).Space()
				.Assign().Space()
				.Literal<T>(value)
				.Semicolon().NewLine();
			return *this;
		}

		template<typename T>
		CppModuleEmitter& CppModuleEmitter::ConstantV(const std::string& name, const std::vector<T>& value)
		{
			_constants
				.Static().Space()
				.Const().Space()
				.Var<T>(name).Dimension(value.size()).Space()
				.Assign().Space()
				.Literal<T>(value)
				.Semicolon().NewLine();

			return *this;
		}
	}
}