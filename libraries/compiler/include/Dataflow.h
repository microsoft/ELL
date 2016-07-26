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
			OutputNode,
			BinaryNode,
			SumNode,
			DotProductV		// Vector version of LinearPredictor
		};

		class Compiler;
		class DataFlowGraph;

		class DataNode
		{
		public:

			virtual void Process(DataFlowGraph& graph, Compiler& compiler);

			virtual void ReceiveData(DataFlowGraph& graph, Compiler& compiler, Variable& data) {}

			virtual DataNodeType Type() const = 0;
			virtual bool HasVectorResult() { return false; }

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
			Variable* Var() { return _pVar;}
			virtual bool HasVectorResult() override { return true; }

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
			Variable* _pVar = nullptr;
		};

		class OutputNode : public DataNode
		{
		public:
			OutputNode(Variable* pDestVar, int elementIndex);

			virtual DataNodeType Type() const override
			{
				return DataNodeType::OutputNode;
			}

			Variable* DestVar()
			{
				return _pDestVar;
			}

			int ElementIndex()
			{
				return _elementIndex;
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
			Variable* _pDestVar;
			Variable* _pVar = nullptr;
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

		class SumNode : public DataNode
		{
		public:
			SumNode(OperatorType op, size_t dim);

			virtual DataNodeType Type() const override
			{
				return DataNodeType::SumNode;
			}

			OperatorType Op() { return _op; }
			size_t Count() { return _count; }
			Variable* Last() { return _pLast; }
			Variable* Var() { return _pTotal; }

			virtual void ReceiveData(DataFlowGraph& graph, Compiler& compiler, Variable& data) override;

		protected:
			virtual Variable* OnProcess(DataFlowGraph& graph, Compiler& compiler) override;

		private:
			size_t _dim;
			size_t _count = 0;
			OperatorType _op;
			Variable* _pTotal = nullptr;
			Variable* _pLast = nullptr;
		};

		class DotProductNode : public DataNode
		{
		public:
			DotProductNode();

			virtual DataNodeType Type() const override
			{
				return DataNodeType::BinaryNode;
			}
			Variable* Var() const { return _pResult; };
			Variable* Src1() const { return _pSrc1; }
			Variable* Src2() const { return _pSrc2; }

			virtual void ReceiveData(DataFlowGraph& graph, Compiler& compiler, Variable& data) override;

		protected:
			virtual Variable* OnProcess(DataFlowGraph& graph, Compiler& compiler) override;

		private:
			Variable* _pSrc1 = nullptr;
			Variable* _pSrc2 = nullptr;
			Variable* _pResult = nullptr;
		};
	}
}
