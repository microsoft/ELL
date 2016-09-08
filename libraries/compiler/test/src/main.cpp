#include "CompilerTest.h"
#include "CppCompilerTest.h"
#include <direct.h>

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
	TestMultiplexer();
	TestSlidingAverage();
	TestDotProductOutput();

	TestLinearPredictor();
	//TestForest();

	TestLLVM();
	TestLLVMShiftRegister();
}


void TestCppCompiler()
{
	TestForestCpp();
	TestAccumulatorCpp(false);
	TestAccumulatorCpp(true);
	TestSumCpp(true);
	TestSumCpp(false);
	TestMultiplexerCpp();
	TestBinaryPredicateCpp();
	TestBinaryVectorCpp(true);
	TestBinaryVectorCpp(false);
	TestDotProductCpp();
	TestEmptyModelCpp();
	TestCppCompilerGeneral();
}

int main(int argc, char* argv[])
{
	_mkdir("C:/temp");
	_mkdir("C:/temp/emll");

	TestCppCompiler();
	TestIRCompiler();

	return 0;
}
