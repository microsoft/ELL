#pragma once

#include <string>
#include <ostream>
#include "Compiler.h"

void TestLLVM();

class ModelBuilder
{
public:
	ModelBuilder();
	ModelBuilder(const ModelBuilder& src);
	ModelBuilder(ModelBuilder&& src);

	template<typename T>
	model::InputNode<T>* Inputs(size_t count);
	template<typename T>
	model::InputNode<T>* Inputs(std::vector<T>& values);

	template<typename T>
	nodes::BinaryOperationNode<T>* Add(const model::OutputPort<T>& x, const model::OutputPort<T>& y);

	template<typename T>
	nodes::ConstantNode<T>* Constant(const T value);
	template<typename T>
	nodes::ConstantNode<T>* Constant(const std::vector<T>& values);

	// Expose as a property!
	model::Model& Model = _model;

private:
	model::Model _model;
};

void TestModelEx();