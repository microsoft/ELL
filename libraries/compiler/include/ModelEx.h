#pragma once

#include "ModelGraph.h"
#include "Node.h"
#include "InputNode.h"
#include "OutputPort.h"
#include "OutputPortElementList.h"
#include "BinaryOperationNode.h"
#include "ConstantNode.h"
#include <functional>

namespace emll
{
	namespace compiler
	{
		class ModelEx
		{
		public:
			
			/// <summary>Return the data type of the given node</summary>
			static model::Port::PortType GetNodeDataType(const model::Node& node);

			/// <summary>Returns true if a node is a leaf node</summary>
			static bool IsLeafNode(const model::Node& node);

			static bool IsPureVector(const model::Node& node);
			static bool IsPureVector(const model::InputPortBase& port);
		};
	}
}