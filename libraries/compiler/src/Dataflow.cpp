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
				NotifyDependencies(compiler, pResult);
			}
		}

		void DataNode::NotifyDependencies(Compiler& compiler, Variable* pResult)
		{
			for (size_t i = 0; i < _dependencies.size(); ++i)
			{
				_dependencies[i]->Process(compiler, pResult);
			}
		}

		void DataNode::ReleaseVariable(Compiler& compiler, Variable* pResult)
		{
			if (pResult->IsScalar())
			{
				if (pResult->Scope() == VariableScope::Local)
				{
					compiler.FreeTemp(pResult->GetAssignedVar());
				}
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