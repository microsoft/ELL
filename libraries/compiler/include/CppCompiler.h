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

			///<summary>Compile an OutputNode</summary>
			virtual void CompileOutputNode(const model::Node& node) override;
			///<summary>Compile a BinaryOperationNode</summary>
			virtual void CompileBinaryNode(const model::Node& node) override;
			///<summary>Compile a DotProductNode</summary>
			virtual void CompileDotProductNode(const model::Node& node) override;
			///<summary>Compile a SumNode</summary>
			virtual void CompileSumNode(const model::Node& node) override;
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
			virtual void EnsureVarEmitted(Variable* pVar) override
			{
				EnsureEmitted(*pVar);
			}

		public:

			///<summary>Ensure a variable is emitted</summary>
			void EnsureEmitted(Variable* pVar) { EnsureEmitted(*pVar); }
			void EnsureEmitted(Variable& var);


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

			///<summary>Compile an OutputNode</summary>
			template<typename T>
			void CompileOutput(const model::OutputNode<T>& node);

		private:
			CppModuleEmitter _module;
			CppFunctionEmitter* _pfn;
		};
	}
}

#include "../tcc/CppCompiler.tcc"