#include "Compiler.h"

namespace emll
{
	namespace compiler
	{
		model::Port::PortType Compiler::GetNodeDataType(model::Node* pNode)
		{
			assert(pNode != nullptr);
			return pNode->GetOutputs()[0]->GetType();
		}
	}
}