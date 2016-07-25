#pragma once

#include "ModelGraph.h"
#include "ConstantNode.h"
#include "BinaryOperationNode.h"
#include "DataflowGraph.h"

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

			void Process(const model::Model& model);
			void Process(const model::Node& node);
			void ProcessConstant(const model::Node& node);
			void ProcessBinaryOperation(const model::Node& node);
			void ProcessInputNode(const model::Node& node);
			void ProcessOutputNode(const model::Node& node);

			DataFlowGraph& Graph() { return _graph; }

		private:

			template<typename DataType>
			void Process(const nodes::ConstantNode<DataType>& node);

			template<typename DataType>
			void Process(const nodes::BinaryOperationNode<DataType>& node);

			template<typename DataType>
			void Process(const model::InputNode<DataType>& node);

			template<typename DataType> 
			void AddOutput(const model::Node& leafNode);

			DataNode* GetSourceNode(const model::InputPortBase* pPort, size_t elementIndex) const;
			void AddDependency(const model::InputPortBase* pPort, size_t elementIndex, DataNode* pDependant);
			void AddDependency(const model::OutputPortBase* pPort, size_t elementIndex, DataNode* pDependant);

			template<typename DataType>
			OperatorType GetOperator(const nodes::BinaryOperationNode<DataType>& node);

		private:
			DataFlowGraph _graph;
			OutputPortDataNodesMap _outputPortMap;
		};
	}
}

#include "../tcc/DataFlowBuilder.tcc"

