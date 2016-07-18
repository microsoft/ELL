#pragma once

#include "ModelGraph.h"
#include "ConstantNode.h"
#include "Types.h"
#include "Variable.h"
#include <vector>

namespace emll
{
	namespace compiler
	{
		enum class DataNodeType
		{
			Literal,
		};

		class DataNode
		{
		public:

			virtual void Process() {}
			virtual DataNodeType Type() const = 0;
		};

		class BinaryNode
		{
		public:
			BinaryNode();
			~BinaryNode();

		private:
		};

		class DataFlowGraph
		{
		public:

			template <class NodeType, typename... Args>
			NodeType* AddNode(Args&&... args);

		private:
			// The data flow graph owns all pointers
			std::vector<std::unique_ptr<DataNode>> _nodes;
		};

		class LiteralNode : public DataNode
		{
		public:
			LiteralNode(double value);
			DataNodeType Type() const
			{
				return DataNodeType::Literal;
			}
			Variable* Var()
			{
				return _pVar.get();
			}

		private:
			std::unique_ptr<Variable> _pVar;
		};
	}
}
#include "../tcc/Dataflow.tcc"
