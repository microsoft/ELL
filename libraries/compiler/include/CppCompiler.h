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

namespace emll
{
	namespace compiler
	{
		class CppCompiler : public Compiler
		{
		public:
			CppCompiler();

			virtual void CompileConstantNode(const model::Node& node) override;
			virtual void CompileInputNode(const model::Node& node) override;
			virtual void CompileOutputNode(const model::Node& node) override;
			virtual void CompileBinaryNode(const model::Node& node) override;
			virtual void CompileDotProductNode(const model::Node& node) override;
			virtual void CompileSumNode(const model::Node& node) override;
			virtual void CompileAccumulatorNode(const model::Node& node) override;
			virtual void CompileDelayNode(const model::Node& node) override;
			virtual void CompileUnaryNode(const model::Node& node) override;
			virtual void CompileBinaryPredicateNode(const model::Node& node) override;
			virtual void CompileElementSelectorNode(const model::Node& node) override;

			virtual void BeginFunction(const std::string& functionName, NamedValueTypeList& args) override;
			virtual void EndFunction() override;

			CppModuleEmitter& Module() { return _module; }

		public:

			///<summary>Ensure a variable is emitted</summary>
			void EnsureEmitted(Variable& var);

			///<summary>Emit a variable</summary>
			void Emit(Variable& var);
			///<summary>Emit a variable</summary>
			template<typename T>
			void Emit(Variable& var);

			///<summary>Emit a scalar variable</summary>
			template<typename T>
			void EmitScalar(Variable& var);
			///<summary>Emit a Global Scalar with an initial value</summary>
			template<typename T>
			void EmitGlobal(InitializedScalarVar<T>& var);

			///<summary>Emit a vector variable</summary>
			template<typename T>
			void EmitVector(Variable& var);


		private:
			CppModuleEmitter _module;
		};
	}
}

#include "../tcc/CppCompiler.tcc"