#include "CompilerTest.h"
#include "CppCompilerTest.h"

using namespace emll::compiler;

int main(int argc, char* argv[])
{
	TestCppCompiler();

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
	TestBinaryPredicate(false);
	TestElementSelector();
	TestSlidingAverage();
	TestDotProductOutput();

	TestLinearPredictor();
	TestForest();

	TestLLVM();
	TestLLVMShiftRegister();

	return 0;
}
