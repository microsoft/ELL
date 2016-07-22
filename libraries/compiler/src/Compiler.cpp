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
		static const std::string c_inputVar = "i";
		static const std::string c_outputVar = "o";

		/// <summary>Base class for ML Compiler.</summary>
		Compiler::Compiler()
		{
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
					emittedVar = _literalVars.Alloc();
					pPrefix = &c_literalVar;
					break;
				case VariableScope::Local:
					emittedVar = _localVars.Alloc();
					pPrefix = &c_localVar;
					break;
				case VariableScope::Global:
					emittedVar = _globalVars.Alloc();
					pPrefix = &c_globalVar;
					break;
				case VariableScope::Input:
					emittedVar = _inputVars.Alloc();
					pPrefix = &c_inputVar;
					break;
				case VariableScope::Output:
					emittedVar = _outputVars.Alloc();
					pPrefix = &c_outputVar;
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
			VariableScope scope = var.Scope();
			switch (scope)
			{
				case VariableScope::Local:
					_localVars.Free(var.GetAssignedVar());
					break;
				case VariableScope::Global:
					_globalVars.Free(var.GetAssignedVar());
					break;
				default:
					// We never free other types
					assert(scope != VariableScope::Local && scope != VariableScope::Global);
					break;
			}
		}

		void Compiler::CompileGraph(const std::string& functionName, DataFlowGraph& graph)
		{
			BeginFunction(functionName, graph);

			// First, process all literals
			for (auto node : graph.Literals())
			{
				node->Process(graph, *this);
			}
			// Now start executing the graph
			for (auto node : graph.Inputs())
			{
				node->Process(graph, *this);
			}
			EndFunction();
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