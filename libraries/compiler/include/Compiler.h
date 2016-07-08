#pragma once

#include "ModelGraph.h"
#include "Node.h"
#include "InputNode.h"
#include "BinaryOperationNode.h"
#include "ConstantNode.h"
#include "SymbolTable.h"

namespace emll
{
	namespace compiler
	{
		/// <summary>The EMLL compiler</summary>
		class Compiler
		{
		public:
			/// <summary>Map node names to NodeTypes the compiler knows how to compile</summary>
			enum class NodeType
			{
				unsupported,
				constant,
				binaryOp
			};

		public:
			Compiler();
			virtual ~Compiler() = default;

			/// <summary>Compile the given model</summary>
			void Compile(model::Node& modelRoot);


			/// <summary>Return the data type of the given node</summary>
			model::Port::PortType GetNodeDataType(model::Node& node);
			
			NodeType GetNodeType(model::Node& node);

		protected:
			virtual void Begin() {}
			virtual void End() {}
			virtual void InitSupportedNodeTypes();
				
		private:
			SymbolTable<NodeType, NodeType::unsupported> _nodeTypes;
		};
	}
}
