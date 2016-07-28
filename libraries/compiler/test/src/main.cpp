#include "CompilerTest.h"

int main(int argc, char* argv[])
{
	TestBinaryVector(false);
	TestBinaryVector(true);
	TestBinaryScalar();
	TestDotProduct(false);
	TestDotProduct(true);
	TestSum(false);
	TestSum(true);
	TestAccumulator(false);
	TestAccumulator(true);
	TestDelay();

	TestLLVM();

	return 0;
}
