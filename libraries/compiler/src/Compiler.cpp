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
		static const std::string c_literalVar = "c_";
		static const std::string c_globalVar = "g_";
		static const std::string c_localVar = "t_";
		static const std::string c_inputVar = "input";
		static const std::string c_outputVar = "output";

		/// <summary>Base class for ML Compiler.</summary>
		Compiler::Compiler()
		{
		}

		void Compiler::CompileConstant(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
				case model::Port::PortType::Real:
					CompileConstant(static_cast<const nodes::ConstantNode<double>&>(node));
					break;
				case model::Port::PortType::Integer:
					CompileConstant(static_cast<const nodes::ConstantNode<int>&>(node));
					break;
				default:
					throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void Compiler::CompileBinaryNode(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
				case model::Port::PortType::Real:
					CompileConstant(static_cast<const nodes::BinaryOperationNode<double>&>(node));
					break;
				case model::Port::PortType::Integer:
					CompileConstant(static_cast<const nodes::BinaryOperationNode<int>&>(node));
					break;
				default:
					throw new CompilerException(CompilerError::portTypeNotSupported);
			}
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

		Variable* Compiler::AllocVar(model::OutputPortBase* pPort)
		{
			assert(pPort->Size() != 0);

			ValueType type = ToValueType(pPort->GetType());
			Variable* pVar = nullptr;
			if (pPort->Size() == 1)
			{
				pVar = _variables.AddLocalScalarVariable(type);
			}
			else
			{
				pVar = _variables.AddVectorVariable(VariableScope::Global, type, pPort->Size());
			}
			SetVariableFor(pPort, pVar);
			return pVar;
		}

		Variable* Compiler::GetVariableFor(const model::OutputPortBase* pPort)
		{
			assert(pPort != nullptr);
			auto search = _portToVarMap.find(pPort);
			if (search != _portToVarMap.end())
			{
				return search->second;
			}
			return nullptr;
		}

		Variable* Compiler::GetVariableFor(const model::OutputPortElement elt)
		{
			return GetVariableFor(elt.ReferencedPort());
		}

		void Compiler::SetVariableFor(const model::OutputPortBase* pPort, Variable* pVar)
		{
			_portToVarMap[pPort] = pVar;
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
			for (auto node : graph.InputArgs())
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

		ValueType Compiler::ToValueType(model::Port::PortType type)
		{
			switch (type)
			{
				case model::Port::PortType::Real:
					return ValueType::Double;
				case model::Port::PortType::Integer:
					return ValueType::Int32;
				default:
					throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void Compiler::Reset()
		{
		}
	}
}