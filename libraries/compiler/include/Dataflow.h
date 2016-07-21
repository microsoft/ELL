#pragma once

#include "Variable.h"

#include <vector>
#include <memory>

namespace emll
{
	namespace compiler
	{
		enum class DataNodeType
		{
			LiteralNode,
			BinaryNode,
		};

		class Compiler;
		class DataFlowGraph;

		class DataNode
		{
		public:

			void Process(DataFlowGraph& graph, Compiler& compiler);
			void Process(DataFlowGraph& graph, Compiler& compiler, Variable& varResult) {}

			virtual DataNodeType Type() const = 0;

			const std::vector<DataNode*>& Dependencies()
			{
				return _dependencies;
			}

			void AddDependent(DataNode* pNode);

		protected:
			virtual Variable* OnProcess(DataFlowGraph& graph, Compiler& compiler)
			{
				return nullptr;
			}
			virtual void OnProcessComplete(DataFlowGraph& graph, Compiler& compiler, Variable& varResult);
			void NotifyDependencies(DataFlowGraph& graph, Compiler& compiler, Variable& varResult);

		private:
			std::vector<DataNode*> _dependencies;
		};

		class BinaryNode : public DataNode
		{
		public:
			BinaryNode(OperatorType op);

			virtual DataNodeType Type() const override
			{
				return DataNodeType::BinaryNode;
			}

		protected:

		private:
			OperatorType _op;
			Variable* _pVar1 = nullptr;
			Variable* _pVar2 = nullptr;
		};

		class LiteralNode : public DataNode
		{
		public:
			LiteralNode(Variable* pVar);
		
			virtual DataNodeType Type() const override
			{
				return DataNodeType::LiteralNode;
			}
			Variable* Var()
			{
				return _pVar;
			}

		private:
			Variable* _pVar;
		};

		class DataFlowGraph
		{
		public:

			template <class NodeType, typename... Args>
			NodeType* AddNode(Args&&... args);

			template <typename VarType, typename... Args>
			VarType* AddVariable(Args&&... args);

			template <typename DataType>
			LiteralNode* AddLiteral(DataType type);

			BinaryNode* AddBinary(OperatorType op);

			DataNode* GetNodeAt(size_t offset) const;

		private:
			// The data flow graph owns all pointers
			std::vector<std::shared_ptr<DataNode>> _nodes;
			std::vector<std::shared_ptr<Variable>> _variables;
			std::vector<DataNode*> _literals;
		};
	}
}
#include "../tcc/Dataflow.tcc"
