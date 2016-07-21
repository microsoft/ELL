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
			: _nodeTypes()
		{
			InitSupportedNodeTypes();
		}

		const std::string& Compiler::InputName() const
		{
			return c_InputVariableName;
		}

		const std::string& Compiler::OutputName() const
		{
			return c_OutputVariableName;
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

		void Compiler::CompileModel(const model::Model& model)
		{
			_inputs = ModelEx::CollectInputNodes(model);
			_outputs = ModelEx::CollectOutputNodes(model);

			NamedValueTypeList fnArgs;
			AddArgs(fnArgs, InputName(), Inputs());
			AddArgs(fnArgs, OutputName(), Outputs());

			BeginMain(c_PredictFunctionName, fnArgs);
			EndMain();
		}

		void Compiler::CompileNode(DataNode& node)
		{
			NamedValueTypeList fnArgs;
			fnArgs.init({ { InputName(), ValueType::PDouble },{ OutputName(), ValueType::PDouble } });

			BeginMain(c_PredictFunctionName, fnArgs);
			switch (node.Type())
			{
				case DataNodeType::LiteralNode:
					Compile(static_cast<LiteralNode&>(node));
					break;
				default:
					throw new CompilerException(CompilerError::notSupported);
			}
			EndMain();
		}

		void Compiler::AddArgs(NamedValueTypeList& args, const std::string& namePrefix, const std::vector<const model::Node*>& nodes)
		{
			for (size_t n = 0; n < nodes.size(); ++n)
			{
				auto node = nodes[n];
				std::string argNamePrefix = MakeVarName(namePrefix, n);
				auto outputs = node->GetOutputPorts();
				for (size_t i = 0; i < outputs.size(); ++i)
				{
					std::string argName;
					if (i > 0)
					{
						argName = MakeVarName(argName, i);
					}
					else
					{
						argName = argNamePrefix;
					}
					AddArgs(args, argName, outputs[i]);
				}
			}
		}

		void Compiler::AddArgs(NamedValueTypeList& args, const std::string& name, const model::OutputPortBase* pOutput)
		{
			model::Port::PortType type = pOutput->GetType();
			switch (type)
			{
			case model::Port::PortType::Real:
				args.push_back({ name, ValueType::PDouble });
				break;
			case model::Port::PortType::Integer:
				args.push_back({ name, ValueType::PInt32 });
				break;
			default:
				throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		std::string Compiler::MakeVarName(const std::string& namePrefix, size_t i)
		{
			std::string name = namePrefix;
			name.append(std::to_string(i));
			return name;
		}

		Compiler::NodeType Compiler::GetNodeType(const model::Node& node) const
		{
			return _nodeTypes.Get(node.GetRuntimeTypeName());
		}

		void Compiler::VerifyInputType(const model::Node& node, const model::Port::PortType type)
		{
			auto inputs = node.GetInputPorts();
			for (auto& port : inputs)
			{
				if (port->GetType() != type)
				{
					throw new CompilerException(CompilerError::portTypeNotSupported);
				}
			}
		}

		void Compiler::VerifyOutputType(const model::Node& node, const model::Port::PortType type)
		{
			auto outputs = node.GetOutputPorts();
			for (auto& port : outputs)
			{
				if (port->GetType() != type)
				{
					throw new CompilerException(CompilerError::portTypeNotSupported);
				}
			}
		}

		void Compiler::InitSupportedNodeTypes()
		{
			_nodeTypes.Init	({
				{"Input", NodeType::input},
				{"ConstantNode", NodeType::constant},
				{"BinaryOperationNode", NodeType::binaryOp}
			});
		}

		void Compiler::Reset()
		{
		}
	}
}