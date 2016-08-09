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

	module.Global<double>("g_1", 33.3);
	module.Global<int>("g_2", 44);

	CppFunctionEmitter& fn = compiler.Function();
	NamedValueTypeList args({ {"input", ValueType::PDouble }, {"output", ValueType::PDouble} });
	fn.Begin("Predict", ValueType::Void, args);
	auto var = compiler.Variables().AddLocalScalarVariable(ValueType::Double);
	compiler.EnsureEmitted(*var);
	fn.End();
	module.AddFunction(fn);

	
	module.Dump();
}