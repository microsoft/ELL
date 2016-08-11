#include <iostream>
#include <string>
#include <ostream>
#include "IndentedTextWriter.h"
#include "CppCompiler.h"
#include "CompilerTest.h"

using namespace emll::compiler;

void TestCppCompilerGeneral()
{
	CppCompiler compiler;

	CppModuleEmitter& module = compiler.Module();
	std::vector<double> data({8.3, 4.33, 7.11});

	NamedValueTypeList args({ {"input", ValueType::PDouble }, {"output", ValueType::PDouble} });
	compiler.BeginFunction("Predict", args);
	auto var = compiler.Variables().AddLocalScalarVariable(ValueType::Double);
	auto varV = compiler.Variables().AddVectorVariable(VariableScope::Global, ValueType::Double, 9);
	auto varV2 = compiler.Variables().AddVariable<InitializedVectorVar<double>>(VariableScope::Global, data);
	compiler.EnsureEmitted(*var);
	compiler.EnsureEmitted(*varV);
	compiler.EnsureEmitted(*varV2);
	compiler.Function().BeginFor("i", 32);
	compiler.Function().BeginIf("j", ComparisonType::Gte, 143);
	compiler.Function().EndIf();
	compiler.Function().EndFor();
	compiler.EndFunction();

	compiler.DebugDump();
}

void TestEmptyModelCpp()
{
	ModelBuilder mb;
	auto input1 = mb.Inputs<double>(4);
	mb.Outputs(input1->output);

	CppCompiler compiler;
	compiler.CompileModel("TestEmpty", mb.Model);
	compiler.DebugDump();
	compiler.WriteToFile("C:\\junk\\model\\TestEmpty.cpp");
}

void TestBinaryVectorCpp(bool expanded)
{
	std::vector<double> data = { 5, 10, 15, 20 };
	std::vector<double> data2 = { 4, 4, 4, 4 };

	ModelBuilder mb;

	auto input1 = mb.Inputs<double>(4);
	auto c1 = mb.Constant<double>(data);
	auto c2 = mb.Constant<double>(data2);

	auto bop = mb.Add(c1->output, input1->output);
	auto bop2 = mb.Multiply(bop->output, c2->output);
	auto output = mb.Outputs<double>(bop2->output);

	CppCompiler compiler;
	compiler.Settings().ShouldUnrollLoops() = expanded;
	compiler.CompileModel("TestBinaryVector", mb.Model);
	compiler.DebugDump();
	compiler.WriteToFile("C:\\junk\\model\\TestBinary.cpp");
}

void TestSumCpp(bool expanded)
{
	std::vector<double> data = { 5, 10, 15, 20 };

	ModelBuilder mb;
	auto c1 = mb.Constant<double>(data);
	auto input1 = mb.Inputs<double>(4);
	auto product = mb.Multiply<double>(c1->output, input1->output);
	auto sum = mb.Sum<double>(product->output);
	auto output = mb.Outputs<double>(sum->output);

	CppCompiler compiler;
	compiler.Settings().ShouldUnrollLoops() = expanded;
	compiler.CompileModel("TestSum", mb.Model);
	compiler.DebugDump();
	compiler.WriteToFile("C:\\junk\\model\\TestSum.cpp");
}

