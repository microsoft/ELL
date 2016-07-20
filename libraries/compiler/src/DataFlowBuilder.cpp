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

		DataNode* OutputPortDataNodes::Get(size_t position)
		{
			return _nodes[position];
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

		OutputPortDataNodes* OutputPortDataNodesMap::Get(const model::OutputPortBase* pPort)
		{
			auto search = _map.find(pPort);
			if (search != _map.end())
			{
				return search->second.get();
			}
			return nullptr;

		}

		const std::string c_ConstantNodeType = "Constant";

		void DataFlowBuilder::Process(const model::Node& node)
		{
			// TODO: Make this a lookup table
			if (node.GetRuntimeTypeName() == c_ConstantNodeType)
			{
				ProcessConstant(node);
			}
		}

		void DataFlowBuilder::ProcessConstant(const model::Node& node)
		{
			switch (ModelEx::GetNodeDataType(node))
			{
			case model::Port::PortType::Real:
				Process(static_cast<const ConstantF&>(node));
				break;
			default:
				throw new CompilerException(CompilerError::portTypeNotSupported);
			}
		}

		void DataFlowBuilder::Process(const ConstantF& node)
		{
			// TODO: support vector literals

		}
	}
}