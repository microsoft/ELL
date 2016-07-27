#pragma once
#include "Compiler.h"
#include "IRInclude.h"
#include "ScalarVar.h"
#include "VectorVar.h"
#include <stdio.h>

namespace emll
{
	namespace compiler
	{
		class IRCompiler : public Compiler
		{
		public:
			IRCompiler(const std::string& moduleName, std::ostream& os);

			virtual void Compile(LiteralNode& node) override;
			virtual void Compile(BinaryNode& node) override;
			virtual void Compile(InputNode& node) override;
			virtual void Compile(OutputNode& node) override;
			virtual void Compile(SumNode& node) override;
			virtual void Compile(DotProductNodeV& node) override;

			llvm::Value* GetEmittedVariable(const VariableScope scope, const std::string& name);

			llvm::Value* EnsureEmitted(Variable* pVar);
			llvm::Value* EnsureEmitted(Variable& var);

			void DebugDump();

			virtual void BeginFunction(const std::string& functionName, NamedValueTypeList& args) override;
			virtual void BeginFunction(const std::string& functionName, DataFlowGraph& graph) override;
			virtual void EndFunction() override;

		public:
		
		private:
			void AddFunctionArgs(DataFlowGraph& graph, NamedValueTypeList& args);
			void AddFunctionArgs(const std::vector<ArgNode*>& argNodes, NamedValueTypeList& fnArgs);
			void RegisterFunctionArgs(NamedValueTypeList& args);

			llvm::Value* Emit(Variable& var);

			llvm::Value* EmitScalar(Variable& var);
			llvm::Value* EmitLiteral(Variable& var);
			llvm::Value* EmitLocalScalar(Variable& var);
			llvm::Value* EmitGlobalScalar(Variable& var);
			llvm::Value* EmitVectorRef(Variable& var);
			void ApplyComputed(Variable& var, llvm::Value* pDest);

			template<typename T>
			llvm::Value* EmitLiteral(LiteralVar<T>& var);
			template<typename T>
			llvm::Value* EmitLocal(ScalarVar<T>& var);
			template<typename T>
			llvm::Value* EmitLocal(InitializedScalarVar<T>& var);
			template<typename T>
			llvm::Value* EmitRef(VectorElementVar<T>& var);
			template<typename T>
			llvm::Value* EmitGlobal(InitializedScalarVar<T>& var);
			template<typename T>
			void ApplyComputed(ComputedVar<T>& var, llvm::Value* pDest);

			llvm::Value* EmitVector(Variable& var);
			llvm::Value* EmitLiteralV(Variable& var);

			template<typename T>
			llvm::Value* EmitLiteralV(LiteralVarV<T>& var);

			llvm::Value* LoadVar(Variable* pVar);
			llvm::Value* LoadVar(Variable& var);

		private:
			std::ostream& _os;
			IREmitter _emitter;
			IRModuleEmitter _module;
			IRFunctionEmitter _fn;
			IRVariableTable _literals;
			IRVariableTable _locals;
			IRVariableTable _globals;
		};
	}
}

#include "../tcc/IRCompiler.tcc"