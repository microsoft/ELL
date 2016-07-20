#include "CompilerTest.h"

int main(int argc, char* argv[])
{
	TestLLVM();

	TestDataFlowBuilder();

	TestModelEx();
	TestCompiler();
	return 0;
}
