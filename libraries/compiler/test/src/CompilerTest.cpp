#include "CompilerTest.h"
#include "IRCompiler.h"
#include <string>
#include <ostream>

using namespace emll::compiler;

void TestCompiler()
{
	IREmitter llvm;
	IRModuleEmitter module(llvm, llvm.AddModule("Looper"));
	module.DeclarePrintf();

	llvm::StructType* structType = module.Struct("ShiftRegister", { ValueType::Int32, ValueType::Double });

	std::vector<double> data({ 3.3, 4.4, 5.5, 6.6, 7.7 });
	llvm::GlobalVariable* pData = module.Constant("g_weights", data);
	llvm::GlobalVariable* pOutput = module.Global("g_output", ValueType::Double, data.size());
	llvm::GlobalVariable* pTotal = module.Global("g_total", ValueType::Double);
	llvm::GlobalVariable* pRegisters = module.Global("g_registers", structType, data.size());

	auto fnMain = module.AddMain();

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

	auto pOtherTotal = module.Global("g_total", ValueType::Double);
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
