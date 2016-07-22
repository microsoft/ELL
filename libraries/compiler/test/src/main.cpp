#include "CompilerTest.h"

int main(int argc, char* argv[])
{
	TestDataFlowCompiler();

	TestLLVM();
	TestDataFlowBuilder();
	TestModelEx();

	return 0;
}
