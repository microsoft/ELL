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

		template <class VarType, typename... Args>
		VarType* DataFlowGraph::AddVariable(Args&&... args)
		{
			static_assert(std::is_base_of<Variable, VarType>::value, "AddVariable requires you inherit from Variable");

			auto var = std::make_shared<VarType>(args...);
			VarType* pVar = var.get();
			_variables.push_back(var);
			return pVar;
		}

		template <class DataType>
		LiteralNode* DataFlowGraph::AddLiteral(DataType value)
		{
			Variable* pVar = AddVariable<LiteralVar<DataType>>(value);
			LiteralNode* pNode = AddNode<LiteralNode>(pVar);
			return pNode;
		}
	}
}