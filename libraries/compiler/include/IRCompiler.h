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

			virtual void CompileConstantNode(const model::Node& node) override;
			virtual void CompileInputNode(const model::Node& node) override;
			virtual void CompileOutputNode(const model::Node& node) override;
			virtual void CompileBinaryNode(const model::Node& node) override;
			virtual void CompileDotProductNode(const model::Node& node) override;
			virtual void CompileSumNode(const model::Node& node) override;
			virtual void CompileAccumulatorNode(const model::Node& node) override;
			virtual void CompileDelayNode(const model::Node& node) override;

			void DebugDump();

			IRModuleEmitter& Module() { return _module; }

			virtual void BeginFunction(const std::string& functionName, NamedValueTypeList& args) override;
			virtual void EndFunction() override;
			
		private:
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
			llvm::Value* EmitLiteralVector(Variable& var);
			llvm::Value* EmitGlobalVector(Variable& var);

			template<typename T>
			llvm::Value* EmitGlobalVector(VectorVar<T>& var);
			template<typename T>
			llvm::Value* EmitGlobalVector(InitializedVectorVar<T>& var);
			template<typename T>
			llvm::Value* EmitLiteralVector(LiteralVarV<T>& var);

			llvm::Value* GetEmittedVariable(const VariableScope scope, const std::string& name);
			llvm::Value* EnsureEmitted(Variable* pVar);
			llvm::Value* EnsureEmitted(Variable& var);
			llvm::Value* EnsureEmitted(model::OutputPortBase* pPort);
			llvm::Value* EnsureEmitted(model::OutputPortElement elt);
			llvm::Value* EnsureEmitted(model::InputPortBase* pPort);

			llvm::Value* LoadVar(Variable* pVar);
			llvm::Value* LoadVar(Variable& var);
			llvm::Value* LoadVar(const model::OutputPortElement elt);
			void SetVar(Variable& var, llvm::Value* pDest, int offset, llvm::Value* pValue);
			
			template<typename T>
			void Compile(const nodes::ConstantNode<T>& node);
			template<typename T>
			void Compile(const model::OutputNode<T>& node);
			template<typename T>
			
			void Compile(const nodes::BinaryOperationNode<T>& node);
			template<typename T>
			void CompileLoop(const nodes::BinaryOperationNode<T>& node);
			template<typename T>
			void CompileExpanded(const nodes::BinaryOperationNode<T>& node);
			template<typename T>
			
			void Compile(const nodes::DotProductNode<T>& node);
			template<typename T>
			void CompileLoop(const nodes::DotProductNode<T>& node);
			template<typename T>
			void CompileExpanded(const nodes::DotProductNode<T>& node);
			
			template<typename T>
			void Compile(const nodes::SumNode<T>& node);
			template<typename T>
			void CompileLoop(const nodes::SumNode<T>& node);
			template<typename T>
			void CompileExpanded(const nodes::SumNode<T>& node);
			
			template<typename T>
			void Compile(const nodes::AccumulatorNode<T>& node);
			template<typename T>
			void CompileLoop(const nodes::AccumulatorNode<T>& node);
			template<typename T>
			void CompileExpanded(const nodes::AccumulatorNode<T>& node);

			template<typename T>
			void CompileDelay(const nodes::DelayNode<T>& node);

			template<typename T>
			OperatorType GetOperator(const nodes::BinaryOperationNode<T>& node);

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