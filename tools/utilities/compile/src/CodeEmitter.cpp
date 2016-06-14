////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CodeGenerator.cpp (compile)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CodeEmitter.h"
#include "Format.h"
#include <assert.h>

void CodeEmitter::Emit(const std::string& inputVar, DataFlowNode& inputNode, uint64_t layerIndex, uint64_t elementIndex)
{
	auto assignment = AllocVar(inputNode);
	EmitStatement(inputNode.GetVariableName(), assignment, inputVar, layerIndex, elementIndex);
}

void CodeEmitter::Emit(const std::string& sourceVar, DataFlowNode& targetNode, LinearOperation op, uint64_t layerIndex, uint64_t elementIndex)
{
	auto assignment = AllocVar(targetNode);
	auto targetVar= targetNode.GetVariableName();
	EmitStatement(targetVar, assignment, op, sourceVar, layerIndex, elementIndex);
}

CodeEmitter::Assignment CodeEmitter::AllocVar(DataFlowNode& node)
{
	if (node.IsInitialized())
	{
		return CodeEmitter::Assignment::IncrementBy;
	}

	if (node.HasFixedVariableName())
	{
		return CodeEmitter::Assignment::Set;
	}

	bool isNovel = _varStack.IsTopNovel();
	node.SetTempVariableIndex(_varStack.Pop());

	if (isNovel)
	{
		return CodeEmitter::Assignment::Declare;
	}
	
	return CodeEmitter::Assignment::Set;
}

void CodeEmitter::ReleaseVar(DataFlowNode& node)
{
	_varStack.Push(node.GetTempVariableIndex());
}

//-------------------------------------------------------
//
// CGenerator
//
//-------------------------------------------------------

CEmitter::CEmitter(std::ostream& os)
	: _os(os)
{
}

void CEmitter::BeginLinear(const char* name, uint64_t inputCount, const layers::CoordinateList& outputs)
{
	auto str = "// Input dimension: %\n// Output dimension: %\n// Output coordinates:";
	utilities::PrintFormat(_os, str, inputCount, outputs.Size());
	for (uint64_t i = 0; i < outputs.Size(); ++i)
	{
		_os << ' ' << outputs[i];
	}
	_os << "\n";
	//
	// Function Declaration
	//
	utilities::PrintFormat(_os, "void %(const double* input, double* output)\n{\n", name);
}

void CEmitter::EndLinear()
{
	_os << "}\n";
}

void CEmitter::EmitStatement(const std::string& var, Assignment assignment, const std::string& inputVar, uint64_t layerIndex, uint64_t elementIndex)
{
	const char* format = ToString(assignment);
	utilities::PrintFormat(_os, format, var, inputVar, layerIndex, elementIndex);
}

void CEmitter::EmitStatement(const std::string& var, Assignment assignment, LinearOperation op, const std::string& sourceVar, uint64_t layerIndex, uint64_t elementIndex)
{
	const char* format = ToString(assignment);
	std::string rhs = ToString(op, sourceVar);
	utilities::PrintFormat(_os, format, var, rhs, layerIndex, elementIndex);
}

const char* CEmitter::ToString(CodeEmitter::Assignment assignment)
{
	const char* format = nullptr;
	switch (assignment)
	{
	default:
		break;
	case Assignment::Declare:
		format = " double % = %; // coordinate (%,%), allocating new temporary variable\n";
		break;
	case Assignment::Set:
		format = " % = %; // coordinate (%,%)\n";
		break;
	case Assignment::IncrementBy:
		format = " % += %; // coordinate (%,%)\n";
		break;
	case Assignment::Reset:
		format = " % = %; // coordinate (%,%), reassigning temporary variable\n";
		break;
	}
	assert(format != nullptr);
	return format;
}

std::string CEmitter::ToString(LinearOperation op, const std::string& sourceVar)
{
	double a = op.MultiplyBy();
	double b = op.IncrementBy();
	if (b == 0)
	{
		if (a != 1)
		{
			return utilities::PrintFormat("% * %", a, sourceVar);
		}
		else
		{
			return sourceVar;
		}
	}
	else
	{
		if (a == 0)
		{
			return std::to_string(b);
		}
		else if (a == 1)
		{
			return utilities::PrintFormat("% + %", sourceVar, b);
		}
		else
		{
			return utilities::PrintFormat("% * % + %", a, sourceVar, b);
		}
	}
}
