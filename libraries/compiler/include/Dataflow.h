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
			
			virtual void ReceiveData(DataFlowGraph& graph, Compiler& compiler, Variable& data) {}

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
		protected:
			virtual Variable* OnProcess(DataFlowGraph& graph, Compiler& compiler);

		private:
			Variable* _pVar;
		};

		class InputNode : public DataNode
		{
		public:
		};

		class BinaryNode : public DataNode
		{
		public:
			BinaryNode(OperatorType op);

			virtual DataNodeType Type() const override
			{
				return DataNodeType::BinaryNode;
			}

			virtual void ReceiveData(DataFlowGraph& graph, Compiler& compiler, Variable& data) override;

			OperatorType Op() const
			{
				return _op;
			}
			Variable& Var1() const
			{
				return *_pVar1;
			}
			Variable& Var2() const
			{
				return *_pVar2;
			}
			Variable& Result() const
			{
				return *_pResult;
			}
		protected:
			virtual Variable* OnProcess(DataFlowGraph& graph, Compiler& compiler) override;

		private:
			OperatorType _op;
			Variable* _pVar1 = nullptr;
			Variable* _pVar2 = nullptr;
			Variable* _pResult = nullptr;
		};

		class DataFlowGraph
		{
		public:

			template <class NodeType, typename... Args>
			NodeType* AddNode(Args&&... args);

			template <typename DataType>
			LiteralNode* AddLiteral(DataType type);

			BinaryNode* AddBinary(OperatorType op);

			size_t Size() const { return _nodes.size(); }
			DataNode* GetNodeAt(size_t offset) const;

			template <typename VarType, typename... Args>
			VarType* AddVariable(Args&&... args);
			
			Variable* AddLocalScalarVariable(ValueType type);

			std::vector<DataNode*>& Literals() { return _literals;}

		private:
			// The data flow graph owns all pointers
			std::vector<std::shared_ptr<DataNode>> _nodes;
			std::vector<std::shared_ptr<Variable>> _variables;
			std::vector<DataNode*> _literals;
		};
	}
}
#include "../tcc/Dataflow.tcc"
