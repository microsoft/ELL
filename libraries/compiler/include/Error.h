#pragma once

#include <exception>
#include <system_error>

namespace emll
{
	namespace core
	{
		template<typename T>
		class EmllException : public std::exception
		{
		public:
			EmllException(T errorCode)
				: _error(errorCode)
			{

			}
			virtual ~EmllException() {

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

