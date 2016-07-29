#include <iostream>
#include <string>
#include <ostream>

#include "CompilerTest.h"
#include "IRInclude.h"
#include "IRCompiler.h"
#include "ScalarVar.h"
#include "VectorVar.h"
#include "testing.h"

using namespace emll::compiler;

void NodePrinter(const model::Node& node)
{
	std::cout << "node_" << node.GetId() << " = " << node.GetRuntimeTypeName() << std::endl;
}

void TestLLVMShiftRegister()
{
	IREmitter llvm;
	IRModuleEmitter module(llvm, llvm.AddModule("Shifter"));
	module.DeclarePrintf();

	std::vector<double> data({ 1.1, 2.1, 3.1, 4.1, 5.1});
	std::vector<double> newData1({ 1.2, 2.2 });
	std::vector<double> newData2({ 3.2, 4.2 });

	auto fn = module.AddMain();
	llvm::GlobalVariable* pRegister = module.Global("g_shiftRegister", data);
	llvm::Value* c1= module.Constant("c_1", newData1);
	llvm::Value* c2 = module.Constant("c_2", newData2);

	fn.Print("Begin\n");
	fn.PrintForEach("%f\n", pRegister, data.size());
	fn.Print("Shift 1\n");
	fn.ShiftAndUpdate<double>(pRegister, data.size(), newData1.size(), c1);
	fn.PrintForEach("%f\n", pRegister, data.size());
	fn.Print("Shift 2\n");
	fn.ShiftAndUpdate<double>(pRegister, data.size(), newData2.size(), c2);
	fn.PrintForEach("%f\n", pRegister, data.size());
	fn.Ret();
	fn.Verify();

	module.Dump();
	module.WriteBitcodeToFile("C:\\junk\\model\\shift.bc");
	}

void TestLLVM()
{
	IREmitter llvm;
	IRModuleEmitter module(llvm, llvm.AddModule("Looper"));
	module.DeclarePrintf();

	llvm::StructType* structType = module.Struct("ShiftRegister", { ValueType::Int32, ValueType::Double });

	std::vector<double> data({ 3.3, 4.4, 5.5, 6.6, 7.7 });
	llvm::GlobalVariable* pData = module.Constant("g_weights", data);
	llvm::GlobalVariable* pOutput = module.Global(ValueType::Double, "g_output", data.size());
	llvm::GlobalVariable* pTotal = module.Global(ValueType::Double, "g_total");
	llvm::GlobalVariable* pRegisters = module.Global("g_registers", structType, data.size());

	auto fnMain = module.AddMain();

	auto vectorResult = fnMain.DotProductF(data.size(), fnMain.Ptr(pData), fnMain.Ptr(pData));
	fnMain.Printf({ fnMain.Literal("DOT %f\n"), fnMain.Load(vectorResult)});

	IRForLoopEmitter forLoop(fnMain);
	auto pBodyBlock = forLoop.Begin(data.size());
	{
		auto printBlock = fnMain.BlockAfter(pBodyBlock, "PrintBlock");
		fnMain.Branch(printBlock);
		fnMain.CurrentBlock(printBlock);

		auto i = forLoop.LoadIterationVar();
		auto item = fnMain.ValueAt(pData, i);
		auto sum = fnMain.Op(OperatorType::AddF, fnMain.Literal(0.3), item);
		fnMain.SetValueAt(pOutput, i, sum);
		fnMain.OpAndUpdate(pTotal, OperatorType::AddF, sum);

		llvm::Value* pRegisterSum = fnMain.PtrOffset(pRegisters, i, fnMain.Literal(1));
		fnMain.Store(pRegisterSum, sum);

		//auto itemInt = fnMain.CastFloatToInt(item);
		IRIfEmitter ife(fnMain);
		ife.If(ComparisonType::LtF, item, fnMain.Literal(5.7));
		{
			fnMain.Print("First IF!\n");
		}
		ife.If(ComparisonType::EqF, item, fnMain.Literal(6.6));
		{
			fnMain.Print("Second If!\n");
		}
		ife.Else();
		{
			fnMain.Print("Else\n");
		}
		ife.End();
		fnMain.Printf({ fnMain.Literal("%d, %f\n"), i,  item });
	}
	forLoop.End();

	fnMain.SetValueAt(pOutput, fnMain.Literal(3), fnMain.Literal(10.0));
	fnMain.SetValueAt(pOutput, fnMain.Literal(4), fnMain.Literal(20.0));

	auto pOtherTotal = module.Global(ValueType::Double, "g_total");
	forLoop.Clear();
	forLoop.Begin(data.size());
	{
		auto ival = forLoop.LoadIterationVar();
		auto v = fnMain.ValueAt(pOutput, ival);

		llvm::Value* pRegisterSum = fnMain.Load(fnMain.PtrOffset(pRegisters, ival, fnMain.Literal(1)));

		fnMain.OpAndUpdate(pOtherTotal, OperatorType::AddF, v);
		fnMain.Printf({ fnMain.Literal("%f, %f\n"), v, pRegisterSum });
	}
	forLoop.End();
	fnMain.Printf({ fnMain.Literal("Total = %f, OtherTotal= %f\n"), fnMain.Load(pTotal), fnMain.Load(pOtherTotal) });

	fnMain.Ret();

	fnMain.Verify();
	module.Dump();
	module.WriteBitcodeToFile("C:\\junk\\model\\loop.bc");
}

ModelBuilder::ModelBuilder()
{
}

ModelBuilder::ModelBuilder(const ModelBuilder& src)
	: _model(src._model)
{
}

ModelBuilder::ModelBuilder(ModelBuilder&& src)
	: _model(std::move(src._model))
{
}

template<typename T>
model::InputNode<T>* ModelBuilder::Inputs(size_t count)
{
	return _model.AddNode<model::InputNode<T>>(count);
}

template<typename T>
model::InputNode<T>* ModelBuilder::Inputs(std::vector<T>& values)
{
	auto node = Inputs<T>(values.size());
	node->SetInput(values);
	return node;
}

template<typename T>
model::OutputNode<T>* ModelBuilder::Outputs(const model::OutputPort<T>& x)
{
	return _model.AddNode<model::OutputNode<T>>(x);
}

template<typename T>
nodes::BinaryOperationNode<T>* ModelBuilder::Add(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
	return _model.AddNode<nodes::BinaryOperationNode<T>>(x, y, nodes::BinaryOperationNode<T>::OperationType::add);
}

template<typename T>
nodes::BinaryOperationNode<T>* ModelBuilder::Subtract(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
	return _model.AddNode<nodes::BinaryOperationNode<T>>(x, y, nodes::BinaryOperationNode<T>::OperationType::subtract);
}

template<typename T>
nodes::BinaryOperationNode<T>* ModelBuilder::Multiply(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
	return _model.AddNode<nodes::BinaryOperationNode<T>>(x, y, nodes::BinaryOperationNode<T>::OperationType::coordinatewiseMultiply);
}

template<typename T>
nodes::BinaryOperationNode<T>* ModelBuilder::Divide(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
	return _model.AddNode<nodes::BinaryOperationNode<T>>(x, y, nodes::BinaryOperationNode<T>::OperationType::divide);
}

template<typename T>
nodes::DotProductNode<T>* ModelBuilder::DotProduct(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
	return _model.AddNode<nodes::DotProductNode<T>>(x, y);
}

template<typename T>
nodes::SumNode<T>* ModelBuilder::Sum(const model::OutputPort<T>& x)
{
	return _model.AddNode<nodes::SumNode<T>>(x);
}

template<typename T>
nodes::DelayNode<T>* ModelBuilder::Delay(const model::OutputPort<T>& x, size_t windowSize)
{
	return _model.AddNode<nodes::DelayNode<T>>(x, windowSize);
}

template<typename T>
nodes::AccumulatorNode<T>* ModelBuilder::Accumulate(const model::OutputPort<T>& x)
{
	return _model.AddNode<nodes::AccumulatorNode<T>>(x);
}

template<typename T>
nodes::ConstantNode<T>* ModelBuilder::Constant(const T value)
{
	return _model.AddNode<nodes::ConstantNode<T>>(value);
}

template<typename T>
nodes::ConstantNode<T>* ModelBuilder::Constant(const std::vector<T>& values)
{
	auto *pNode = _model.AddNode<nodes::ConstantNode<T>>(values);
	// Work around a bug. Make sure literal values are propagated to outputs
	_model.ComputeNodeOutput<T>(pNode->output);
	return pNode;
}

model::Model InitTestModelBinOp()
{
	ModelBuilder builder;
	std::vector<double> data = { 10, 100, 1000, 10000 };

	auto input = builder.Inputs<double>(data);
	auto c = builder.Constant<double>({ 5, 50, 500, 5000 });

	nodes::BinaryOperationNode<double>* multNode = builder.Multiply<double>(input->output, c->output);
	multNode = builder.Multiply<double>(multNode->output, c->output);	
	multNode = builder.Multiply<double>(multNode->output, c->output);
	auto addNode = builder.Add<double>(input->output, multNode->output);
	addNode = builder.Add<double>(c->output, addNode->output);
	multNode = builder.Multiply<double>(addNode->output, c->output);
	multNode = builder.Multiply<double>(multNode->output, c->output);
	addNode = builder.Add<double>(c->output, multNode->output);

	return builder.Model;
}

model::Model InitTestModelSimple()
{
	ModelBuilder mb;
	auto input = mb.Inputs<double>(2);
	auto c = mb.Constant<double>({ 5, 3});

	nodes::BinaryOperationNode<double>* multNode = mb.Multiply<double>(input->output, c->output);
	auto addNode = mb.Add<double>(c->output, multNode->output);
	addNode = mb.Add<double>(c->output, addNode->output);
	return mb.Model;
}

void TestBinaryVector(bool expanded)
{
	std::vector<double> data = { 5, 10, 15, 20};
	std::vector<double> data2 = { 4, 4, 4, 4};

	ModelBuilder mb;

	auto input1 = mb.Inputs<double>(4);
	auto c1 = mb.Constant<double>(data);
	auto c2 = mb.Constant<double>(data2);

	auto bop = mb.Add(c1->output, input1->output);
	auto bop2 = mb.Multiply(bop->output, c2->output);
	auto output = mb.Outputs<double>(bop2->output);

	IRCompiler compiler("EMLL", std::cout);
	compiler.ShouldUnrollLoops() = expanded;
	compiler.CompileModel("TestBinaryVector", mb.Model);
	compiler.DebugDump();
}

void TestBinaryScalar()
{
	std::vector<double> data = {5};
	std::vector<double> data2 = {4};

	ModelBuilder mb;

	auto input1 = mb.Inputs<double>(1);
	auto c1 = mb.Constant<double>(data);
	auto c2 = mb.Constant<double>(data2);

	auto bop = mb.Add(c1->output, input1->output);
	//auto bop2 = mb.Multiply(bop->output, c2->output);
	auto output = mb.Outputs<double>(bop->output);

	IRCompiler compiler("EMLL", std::cout);
	compiler.CompileModel("TestBinaryScalar", mb.Model);
	compiler.DebugDump();
}

void TestDotProduct(bool expanded)
{
	std::vector<double> data = { 5, 10, 15, 20 };

	ModelBuilder mb;
	auto c1 = mb.Constant<double>(data);
	auto input1 = mb.Inputs<double>(4);
	auto dotProduct = mb.DotProduct<double>(c1->output, input1->output);
	auto output = mb.Outputs<double>(dotProduct->output);

	IRCompiler compiler("EMLL", std::cout);
	compiler.ShouldUnrollLoops() = expanded;
	compiler.CompileModel("TestDotProduct", mb.Model);
	compiler.DebugDump();
}

void TestSum(bool expanded)
{
	std::vector<double> data = { 5, 10, 15, 20 };

	ModelBuilder mb;
	auto c1 = mb.Constant<double>(data);
	auto input1 = mb.Inputs<double>(4);
	auto product = mb.Multiply<double>(c1->output, input1->output);
	auto sum = mb.Sum<double>(product->output);
	auto output = mb.Outputs<double>(sum->output);

	IRCompiler compiler("EMLL", std::cout);
	compiler.ShouldUnrollLoops() = expanded;
	compiler.CompileModel("TestSum", mb.Model);
	compiler.DebugDump();
}

void TestAccumulator(bool expanded)
{
	std::vector<double> data = { 5, 10, 15, 20 };

	ModelBuilder mb;
	auto c1 = mb.Constant<double>(data);
	auto input1 = mb.Inputs<double>(4);
	auto product = mb.Multiply<double>(c1->output, input1->output);
	auto accumulate = mb.Accumulate<double>(product->output);
	auto output = mb.Outputs<double>(accumulate->output);

	IRCompiler compiler("EMLL", std::cout);
	compiler.ShouldUnrollLoops() = expanded;
	compiler.CompileModel("TestAccumulator", mb.Model);
	compiler.DebugDump();
}

void TestDelay()
{
	ModelBuilder mb;
	auto input1 = mb.Inputs<double>(4);
	auto delay = mb.Delay<double>(input1->output, 3);
	auto output = mb.Outputs<double>(delay->output);

	IRCompiler compiler("EMLL", std::cout);
	compiler.CompileModel("TestDelay", mb.Model);
	compiler.DebugDump();
}

void TestSlidingAverage()
{
	ModelBuilder mb;
	auto dim = mb.Constant<double>(4);
	auto input1 = mb.Inputs<double>(4);
	auto delay = mb.Delay<double>(input1->output, 2);
	auto sum = mb.Sum<double>(delay->output);
	auto avg = mb.Divide<double>(sum->output, dim->output);
	auto output = mb.Outputs<double>(avg->output);

	IRCompiler compiler("EMLL", std::cout);
	compiler.CompileModel("TestSlidingAverage", mb.Model);
	compiler.DebugDump();
}
