#include "ModelEx.h"

namespace emll
{
	namespace compiler
	{
		model::Port::PortType ModelEx::GetNodeDataType(const model::Node& node)
		{
			return node.GetOutputPorts()[0]->GetType();
		}

		std::vector<const model::Node*> ModelEx::CollectOutputNodes(const model::Model& model)
		{
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
				count += n->GetOutputPorts().size();
			}
			return count;
		}

		size_t ModelEx::CountInputs(std::vector<const model::Node*>& nodes)
		{
			size_t count = 0;
			for (auto n : nodes)
			{
				count += n->GetInputPorts().size();
			}
			return count;
		}

		bool ModelEx::IsLeafNode(const model::Node& node)
		{
			return (node.GetDependentNodes().size() == 0);
		}
	}
}