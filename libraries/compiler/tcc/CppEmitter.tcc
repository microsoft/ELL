////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CppEmitter.tcc (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
	namespace compiler
	{
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
		CppEmitter& CppEmitter::Cmp(const std::string& varName, ComparisonType cmp, T value)
		{
			return Identifier(varName).Space()
					.Cmp(cmp).Space()
					.Literal<T>(value);
		}
	}
}