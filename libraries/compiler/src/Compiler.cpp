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
		const std::string c_InputVariableName = "input";
		const std::string c_OutputVariableName = "output";
		const std::string c_PredictFunctionName = "Predict";

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

		TempVar Compiler::AllocTemp()
		{
			return _tempVars.Alloc();
		}

		void Compiler::FreeTemp(TempVar var)
		{
			_tempVars.Free(var);
		}

		uint64_t Compiler::AllocGlobal()
		{
			_globalVars++;
			return _globalVars;
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
				case DataNodeType::Literal:
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