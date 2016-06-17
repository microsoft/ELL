////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CodeGenerator.cpp (compile)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CodeEmitter.h"
#include "Format.h"
#include <assert.h>

namespace emll {
	namespace compiler {
		CodeEmitter::CodeEmitter(DataFlowGraph& graph)
			: _graph(graph)
		{

		}

		void CodeEmitter::Assign(DataFlowNode& srcNode, DataFlowNode& destNode, const layers::Coordinate& destCoordinate)
		{
			auto assignment = EnsureVar(destNode);
			EmitAssign(assignment, srcNode.Variable(), destNode.Variable(), destCoordinate);
		}

		void CodeEmitter::Assign(ScalarVariable& srcVar, DataFlowNode& destNode, const layers::Coordinate& destCoordinate)
		{
			auto assignment = EnsureVar(destNode);
			EmitAssign(assignment, srcVar, destNode.Variable(), destCoordinate);
		}

		DataFlowNode& CodeEmitter::LinearOp(const LinearOperation& op, DataFlowNode& srcNode, const layers::Coordinate& destCoordinate)
		{
			auto& destNode = _graph.GetNode(destCoordinate);
			LinearOp(op, srcNode, destNode, destCoordinate);
			return destNode;
		}

		void CodeEmitter::LinearOp(const LinearOperation& op, DataFlowNode& srcNode, DataFlowNode& destNode, const layers::Coordinate& destCoordinate)
		{
			auto assignment = EnsureVar(destNode);
			EmitLinearOp(op, assignment, srcNode.Variable(), destNode.Variable(), destCoordinate);
		}

		CodeEmitter::Assignment CodeEmitter::EnsureVar(DataFlowNode& node)
		{
			if (node.IsInitialized())
			{
				return CodeEmitter::Assignment::IncrementBy;
			}

			if (node.HasFixedVariableName())
			{
				return CodeEmitter::Assignment::Set;
			}

			bool isNovel = _varStack.IsTopNovel();
			node.SetTempVariableIndex(_varStack.Pop());

			if (isNovel)
			{
				return CodeEmitter::Assignment::Declare;
			}

			return CodeEmitter::Assignment::Reset;
		}

		void CodeEmitter::ReleaseVar(DataFlowNode& node)
		{
			_varStack.Push(node.GetTempVariableIndex());
		}

		//-------------------------------------------------------
		//
		// CEmitter
		//
		//-------------------------------------------------------

		CEmitter::CEmitter(DataFlowGraph& graph, std::ostream& os)
			: CodeEmitter(graph), _os(os)
		{
		}

		void CEmitter::BeginLinear(const std::string& functionName, const std::string& inputVarName, uint64_t inputCount, const std::string& outputVarName, const layers::CoordinateList& outputs)
		{
			auto str = "// New Compiler \n// Input dimension: %\n// Output dimension: %\n// Output coordinates:";
			utilities::PrintFormat(_os, str, inputCount, outputs.Size());
			for (uint64_t i = 0; i < outputs.Size(); ++i)
			{
				_os << ' ' << outputs[i];
			}
			_os << "\n";
			//
			// Function Declaration
			//
			utilities::PrintFormat(_os, "void %(const double* %, double* %)\n{\n", functionName, inputVarName, outputVarName);
		}

		void CEmitter::EndLinear()
		{
			_os << "}\n";
		}

		void CEmitter::EmitAssign(Assignment assignment, ScalarVariable& srcVar, ScalarVariable& destVar, const layers::Coordinate& destCoordinate)
		{
			EnsureEmittedName(srcVar);
			EnsureEmittedName(destVar);

			const char* format = ToString(assignment);
			utilities::PrintFormat(_os, format, destVar.EmittedName(), srcVar.EmittedName(), destCoordinate.GetLayerIndex(), destCoordinate.GetElementIndex());
		}

		void CEmitter::EmitLinearOp(const LinearOperation& op, Assignment assignment, ScalarVariable& srcVar, ScalarVariable& destVar, const layers::Coordinate& destCoordinate)
		{
			EnsureEmittedName(srcVar);
			EnsureEmittedName(destVar);

			const char* format = ToString(assignment);
			std::string rhs = ToString(op, srcVar.EmittedName());
			utilities::PrintFormat(_os, format, destVar.EmittedName(), rhs, destCoordinate.GetLayerIndex(), destCoordinate.GetElementIndex());
		}

		void CEmitter::EnsureEmittedName(ScalarVariable& var)
		{
			if (!var.HasEmittedName())
			{
				if (var.IsArrayElement())
				{
					var.SetEmittedName(var.Name() + "[" + std::to_string(var.ElementOffset()) + "]");
				}
				else
				{
					var.SetEmittedName(var.Name());
				}
			}
		}

		const char* CEmitter::ToString(CodeEmitter::Assignment assignment)
		{
			const char* format = nullptr;
			switch (assignment)
			{
				default:
					break;
				case Assignment::Declare:
					format = "    double % = %; // coordinate (%,%), allocating new temporary variable\n";
					break;
				case Assignment::Set:
					format = "    % = %; // coordinate (%,%)\n";
					break;
				case Assignment::IncrementBy:
					format = "    % += %; // coordinate (%,%)\n";
					break;
				case Assignment::Reset:
					format = "    % = %; // coordinate (%,%), reassigning temporary variable\n";
					break;
			}
			assert(format != nullptr);
			return format;
		}

		std::string CEmitter::ToString(const LinearOperation& op, const std::string& sourceVar)
		{
			double a = op.MultiplyBy();
			double b = op.IncrementBy();
			if (b == 0)
			{
				if (a != 1)
				{
					return utilities::PrintFormat("% * %", a, sourceVar);
				}
				else
				{
					return sourceVar;
				}
			}
			else
			{
				if (a == 0)
				{
					return std::to_string(b);
				}
				else if (a == 1)
				{
					return utilities::PrintFormat("% + %", sourceVar, b);
				}
				else
				{
					return utilities::PrintFormat("% * % + %", a, sourceVar, b);
				}
			}
		}


		//-------------------------------------------------------
		//
		// IREmitter
		//
		//-------------------------------------------------------

		IREmitter::IREmitter(DataFlowGraph& graph, std::ostream& os)
			: CodeEmitter(graph), _os(os)
		{
			_module = std::make_unique<ir::ModuleEmitter>(&_emitter, _emitter.AddModule("EMLL"));
		}

		void IREmitter::BeginLinear(const std::string& functionName, const std::string& inputVarName, uint64_t inputCount, const std::string& outputVarName, const layers::CoordinateList& outputs)
		{
			ir::NamedValueTypeList fnArgs;
			fnArgs.init({ {inputVarName, ir::ValueType::PDouble}, { outputVarName, ir::ValueType::PDouble }});
			_fn = _module->Function(functionName, ir::ValueType::Void, fnArgs, true);	
			//
			// Register input and out as variables for the function body to use
			//
			auto args = _fn.Args().begin();
			_variables.Set(inputVarName, &(*args));
			args++;
			_variables.Set(outputVarName, &(*args));
		}

		void IREmitter::EndLinear()
		{
			_fn.Ret();
			_module->dump();
		}

		void IREmitter::EmitAssign(Assignment assignment, ScalarVariable& srcVar, ScalarVariable& destVar, const layers::Coordinate& destCoordinate)
		{
			llvm::Value *pSrc = EnsureVar(srcVar);
			llvm::Value *pDest = EnsureVar(destVar);
			switch (assignment)
			{
				case Assignment::Declare:
				case Assignment::Set:
				case Assignment::Reset:
					_fn.Store(pDest, pSrc);
					break;
				case Assignment::IncrementBy:
				{
					llvm::Value* pSum = _fn.Op(ir::OperatorType::AddF, _fn.Load(pSrc), _fn.Load(pDest));
					_fn.Store(pDest, pSum);
					break;
				}
				default:
					break;
			}
		}

		void IREmitter::EmitLinearOp(const LinearOperation& op, Assignment assignment, ScalarVariable& srcVar, ScalarVariable& destVar, const layers::Coordinate& destCoordinate)
		{

		}


		llvm::Value* IREmitter::EnsureVar(ScalarVariable& var)
		{
			auto name = var.Name();
			llvm::Value* pValue = _variables.Get(name);			
			if (pValue == nullptr)
			{
				if (var.IsArrayElement())
				{
					throw new CodeEmitterException(CodeEmitterError::ArrayMustBeAllocated);
				}
				// We currently assume that all variables are doubles
				pValue = _fn.Var(ir::ValueType::Double, name);
				_variables.Set(name, pValue);
			}
			return pValue;
		}
	}
}

