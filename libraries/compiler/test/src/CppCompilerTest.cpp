#include <iostream>
#include <string>
#include <ostream>
#include "IndentedTextWriter.h"
#include "CppCompiler.h"

using namespace emll::compiler;

void TestCppCompiler()
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

	module.Dump();
}