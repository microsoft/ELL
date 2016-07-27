#include "Dataflow.h"
#include "ScalarVar.h"
#include "VectorVar.h"
#include "Compiler.h"
#include "DataFlowGraph.h"

namespace emll
{
	namespace compiler
	{
		bool DataNode::ShouldProcess(DataFlowGraph& graph, Compiler& compiler)
		{
			return (_dependencies.size() > 0);
		}

		void DataNode::Process(DataFlowGraph& graph, Compiler& compiler)
		{
			if (!ShouldProcess(graph, compiler))
			{
				return;
			}

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

		LiteralNode::LiteralNode(Variable* pVar)
			: _pVar(pVar)
		{
			assert(pVar != nullptr && (pVar->IsLiteral() || (pVar->IsGlobal() && !pVar->IsMutable())));
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
			_pVar = graph.Variables().AddVectorElementVariable(data.Type(), data, _elementIndex);
			Process(graph, compiler);
		}

		Variable* InputNode::OnProcess(DataFlowGraph& graph, Compiler& compiler)
		{
			compiler.Compile(*this);
			return _pVar;
		}

		OutputNode::OutputNode(Variable* pDestVar,  int elementIndex)
			: _pDestVar(pDestVar), _elementIndex(elementIndex)
		{
		}

		void OutputNode::ReceiveData(DataFlowGraph& graph, Compiler& compiler, Variable& data)
		{
			assert(data.IsScalar());
			_pVar = &data;
			Process(graph, compiler);
		}

		Variable* OutputNode::OnProcess(DataFlowGraph& graph, Compiler& compiler)
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
			else
			{
				_pSrc2 = &data;
				Process(graph, compiler);
			}
		}

		Variable* BinaryNode::OnProcess(DataFlowGraph& graph, Compiler& compiler)
		{
			assert(_pSrc1->Type() == _pSrc2->Type());

			_pResult = _pSrc1->Combine(graph.Variables(), *_pSrc2, _op);
			// If we have a computed result, we'll postpone actual code generation
			if (_pResult == nullptr || !_pResult->IsComputed())
			{
				_pResult = graph.Variables().AddLocalScalarVariable(_pSrc1->Type());
				compiler.Compile(*this);
			}
			return _pResult;
		}

		SumNode::SumNode(OperatorType op, size_t dim)
			: _op(op), _dim(dim)
		{

		}

		void SumNode::ReceiveData(DataFlowGraph& graph, Compiler& compiler, Variable& data)
		{
			if (_count == 0)
			{
				_pTotal = graph.Variables().AddLocalScalarVariable(data.Type());
			}
			_pLast = &data;
			_count++;
			if (_count == _dim)
			{
				Process(graph, compiler);
			}
		}

		Variable* SumNode::OnProcess(DataFlowGraph& graph, Compiler& compiler)
		{
			return _pTotal;
		}

		DotProductNodeV::DotProductNodeV()
		{
		}

		void DotProductNodeV::ReceiveData(DataFlowGraph& graph, Compiler& compiler, Variable& data)
		{
			if (_pSrc1 == nullptr)
			{
				_pSrc1 = &data;
			}
			else
			{
				_pSrc2 = &data;
				Process(graph, compiler);
			}
		}

		Variable* DotProductNodeV::OnProcess(DataFlowGraph& graph, Compiler& compiler)
		{
			assert(_pSrc1->Type() == _pSrc2->Type() && _pSrc1->Dimension() == _pSrc2->Dimension());

			_pResult = graph.Variables().AddLocalScalarVariable(_pSrc1->Type());
			compiler.Compile(*this);

			return _pResult;
		}
	}
}