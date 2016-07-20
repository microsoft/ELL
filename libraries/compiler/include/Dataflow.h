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
			Literal,
			BinaryNode,
		};

		class Compiler;
		class DataNode
		{
		public:

			void Process(Compiler& compiler);
			void Process(Compiler& compiler, Variable* pVar) {}

			virtual DataNodeType Type() const = 0;

			std::vector<DataNode*>& Dependencies()
			{
				return _dependencies;
			}

		protected:
			virtual Variable* OnProcess(Compiler& compiler)
			{
				return nullptr;
			}
			virtual void OnProcessComplete(Compiler& compiler, Variable* pResult);
			void NotifyDependencies(Compiler& compiler, Variable* pResult);

		private:
			std::vector<DataNode*> _dependencies;
		};

		class DataFlowGraph
		{
		public:

			template <class NodeType, typename... Args>
			NodeType* AddNode(Args&&... args);

			DataNode* GetNodeAt(size_t offset) const;

		private:
			// The data flow graph owns all pointers
			std::vector<std::shared_ptr<DataNode>> _nodes;
		};

		class BinaryNode : public DataNode
		{
		public:
			BinaryNode();
			~BinaryNode();

		private:
			Variable* _pVar1 = nullptr;
			Variable* _pVar2 = nullptr;
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
