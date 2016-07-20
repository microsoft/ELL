#include "Dataflow.h"
#include "ScalarVar.h"
#include "Compiler.h"

namespace emll
{
	namespace compiler
	{
		void DataNode::Process(Compiler& compiler)
		{
			Variable* pResult = OnProcess(compiler);
			if (pResult != nullptr)
			{
				NotifyDependencies(compiler, *pResult);
				OnProcessComplete(compiler, *pResult);
			}
		}

		void DataNode::OnProcessComplete(Compiler& compiler, Variable& varResult)
		{
			compiler.FreeVar(varResult);
		}

		void DataNode::NotifyDependencies(Compiler& compiler, Variable& varResult)
		{
			for (size_t i = 0; i < _dependencies.size(); ++i)
			{
				_dependencies[i]->Process(compiler, varResult);
			}
		}

		DataNode* DataFlowGraph::GetNodeAt(size_t offset) const
		{
			return _nodes[offset].get();
		}

		BinaryNode::BinaryNode()
		{
		}

		BinaryNode::~BinaryNode()
		{
		}

		LiteralNode::LiteralNode(double value)
		{
			_pVar = std::make_unique<InitializedScalarF>(VariableScope::Local, value);
		}
	}
}