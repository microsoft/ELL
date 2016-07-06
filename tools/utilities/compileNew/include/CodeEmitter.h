////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CodeEmitter.h
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IntegerStack.h"
#include "DataFlowGraph.h"
#include "DataFlowNode.h"
#include "IRCompiler.h"

#include <string>
#include <ostream>

namespace emll 
{
	namespace compiler 
	{
		enum class CodeEmitterError
		{
			NotSupported,
			ArrayMustBeAllocated
		};

		using CodeEmitterException = CompilerException<CodeEmitterError>;


		//
		// Abstract base class for Code Generators
		//
		class CodeEmitter
		{
		public:
			enum class Assignment
			{
				Declare,
				Set,
				Reset,
				IncrementBy
			};

		public:
			CodeEmitter(DataFlowGraph& graph);

			virtual void Begin() {}
			virtual void End() {}
			virtual void EmitTest(const std::string& fnName, int featureCount, int outputCount, double startValue) {}

			virtual void BeginLinear(const std::string& functionName, const std::string& inputVarName, uint64_t inputCount, const std::string& outputVarName, const layers::CoordinateList& outputs) = 0;

			void Assign(DataFlowNode& srcNode, DataFlowNode& destNode, const layers::Coordinate& destCoordinate);
			void Assign(ScalarVariable& srcVar, DataFlowNode& destNode, const layers::Coordinate& destCoordinate);

			DataFlowNode& LinearOp(const LinearOperation& op, DataFlowNode& srcNode, const layers::Coordinate& destCoordinate);
			void LinearOp(const LinearOperation& op, DataFlowNode& srcNode, DataFlowNode& destNode, const layers::Coordinate& destCoordinate);

			virtual void EndLinear() = 0;

			Assignment EnsureVar(DataFlowNode& node);
			void ReleaseVar(DataFlowNode& node);

			DataFlowGraph& graph()
			{
				return _graph;
			}

		protected:
			virtual void EmitAssign(Assignment assignment, ScalarVariable& srcVar, ScalarVariable& destVar, const layers::Coordinate& destCoordinate) = 0;
			virtual void EmitLinearOp(const LinearOperation& op, Assignment assignment, ScalarVariable& srcVar, ScalarVariable& destVar, const layers::Coordinate& destCoordinate) = 0;


		private:
			utilities::IntegerStack _varStack;
			DataFlowGraph& _graph;
		};

		class CCodeEmitter : public CodeEmitter
		{
		public:
			CCodeEmitter(DataFlowGraph& graph, std::ostream& os);

			void BeginLinear(const std::string& functionName, const std::string& inputVarName, uint64_t inputCount, const std::string& outputVarName, const layers::CoordinateList& outputs) override;
			void EndLinear() override;

		protected:
			virtual void EmitAssign(Assignment assignment, ScalarVariable& srcVar, ScalarVariable& destVar, const layers::Coordinate& destCoordinate) override;
			virtual void EmitLinearOp(const LinearOperation& op, Assignment assignment, ScalarVariable& srcVar, ScalarVariable& destVar, const layers::Coordinate& destCoordinate) override;

		private:

			void EnsureEmittedName(ScalarVariable& var);

			const char* ToString(Assignment assignment);
			std::string ToString(const LinearOperation& op, const std::string& sourceVar);

		private:
			std::ostream& _os;
		};

		class IRCodeEmitter : public CodeEmitter
		{
		public:
			IRCodeEmitter(DataFlowGraph& graph, std::ostream& os);

			void Begin() override;
			void End() override;

			void BeginLinear(const std::string& functionName, const std::string& inputVarName, uint64_t inputCount, const std::string& outputVarName, const layers::CoordinateList& outputs) override;
			void EndLinear() override;

			void EmitTest(const std::string& fnName, int featureCount, int outputCount, double startValue) override;

		protected:
			virtual void EmitAssign(Assignment assignment, ScalarVariable& srcVar, ScalarVariable& destVar, const layers::Coordinate& destCoordinate) override;
			virtual void EmitLinearOp(const LinearOperation& op, Assignment assignment, ScalarVariable& srcVar, ScalarVariable& destVar, const layers::Coordinate& destCoordinate) override;

		private:
			llvm::Value* EnsureVar(ScalarVariable& var);
			llvm::Value* LoadVar(ScalarVariable& var);

			llvm::Value* Emit(ScalarVariable& srcVar, const LinearOperation& op);
			void Store(ScalarVariable& destVar, llvm::Value* pValue);
			void Increment(ScalarVariable& destVar, llvm::Value* pValue);

		private:
			std::ostream& _os;
			IREmitter _emitter;
			IRModuleEmitter _module;
			IRFunctionEmitter _fn;
			IRVariableTable _variables;
		};
	}
}
