////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Compiler.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ModelEx.h"
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

			EmittedVar AllocTemp();
			void FreeTemp(EmittedVar var);
			EmittedVar AllocGlobal();
			void FreeGlobal(EmittedVar var);

		protected:
			virtual void BeginMain(const std::string& functionName, NamedValueTypeList& args) = 0;
			virtual void EndMain() = 0;
			virtual void Compile(LiteralNode& node) = 0;

			virtual void InitSupportedNodeTypes();
			
			void AddArgs(NamedValueTypeList& args, const std::string& namePrefix, const std::vector<const model::Node*>& nodes);
			void AddArgs(NamedValueTypeList& args, const std::string& name, const model::OutputPortBase* pOutput);

			std::string MakeVarName(const std::string& namePrefix, size_t i);

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
			EmittedVarAllocator _tempVars;
			EmittedVarAllocator _globalVars;
		};
	}
}
