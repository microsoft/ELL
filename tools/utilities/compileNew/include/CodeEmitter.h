////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     CodeGenerator.h (compile)
//  Authors:  Umesh Madan
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "IntegerStack.h"
#include "DataFlowNode.h"

#include <string>
#include <ostream>

//
// Abstract base class for Code Generators
//
class CodeEmitter
{
public:
	enum Assignment
	{
		Declare,
		Set,
		Reset,
		IncrementBy
	};

public:	

	virtual void BeginLinear(const char* name, uint64_t inputCount, const layers::CoordinateList& outputs) = 0;
	void Emit(const std::string& inputVar, DataFlowNode& inputNode, uint64_t layerIndex, uint64_t elementIndex);
	void Emit(const std::string& sourceVar, DataFlowNode& targetNode, LinearOperation op, uint64_t layerIndex, uint64_t elementIndex);
	virtual void EndLinear() = 0;

	Assignment AllocVar(DataFlowNode& node);
	void ReleaseVar(DataFlowNode& node);

protected:
	virtual void EmitStatement(const std::string& var, Assignment assignment, const std::string& inputVar, uint64_t layerIndex, uint64_t elementIndex) = 0;
	virtual void EmitStatement(const std::string& var, Assignment assignment, LinearOperation op, const std::string& sourceVar, uint64_t layerIndex, uint64_t elementIndex) = 0;

private:
	
private:
	utilities::IntegerStack _varStack;
};

class CEmitter : public CodeEmitter
{
public:
	CEmitter(std::ostream& os);

	void BeginLinear(const char* name, uint64_t inputCount, const layers::CoordinateList& outputs) override;
	void EndLinear() override;

protected:
	virtual void EmitStatement(const std::string& var, Assignment assignment, const std::string& inputVar, uint64_t layerIndex, uint64_t elementIndex) override;
	virtual void EmitStatement(const std::string& var, Assignment assignment, LinearOperation op, const std::string& sourceVar, uint64_t layerIndex, uint64_t elementIndex) override;

private:
	const char* ToString(Assignment assignment);
	std::string ToString(LinearOperation op, const std::string& sourceVar);

private:
	std::ostream& _os;
};