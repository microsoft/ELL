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
#include "DataFlow.h"

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
			virtual void CompileModel(const model::Model& model);
			virtual void CompileNode(DataNode& node);
			virtual void Begin() = 0;
			virtual void End() = 0;

			/// <summary>Return the type of the given node</summary>
			NodeType GetNodeType(const model::Node& node) const;

			TempVar AllocTemp();
			void FreeTemp(TempVar var);
			uint64_t AllocGlobal();

		protected:
			virtual void BeginMain(const std::string& functionName) = 0;
			virtual void EndMain() = 0;
			virtual void Compile(LiteralNode& node) = 0;

			virtual void InitSupportedNodeTypes();
			

			void VerifyInputType(const model::Node& node, const model::Port::PortType type);
			void VerifyOutputType(const model::Node& node, const model::Port::PortType type);
			const std::string& InputName() const;
			const std::string& OutputName() const;
			const std::vector<const model::Node*>& Inputs() const
			{
				return _inputs;
			}
			const std::vector<const model::Node*>& Outputs() const
			{
				return _outputs;
			}

		private:
			void Reset();

		private:
			SymbolTable<NodeType, NodeType::unsupported> _nodeTypes;
			std::vector<const model::Node*> _inputs;
			std::vector<const model::Node*> _outputs;
			TempVarAllocator _tempVars;
			uint64_t _globalVars = 0;
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
