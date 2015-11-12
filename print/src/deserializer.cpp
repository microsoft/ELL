// deserializer.cpp

#include "deserializer.h"
#include "print.h"
#include <memory>
#include <stdexcept>

using std::make_shared;
using std::runtime_error;

void mappings::Deserialize(JsonSerializer& js, std::shared_ptr<mappings::Mapping>& up)
{
	auto type = js.read<string>("_type");
	auto version = js.read<int>("_version");

	if (type == "Constant")
	{
		up = make_shared<print::PrintableConstant>();
	}

	else if(type == "Scale")
	{
		up = make_shared<print::PrintableScale>();
	}

	else if(type == "Shift")
	{
		up = make_shared<print::PrintableShift>();
	}

	else if(type == "Sum")
	{
		up = make_shared<print::PrintableSum>();
	}

	else if (type == "DecisionTreePath")
	{
		up = make_shared<print::PrintableDecisionTreePath>();
	}

	else if (type == "Row")
	{
		up = make_shared<print::PrintableRow>();
	}

	else if (type == "Column")
	{
		up = make_shared<print::PrintableColumn>();
	}

	else
	{
		throw runtime_error("unidentified type in map file: " + type);
	}

	up->Deserialize(js, version);
}

