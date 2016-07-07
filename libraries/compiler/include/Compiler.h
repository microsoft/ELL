#pragma once

#include "ModelGraph.h"
#include "Node.h"
#include "InputNode.h"
#include "BinaryOperationNode.h"
#include "ConstantNode.h"

namespace emll
{
	namespace compiler
	{
		/// <summary>The EMLL compiler</summary>
		class Compiler
		{
		public:
			Compiler() = default;
			virtual ~Compiler() = default;

			virtual void Begin() {}
			virtual void End() {}

			static model::Port::PortType GetNodeDataType(model::Node* pNode);
		};
	}
}
