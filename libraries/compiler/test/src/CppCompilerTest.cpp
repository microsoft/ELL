#include <string>
#include <ostream>
#include "CppCompiler.h"

using namespace emll::compiler;

void TestCppCompiler()
{
	CppCompiler compiler;
	CppModuleEmitter& module = compiler.Module();
	module.Global<double>("g_1", 33.3);
	module.Global<int>("g_2", 44);
	module.Dump();
}