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

// testing
#include "testing.h"

using namespace ell;
using namespace ell::emitters;

void TestIRCompiler()
{
    // VerboseRegion region;

    TestCompileIsEqual();    
    TestSimpleMap(true);
    TestCompiledMapMove();
    TestBinaryVector(true);
    TestBinaryVector(false);
    TestBinaryVector(true, true);
    TestBinaryScalar();
    TestDotProduct();
    TestSum(false);
    TestSum(true);
    TestAccumulator(false);
    TestAccumulator(true);
    TestDelay();
    TestSqrt();
    TestBinaryPredicate(false);
    // TestMultiplexer(); // Fails
    // TestSlidingAverage(); // Fails
    TestDotProductOutput();
    // TestLinearPredictor(); // Fails
    // TestForest(); // Fails
    // TestForestMap(); // Fails
    TestSteppableMap(false);
    // TestSteppableMap(true); // Fails on Windows

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
