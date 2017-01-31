////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableNodesTest.h"
#include "CompilerTest.h"
#include "ModelBuilder.h"

// testing
#include "testing.h"

using namespace ell;
using namespace ell::emitters;

void TestIRCompiler()
{
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
    // TestMultiplexer(); // Fails due to missing CompilableConstantNode<bool>
    TestSlidingAverage();
    TestDotProductOutput();
    // TestLinearPredictor(); // Fails
    // TestForest(); // Fails
    // TestForestMap(); // Fails

    TestCompilableAccumulatorNode();
    TestCompilableConstantNode();
    TestCompilableDotProductNode();
    TestCompilableDelayNode();
    TestCompilableDTWDistanceNode();
    TestCompilableMulticlassDTW();
    TestCompilableSumNode();
    TestCompilableUnaryOperationNode();
    TestCompilableBinaryOperationNode();
    //    TestCompilableBinaryPredicateNode(); // Fails
    TestCompilableMultiplexerNode();
    TestCompilableTypeCastNode();
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
