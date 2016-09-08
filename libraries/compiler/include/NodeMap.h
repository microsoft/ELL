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
		///<summary>Used by the compiler to maintain tables for data associated with each node.</summary>
		template<typename T, T Default>
		class NodeMap
		{
		public:
			///<summary>Get the mapped value for this node, or the default</summary>
			T Get(const model::Node& node) const;
			///<summary>Get the mapped value for this node, or the default</summary>
			T Get(const model::Node* pNode) const;
			///<summary>Set the mapped value for this node</summary>
			void Set(const model::Node& node, T value);
			///<summary>True if the given node is mapped</summary>
			bool Contains(const model::Node& node) const;
			///<summary>Remove the mapped value for this node</summary>
			void Remove(const model::Node& node);
			///<summary>Clear the map</summary>
			void Clear();

		private:
			std::unordered_map<model::Node::NodeId, T> _map;
		};
	}
}

#include "../tcc/NodeMap.tcc"

