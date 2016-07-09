namespace emll
{
	namespace compiler
	{
		template<typename T, T Default>
		void SymbolTable<T, Default>::Init(std::initializer_list<SymbolValue> values)
		{
			_map.clear();
			_map.insert(values);
		}

		template<typename T, T Default>
		T SymbolTable<T, Default>::Get(const std::string& name) const
		{
			T value = Default;
			auto search = _map.find(name);
			if (search != _map.end())
			{
				value = search->second;
			}
			return value;
		}

		template<typename T, T Default>
		void SymbolTable<T, Default>::Set(const std::string name, T value) 
		{
			_map[std::move(name)] = value;
		}

		template<typename T, T Default>
		bool SymbolTable<T, Default>::Contains(const std::string& name) const
		{
			return (Get(name) != nullptr);
		}
			
		template<typename T, T Default>
		void SymbolTable<T, Default>::Remove(const std::string& name)
		{
			auto search = _map.find(name);
			if (search != _map.end())
			{
				_map.erase(search);
			}
		}

		template<typename T, T Default>
		void SymbolTable<T, Default>::Clear()
		{
			_map.clear();
		}
	}
}
