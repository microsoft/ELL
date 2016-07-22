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
				for (size_t i = 0; i < _dependencies.size(); ++i)
				{
					_dependencies[i]->ReceiveData(graph, compiler, *pResult);
				}
				compiler.FreeVar(*pResult);
			}
		}

		void DataNode::AddDependent(DataNode* pNode)
		{
			assert(pNode != nullptr);
			_dependencies.push_back(pNode);
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

		Variable* DataFlowGraph::AddVectorElementVariable(ValueType type, Variable& src, int offset)
		{
			switch (type)
			{
				case ValueType::Double:
					return AddVariable<VectorElementVar<double>>(src, offset);
				default:
					throw new CompilerException(CompilerError::valueTypeNotSupported);
			}
		}


		LiteralNode::LiteralNode(Variable* pVar)
			: _pVar(pVar)
		{
			assert(pVar != nullptr && pVar->IsLiteral());
		}

		Variable* LiteralNode::OnProcess(DataFlowGraph& graph, Compiler& compiler)
		{
			compiler.Compile(*this);
			return _pVar;
		}

		ArgNode::ArgNode(Variable *pVar)
			: _pVar(pVar)
		{
		}

		Variable* ArgNode::OnProcess(DataFlowGraph& graph, Compiler& compiler)
		{
			return _pVar;
		}

		InputNode::InputNode(int elementIndex)
			: _elementIndex(elementIndex)
		{
		}

		void InputNode::ReceiveData(DataFlowGraph& graph, Compiler& compiler, Variable& data)
		{
			assert(data.IsVector());
			_pVar = graph.AddVectorElementVariable(data.Type(), data, _elementIndex);
			Process(graph, compiler);
		}

		Variable* InputNode::OnProcess(DataFlowGraph& graph, Compiler& compiler)
		{
			compiler.Compile(*this);
			return _pVar;
		}

		BinaryNode::BinaryNode(OperatorType op)
			: _op(op)
		{
		}

		void BinaryNode::ReceiveData(DataFlowGraph& graph, Compiler& compiler, Variable& data)
		{
			if (_pSrc1 == nullptr)
			{
				_pSrc1 = &data;
			}
			else if (_pSrc2 == nullptr)
			{
				_pSrc2 = &data;
			}

			if (_pSrc1 != nullptr && _pSrc2 != nullptr)
			{
				assert(_pSrc1->Type() == _pSrc2->Type());
				_pResult = graph.AddLocalScalarVariable(_pSrc1->Type());
				Process(graph, compiler);
			}
		}

		Variable* BinaryNode::OnProcess(DataFlowGraph& graph, Compiler& compiler)
		{
			compiler.Compile(*this);
			return _pResult;
		}
	}
}