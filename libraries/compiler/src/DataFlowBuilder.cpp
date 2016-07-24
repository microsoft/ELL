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
				_map[pPort] = portNodes;
				pPortNodes = portNodes.get();
			}
			return pPortNodes;
		}

		OutputPortDataNodes* OutputPortDataNodesMap::Get(const model::OutputPortBase* pPort) const
		{
			auto search = _map.find(pPort);
			if (search != _map.end())
			{
				return search->second.get();
			}
			return nullptr;
		}

		void OutputPortDataNodesMap::Add(DataNode* pNode, const model::OutputPortBase* pPort)
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

		static const std::string c_ConstantNodeType = "Constant";
		static const std::string c_BinaryNodeType = "BinaryOperationNode";
		static const std::string c_InputNodeType = "Input";

		void DataFlowBuilder::Process(const model::Model& mode)
		{
			mode.Visit([this](const model::Node& node){
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

				case Bop::OperationType::multiply:
					return OperatorType::MultiplyF;

				case Bop::OperationType::divide:
					return OperatorType::DivideF;

				default:
					throw new CompilerException(CompilerError::operationTypeNotSupported);
			}
		}

	}
}