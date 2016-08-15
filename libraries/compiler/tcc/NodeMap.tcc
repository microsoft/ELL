////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     NodeMap.tcc (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

namespace emll
{
	namespace compiler
	{
		template<typename T, T Default>
		T NodeMap<T, Default>::Get(const model::Node& node) const
		{
			T value = Default;
			auto search = _map.find(node.GetId());
			if (search != _map.end())
			{
				value = search->second;
			}
			return value;
		}

		template<typename T, T Default>
		T NodeMap<T, Default>::Get(const model::Node* pNode) const
		{
			assert(pNode != nullptr);
			return Get(*pNode);
		}

		template<typename T, T Default>
		void NodeMap<T, Default>::Set(const model::Node& node, T value)
		{
			_map[node.GetId()] = value;
		}

		template<typename T, T Default>
		bool NodeMap<T, Default>::Contains(const model::Node& node) const
		{
			return (Get(node) != nullptr);
		}

		template<typename T, T Default>
		void NodeMap<T, Default>::Remove(const model::Node& node)
		{
			auto search = _map.find(node.GetId());
			if (search != _map.end())
			{
				_map.erase(search);
			}
		}

		template<typename T, T Default>
		void NodeMap<T, Default>::Clear()
		{
			_map.clear();
		}

	}
}