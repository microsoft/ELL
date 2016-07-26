#include "CompilerTest.h"

int main(int argc, char* argv[])
{
	TestLinearPredictor();
	TestDataFlowCompiler();

	TestLLVM();
	TestDataFlowBuilder();
	TestModelEx();

	return 0;
}
