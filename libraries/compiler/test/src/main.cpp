#include "CompilerTest.h"
#include "CppCompilerTest.h"

using namespace emll::compiler;

void TestIRCompiler()
{
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
}


void TestCppCompiler()
{
	TestForestCpp();
	TestSumCpp(true);
	TestSumCpp(false);
	TestElementSelectorCpp();
	TestBinaryPredicateCpp();
	TestBinaryVectorCpp(true);
	TestBinaryVectorCpp(false);
	TestEmptyModelCpp();
	TestCppCompilerGeneral();
}

int main(int argc, char* argv[])
{
	TestCppCompiler();
	TestIRCompiler();

	return 0;
}
