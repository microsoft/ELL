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

		void DataFlowBuilder::Process(const model::Model& mode)
		{
			mode.Visit([this](const model::Node& node){
				Process(node);
			});
		}

		void DataFlowBuilder::Process(const model::Node& node)
		{
			// TODO: Make this a lookup table
			if (node.GetRuntimeTypeName() == c_ConstantNodeType)
			{
				ProcessConstant(node);
			}
			else if (node.GetRuntimeTypeName() == c_BinaryNodeType)
			{
				ProcessBinaryOperation(node);
			}
		}

		void DataFlowBuilder::ProcessConstant(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
				case model::Port::PortType::Real:
					Process<double>(static_cast<const ConstantF&>(node));
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
				Process<double>(static_cast<const BinaryOperationF&>(node));
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
			pNode->AddDependent(pDependant);
		}
	}
}