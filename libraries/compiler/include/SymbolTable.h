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
		///<summary>A table for managing compiler symbols: named values</summary>
		template<class T, T Default>
		class SymbolTable
		{
		public:
			///<summary>A symbol table contains symbol values</summary>
			using SymbolValue = std::pair<const std::string, T>;

		public:
			///<summary>Construct a symbol table</summary>
			SymbolTable() {}
			///<summary>Construct a symbol table</summary>
			SymbolTable(std::initializer_list<SymbolValue> values)
			{
				Init(values);
			}
			///<summary>Initialize this symbol table with new values, clearing old ones</summary>
			void Init(std::initializer_list<SymbolValue> values);

			///<summary>Get a symbol's value with the given name, else return the default</summary>
			T Get(const std::string& name) const;
			///<summary>Set a symbol's value</summary>
			void Set(std::string name, T value);
			///<summary>Returns true if the table contains a matching symbol</summary>
			bool Contains(const std::string& name) const;
			///<summary>Remove the symbol</summary>
			void Remove(const std::string& name);
			///<summary>Clear the symbol table</summary>
			void Clear();

		private:
			std::unordered_map<std::string, T> _map;
		};
	}
}
#include "../tcc/SymbolTable.tcc"
