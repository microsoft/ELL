#include "Dataflow.h"
#include "ScalarVar.h"

namespace emll
{
	namespace compiler
	{
		BinaryNode::BinaryNode()
		{
		}

		BinaryNode::~BinaryNode()
		{
		}

		LiteralNode::LiteralNode(double value)
		{
			_pVar = std::make_unique<GlobalScalarF>(value);
		}
	}
}