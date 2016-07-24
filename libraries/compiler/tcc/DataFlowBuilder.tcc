#include "OutputPort.h"

namespace emll
{
	namespace compiler
	{
		template<typename DataType>
		void DataFlowBuilder::Process(const nodes::ConstantNode<DataType>& node)
		{
			auto pOutputPort = node.GetOutputPorts()[0];
			auto output = node.output.GetOutput();
			for (size_t i = 0; i < output.size(); ++i)
			{
				auto pNode = _graph.AddLiteral<DataType>(output[i]);
				_outputPortMap.Add(pNode, pOutputPort);
			}
		}

		template<typename DataType>
		void DataFlowBuilder::Process(const nodes::BinaryOperationNode<DataType>& node)
		{
			// BinaryOperations have 1 output and 2 input ports
			auto pOutputPort = node.GetOutputPorts()[0];
			auto leftInput = node.GetInputPorts()[0];
			auto rightInput = node.GetInputPorts()[1];
			for (size_t i = 0; i < pOutputPort->Size(); ++i)
			{
				auto pNode = _graph.AddNode<BinaryNode>(GetOperator<DataType>(node));
				_outputPortMap.Add(pNode, pOutputPort);
				AddDependency(leftInput, i, pNode);
				AddDependency(rightInput, i, pNode);
			}
		}

		template<typename DataType>
		void DataFlowBuilder::Process(const model::InputNode<DataType>& node)
		{
			// InputNodes have exactly 1 output port
			auto pOutputPort = node.GetOutputPorts()[0];
			ArgNode* pArg = _graph.AddArg<DataType>(pOutputPort->Size(), true);			
			for (size_t i = 0; i < pOutputPort->Size(); ++i)
			{
				auto pInput = _graph.AddNode<InputNode>(i);
				_outputPortMap.Add(pInput, pOutputPort);
				pArg->AddDependent(pInput);
			}
		}

		template<typename DataType>
		void DataFlowBuilder::AddOutput(const model::Node& leafNode)
		{
			for (auto pOutputPort : leafNode.GetOutputPorts())
			{
				ArgNode* pArg = _graph.AddArg<DataType>(pOutputPort->Size(), false);
				for (size_t i = 0; i < pOutputPort->Size(); ++i)
				{
					auto pOutput = _graph.AddNode<OutputNode>(pArg->Var(), i);
					AddDependency(pOutputPort, i, pOutput);
				}
			}
		}
	}
}