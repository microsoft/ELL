namespace emll
{
	namespace compiler
	{
		template<typename T>
		T* SymbolTable<T>::Get(const std::string& name)
		{
			T* value = nullptr;
			auto search = _map.find(name);
			if (search != _map.end())
			{
				value = search->second;
			}
			return value;
		}

		template<typename T>
		void SymbolTable<T>::Set(const std::string name, T* pValue)
		{
			_map[std::move(name)] = pValue;
		}

		template<typename T>
		bool SymbolTable<T>::Contains(const std::string& name)
		{
			return (Get(name) != nullptr);
		}
			
		template<typename T>
		void SymbolTable<T>::Remove(const std::string& name)
		{
			auto search = _map.find(name);
			if (search != _map.end())
			{
				_map.erase(search);
			}
		}

		template<typename T>
		void SymbolTable<T>::Clear()
		{
			_map.clear();
		}
	}
}
