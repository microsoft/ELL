////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (model_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

//
// Model tests
//

#include "Map_test.h"
#include "ModelBuilder_test.h"
#include "Model_test.h"
#include "PortElements_test.h"

// testing
#include "testing.h"

// utilities
#include "Exception.h"

// stl
#include <iostream>

using namespace ell;

int main()
{
    try
    {
        // Model tests
        TestStaticModel();
        TestNodeIterator();
        TestModelSerialization();
        TestModelMetadata();
        TestInputRouting1();
        TestInputRouting2();

        TestCopyModel();
        TestRefineSplitOutputs();

        // PortElements tests
        TestSlice();
        TestAppend();
        TestParsePortElements();

        // Map tests
        TestMapCreate();
        TestMapCompute();
        TestMapComputeDataVector();
        TestMapRefine();
        TestMapSerialization();
        TestMapClockNode();

        TestCustomRefine();

        //
        // ModelBuilder tests
        //

        // Test passthrough to Model::AddNode
        TestTemplateAddNode();

        // Test ModelBuilder::GetAddNodeArgs(string)
        TestInputNodeGetArgumentTypes();
        TestConstantNodeGetArgumentTypes();
        TestOutputNodeGetArgumentTypes();
        TestBinaryOpNodeGetArgumentTypes();

        // Test ModelBuilder::AddNode(string, vector<Variant>) with exact types
        TestVariantAddConstantNodes();
        TestVariantAddInputNodes();
        TestVariantAddOutputNode();
        TestVariantAddBinaryOpNode();

        // Test ModelBuilder::AddNode(string, vector<Variant>) with convertable types
        TestVariantAddInputNodesConvertableArgs();

        // Test ModelBuilder::AddNode(string, vector<string>)
        TestVariantAddInputNodesParsedArgs();
    }
    catch (const utilities::Exception& exception)
    {
        std::cerr << "ERROR, got ELL exception. Message: " << exception.GetMessage() << std::endl;
        return 1;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "ERROR, got unhandled exception. Message: " << exception.what() << std::endl;
        return 1;
    }

    if (testing::DidTestFail())
    {
        return 1;
    }

    return 0;
}
