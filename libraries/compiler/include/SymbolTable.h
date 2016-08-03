////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SymbolTable.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <unordered_map>

namespace emll
{
	namespace compiler
	{
		template<class T, T Default>
		class SymbolTable
		{
		public:
			using SymbolValue = std::pair<const std::string, T>;

		public:
			SymbolTable() {}
			SymbolTable(std::initializer_list<SymbolValue> values)
			{
				Init(values);
			}

			void Init(std::initializer_list<SymbolValue> values);

			T Get(const std::string& name) const;
			void Set(std::string name, T value);
			bool Contains(const std::string& name) const;
			void Remove(const std::string& name);

			void Clear();

		private:
			std::unordered_map<std::string, T> _map;
		};
	}
}
#include "../tcc/SymbolTable.tcc"
