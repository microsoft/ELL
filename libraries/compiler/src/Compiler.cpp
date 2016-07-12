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

		/// <summary>Base class for ML Compiler.</summary>
		Compiler::Compiler()
			: _nodeTypes()
		{
			InitSupportedNodeTypes();
		}

		void Compiler::Compile(const model::Model& model)
		{
			_inputs = ModelEx::CollectInputNodes(model);
			_outputs = ModelEx::CollectOutputNodes(model);
		}

		void Compiler::CompileLinear(const model::Node& modelRoot, const std::string& functionName)
		{
			VariableDecl input = { ValueType::PDouble, c_InputVariableName };
			VariableDecl output = { ValueType::PDouble, c_OutputVariableName };
			BeginFunction(functionName, input, output);
		}

		void Compiler::CompileInput(const model::Node& input)
		{
			NodeType type = GetNodeType(input);
			if (type != NodeType::input)
			{
				throw new CompilerException(CompilerError::InputNodeExpected);
			}
			VerifyOutputType(input, model::Port::PortType::Real);
		}

		Compiler::NodeType Compiler::GetNodeType(const model::Node& node) const
		{
			return _nodeTypes.Get(node.GetRuntimeTypeName());
		}

		void Compiler::VerifyInputType(const model::Node& node, const model::Port::PortType type)
		{
			auto inputs = node.GetInputs();
			for (auto& port : inputs)
			{
				if (port->GetType() != type)
				{
					throw new CompilerException(CompilerError::InputPortTypeNotSupported);
				}
			}
		}

		void Compiler::VerifyOutputType(const model::Node& node, const model::Port::PortType type)
		{
			auto outputs = node.GetOutputs();
			for (auto& port : outputs)
			{
				if (port->GetType() != type)
				{
					throw new CompilerException(CompilerError::OutputPortTypeNotSupported);
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

		model::Port::PortType ModelEx::GetNodeDataType(const model::Node& node)
		{
			return node.GetOutputs()[0]->GetType();
		}

		std::vector<const model::Node*> ModelEx::CollectOutputNodes(const model::Model& model)
		{
			auto findNodes = [](const model::Node& node)
			{
				return (typeid(node) == typeid(model::InputNode<double>) ||
					typeid(node) == typeid(model::InputNode<int>));
			};
			return CollectNodes(model, [](const model::Node& node) { return IsLeafNode(node); });
		}

		std::vector<const model::Node*> ModelEx::CollectInputNodes(const model::Model& model)
		{
			auto findNodes = [](const model::Node& node)
			{
				return (typeid(node) == typeid(model::InputNode<double>) ||
						typeid(node) == typeid(model::InputNode<int>));
			};
			return CollectNodes(model, findNodes);
		}

		std::vector<const model::Node*> ModelEx::CollectNodes(const model::Model& model, std::function<bool(const model::Node& node)> predicate)
		{
			std::vector<const model::Node*> matches;
			model.Visit([&matches, &predicate](const model::Node& node) {
				if (predicate(node))
				{
					matches.push_back(&node);
				}
			});
			return matches;
		}


		size_t ModelEx::CountOutputs(std::vector<const model::Node*>& nodes)
		{
			size_t count = 0;
			for (auto n : nodes)
			{
				count += n->GetOutputs().size();
			}
			return count;
		}

		size_t ModelEx::CountInputs(std::vector<const model::Node*>& nodes)
		{
			size_t count = 0;
			for (auto n : nodes)
			{
				count += n->GetInputs().size();
			}
			return count;
		}

		bool ModelEx::IsLeafNode(const model::Node& node)
		{
			return (node.GetDependentNodes().size() == 0);
		}
	}
}