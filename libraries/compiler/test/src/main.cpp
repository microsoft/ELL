#include "CompilerTest.h"

using namespace emll::compiler;

int main(int argc, char* argv[])
{
	TestDotProductOutput();

	TestBinaryPredicate(false);
	TestBinaryVector(false);
	TestBinaryVector(true);
	TestBinaryScalar();
	TestDotProduct();
	TestSum(false);
	TestSum(true);
	TestAccumulator(false);
	TestAccumulator(true);
	TestDelay();
	TestSqrt();
	TestSlidingAverage();

	TestLLVM();
	TestLLVMShiftRegister();

	return 0;
}
