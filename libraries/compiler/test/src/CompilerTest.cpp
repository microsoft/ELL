#include <iostream>
#include <string>
#include <ostream>

#include "CompilerTest.h"
#include "IRInclude.h"
#include "IRCompiler.h"
#include "ScalarVar.h"
#include "VectorVar.h"
#include "DataFlowBuilder.h"
#include "testing.h"

using namespace emll::compiler;

void NodePrinter(const model::Node& node)
{
	std::cout << "node_" << node.GetId() << " = " << node.GetRuntimeTypeName() << std::endl;
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

	fnMain.MemMove(pOutput, 2, 0, 3);
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
nodes::BinaryOperationNode<T>* ModelBuilder::Add(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
	return _model.AddNode<nodes::BinaryOperationNode<T>>(x, y, nodes::BinaryOperationNode<T>::OperationType::add);
}

template<typename T>
nodes::BinaryOperationNode<T>* ModelBuilder::Multiply(const model::OutputPort<T>& x, const model::OutputPort<T>& y)
{
	return _model.AddNode<nodes::BinaryOperationNode<T>>(x, y, nodes::BinaryOperationNode<T>::OperationType::coordinatewiseMultiply);
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

nodes::LinearPredictorNode* ModelBuilder::Linear(const model::OutputPort<double>& features, const predictors::LinearPredictor& predictor)
{
	return _model.AddNode<nodes::LinearPredictorNode>(features, predictor);
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

void TestDataFlowBuilder()
{
	model::Model model = InitTestModelSimple();
	DataFlowBuilder db;
	db.Process(model);
}

void TestDataFlowCompiler()
{
	//model::Model model = InitTestModelSimple();
	model::Model model = InitTestModelBinOp();
	DataFlowBuilder db;
	db.Process(model);
	
	IRCompiler compiler("EMLL", std::cout);
	compiler.CompileGraph("Predict", db.Graph());
	compiler.DebugDump();
}

void TestLinearPredictor()
{
	ModelBuilder mb;
	predictors::LinearPredictor p(4);
	p.GetBias() = 0.35;
	double w = 0.003;
	for (size_t i = 0; i < p.GetDimension(); ++i)
	{
		p.GetVector()[i] = w * i;
	}

	auto input = mb.Inputs<double>(4);
	mb.Linear(input->output, p);

	DataFlowBuilder db;
	db.Process(mb.Model);

	IRCompiler compiler("EMLL", std::cout);
	compiler.CompileGraph("Predict", db.Graph());
	compiler.DebugDump();

}

void TestModelCompiler()
{
	std::vector<double> data = { 5, 10, 15, 20};
	std::vector<double> data2 = { 4, 4, 4, 4};

	ModelBuilder mb;

	auto c1 = mb.Constant<double>(data);
	auto c2 = mb.Constant<double>(data2);
	auto bop = mb.Add(c1->output, c1->output);
	auto bop2 = mb.Multiply(bop->output, c2->output);

	IRCompiler compiler("EMLL", std::cout);

	compiler.BeginFunctionPredict();
	compiler.CompileModel(mb.Model);
	compiler.EndFunction();
	compiler.DebugDump();
}

