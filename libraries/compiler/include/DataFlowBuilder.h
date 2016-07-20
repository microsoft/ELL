#pragma once

#include "ModelGraph.h"
#include "ConstantNode.h"

#include "Dataflow.h"

namespace emll
{
	namespace compiler
	{
		class OutputPortDataNodes
		{
		public:
			size_t Size()
			{
				return _nodes.size();
			}
			void Add(DataNode* pNode);
			DataNode* Get(size_t position);

		private:
			std::vector<DataNode*> _nodes;
		};

		class OutputPortDataNodesMap
		{
		public:

			OutputPortDataNodes* Ensure(const model::OutputPortBase* pPort);
			OutputPortDataNodes* Get(const model::OutputPortBase* pPort);

		private:

		private:
			std::unordered_map<const model::OutputPortBase*, std::shared_ptr<OutputPortDataNodes>> _map;
		};

		class DataFlowBuilder
		{
		public:

			void Process(const model::Node& node);
			void ProcessConstant(const model::Node& node);

		private:
			using ConstantF = nodes::ConstantNode<double>;

			void Process(const ConstantF& node);

		private:
			DataFlowGraph _graph;
		};
	}
}
