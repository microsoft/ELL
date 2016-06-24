#pragma once

#include <exception>
#include <system_error>

namespace emll
{
	namespace compiler
	{
		template<typename T>
		class CompilerException : public std::exception
		{
		public:
			CompilerException(T errorCode)
				: _error(errorCode)
			{

			}
			virtual ~CompilerException() {

			}

			T& error()
			{
				return _error;
			}

		private:
			T _error;
		};
	}
}

