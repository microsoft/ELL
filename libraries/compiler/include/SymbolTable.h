#pragma once

#include <unordered_map>

namespace emll
{
	namespace compiler
	{
		template<typename T>
		class SymbolTable
		{
		public:
			T* Get(const std::string& name);
			void Set(const std::string name, T* pValue);
			bool Contains(const std::string& name);
			void Remove(const std::string& name);

			void Clear();
		private:
			std::unordered_map<std::string, T*> _map;
		};
	}
}
#include "../tcc/SymbolTable.tcc"
