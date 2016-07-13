#pragma once

#include <string>
#include <vector>

namespace emll
{
	namespace compiler
	{
		enum class VariableType
		{

		};

		class Variable
		{
		public:
			Variable();
			Variable(std::string name);

			virtual VariableType GetType() = 0;

			bool HasName() const
			{
				return !_name.empty();
			}
			const std::string& Name() const
			{
				return _name;
			}
			void SetName(std::string name);
			bool IsEmitted() const
			{
				return _isEmitted;
			}
			void SetEmitted(bool value)
			{
				_isEmitted = value;
			}
		private:
			std::string _name;
			bool _isEmitted = false;
		};
	}
}
