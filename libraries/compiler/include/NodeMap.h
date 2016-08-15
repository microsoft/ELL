////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     NodeMap.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <unordered_map>

namespace emll
{
	namespace compiler
	{
		template<typename T, T Default>
		class NodeMap
		{
		public:

			T Get(const model::Node& node) const;
			T Get(const model::Node* pNode) const;
			void Set(const model::Node& node, T value);
			bool Contains(const model::Node& node) const;
			void Remove(const model::Node& node);

			void Clear();

		private:
			std::unordered_map<model::Node::NodeId, T> _map;
		};
	}
}

#include "../tcc/NodeMap.tcc"

