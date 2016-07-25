#pragma once

#include "Dataflow.h"

namespace emll
{
	namespace compiler
	{
		class DataFlowGraph
		{
		public:

			template <class NodeType, typename... Args>
			NodeType* AddNode(Args&&... args);
			template <typename DataType>
			LiteralNode* AddLiteral(DataType value);
			template <typename DataType>
			LiteralNode* AddLiteralV(const std::vector<DataType>& data);
			template <typename DataType>
			ArgNode* AddArg(size_t size, bool isInput);

			size_t Size() const { return _nodes.size(); }
			DataNode* GetNodeAt(size_t offset) const;

			VariableAllocator& Variables() { return _variables; }

			const std::vector<DataNode*>& Literals() { return _literals; }
			const std::vector<ArgNode*>& InputArgs() { return _inputs; }
			const std::vector<ArgNode*>& OutputArgs() { return _outputs; }

		private:
			// The data flow graph owns all pointers
			std::vector<std::shared_ptr<DataNode>> _nodes;
			VariableAllocator _variables;
			std::vector<DataNode*> _literals;
			std::vector<ArgNode*> _inputs;
			std::vector<ArgNode*> _outputs;
		};
	}
}
#include "../tcc/DataFlowGraph.tcc"
