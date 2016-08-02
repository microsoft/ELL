#include "ModelEx.h"

namespace emll
{
	namespace compiler
	{
		model::Port::PortType ModelEx::GetNodeDataType(const model::Node& node)
		{
			return node.GetOutputPorts()[0]->GetType();
		}

		bool ModelEx::IsLeafNode(const model::Node& node)
		{
			return (node.GetDependentNodes().size() == 0);
		}

		bool ModelEx::IsPureVector(const model::InputPortBase& port)
		{
			auto inputs = port.GetInputRanges();
			return (inputs.NumRanges() == 1 && (inputs.begin()->Size() > 1));
		}

		bool ModelEx::IsScalar(const model::InputPortBase& port)
		{
			return (port.Size() == 1);
		}

		bool ModelEx::IsScalar(const model::OutputPortBase& port)
		{
			return (port.Size() == 1);
		}
	}
}