#pragma once

#include "ModelGraph.h"
#include "Node.h"
#include "InputNode.h"
#include "OutputPort.h"
#include "OutputPortElementList.h"
#include "ConstantNode.h"
#include "BinaryOperationNode.h"
#include "SumNode.h"
#include "LinearPredictorNode.h"
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

		///<summary>Stores the DataNode allocated for each element in an outport port</summary>
		class OutputPortDataNodesMap
		{
		public:

			OutputPortDataNodes* Ensure(const model::OutputPortBase* pPort);
			OutputPortDataNodes* Get(const model::OutputPortBase* pPort) const;

			void Add(const model::OutputPortBase* pPort, DataNode* pNode);
			DataNode* Get(const model::OutputPortBase* pPort, const size_t position) const;

			void SetV(const model::OutputPortBase* pPort, DataNode* pNode);
			DataNode* GetV(const model::OutputPortBase* pPort) const;

		private:
			std::unordered_map<const model::OutputPortBase*, std::shared_ptr<OutputPortDataNodes>> _scalarMap;
			std::unordered_map<const model::OutputPortBase*, DataNode*> _vectorMap;
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
			void ProcessSumNode(const model::Node& node);
			void ProcessLinearPredictorNode(const model::Node& node);

			DataFlowGraph& Graph() { return _graph; }

		private:

			template<typename DataType>
			void Process(const nodes::ConstantNode<DataType>& node);
			template<typename DataType>
			void Process(const nodes::BinaryOperationNode<DataType>& node);
			template<typename DataType>
			void Process(const model::InputNode<DataType>& node);
			template<typename DataType>
			void Process(const nodes::SumNode<DataType>& node);
			template<typename DataType> 
			void AddOutput(const model::Node& leafNode);			
			void ProcessLinearPredictorV(const nodes::LinearPredictorNode& node);

			DataNode* GetSourceNode(const model::InputPortBase* pPort, size_t elementIndex) const;
			void AddDependency(const model::InputPortBase* pPort, size_t elementIndex, DataNode* pDependant);
			void AddDependencyV(const model::InputPortBase* pPort, DataNode* pDependant);
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

