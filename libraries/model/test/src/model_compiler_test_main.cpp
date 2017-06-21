////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     model_compiler_test_main.cpp (model_compiler_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableNodesTest.h"
#include "CompilerTest.h"
#include "ModelTestUtilities.h"
#include "PerformanceCountersTest.h"

// testing
#include "testing.h"

using namespace ell;
using namespace ell::emitters;

void TestIRCompiler()
{
    // VerboseRegion region;

    TestFloatNode();
    TestCompilableDotProductNode2(3);
    TestCompilableDotProductNode2(4);

    TestCompileIsEqual();    
    TestSimpleMap(false);
    TestSimpleMap(true);
    TestCompiledMapMove();
    TestBinaryScalar();
    TestBinaryVector(true);
    TestBinaryVector(false);
    TestBinaryVector(true, true);
    TestDotProduct();
    TestSum(false);
    TestSum(true);
    TestAccumulator(false);
    TestAccumulator(true);
    TestDelay();
    TestSqrt();
    TestBinaryPredicate(false);
    TestSlidingAverage();
    TestDotProductOutput();
    TestSteppableMap(false);
    // TestSteppableMap(true); // Fails on Windows
    // TestMultiplexer(); // FAILS -- crash 
    // TestLinearPredictor(); // FAILS -- crash
    // TestForest(); // FAILS -- crash

    TestCompilableScalarOutputNode();
    TestCompilableVectorOutputNode();
    TestCompilableAccumulatorNode();
    TestCompilableConstantNode();
    TestCompilableDotProductNode();
    TestCompilableDelayNode();
    TestCompilableDTWDistanceNode();
    TestCompilableMulticlassDTW();
    TestCompilableScalarSumNode();
    TestCompilableSumNode();
    TestCompilableUnaryOperationNode();
    TestCompilableBinaryOperationNode();
    TestCompilableScalarBinaryPredicateNode();
    TestCompilableBinaryPredicateNode();
    TestCompilableMultiplexerNode();
    TestCompilableTypeCastNode();
    TestCompilableAccumulatorNodeFunction();
    TestCompilableSourceNode(false);
    // TestCompilableSourceNode(true); // Fails on Windows
    TestCompilableAccumulatorNodeFunction(); 

    TestPerformanceCounters();
    TestCompilableDotProductNode2(3); // uses IR
    TestCompilableDotProductNode2(4); // uses IR
}

int main(int argc, char* argv[])
{
    try
    {
        TestIRCompiler();
    }
    catch (const utilities::Exception& exception)
    {
        std::cerr << "ERROR, got ELL exception. Message: " << exception.GetMessage() << std::endl;
        throw;
    }

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
