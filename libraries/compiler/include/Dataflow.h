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
			ArgNode,
			LiteralNode,
			InputNode,
			BinaryNode,
		};

		class Compiler;
		class DataFlowGraph;

		class DataNode
		{
		public:

			virtual void Process(DataFlowGraph& graph, Compiler& compiler);

			virtual void ReceiveData(DataFlowGraph& graph, Compiler& compiler, Variable& data) {}

			virtual DataNodeType Type() const = 0;

			bool HasDependencies() const
			{
				return (_dependencies.size() > 0);
			}

			const std::vector<DataNode*>& Dependencies()
			{
				return _dependencies;
			}

			void AddDependent(DataNode* pNode);
		
		protected:
			virtual Variable* OnProcess(DataFlowGraph& graph, Compiler& compiler) = 0;
		
		private:
			std::vector<DataNode*> _dependencies;
		};

		class ArgNode : public DataNode
		{
		public:
			ArgNode(Variable* pVar);

			virtual DataNodeType Type() const override
			{
				return DataNodeType::ArgNode;
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
			virtual Variable* OnProcess(DataFlowGraph& graph, Compiler& compiler) override;

		private:
			Variable* _pVar;
		};

		class InputNode : public DataNode
		{
		public:
			InputNode(int elementIndex);

			virtual DataNodeType Type() const override
			{
				return DataNodeType::InputNode;
			}
			Variable* Var()
			{
				return _pVar;
			}

			virtual void ReceiveData(DataFlowGraph& graph, Compiler& compiler, Variable& data) override;

		protected:
			virtual Variable* OnProcess(DataFlowGraph& graph, Compiler& compiler) override;

		private:
			int _elementIndex;
			Variable* _pVar;
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
			Variable* Var()
			{
				return _pResult;
			}
			Variable* Src1() const
			{
				return _pSrc1;
			}
			Variable* Src2() const
			{
				return _pSrc2;
			}

		protected:
			virtual Variable* OnProcess(DataFlowGraph& graph, Compiler& compiler) override;

		private:
			OperatorType _op;
			Variable* _pSrc1 = nullptr;
			Variable* _pSrc2 = nullptr;
			Variable* _pResult = nullptr;
		};

		class DataFlowGraph
		{
		public:

			template <class NodeType, typename... Args>
			NodeType* AddNode(Args&&... args);

			template <typename DataType>
			LiteralNode* AddLiteral(DataType type);

			template <typename DataType>
			ArgNode* AddArg(size_t size);

			size_t Size() const { return _nodes.size(); }
			DataNode* GetNodeAt(size_t offset) const;

			template <typename VarType, typename... Args>
			VarType* AddVariable(Args&&... args);
			
			Variable* AddLocalScalarVariable(ValueType type);
			Variable* AddVectorElementVariable(ValueType type, Variable& src, int offset);
			
			const std::vector<DataNode*>& Literals() { return _literals;}
			const std::vector<DataNode*>& Arguments() { return _args; }

		private:
			// The data flow graph owns all pointers
			std::vector<std::shared_ptr<DataNode>> _nodes;
			std::vector<std::shared_ptr<Variable>> _variables;
			std::vector<DataNode*> _literals;
			std::vector<DataNode*> _args;
		};
	}
}
#include "../tcc/Dataflow.tcc"
