#include "Dataflow.h"
#include "ScalarVar.h"
#include "Compiler.h"

namespace emll
{
	namespace compiler
	{
		void DataNode::Process(DataFlowGraph& graph, Compiler& compiler)
		{
			Variable* pResult = OnProcess(graph, compiler);
			if (pResult != nullptr)
			{
				NotifyDependencies(graph, compiler, *pResult);
				OnProcessComplete(graph, compiler, *pResult);
			}
		}

		void DataNode::AddDependent(DataNode* pNode)
		{
			assert(pNode != nullptr);
			_dependencies.push_back(pNode);
		}

		void DataNode::OnProcessComplete(DataFlowGraph& graph, Compiler& compiler, Variable& varResult)
		{
			compiler.FreeVar(varResult);
		}

		void DataNode::NotifyDependencies(DataFlowGraph& graph, Compiler& compiler, Variable& varResult)
		{
			for (size_t i = 0; i < _dependencies.size(); ++i)
			{
				_dependencies[i]->ReceiveData(graph, compiler, varResult);
			}
		}

		DataNode* DataFlowGraph::GetNodeAt(size_t offset) const
		{
			return _nodes[offset].get();
		}

		Variable* DataFlowGraph::AddLocalScalarVariable(ValueType type)
		{
			switch (type)
			{
				case ValueType::Double:
					return AddVariable<ScalarF>(VariableScope::Local);
				default:
					throw new CompilerException(CompilerError::valueTypeNotSupported);
			}
		}

		BinaryNode* DataFlowGraph::AddBinary(OperatorType op)
		{
			return AddNode<BinaryNode>(op);
		}

		BinaryNode::BinaryNode(OperatorType op)
			: _op(op)
		{
		}

		void BinaryNode::ReceiveData(DataFlowGraph& graph, Compiler& compiler, Variable& data)
		{
			if (_pVar1 == nullptr)
			{
				_pVar1 = &data;
			}
			else if (_pVar2 == nullptr)
			{
				_pVar2 = &data;
			}
			
			if (_pVar1 != nullptr && _pVar2 != nullptr)
			{			
				assert(_pVar1->Type() == _pVar2->Type());
				_pResult = graph.AddLocalScalarVariable(_pVar1->Type());
				Process(graph, compiler);
			}
		}

		Variable* BinaryNode::OnProcess(DataFlowGraph& graph, Compiler& compiler)
		{
			// TODO: Handle Constant folding
			compiler.Compile(*this);
			return _pResult;
		}

		LiteralNode::LiteralNode(Variable* pVar)
			: _pVar(pVar)
		{
			assert(pVar != nullptr);
		}

		Variable* LiteralNode::OnProcess(DataFlowGraph& graph, Compiler& compiler)
		{
			compiler.Compile(*this);
			return _pVar;
		}

	}
}