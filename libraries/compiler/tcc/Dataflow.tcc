#include <type_traits>
#include "ScalarVar.h"

namespace emll
{
	namespace compiler
	{
		template <typename NodeType, typename... Args>
		NodeType* DataFlowGraph::AddNode(Args&&... args)
		{
			static_assert(std::is_base_of<DataNode, NodeType>::value, "AddNode requires you inherit from DataNode");
			
			auto node = std::make_shared<NodeType>(args...);
			NodeType* pNode = node.get();
			_nodes.push_back(node);
			return pNode;
		}		

		template <typename DataType>
		LiteralNode* DataFlowGraph::AddLiteral(DataType value)
		{
			Variable* pVar = _variables.AddVariable<LiteralVar<DataType>>(value);
			LiteralNode* pNode = AddNode<LiteralNode>(pVar);
			_literals.push_back(pNode);
			return pNode;
		}

		template <typename DataType>
		ArgNode* DataFlowGraph::AddArg(size_t size, bool isInput)
		{
			Variable* pVar = nullptr;
			VariableScope scope = isInput ? VariableScope::Input : VariableScope::Output;
			if (size > 1)
			{
				pVar = _variables.AddVariable<VectorVar<DataType>>(scope, size);
			}
			else
			{
				pVar = _variables.AddVariable<ScalarVar<DataType>>(scope);
			}
			ArgNode* pNode = AddNode<ArgNode>(pVar);
			if (isInput)
			{
				_inputs.push_back(pNode);
			}
			else
			{
				_outputs.push_back(pNode);
			}
			return pNode;
		}
	}
}