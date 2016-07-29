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
			IRCompiler(const std::string& moduleName);

			virtual void CompileConstantNode(const model::Node& node) override;
			virtual void CompileInputNode(const model::Node& node) override;
			virtual void CompileOutputNode(const model::Node& node) override;
			virtual void CompileBinaryNode(const model::Node& node) override;
			virtual void CompileDotProductNode(const model::Node& node) override;
			virtual void CompileSumNode(const model::Node& node) override;
			virtual void CompileAccumulatorNode(const model::Node& node) override;
			virtual void CompileDelayNode(const model::Node& node) override;

			virtual void BeginFunction(const std::string& functionName, NamedValueTypeList& args) override;
			virtual void EndFunction() override;

			void DebugDump();
			void WriteAsmToFile(const std::string& name);
			void WriteBitcodeToFile(const std::string& name);

			IRModuleEmitter& Module() { return _module; }

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
			
			///<summary>Compile a ConstantNode</summary>
			template<typename T>
			void CompileConstant(const nodes::ConstantNode<T>& node);
			///<summary>Compile an OutputNode</summary>
			template<typename T>
			void CompileOutput(const model::OutputNode<T>& node);

			///<summary>Compile a BinaryOperationNode</summary>
			template<typename T>			
			void CompileBinary(const nodes::BinaryOperationNode<T>& node);
			///<summary>Compile a BinaryOperationNode with pure vector inputs as a loop</summary>
			template<typename T>
			void CompileBinaryLoop(const nodes::BinaryOperationNode<T>& node);
			///<summary>Compile a BinaryOperationNode as a sequence of scalar operations</summary>
			template<typename T>
			void CompileBinaryExpanded(const nodes::BinaryOperationNode<T>& node);

			///<summary>Compile a DotProductNode</summary>
			template<typename T>
			void CompileDotProduct(const nodes::DotProductNode<T>& node);
			///<summary>Compile a DotProductNode with pure vector inputs as a loop</summary>
			template<typename T>
			void CompileDotProductLoop(const nodes::DotProductNode<T>& node);
			///<summary>Compile a DotProductNode as a sequence of scalar operations</summary>
			template<typename T>
			void CompileDotProductExpanded(const nodes::DotProductNode<T>& node);
			
			///<summary>Compile a SumNode</summary>
			template<typename T>
			void CompileSum(const nodes::SumNode<T>& node);
			///<summary>Compile a SumNode with pure vector inputs as a loop</summary>
			template<typename T>
			void CompileSumLoop(const nodes::SumNode<T>& node);
			///<summary>Compile a SumNode as a sequence of scalar operations</summary>
			template<typename T>
			void CompileSumExpanded(const nodes::SumNode<T>& node);
			
			///<summary>Compile an Accumulator node</summary>
			template<typename T>
			void CompileAccumulator(const nodes::AccumulatorNode<T>& node);
			///<summary>Compile an Accumulator with pure vector inputs as a loop</summary>
			template<typename T>
			void CompileAccumulatorLoop(const nodes::AccumulatorNode<T>& node);
			///<summary>Compile an Accumulator into a sequence of scalar operations</summary>
			template<typename T>
			void CompileAccumulatorExpanded(const nodes::AccumulatorNode<T>& node);

			///<summary>Compile a DelayNode</summary>
			template<typename T>
			void CompileDelay(const nodes::DelayNode<T>& node);
			
			///<summary>Translate the binary operation operator into a more strongly typed operator for</summary>
			template<typename T>
			OperatorType GetOperator(const nodes::BinaryOperationNode<T>& node) const;

		private:
			IREmitter _emitter;				// Object that makes lower level LLVM calls
			IRModuleEmitter _module;		// The emitted IR module
			IRFunctionEmitter _fn;			// The main function for the module we are writing into.  
			IRVariableTable _literals;		// Symbol table - name to literals
			IRVariableTable _locals;		// Symbol table - name to stack variables
			IRVariableTable _globals;		// Symbol table - name to global variables
		};
	}
}

#include "../tcc/IRCompiler.tcc"