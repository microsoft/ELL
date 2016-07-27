#include "CompilerTest.h"

int main(int argc, char* argv[])
{
	TestModelCompiler();
	TestLinearPredictor();
	TestDataFlowCompiler();

	TestLLVM();
	TestDataFlowBuilder();

	return 0;
}
