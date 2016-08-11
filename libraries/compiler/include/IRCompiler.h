////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     IRCompiler.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "Compiler.h"
#include "IRInclude.h"
#include "ScalarVar.h"
#include "VectorVar.h"
#include "IRRuntime.h"
#include <stdio.h>

namespace emll
{
	namespace compiler
	{
		///<summary>Compiles EMLL Models to LLVM IR</summary>
		class IRCompiler : public Compiler
		{
		public:
			///<summary>Create a compiler to produce an LLVM module with the default name</summary>
			IRCompiler();
			///<summary>Create a compiler to produce an LLVM module with the given name</summary>
			IRCompiler(const std::string& moduleName);

			///<summary>Emit LLVM IR to std::out for debugging</summary>
			void DebugDump();
			///<summary>Output the compiled model to the given file</summary>
			virtual void WriteToFile(const std::string& filePath) override
			{
				WriteAsmToFile(filePath);
			}
			///<summary>Emit LLVM IR assembly to a file</summary>
			void WriteAsmToFile(const std::string& filePath);
			///<summary>Emit LLVM IR bitcode to a file</summary>
			void WriteBitcodeToFile(const std::string& filePath);
		
			IRModuleEmitter& Module() { return _module; }

		protected:
			///<summary>Compile an OutputNode</summary>
			virtual void CompileOutputNode(const model::OutputNode<double>& node) override { CompileOutput<double>(node);}
			///<summary>Compile an OutputNode</summary>
			virtual void CompileOutputNode(const model::OutputNode<int>& node) override { CompileOutput<int>(node);}
			///<summary>Compile an OutputNode</summary>
			virtual void CompileOutputNode(const model::OutputNode<bool>& node) override { CompileOutput<bool>(node);}

			///<summary>Compile a BinaryNode</summary>
			virtual void CompileBinaryNode(const nodes::BinaryOperationNode<double>& node) override { CompileBinary<double>(node);}
			///<summary>Compile a BinaryNode</summary>
			virtual void CompileBinaryNode(const nodes::BinaryOperationNode<int>& node) override { CompileBinary<int>(node); }
			///<summary>Compile a SumNode</summary>
			virtual void CompileSumNode(const nodes::SumNode<double>& node) override { CompileSum<double>(node); }
			///<summary>Compile a SumNode</summary>
			virtual void CompileSumNode(const nodes::SumNode<int>& node) override { CompileSum<int>(node);}

			///<summary>Compile a DotProductNode</summary>
			virtual void CompileDotProductNode(const model::Node& node) override;
			///<summary>Compile a AccmulatorNode</summary>
			virtual void CompileAccumulatorNode(const model::Node& node) override;
			///<summary>Compile a Delay node</summary>
			virtual void CompileDelayNode(const model::Node& node) override;
			///<summary>Compile a Unary node</summary>
			virtual void CompileUnaryNode(const model::Node& node) override;
			///<summary>Compile a binary predicate</summary>
			virtual void CompileBinaryPredicateNode(const model::Node& node) override;
			///<summary>Compile an elementselectorNode</summary>
			virtual void CompileElementSelectorNode(const model::Node& node) override;


			///<summary>Begins the IR function that will contain our compiled model</summary>
			virtual void BeginFunction(const std::string& functionName, NamedValueTypeList& args) override;
			///<summary>Ends the IR function that will contain our compiled model</summary>
			virtual void EndFunction() override;
			///<summary>Ensure a variable is emitted</summary>
			virtual void EnsureVarEmitted(Variable* pVar)
			{
				EnsureEmitted(pVar);
			}

		private:
			void RegisterFunctionArgs(NamedValueTypeList& args);

			///<summary>Emit IR for a variable</summary>
			llvm::Value* Emit(Variable& var);
			///<summary>Emit IR for a variable</summary>
			template<typename T>
			llvm::Value* Emit(Variable& var);

			///<summary>Emit IR for a scalar variable</summary>
			template<typename T>
			llvm::Value* EmitScalar(Variable& var);
			///<summary>Emit IR for a constant</summary>
			template<typename T>
			llvm::Value* EmitLiteral(LiteralVar<T>& var);
			///<summary>Emit IR for a stack scalar variable</summary>
			template<typename T>
			llvm::Value* EmitLocal(ScalarVar<T>& var);
			///<summary>Emit IR for a stack scalar variable with an initial value</summary>
			template<typename T>
			llvm::Value* EmitLocal(InitializedScalarVar<T>& var);
			///<summary>Emit IR for a reference to an element in a vector</summary>
			template<typename T>
			llvm::Value* EmitRef(VectorElementVar<T>& var);
			///<summary>Emit IR for a Global Scalar with an initial value</summary>
			template<typename T>
			llvm::Value* EmitGlobal(InitializedScalarVar<T>& var);
			
			
			///<summary>Emit IR for a vector variable</summary>
			template<typename T>
			llvm::Value* EmitVector(Variable& var);
			///<summary>Emit IR for a Global Vector. Global is initialized to zero</summary>
			template<typename T>
			llvm::Value* EmitGlobalVector(VectorVar<T>& var);
			///<summary>Emit IR for a Global Vector with initial values</summary>
			template<typename T>
			llvm::Value* EmitGlobalVector(InitializedVectorVar<T>& var);
			///<summary>Emit IR for a Constant Vector</summary>
			template<typename T>
			llvm::Value* EmitLiteralVector(LiteralVarV<T>& var);

			void ApplyComputed(Variable& var, llvm::Value* pDest);
			template<typename T>
			void ApplyComputed(ComputedVar<T>& var, llvm::Value* pDest);

			///<summary>Gets an emitted variable with the given name and scope</summary>
			llvm::Value* GetEmittedVariable(const VariableScope scope, const std::string& name);
			///<summary>Ensure that the given variable has been declared in IR</summary>
			llvm::Value* EnsureEmitted(Variable* pVar);
			///<summary>Ensure that the given variable has been declared in IR</summary>
			llvm::Value* EnsureEmitted(Variable& var);
			///<summary>Ensure that variable for the given port has been declared in IR</summary>
			llvm::Value* EnsureEmitted(model::OutputPortBase* pPort);
			///<summary>Ensure that variable for the given port element has been declared in IR</summary>
			llvm::Value* EnsureEmitted(model::OutputPortElement elt);
			///<summary>Ensure that variable for the outport port referenced by this input port has been declared in IR</summary>
			llvm::Value* EnsureEmitted(model::InputPortBase* pPort);

			///<summary>Ensure that the given variable is loaded into a register</summary>
			llvm::Value* LoadVar(Variable* pVar);
			///<summary>Ensure that the given variable is loaded into a register</summary>
			llvm::Value* LoadVar(Variable& var);
			///<summary>Ensure that the variable for this outport port element is loaded into a register. SThis will automatically
			/// dereference any pointers it needs to.</summary>
			llvm::Value* LoadVar(const model::OutputPortElement elt);
			///<summary>Load the variable for the outport port referenced by this input port</summary>
			llvm::Value* LoadVar(model::InputPortBase* pPort);
			///<summary>Updates the value at a given offset of the given variable. Checks for index out of range etc.</summary>
			void SetVar(Variable& var, llvm::Value* pDest, int offset, llvm::Value* pValue);
			
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
			
			///<summary>Compile a UnaryNode</summary>
			template<typename T>
			void CompileUnary(const nodes::UnaryOperationNode<T>& node);

			///<summary>Compile a BinarPredicate</summary>
			template<typename T>
			void CompileBinaryPredicate(const nodes::BinaryPredicateNode<T>& node);

			///<summary>Compile an element selector node</summary>
			template<typename T>
			void CompileElementSelectorNode(const model::Node& node);
			///<summary>Compile an element selector node</summary>
			template<typename T, typename SelectorType>
			void CompileElementSelector(const nodes::ElementSelectorNode<T, SelectorType>& node);
			///<summary>Compile an element selector node</summary>
			template<typename T, typename SelectorType>
			void CompileElementSelectorBinary(const nodes::ElementSelectorNode<T, SelectorType>& node);

		private:
			IREmitter _emitter;			// Object that makes lower level LLVM calls
			IRModuleEmitter _module;	// The emitted IR module
			IRFunctionEmitter _fn;		// The main function for the module we are writing into.  
			IRVariableTable _literals;	// Symbol table - name to literals
			IRVariableTable _locals;	// Symbol table - name to stack variables
			IRVariableTable _globals;	// Symbol table - name to global variables
			IRRuntime _runtime;			// Manages emission of runtime functions	
		};
	}
}

#include "../tcc/IRCompiler.tcc"