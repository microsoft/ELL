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
		CompilerSettings::CompilerSettings(const CompilerSettings& src)
			: _unrollLoops(src._unrollLoops),
			  _inlineOperators(src._inlineOperators)
		{

		}

		static const std::string c_InputVariableName = "input";
		static const std::string c_OutputVariableName = "output";
		static const std::string c_literalVar = "c_";
		static const std::string c_globalVar = "g_";
		static const std::string c_localVar = "t_";
		static const std::string c_fnVar = "Fn";
		static const std::string c_nodeVar = "Node";
		static const std::string c_inputVar = "input";
		static const std::string c_outputVar = "output";

		static const std::string c_ConstantNodeType = "ConstantNode";
		static const std::string c_BinaryNodeType = "BinaryOperationNode";
		static const std::string c_InputNodeType = "InputNode";
		static const std::string c_OutputNodeType = "OutputNode";
		static const std::string c_DotProductType = "DotProductNode";
		static const std::string c_LinearNodeType = "LinearNode";
		static const std::string c_SumNodeType = "SumNode";
		static const std::string c_AccumulatorNodeType = "AccumulatorNode";
		static const std::string c_DelayNodeType = "DelayNode";
		static const std::string c_UnaryNodeType = "UnaryOperationNode";
		static const std::string c_BinaryPredicateNodeType = "BinaryPredicateNode";
		static const std::string c_ElementSelectorNodeType = "ElementSelectorNode";

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
				else if (IsNodeType(typeName, c_AccumulatorNodeType))
				{
					CompileAccumulatorNode(node);
				}
				else if (IsNodeType(typeName, c_DelayNodeType))
				{
					CompileDelayNode(node);
				}
				else if (IsNodeType(typeName, c_UnaryNodeType))
				{
					CompileUnaryNode(node);
				}
				else if (IsNodeType(typeName, c_BinaryPredicateNodeType))
				{
					CompileBinaryPredicateNode(node);
				}
				else if (IsNodeType(typeName, c_ElementSelectorNodeType))
				{
					CompileElementSelectorNode(node);
				}
				else
				{
					throw new CompilerException(CompilerError::modelNodeTypeNotSupported);
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
				case VariableScope::RValue:
					emittedVar = _rValueVars.Alloc();
					pPrefix = (var.IsTreeNode()) ? &c_nodeVar : &c_fnVar;
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
			if (isInput)
			{
				_inputArgs.push_back({ pVar->EmittedName(), GetPtrType(varType) });
			}
			
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

		Variable* Compiler::EnsureVariableFor(const model::OutputPortBase* pPort)
		{
			Variable* pVar = GetVariableFor(pPort);
			if (pVar == nullptr)
			{
				throw new CompilerException(CompilerError::variableForOutputNotFound);
			}
			return pVar;
		}

		Variable* Compiler::GetVariableFor(const model::OutputPortElement elt)
		{
			return GetVariableFor(elt.ReferencedPort());
		}

		Variable* Compiler::EnsureVariableFor(const model::OutputPortElement elt)
		{
			return EnsureVariableFor(elt.ReferencedPort());
		}

		void Compiler::SetVariableFor(const model::OutputPortBase* pPort, Variable* pVar)
		{
			_portToVarMap[pPort] = pVar;
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
				case model::Port::PortType::Boolean:
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
				else if (ModelEx::IsLeafNode(node))
				{
					HandleLeafNode(node);
				}
			});
		}

		void Compiler::HandleLeafNode(const model::Node& node)
		{
			AllocArg(node.GetOutputPorts()[0], false);
		}

		void Compiler::VerifyIsScalar(const model::InputPortBase& port)
		{
			if (!ModelEx::IsScalar(port))
			{
				throw new CompilerException(CompilerError::scalarInputsExpected);
			}
		}

		void Compiler::VerifyIsScalar(const model::OutputPortBase& port)
		{
			if (!ModelEx::IsScalar(port))
			{
				throw new CompilerException(CompilerError::scalarOutputsExpected);
			}
		}

		void Compiler::VerifyIsPureBinary(const model::Node& node)
		{
			if (!ModelEx::IsPureBinary(node))
			{
				// Only support binary right now
				throw new CompilerException(CompilerError::binaryInputsExpected);
			}
		}

		void Compiler::Reset()
		{
		}

		void Compiler::CompileInputNode(const model::Node& node)
		{
			// Input node is typically set up during pass1. By default, no further work needed
		}

		void Compiler::CompileOutputNode(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
				case model::Port::PortType::Real:
					CompileOutputNode(static_cast<const model::OutputNode<double>&>(node));
					break;
				case model::Port::PortType::Integer:
					CompileOutputNode(static_cast<const model::OutputNode<int>&>(node));
					break;
				case model::Port::PortType::Boolean:
					CompileOutputNode(static_cast<const model::OutputNode<bool>&>(node));
					break;
				default:
					throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void Compiler::CompileConstantNode(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
				case model::Port::PortType::Real:
					CompileConstant<double>(static_cast<const nodes::ConstantNode<double>&>(node));
					break;
				case model::Port::PortType::Integer:
					CompileConstant<int>(static_cast<const nodes::ConstantNode<int>&>(node));
					break;
				case model::Port::PortType::Boolean:
					CompileConstantBool(static_cast<const nodes::ConstantNode<bool>&>(node));
					break;
				default:
					throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void Compiler::CompileConstantBool(const nodes::ConstantNode<bool>& node)
		{
			auto output = node.GetOutputPorts()[0];
			if (!ModelEx::IsScalar(*output))
			{
				throw new CompilerException(CompilerError::scalarInputsExpected);
			}
			auto values = node.GetValues();
			// We always convert booleans as integers
			Variable* pVar = Variables().AddVariable<LiteralVar<int>>(values[0]);
			SetVariableFor(output, pVar);
		}

		void Compiler::CompileBinaryNode(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
				case model::Port::PortType::Real:
					CompileBinaryNode(static_cast<const nodes::BinaryOperationNode<double>&>(node));
					break;
				case model::Port::PortType::Integer:
					CompileBinaryNode(static_cast<const nodes::BinaryOperationNode<int>&>(node));
					break;
				default:
					throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void Compiler::CompileSumNode(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
				case model::Port::PortType::Real:
					CompileSumNode(static_cast<const nodes::SumNode<double>&>(node));
					break;
				case model::Port::PortType::Integer:
					CompileSumNode(static_cast<const nodes::SumNode<int>&>(node));
					break;
				default:
					throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void Compiler::CompileBinaryPredicateNode(const model::Node& node)
		{
			auto input = node.GetInputPorts()[0];
			switch (input->GetType())
			{
				case model::Port::PortType::Real:
					CompileBinaryPredicateNode(static_cast<const nodes::BinaryPredicateNode<double>&>(node));
					break;
				default:
					throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void Compiler::CompileElementSelectorNode(const model::Node& node)
		{
			auto value = node.GetInputPorts()[0];
			auto selector = node.GetInputPorts()[1];
			if (value->GetType() == model::Port::PortType::Real &&
				selector->GetType() == model::Port::PortType::Boolean)
			{
				CompileElementSelectorNode(static_cast<const nodes::ElementSelectorNode<double, bool>&>(node));
			}
			else
			{
				throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		template<>
		OperatorType Compiler::GetOperator<double>(const nodes::BinaryOperationNode<double>& node) const
		{
			using Bop = nodes::BinaryOperationNode<double>;
			switch (node.GetOperation())
			{
				case Bop::OperationType::add:
					return OperatorType::AddF;
				case Bop::OperationType::subtract:
					return OperatorType::SubtractF;
				case Bop::OperationType::coordinatewiseMultiply:
					return OperatorType::MultiplyF;
				case Bop::OperationType::divide:
					return OperatorType::DivideF;
				default:
					throw new CompilerException(CompilerError::binaryOperationTypeNotSupported);
			}
		}

		template<>
		OperatorType Compiler::GetOperator<int>(const nodes::BinaryOperationNode<int>& node) const
		{
			using Bop = nodes::BinaryOperationNode<int>;
			switch (node.GetOperation())
			{
				case Bop::OperationType::add:
					return OperatorType::Add;
				case Bop::OperationType::subtract:
					return OperatorType::Subtract;
				case Bop::OperationType::coordinatewiseMultiply:
					return OperatorType::Multiply;
				case Bop::OperationType::divide:
					return OperatorType::DivideS;
				default:
					throw new CompilerException(CompilerError::binaryOperationTypeNotSupported);
			}
		}

		template<>
		ComparisonType Compiler::GetComparison<double>(const nodes::BinaryPredicateNode<double>& node) const
		{
			using Bop = nodes::BinaryPredicateNode<double>;
			switch (node.GetPredicateType())
			{
				case Bop::PredicateType::equal:
					return ComparisonType::EqF;
				case Bop::PredicateType::greater:
					return ComparisonType::GtF;
				case Bop::PredicateType::less:
					return ComparisonType::LtF;
				default:
					throw new CompilerException(CompilerError::binaryOperationTypeNotSupported);
			}
		}

		template<>
		ComparisonType Compiler::GetComparison<int>(const nodes::BinaryPredicateNode<int>& node) const
		{
			using Bop = nodes::BinaryPredicateNode<int>;
			switch (node.GetPredicateType())
			{
				case Bop::PredicateType::equal:
					return ComparisonType::Eq;
				case Bop::PredicateType::greater:
					return ComparisonType::Gt;
				case Bop::PredicateType::less:
					return ComparisonType::Lt;
				default:
					throw new CompilerException(CompilerError::binaryOperationTypeNotSupported);
			}
		}

	}
}