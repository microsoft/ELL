// llvmTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CodeGen.h"
#include "Tests.h"

using namespace emll;

void genAddInt64(codegen::ModuleEmitter& module)
{
	//
	// Declare the add function
	//
	codegen::FunctionEmitter fn = module.function("addInt64", codegen::ValueType::Int64, { codegen::ValueType::Int64, codegen::ValueType::Int64 });
	// Now write code to just add up the arguments passed into this function
	auto result = fn.op(codegen::OperatorType::Add, fn.args());
	// Write code to print the result of the addition
	fn.call("printf", { fn.literal("Add Result == %d\r\n"), result });

	// Return the result
	fn.ret(result);
}

void genLinear2D(codegen::ModuleEmitter& module, double x, double y)
{
	auto fn = module.function("linear2D", codegen::ValueType::Double, {codegen::ValueType::Double, codegen::ValueType::Double});
	auto args = fn.args().begin();
	fn.ret( 
		fn.op(
			codegen::OperatorType::AddF,
			fn.op(codegen::OperatorType::MultiplyF, fn.literal(x), &(*args)),
			fn.op(codegen::OperatorType::MultiplyF, fn.literal(y), &(*(++args)))
	));
}

void genLinear3D(codegen::ModuleEmitter& module, double x, double y, double z)
{
	auto fn = module.function(
		"linear3D", 
		codegen::ValueType::Double, 
		{ codegen::ValueType::Double, codegen::ValueType::Double, codegen::ValueType::Double }
	);
	auto args = fn.args().begin();
	auto lx = fn.op(codegen::OperatorType::MultiplyF, fn.literal(x), &(*(args)));
	auto ly = fn.op(codegen::OperatorType::MultiplyF, fn.literal(y), &(*(++args)));
	auto lz = fn.op(codegen::OperatorType::MultiplyF, fn.literal(z), &(*(++args)));
	fn.ret(
		fn.op(
			codegen::OperatorType::AddF, 
			fn.op(codegen::OperatorType::AddF, lx, ly), 
			lz)
	);
}

//
// Generate a predictor using Register allocations only
//
void genLinear(codegen::ModuleEmitter& module, std::vector<double>& weights)
{
	auto fn = module.function("linearPredict", codegen::ValueType::Double, {codegen::ValueType::PDouble});
	llvm::Value* retVal = nullptr;
	llvm::Value* features = &fn.firstArg();
	for (size_t i = 0; i < weights.size(); ++i)
	{
		auto product = fn.op(codegen::OperatorType::MultiplyF, fn.literal(weights[i]), fn.valueAtA(features, i));
		if (retVal != nullptr)
		{
			retVal = fn.op(codegen::OperatorType::AddF, retVal, product);
		}
		else
		{
			retVal = product;
		}
	}
	if (retVal == nullptr)
	{
		retVal = fn.literal(0.0);
	}
	fn.ret(retVal);
	fn.verify();
}

void genLinear2(codegen::ModuleEmitter& module, std::vector<double>& weights)
{
	auto fn = module.function("linearPredict2", codegen::ValueType::Double, { codegen::ValueType::PDouble });

	llvm::Value* total = fn.var(codegen::ValueType::Double);
	fn.store(total, fn.literal(0.0));

	llvm::Value* features = &fn.firstArg();
	for (size_t i = 0; i < weights.size(); ++i)
	{
		auto product = fn.op(codegen::OperatorType::MultiplyF, fn.literal(weights[i]), fn.valueAtA(features, i));
		auto sum = fn.op(codegen::OperatorType::AddF, product, fn.load(total));
		fn.store(total, sum);
	}
	fn.ret(fn.load(total));
	fn.verify();
}

void declareExtern(codegen::ModuleEmitter& module)
{
	//
	// yes, LLVM requires you declare "extern" references to any C Runtime functions you use
	//
	module.declarePrintf();
	module.declareMalloc();
	module.declareFree();
}

void generateMathFunctions(codegen::ModuleEmitter& module)
{
	//
	// Generate a function to add 2 int64s
	//
	genAddInt64(module);
	//
	// Linear operations
	//
	genLinear2D(module, 33.3, 47.6);
	genLinear3D(module, 33.35, 0.323, 1.355);
	// General purpose linear
	//std::vector<double> weights({ -10879.3, -1344.62, -28.0607, 5136.25, 2627.37 });
	std::vector<double> weights({ 1.5, 2.5, 3.0, 4.0});
	genLinear(module, weights);
	genLinear2(module, weights);
}

llvm::Value* generateFeatures(codegen::FunctionEmitter& fn)
{
	llvm::Value* features = fn.var(codegen::ValueType::Double, 4);
	/*
	fn.setValueAt(features, 0, fn.literal(33.53));
	fn.setValueAt(features, 1, fn.literal(47.99));
	fn.setValueAt(features, 2, fn.literal(0.667));
	fn.setValueAt(features, 3, fn.literal(4.85));
	*/
	fn.setValueAtA(features, 0, fn.literal(10.0));
	fn.setValueAtA(features, 1, fn.literal(2.5));
	fn.setValueAtA(features, 2, fn.literal(10.0));
	fn.setValueAtA(features, 3, fn.literal(10.0));
	return features;
}

void generateMallocTest(codegen::FunctionEmitter& fn)
{
	//
	// Try malloc and free
	//
	fn.call("printf", { fn.literal("Starting malloc test\r\n") });
	auto ptrVar = fn.var(codegen::ValueType::PInt32);

	auto pIntArray = fn.malloc(codegen::ValueType::PInt32, sizeof(int) * 10);
	fn.store(ptrVar, pIntArray);
	for (int i = 0; i < 3; ++i)
	{
		fn.setValueAtH(ptrVar, i, fn.literal(33 + i));
	}
	fn.call("printf", { fn.literal("Get data \r\n") });
	for (int i = 0; i < 3; ++i)
	{
		auto fromArray = fn.valueAtH(ptrVar, i);
		fn.call("printf", { fn.literal("Got data %d\r\n"), fromArray });
	}
	fn.free(pIntArray);
	fn.call("printf", { fn.literal("End malloc test\r\n") });
}

void generateMain(codegen::ModuleEmitter& module)
{
	//
	// Define "main" method for the module
	//
	auto fn = module.addMain();
	//
	// Print Hello world
	//
	fn.call("printf", { fn.literal("Hello from EMLL %s"), fn.literal("Manchester United!") });
	fn.call("printf", { fn.literal("Number == %d\r\n"), fn.literal(43) });

	auto linearResult = fn.call("linear2D", { fn.literal(51.3), fn.literal(83.832) });
	auto linearResult2 = fn.call("linear3D", { fn.literal(51.3), fn.literal(83.832), fn.literal(93.33) });
	fn.call("printf", { fn.literal("Math == %f\r\n"), linearResult });
	//
	// Call Predict
	//
	auto features = generateFeatures(fn);
	//auto prediction = fn.call("linearPredict", { features });
	//fn.call("printf", { fn.literal("Prediction  %f\r\n"), prediction});
	auto prediction = fn.call("linearPredict2", { features });
	fn.call("printf", { fn.literal("Prediction2  %f\r\n"), prediction });
	//
	// Add 2 numbers using our function
	//
	fn.call("addInt64", { fn.literal(12LL), fn.literal(17LL) });
	fn.call("addInt64", { fn.literal(43LL), fn.literal(17LL) });

	generateMallocTest(fn);
	//
	// Return
	//
	fn.ret();

	fn.verify();
}

void generateHelloWorld()
{
	codegen::LLVMEmitter emitter;
	codegen::ModuleEmitter module(&emitter, emitter.addModule("Hello EMLL"));

	declareExtern(module);  // Declare references to C functions
	
	generateMathFunctions(module);

	generateMain(module);

	module.dump();
	
	module.writeAsmToFile("C:\\junk\\helloAsm.ll");
	module.writeBitcodeToFile("C:\\junk\\hello.bc");
}

int main()
{
	double treeInput[3] = { 0.2, 0.5, 0.0 };
	double dtree = TestForest(treeInput);


	generateHelloWorld();
    return 0;
}

