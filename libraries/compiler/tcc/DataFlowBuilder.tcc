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
				auto *pNode = _graph.AddLiteral<DataType>(output[i]);
				_outputPortMap.Add(pNode, pOutputPort);
			}
		}

		template<typename DataType>
		void DataFlowBuilder::Process(const nodes::BinaryOperationNode<DataType>& node)
		{
			auto pOutputPort = node.GetOutputPorts()[0];
			auto leftInput = node.GetInputPorts()[0];
			auto rightInput = node.GetInputPorts()[1];
			for (size_t i = 0; i < pOutputPort->Size(); ++i)
			{
				auto *pNode = _graph.AddBinary(GetOperator<DataType>(node));
				_outputPortMap.Add(pNode, pOutputPort);
				AddDependency(leftInput, i, pNode);
				AddDependency(rightInput, i, pNode);
			}
		}
	}
}