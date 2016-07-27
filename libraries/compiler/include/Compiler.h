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

			void CompileGraph(const std::string& functionName, DataFlowGraph& graph);

			virtual void Compile(LiteralNode& node) = 0;
			virtual void Compile(BinaryNode& node) = 0;
			virtual void Compile(InputNode& node) = 0;
			virtual void Compile(OutputNode& node) = 0;
			virtual void Compile(SumNode& node) = 0;
			virtual void Compile(DotProductNodeV& node) = 0;

			void CompileConstant(const model::Node& node);
			virtual void CompileConstant(const nodes::ConstantNode<double>& node) = 0;
			virtual void CompileConstant(const nodes::ConstantNode<int>& node) = 0;

			void CompileBinaryNode(const model::Node& node);
			virtual void CompileBinaryNode(const nodes::BinaryOperationNode<double>& node) = 0;
			virtual void CompileBinaryNode(const nodes::BinaryOperationNode<int>& node) = 0;

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
			ValueType ToValueType(model::Port::PortType type);

		private:
			void Reset();

		private:
			EmittedVarAllocator _inputVars;
			EmittedVarAllocator _outputVars;
			EmittedVarAllocator _literalVars;
			EmittedVarAllocator _localVars;
			EmittedVarAllocator _globalVars;

			VariableAllocator _variables;
			std::unordered_map<const model::OutputPortBase*, Variable*> _portToVarMap;
		};
	}
}
