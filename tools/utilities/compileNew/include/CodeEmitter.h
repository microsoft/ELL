////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CodeGenerator.h (compile)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IntegerStack.h"
#include "DataFlowGraph.h"
#include "DataFlowNode.h"
#include "LLVMCompiler.h"

#include <string>
#include <ostream>

namespace emll {
	namespace compiler {
		//
		// Abstract base class for Code Generators
		//
		class CodeEmitter
		{
		public:
			enum Assignment
			{
				Declare,
				Set,
				Reset,
				IncrementBy
			};

		public:
			CodeEmitter(DataFlowGraph& graph);

			virtual void BeginLinear(const char* name, uint64_t inputCount, const layers::CoordinateList& outputs) = 0;

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

		class CEmitter : public CodeEmitter
		{
		public:
			CEmitter(DataFlowGraph& graph, std::ostream& os);

			void BeginLinear(const char* name, uint64_t inputCount, const layers::CoordinateList& outputs) override;
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

		class IREmitter : public CodeEmitter
		{
		public:
			IREmitter(DataFlowGraph& graph, std::ostream& os);

			void BeginLinear(const char* name, uint64_t inputCount, const layers::CoordinateList& outputs) override;
			void EndLinear() override;

		protected:
			virtual void EmitAssign(Assignment assignment, ScalarVariable& srcVar, ScalarVariable& destVar, const layers::Coordinate& destCoordinate) override;
			virtual void EmitLinearOp(const LinearOperation& op, Assignment assignment, ScalarVariable& srcVar, ScalarVariable& destVar, const layers::Coordinate& destCoordinate) override;

		private:
			llvm::Value* EnsureVar(ScalarVariable& var);

		private:
			std::ostream& _os;
			ir::LLVMEmitter _emitter;
			ir::FunctionEmitter _fn;
			std::unique_ptr<ir::ModuleEmitter> _module;
			ir::VariableTable _variables;
		};
	}
}
