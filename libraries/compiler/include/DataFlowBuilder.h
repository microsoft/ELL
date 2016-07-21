#pragma once

#include "ModelGraph.h"
#include "ConstantNode.h"
#include "BinaryOperationNode.h"
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
			DataNode* Get(size_t position) const;

		private:
			std::vector<DataNode*> _nodes;
		};

		class OutputPortDataNodesMap
		{
		public:

			OutputPortDataNodes* Ensure(const model::OutputPortBase* pPort);
			OutputPortDataNodes* Get(const model::OutputPortBase* pPort) const;

			void Add(DataNode* pNode, const model::OutputPortBase* pPort);
			DataNode* Get(const model::OutputPortBase* pPort, const size_t position) const;

		private:

		private:
			std::unordered_map<const model::OutputPortBase*, std::shared_ptr<OutputPortDataNodes>> _map;
		};

		class DataFlowBuilder
		{
		public:

			void Process(const model::Node& node);
			void ProcessConstant(const model::Node& node);
			void ProcessBinaryOperation(const model::Node& node);

		private:
			using ConstantF = nodes::ConstantNode<double>;
			using BinaryOperationF = nodes::BinaryOperationNode<double>;

			void Process(const ConstantF& node);
			void Process(const BinaryOperationF& node);

			DataNode* GetSourceNode(const model::InputPortBase* pPort, size_t index) const;

		private:
			DataFlowGraph _graph;
			OutputPortDataNodesMap _outputPortMap;
		};
	}
}
