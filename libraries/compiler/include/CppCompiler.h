////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CppCompiler.h (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Compiler.h"
#include "CppModuleEmitter.h"
#include "CppFunctionEmitter.h"

namespace emll
{
	namespace compiler
	{
		class CppCompiler : public Compiler
		{
		public:
			CppCompiler();

			CppModuleEmitter& Module() { return _module; }
			CppFunctionEmitter& Function() { return *_pfn; }

			void DebugDump() { _module.Dump(); }

			///<summary>Output the compiled model to the given file</summary>
			virtual void WriteToFile(const std::string& filePath) override;
				
		public:
			///<summary>Begins the function that will contain our compiled model</summary>
			virtual void BeginFunction(const std::string& functionName, NamedValueTypeList& args) override;
			///<summary>Ends the function that will contain our compiled model</summary>
			virtual void EndFunction() override;

			///<summary>Ensure a variable is emitted</summary>
			void EnsureEmitted(Variable* pVar) { EnsureEmitted(*pVar); }
			void EnsureEmitted(Variable& var);

		protected:
			///<summary>Compile an OutputNode</summary>
			virtual void CompileOutputNode(const model::OutputNode<double>& node) override { CompileOutput<double>(node); }
			///<summary>Compile an OutputNode</summary>
			virtual void CompileOutputNode(const model::OutputNode<int>& node) override { CompileOutput<int>(node); }
			///<summary>Compile an OutputNode</summary>
			virtual void CompileOutputNode(const model::OutputNode<bool>& node) override { CompileOutput<bool>(node); }

			///<summary>Compile a BinaryNode</summary>
			virtual void CompileBinaryNode(const nodes::BinaryOperationNode<double>& node) override { CompileBinary<double>(node); }
			///<summary>Compile a BinaryNode</summary>
			virtual void CompileBinaryNode(const nodes::BinaryOperationNode<int>& node) override { CompileBinary<int>(node); }

			///<summary>Compile a SumNode</summary>
			virtual void CompileSumNode(const nodes::SumNode<double>& node) { CompileSum<double>(node); };
			///<summary>Compile a SumNode</summary>
			virtual void CompileSumNode(const nodes::SumNode<int>& node) { CompileSum<int>(node); };

			///<summary>Compile a DotProductNode</summary>
			virtual void CompileDotProductNode(const model::Node& node) override;
			///<summary>Compile a AccmulatorNode</summary>
			virtual void CompileAccumulatorNode(const model::Node& node) override;
			///<summary>Compile a Delay node</summary>
			virtual void CompileDelayNode(const model::Node& node) override;
			///<summary>Compile a Unary node</summary>
			virtual void CompileUnaryNode(const model::Node& node) override;
			///<summary>Compile an elementselectorNode</summary>
			virtual void CompileElementSelectorNode(const model::Node& node) override;

			///<summary>Compile a binary predicate</summary>
			virtual void CompileBinaryPredicateNode(const nodes::BinaryPredicateNode<double>& node) override 
			{ 
				CompileBinaryPredicate<double>(node); 
			}

			///<summary>Ensure a variable is emitted</summary>
			virtual void EnsureVarEmitted(Variable* pVar) override
			{
				EnsureEmitted(*pVar);
			}
			virtual void HandleLeafNode(const model::Node& node) override {}

		private:

			Variable* EnsureEmitted(model::OutputPortBase* pPort);
			Variable* EnsureEmitted(model::OutputPortElement elt);
			Variable* EnsureEmitted(model::InputPortBase* pPort);

			///<summary>Emit a variable</summary>
			void Emit(Variable& var);
			///<summary>Emit a variable</summary>
			template<typename T>
			void Emit(Variable& var);

			///<summary>Emit a scalar variable</summary>
			template<typename T>
			void EmitScalar(Variable& var);
			///<summary>Emit a literal veriable</summary>
			template<typename T>
			void EmitLiteral(LiteralVar<T>& var);
			///<summary>Emit a stack scalar variable</summary>
			template<typename T>
			void EmitLocal(ScalarVar<T>& var);
			///<summary>Emit a stack scalar variable with an initial value</summary>
			template<typename T>
			void EmitLocal(InitializedScalarVar<T>& var);
			///<summary>Emit IR for a reference to an element in a vector</summary>
			template<typename T>
			void EmitRef(VectorElementVar<T>& var);
			///<summary>Emit a Global Scalar with an initial value</summary>
			template<typename T>
			void EmitGlobal(InitializedScalarVar<T>& var);

			///<summary>Emit a vector variable</summary>
			template<typename T>
			void EmitVector(Variable& var);
			///<summary>Emit a Global Vector. Global is initialized to zero</summary>
			template<typename T>
			void EmitGlobalVector(VectorVar<T>& var);
			///<summary>Emit a Global Vector with initial values</summary>
			template<typename T>
			void EmitGlobalVector(InitializedVectorVar<T>& var);
			///<summary>Emit a Constant Vector</summary>
			template<typename T>
			void EmitLiteralVector(LiteralVarV<T>& var);

			///<summary>Ensure that the variable for this outport port element is loaded into a register. SThis will automatically
			/// dereference any pointers it needs to.</summary>
			void LoadVar(const model::OutputPortElement elt);
			///<summary>Load the variable for the outport port referenced by this input port</summary>
			void LoadVar(model::InputPortBase* pPort);
			///<summary>Updates the value at a given offset of the given variable. Checks for index out of range etc.</summary>
			void SetVar(Variable& var, int offset);
			///<summary>Updates the value at a given offset of the given variable. Checks for index out of range etc.</summary>
			void SetVar(Variable& var);

			const std::string& LoopVarName();

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
			
			///<summary>Compile a SumNode</summary>
			template<typename T>
			void CompileSum(const nodes::SumNode<T>& node);
			///<summary>Compile a SumNode</summary>
			template<typename T>
			void CompileSumLoop(const nodes::SumNode<T>& node);
			///<summary>Compile a SumNode</summary>
			template<typename T>
			void CompileSumExpanded(const nodes::SumNode<T>& node);

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
			CppModuleEmitter _module;
			CppFunctionEmitter* _pfn;
		};
	}
}

#include "../tcc/CppCompiler.tcc"