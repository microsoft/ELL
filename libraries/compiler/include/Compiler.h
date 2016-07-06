#pragma once

#include "ModelGraph.h"
#include "Node.h"
#include "InputNode.h"
#include "BinaryOperationNode.h"

namespace emll
{
	namespace compiler
	{
		/// <summary>The EMLL compiler</summary>
		class Compiler
		{
		public:

			static model::Port::PortType GetNodeDataType(model::Node* pNode);
		};
	}
}
