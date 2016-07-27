#include "CompilerTest.h"

int main(int argc, char* argv[])
{
	TestBinaryVector(false);
	TestBinaryVector(true);
	TestBinaryScalar();
	TestDotProduct(false);
	TestDotProduct(true);
	TestLLVM();

	return 0;
}
