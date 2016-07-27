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
#include "Variable.h"
#include "ScalarVar.h"
#include "VectorVar.h"
#include "DataFlowGraph.h"
#include "DataFlowBuilder.h"

#include "ConstantNode.h"
#include "InputNode.h"
#include "OutputNode.h"
#include "BinaryOperationNode.h"
#include "DotProductNode.h"

#include <functional>

namespace emll
{
	namespace compiler
	{
		/// <summary>The EMLL compiler</summary>
		class Compiler
		{
		public:

		public:
			Compiler();
			virtual ~Compiler() = default;

			bool& ShouldUnrollLoops() { return _unrollLoops; }

			void CompileGraph(const std::string& functionName, DataFlowGraph& graph);

			virtual void Compile(LiteralNode& node) = 0;
			virtual void Compile(BinaryNode& node) = 0;
			virtual void Compile(InputNode& node) = 0;
			virtual void Compile(OutputNode& node) = 0;
			virtual void Compile(SumNode& node) = 0;
			virtual void Compile(DotProductNodeV& node) = 0;

			void CompileModel(const std::string& functionName, model::Model& model);

			virtual void CompileConstantNode(const model::Node& node) = 0;
			virtual void CompileInputNode(const model::Node& node) = 0;
			virtual void CompileOutputNode(const model::Node& node) = 0;
			virtual void CompileBinaryNode(const model::Node& node) = 0;
			virtual void CompileDotProductNode(const model::Node& node) = 0;

			void BeginFunctionPredict();
			virtual void BeginFunction(const std::string& functionName, NamedValueTypeList& args) = 0;
			virtual void BeginFunction(const std::string& functionName, DataFlowGraph& graph) = 0;
			virtual void EndFunction() = 0;

			void AllocVar(Variable& var);
			void FreeVar(Variable& var);

			VariableAllocator& Variables() { return _variables; }
			Variable* AllocVar(model::OutputPortBase* pPort);
			Variable* GetVariableFor(const model::OutputPortBase* pPort);
			Variable* GetVariableFor(const model::OutputPortElement elt);
			void SetVariableFor(const model::OutputPortBase* pPort, Variable* pVar);

		protected:
			bool IsNodeType(const std::string& nodeTypeName , const std::string& typeName);
			ValueType ToValueType(model::Port::PortType type);
			void CollectInputsAndOutputs(model::Model& model);


		private:
			Variable* AllocArg(const model::OutputPortBase* pPort, bool isInput);
			void Reset();
			
		private:
			EmittedVarAllocator _inputVars;
			EmittedVarAllocator _outputVars;
			EmittedVarAllocator _literalVars;
			EmittedVarAllocator _localVars;
			EmittedVarAllocator _globalVars;

			VariableAllocator _variables;
			NamedValueTypeList _args;
			std::unordered_map<const model::OutputPortBase*, Variable*> _portToVarMap;

			//
			// Compiler settings
			//
			bool _unrollLoops = false;
		};
	}
}
