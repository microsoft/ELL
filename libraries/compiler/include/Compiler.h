////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Compiler.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ModelGraph.h"
#include "Node.h"
#include "InputNode.h"
#include "BinaryOperationNode.h"
#include "ConstantNode.h"
#include "Types.h"
#include "SymbolTable.h"
#include <functional>

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
				input,
				constant,
				binaryOp
			};

		public:
			Compiler();
			virtual ~Compiler() = default;

			/// <summary>Compile the given model</summary>
			virtual void Compile(const model::Model& model);

			virtual void CompileLinear(const model::Node& modelRoot, const std::string& functionName);

			/// <summary>Return the type of the given node</summary>
			NodeType GetNodeType(const model::Node& node) const;

		protected:
			virtual void Begin() = 0;
			virtual void End() = 0;			
			virtual void BeginFunction(const std::string& functionName, const VariableDecl& input, const VariableDecl& output) = 0;
			virtual void EndFunction() = 0;

			virtual void CompileInput(const model::Node& input);
			
			virtual void InitSupportedNodeTypes();
			
			void VerifyInputType(const model::Node& node, const model::Port::PortType type);
			void VerifyOutputType(const model::Node& node, const model::Port::PortType type);

		private:
			void Reset();

		private:
			SymbolTable<NodeType, NodeType::unsupported> _nodeTypes;
			std::vector<const model::Node*> _inputs;
			std::vector<const model::Node*> _outputs;
		};

		class ModelEx
		{
		public:
			/// <summary>Return the data type of the given node</summary>
			static model::Port::PortType GetNodeDataType(const model::Node& node);

			/// <summary>Collect the outputNode for the given model</summary>
			static std::vector<const model::Node*> CollectInputNodes(const model::Model& model);

			/// <summary>Collect the outputNode for the given model</summary>
			static std::vector<const model::Node*>  CollectOutputNodes(const model::Model& model);
			
			static std::vector<const model::Node*> CollectNodes(const model::Model& model, std::function<bool (const model::Node& node)> predicate);

			static size_t CountOutputs(std::vector<const model::Node*>& nodes);
			static size_t CountInputs(std::vector<const model::Node*>& nodes);

			/// <summary>Returns true if a node is a leaf node</summary>
			static bool IsLeafNode(const model::Node& node);
		};
	}
}
