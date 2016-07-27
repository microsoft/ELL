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

		static const std::string c_ConstantNodeType = "ConstantNode";
		static const std::string c_BinaryNodeType = "BinaryOperationNode";
		static const std::string c_InputNodeType = "InputNode";
		static const std::string c_OutputNodeType = "OutputNode";
		static const std::string c_DotProductType = "DotProductNode";
		static const std::string c_LinearNodeType = "LinearNode";
		static const std::string c_SumNodeType = "SumNode";

		/// <summary>Base class for ML Compiler.</summary>
		Compiler::Compiler()
		{
		}

		void Compiler::CompileModel(const std::string& functionName, model::Model& model)
		{
			_args.clear();
			CollectInputsAndOutputs(model);
			BeginFunction(functionName, _args);
			model.Visit([this](const model::Node& node) {
				std::string typeName = node.GetRuntimeTypeName();	
				//
				// TODO: Make this a lookup table
				//
				if (IsNodeType(typeName, c_BinaryNodeType))
				{
					CompileBinaryNode(node);
				}
				else if (IsNodeType(typeName, c_ConstantNodeType))
				{
					CompileConstantNode(node);
				}
				else if (IsNodeType(typeName, c_InputNodeType))
				{
					CompileInputNode(node);
				}
				else if (IsNodeType(typeName, c_OutputNodeType))
				{
					CompileOutputNode(node);
				}
				else if (IsNodeType(typeName, c_DotProductType))
				{
					CompileDotProductNode(node);
				}
				else if (IsNodeType(typeName, c_SumNodeType))
				{
					CompileSumNode(node);
				}
				else
				{
					throw new CompilerException(CompilerError::nodeTypeNotSupported);
				}

			});
			EndFunction();
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

		Variable* Compiler::AllocArg(const model::OutputPortBase* pPort, bool isInput)
		{
			ValueType varType = ToValueType(pPort->GetType());
			VariableScope scope = isInput ? VariableScope::Input : VariableScope::Output;
			//
			// For now, all inputs and outputs are modelled as Vectors... unlike regular variables, we don't optimize for scalars
			//
			Variable* pVar = _variables.AddVectorVariable(scope, varType, pPort->Size());
			AllocVar(*pVar);
			SetVariableFor(pPort, pVar);

			_args.push_back({pVar->EmittedName(), GetPtrType(varType)});
			
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

		void Compiler::BeginFunctionPredict()
		{
			NamedValueTypeList fnArgs;
			fnArgs.init({ {c_InputVariableName, ValueType::PDouble },{ c_OutputVariableName, ValueType::PDouble } });
			BeginFunction(c_PredictFunctionName, fnArgs);
		}

		bool Compiler::IsNodeType(const std::string& nodeTypeName, const std::string& typeName)
		{
			return (nodeTypeName.compare(0, typeName.size(), typeName) == 0);
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

		void Compiler::CollectInputsAndOutputs(model::Model& model)
		{
			model.Visit([this](const model::Node& node) {
				auto typeName = node.GetRuntimeTypeName();
				if (IsNodeType(typeName, c_InputNodeType))
				{
					AllocArg(node.GetOutputPorts()[0], true);
				}
				else if (IsNodeType(typeName, c_OutputNodeType))
				{
					AllocArg(node.GetOutputPorts()[0], false);
				}
			});
		}

		void Compiler::Reset()
		{
		}
	}
}