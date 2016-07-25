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
#include "DataFlowGraph.h"

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
			virtual void Compile(DotProductNode& node) = 0;

			void BeginFunctionPredict();
			virtual void BeginFunction(const std::string& functionName, NamedValueTypeList& args) = 0;
			virtual void BeginFunction(const std::string& functionName, DataFlowGraph& graph) = 0;
			virtual void EndFunction() = 0;

			void AllocVar(Variable& var);
			void FreeVar(Variable& var);

		private:
			void Reset();

		private:
			EmittedVarAllocator _inputVars;
			EmittedVarAllocator _outputVars;
			EmittedVarAllocator _literalVars;
			EmittedVarAllocator _localVars;
			EmittedVarAllocator _globalVars;
		};
	}
}
