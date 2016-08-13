////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ModelEx.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ModelGraph.h"
#include "Node.h"
#include "InputNode.h"
#include "OutputPort.h"
#include "OutputPortElements.h"
#include <functional>

namespace emll
{
	namespace compiler
	{
		///<summary>model extension methods used by the compiler</summary>
		class ModelEx
		{
		public:
			
			/// <summary>Return the data type of the given node</summary>
			static model::Port::PortType GetNodeDataType(const model::Node& node);

			/// <summary>Return the source node</summary>
			static const model::Node* GetSourceNode(const model::OutputPortElement& elt);

			/// <summary>Returns true if a node is a leaf node</summary>
			static bool IsLeafNode(const model::Node& node);
			
			/// <summary>True if port has dimension greater than 1, and references exactly one output port</summary>
			static bool IsPureVector(const model::InputPortBase& port);

			/// <summary>True if a node has 2 input ports, each with exactly 1 element</summary>
			static bool IsPureBinary(const model::Node& node);

			/// <summary>Returns true if this port is scalar</summary>
			static bool IsScalar(const model::InputPortBase& port);

			/// <summary>Returns true if this port is scalar</summary>
			static bool IsScalar(const model::OutputPortBase& port);

			/// <summary>Does this node have a single descendant?</summary>
			static bool HasSingleDescendant(const model::Node& node);

			/// <summary>Does this node have a single descendant?</summary>
			static bool HasSingleDescendant(const model::OutputPortElement& elt);
		};
	}
}