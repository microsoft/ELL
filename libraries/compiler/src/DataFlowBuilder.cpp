#include "DataFlowBuilder.h"
#include "ModelEx.h"

namespace emll
{
	namespace compiler
	{		
		void OutputPortDataNodes::Add(DataNode* pNode)
		{
			assert(pNode != nullptr);
			_nodes.push_back(pNode);
		}

		DataNode* OutputPortDataNodes::Get(size_t position) const
		{
			if (position < _nodes.size())
			{
				return _nodes[position];
			}
			return nullptr;
		}


		OutputPortDataNodes* OutputPortDataNodesMap::Ensure(const model::OutputPortBase* pPort)
		{
			assert(pPort != nullptr);
			auto pPortNodes = Get(pPort);
			if (pPortNodes == nullptr)
			{
				auto portNodes = std::make_shared<OutputPortDataNodes>();
				_scalarMap[pPort] = portNodes;
				pPortNodes = portNodes.get();
			}
			return pPortNodes;
		}

		OutputPortDataNodes* OutputPortDataNodesMap::Get(const model::OutputPortBase* pPort) const
		{
			auto search = _scalarMap.find(pPort);
			if (search != _scalarMap.end())
			{
				return search->second.get();
			}
			return nullptr;
		}

		void OutputPortDataNodesMap::Add(const model::OutputPortBase* pPort, DataNode* pNode)
		{
			Ensure(pPort)->Add(pNode);
		}

		DataNode* OutputPortDataNodesMap::Get(const model::OutputPortBase* pPort, size_t position) const
		{
			OutputPortDataNodes* pPortNodes = Get(pPort);
			if (pPortNodes != nullptr)
			{
				return pPortNodes->Get(position);
			}
			return nullptr;
		}

		void OutputPortDataNodesMap::SetV(const model::OutputPortBase* pPort, DataNode* pNode)
		{
			assert(pNode != nullptr);
			assert(pNode->HasVectorResult());
			assert(pPort != nullptr);
			_vectorMap[pPort] = pNode;
		}

		DataNode* OutputPortDataNodesMap::GetV(const model::OutputPortBase* pPort) const
		{
			assert(pPort != nullptr);
			auto search = _vectorMap.find(pPort);
			if (search != _vectorMap.end())
			{
				return search->second;
			}
			return nullptr;

		}

		static const std::string c_ConstantNodeType = "Constant";
		static const std::string c_BinaryNodeType = "BinaryOperationNode";
		static const std::string c_InputNodeType = "Input";
		static const std::string c_DotProductType = "DotProductNode";
		static const std::string c_LinearNodeType = "LinearNode";
		static const std::string c_SumNodeType = "SumNode";

		void DataFlowBuilder::Process(const model::Model& mode)
		{
			mode.Visit([this](const model::Node& node) {
				Process(node);
			});
		}

		void DataFlowBuilder::Process(const model::Node& node)
		{
			std::string typeName = node.GetRuntimeTypeName();
			//
			// TODO: Make this a lookup table
			//
			if (typeName == c_BinaryNodeType)
			{
				ProcessBinaryOperation(node);
			}
			else if (typeName == c_ConstantNodeType)
			{
				ProcessConstant(node);
			}
			else if (typeName == c_InputNodeType)
			{
				ProcessInputNode(node);
			}
			else if (typeName == c_LinearNodeType)
			{
				ProcessLinearPredictorNode(node);
			}
			else
			{
				throw new CompilerException(CompilerError::nodeTypeNotSupported);
			}

			if (ModelEx::IsLeafNode(node))
			{
				ProcessOutputNode(node);
			}
		}

		void DataFlowBuilder::ProcessConstant(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
			case model::Port::PortType::Real:
				Process<double>(static_cast<const nodes::ConstantNode<double>&>(node));
				break;
			default:
				throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void DataFlowBuilder::ProcessBinaryOperation(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
			case model::Port::PortType::Real:
				Process<double>(static_cast<const nodes::BinaryOperationNode<double>&>(node));
				break;
			default:
				throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void DataFlowBuilder::ProcessInputNode(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
			case model::Port::PortType::Real:
				Process<double>(static_cast<const model::InputNode<double>&>(node));
				break;
			default:
				throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void DataFlowBuilder::ProcessOutputNode(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
				case model::Port::PortType::Real:
					AddOutput<double>(node);
					break;
				default:
					throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void DataFlowBuilder::ProcessSumNode(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
				case model::Port::PortType::Real:
					Process<double>(static_cast<const nodes::SumNode<double>&>(node));
					break;
				default:
					throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void DataFlowBuilder::ProcessLinearPredictorNode(const model::Node& node)
		{
			const nodes::LinearPredictorNode& lpNode = static_cast<const nodes::LinearPredictorNode&>(node);
			//
			// LinearPredictorNode has exactly 1 input Port and 1 output port
			//
			auto inputPort = lpNode.GetInputPorts()[0];
			if (!ModelEx::IsPureVector(*inputPort))
			{
				// TODO: Compilation for cases where the input port is not a "pure" vector - i.e. it takes inputs from multiple output ports
				throw new CompilerException(CompilerError::notSupported);
			}
			ProcessLinearPredictorV(lpNode);
		}

		void DataFlowBuilder::ProcessLinearPredictorV(const nodes::LinearPredictorNode& node)
		{
			//
			// LinearPredictorNode has exactly 1 input Port and 1 output port
			//
			auto inputPort = node.GetInputPorts()[0];
			auto pOutputPort = node.GetOutputPorts()[0];
			assert(pOutputPort->Size() == 1);

			auto predictor = node.GetPredictor();
			DotProductNodeV* pDotProduct = _graph.AddNode<DotProductNodeV>();
			LiteralNode* pWeights = _graph.AddLiteralV<double>(predictor.GetVector());
			pWeights->AddDependent(pDotProduct);
			AddDependencyV(inputPort, pDotProduct);

			LiteralNode* pBias = _graph.AddLiteral<double>(predictor.GetBias());
			BinaryNode *pResult = _graph.AddNode<BinaryNode>(OperatorType::AddF);
			_outputPortMap.Add(pOutputPort, pResult);
			pBias->AddDependent(pResult);
			pDotProduct->AddDependent(pResult);
		}

		DataNode* DataFlowBuilder::GetSourceNode(const model::InputPortBase* pPort, size_t elementIndex) const
		{
			assert(pPort != nullptr);

			auto elt = pPort->GetOutputPortElement(elementIndex);
			return _outputPortMap.Get(elt.ReferencedPort(), elt.GetIndex());
		}

		void DataFlowBuilder::AddDependency(const model::InputPortBase* pPort, size_t elementIndex, DataNode* pDependant)
		{			
			assert(pDependant != nullptr);
			DataNode* pNode = GetSourceNode(pPort, elementIndex);
			assert(pNode != nullptr);
			pNode->AddDependent(pDependant);
		}

		void DataFlowBuilder::AddDependencyV(const model::InputPortBase* pPort, DataNode* pDependant)
		{
			assert(pPort != nullptr);
			assert(pDependant != nullptr);

			auto elt = pPort->GetOutputPortElement(0);
			DataNode* pNode = _outputPortMap.GetV(elt.ReferencedPort());
			assert(pNode != nullptr && pNode->HasVectorResult());
			pNode->AddDependent(pDependant);
		}

		void DataFlowBuilder::AddDependency(const model::OutputPortBase* pPort, size_t elementIndex, DataNode* pDependant)
		{
			assert(pDependant != nullptr);
			DataNode* pNode = _outputPortMap.Get(pPort, elementIndex);
			assert(pNode != nullptr);
			pNode->AddDependent(pDependant);
		}

		template<>
		OperatorType DataFlowBuilder::GetOperator<double>(const nodes::BinaryOperationNode<double>& node)
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
					throw new CompilerException(CompilerError::operationTypeNotSupported);
			}
		}

	}
}