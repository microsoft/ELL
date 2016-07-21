#include "CompilerTest.h"

int main(int argc, char* argv[])
{
	//TestLLVM();

	TestDataFlowBuilder();
	TestDataFlowCompiler();

	TestModelEx();
	TestCompiler();
	return 0;
}
