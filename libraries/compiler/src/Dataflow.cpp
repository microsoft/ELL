#include "Dataflow.h"
#include "ScalarVar.h"

namespace emll
{
	namespace compiler
	{
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