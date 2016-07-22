////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     Compiler.cpp (compiler)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Compiler.h"
#include "CompilerException.h"

namespace emll
{
	namespace compiler
	{
		static const std::string c_InputVariableName = "input";
		static const std::string c_OutputVariableName = "output";
		static const std::string c_PredictFunctionName = "Predict";
		static const std::string c_literalVar = "c";
		static const std::string c_globalVar = "g";
		static const std::string c_localVar = "t";

		/// <summary>Base class for ML Compiler.</summary>
		Compiler::Compiler()
		{
		}

		EmittedVar Compiler::AllocLocal()
		{
			return _localVars.Alloc();
		}

		void Compiler::FreeLocal(EmittedVar var)
		{
			_localVars.Free(var);
		}

		EmittedVar Compiler::AllocLiteral()
		{
			return _literalVars.Alloc();
		}

		EmittedVar Compiler::AllocGlobal()
		{
			return _globalVars.Alloc();
		}

		void Compiler::FreeGlobal(EmittedVar var)
		{
			_globalVars.Free(var);
		}

		void Compiler::AllocVar(Variable& var)
		{
			if (var.HasEmittedName())
			{
				return;
			}

			EmittedVar emittedVar;
			const std::string* pPrefix = nullptr;
			switch (var.Scope())
			{
				case VariableScope::Literal:
					emittedVar = AllocLiteral();
					pPrefix = &c_literalVar;
					break;
				case VariableScope::Local:
					emittedVar = AllocLocal();
					pPrefix = &c_localVar;
					break;
				case VariableScope::Global:
					emittedVar = AllocGlobal();
					pPrefix = &c_globalVar;
					break;
				default:
					throw new CompilerException(CompilerError::variableScopeNotSupported);
			}			
			var.AssignVar(emittedVar);
			var.SetEmittedName(*pPrefix + std::to_string(emittedVar.varIndex));
		}

		void Compiler::FreeVar(Variable& var)
		{
			if (!var.HasEmittedName())
			{
				return;
			}

			switch (var.Scope())
			{
				case VariableScope::Literal:
					// We never free literals
					break;
				case VariableScope::Local:
					FreeLocal(var.GetAssignedVar());
					break;
				case VariableScope::Global:
					FreeGlobal(var.GetAssignedVar());
					break;
				default:
					break;
			}
		}

		void Compiler::CompileGraph(DataFlowGraph& graph)
		{
			for (auto node : graph.Literals())
			{
				node->Process(graph, *this);
			}
		}

		void Compiler::BeginFunctionPredict()
		{
			NamedValueTypeList fnArgs;
			fnArgs.init({ {c_InputVariableName, ValueType::PDouble },{ c_OutputVariableName, ValueType::PDouble } });
			BeginFunction(c_PredictFunctionName, fnArgs);
		}

		void Compiler::Reset()
		{
		}
	}
}