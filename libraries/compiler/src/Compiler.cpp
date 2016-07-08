#include "Compiler.h"

namespace emll
{
	namespace compiler
	{
		Compiler::Compiler()
			: _nodeTypes()
		{
			InitSupportedNodeTypes();
		}

		model::Port::PortType Compiler::GetNodeDataType(model::Node& node)
		{
			return node.GetOutputs()[0]->GetType();
		}

		Compiler::NodeType Compiler::GetNodeType(model::Node& node)
		{
			return _nodeTypes.Get(node.GetRuntimeTypeName());
		}

		void Compiler::InitSupportedNodeTypes()
		{
			_nodeTypes.Init	({
				{"ConstantNode", NodeType::constant},
				{"BinaryOperationNode", NodeType::binaryOp}
			});
		}
	}
}