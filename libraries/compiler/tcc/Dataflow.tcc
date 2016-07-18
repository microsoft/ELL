#include <type_traits>

namespace emll
{
	namespace compiler
	{
		template <typename NodeType, typename... Args>
		NodeType* DataFlowGraph::AddNode(Args&&... args)
		{
			static_assert(std::is_base_of<DataNode, NodeType>::value, "DataFlowGraph requires you inherit from DataFlowNode");
			
			auto node = std::make_unique<NodeType>(args...);
			NodeType* pNode = node.get();
			_nodes.push_back(std::move(node));
			return pNode;
		}
	}
}