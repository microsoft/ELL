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

			/// <summary>Collect the outputNode for the given model</summary>
			static std::vector<const model::Node*> CollectInputNodes(const model::Model& model);

			/// <summary>Collect the outputNode for the given model</summary>
			static std::vector<const model::Node*>  CollectOutputNodes(const model::Model& model);

			static std::vector<const model::Node*> CollectNodes(const model::Model& model, std::function<bool(const model::Node& node)> predicate);

			static size_t CountOutputs(std::vector<const model::Node*>& nodes);
			static size_t CountInputs(std::vector<const model::Node*>& nodes);

			/// <summary>Returns true if a node is a leaf node</summary>
			static bool IsLeafNode(const model::Node& node);
		};
	}
}